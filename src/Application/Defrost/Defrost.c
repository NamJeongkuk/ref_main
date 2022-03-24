/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Defrost.h"
#include "DefrostHsmState.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "CalculatedGridLines.h"
#include "DefrostTimerRequest.h"
#include "DataModelErdPointerAccess.h"
#include "DefrostState.h"
#include "utils.h"

enum
{
   PowerUpFactor = 5
};

enum
{
   Signal_PowerUpDelayComplete = Hsm_UserSignalStart
};

static bool State_PowerUp(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PostPrechill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_PowerUp, HSM_NO_PARENT },
   { State_Idle, HSM_NO_PARENT },
   { State_Dwell, HSM_NO_PARENT },
   { State_PrechillPrep, HSM_NO_PARENT },
   { State_PostPrechill, HSM_NO_PARENT },
   { State_HeaterOnEntry, HSM_NO_PARENT }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static Defrost_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(Defrost_t, _private.hsm, hsm);
};

static void SetHsmStateTo(Defrost_t *instance, DefrostHsmState_t state)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->defrostHsmStateErd, &state);
};

static void SetDoorHoldoffRequestTo(Defrost_t *instance, DefrostHsmState_t state)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->defrostDoorHoldoffRequestErd, &state);
};

static void ResetDefrostTimerModule(Defrost_t *instance)
{
   DefrostTimerRequest_t defrostTimerRequest;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerRequestErd,
      &defrostTimerRequest);

   defrostTimerRequest.request = DefrostTimer_Reset;
   defrostTimerRequest.requestId++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->defrostTimerRequestErd,
      &defrostTimerRequest);
};

static void SaveFzAbnormalDefrostData(Defrost_t *instance)
{
   bool fzDefrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostWasAbnormalErd,
      &fzDefrostWasAbnormal);

   uint16_t fzAbnormalDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzAbnormalDefrostCycleCountErd,
      &fzAbnormalDefrostCycleCount);

   uint16_t fzDefrostCycleCount;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostCycleCountErd,
      &fzDefrostCycleCount);

   if(fzAbnormalDefrostCycleCount != fzDefrostCycleCount)
   {
      uint16_t numberOfFzAbnormalDefrostCycles;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->numberOfFzAbnormalDefrostCyclesErd,
         &numberOfFzAbnormalDefrostCycles);

      numberOfFzAbnormalDefrostCycles++;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->numberOfFzAbnormalDefrostCyclesErd,
         &numberOfFzAbnormalDefrostCycles);
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fzAbnormalDefrostCycleCountErd,
      &fzDefrostCycleCount);

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fzDefrostWasAbnormalErd,
      set);
};

static bool LastDefrostWasAbnormalBecauseOfAbnormalFilteredFzCabinetTemperature(Defrost_t *instance)
{
   TemperatureDegFx100_t fzFilteredTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureErd,
      &fzFilteredTemperature);

   CalculatedGridLines_t calcGridLines;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedGridLinesErd,
      &calcGridLines);

   TemperatureDegFx100_t gridFzExtremeHystTemperature = calcGridLines.gridLines[GridDelta_Fz].gridLinesDegFx100[GridLine_FzExtremeHigh];

   return (fzFilteredTemperature > gridFzExtremeHystTemperature ||
      fzFilteredTemperature >= instance->_private.defrostParametricData->fzDefrostTerminationTemperatureInDegFx100);
};

static void PowerUpDelay(void *context)
{
   REINTERPRET(instance, context, Defrost_t *);

   Hsm_SendSignal(&instance->_private.hsm, Signal_PowerUpDelayComplete, NULL);
};

static void StartTimer(Defrost_t *instance, TimerTicks_t ticks, TimerCallback_t callback)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      ticks,
      callback,
      instance);
};

static bool State_PowerUp(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   DefrostState_t lastDefrostState;
   bool defrostWasAbnormal;
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PowerUp);
         SetDoorHoldoffRequestTo(instance, ENABLED);
         StartTimer(
            instance,
            instance->_private.gridParametricData->gridPeriodicRunRateInMSec * PowerUpFactor,
            PowerUpDelay);
         break;

      case Signal_PowerUpDelayComplete:
         DataModel_Read(
            instance->_private.dataModel,
            instance->_private.config->defrostStateErd,
            &lastDefrostState);

         DataModel_Read(
            instance->_private.dataModel,
            instance->_private.config->fzDefrostWasAbnormalErd,
            &defrostWasAbnormal);

         if(LastDefrostWasAbnormalBecauseOfAbnormalFilteredFzCabinetTemperature(instance))
         {
            ResetDefrostTimerModule(instance);
            SaveFzAbnormalDefrostData(instance);

            (lastDefrostState == DefrostState_HeaterOn) ? Hsm_Transition(hsm, State_Dwell) : Hsm_Transition(hsm, State_Idle);
         }
         else
         {
            if(lastDefrostState == DefrostState_Prechill)
            {
               (defrostWasAbnormal) ? Hsm_Transition(hsm, State_PostPrechill) : Hsm_Transition(hsm, State_PrechillPrep);
            }
            else if(lastDefrostState == DefrostState_HeaterOn)
            {
               Hsm_Transition(hsm, State_HeaterOnEntry);
            }
            else
            {
               Hsm_Transition(hsm, State_Idle);
            }
         }
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Idle);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PrechillPrep);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_PostPrechill(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PostPrechill);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_HeaterOnEntry);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Dwell);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = defrostConfig;
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
   instance->_private.sabbathParametricData = PersonalityParametricData_Get(dataModel)->sabbathData;
   instance->_private.gridParametricData = PersonalityParametricData_Get(dataModel)->gridData;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_PowerUp);
}
