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
#include "DataModelErdPointerAccess.h"
#include "DefrostState.h"
#include "HeaterVotedState.h"
#include "utils.h"
#include "FanSpeed.h"
#include "Setpoint.h"
#include "CompressorState.h"
#include "Signal.h"

enum
{
   PowerUpFactor = 5
};

enum
{
   DontCare = false,
   Care = true
};

enum
{
   Signal_DefrostReadyTimerIsSatisfied = Hsm_UserSignalStart
};

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_PrechillPrep(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Prechill(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_HeaterOnEntry(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Dwell(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Idle, HSM_NO_PARENT },
   { State_Dwell, HSM_NO_PARENT },
   { State_PrechillPrep, HSM_NO_PARENT },
   { State_Prechill, HSM_NO_PARENT },
   { State_HeaterOnEntry, HSM_NO_PARENT }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static Defrost_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(Defrost_t, _private.hsm, hsm);
}

static void SetHsmStateTo(Defrost_t *instance, DefrostHsmState_t state)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->defrostHsmStateErd, &state);
}

static void DataModelChanged(void *context, const void *args)
{
   Defrost_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->defrostReadyTimerIsSatisfied)
   {
      const bool *state = onChangeData->data;

      if(*state)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DefrostReadyTimerIsSatisfied, NULL);
      }
   }
}

static bool FreshFoodDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodDefrostWasAbnormalErd,
      &defrostWasAbnormal);

   return defrostWasAbnormal;
}

static bool FreezerDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerDefrostWasAbnormalErd,
      &defrostWasAbnormal);

   return defrostWasAbnormal;
}

static bool ConvertibleCompartmentDefrostWasAbnormal(Defrost_t *instance)
{
   bool defrostWasAbnormal;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->convertibleCompartmentDefrostWasAbnormalErd,
      &defrostWasAbnormal);

   return defrostWasAbnormal;
}

static bool AnyPreviousDefrostWasAbnormal(Defrost_t *instance)
{
   return (FreshFoodDefrostWasAbnormal(instance) ||
      FreezerDefrostWasAbnormal(instance) ||
      ConvertibleCompartmentDefrostWasAbnormal(instance));
}

static bool FreezerCompartmentWasTooWarmOnPowerUp(Defrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &state);

   return state;
}

static void ResetFreezerCompartmentWasTooWarmOnPowerUp(Defrost_t *instance)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      clear);
}

static HsmState_t InitialState(Defrost_t *instance)
{
   bool freezerFilteredTemperatureTooWarmAtPowerUp;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureWasTooWarmOnPowerUpErd,
      &freezerFilteredTemperatureTooWarmAtPowerUp);

   DefrostState_t defrostState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &defrostState);

   HsmState_t initialState = State_Idle;

   if(freezerFilteredTemperatureTooWarmAtPowerUp)
   {
      if(defrostState == DefrostState_HeaterOn)
      {
         initialState = State_Dwell;
      }
   }
   else
   {
      if(defrostState == DefrostState_Dwell)
      {
         initialState = State_Dwell;
      }
      else if(defrostState == DefrostState_Prechill || defrostState == DefrostState_HeaterOn)
      {
         initialState = State_HeaterOnEntry;
      }
   }

   return initialState;
}

static bool State_Idle(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Idle);
         break;

      case Signal_DefrostReadyTimerIsSatisfied:
         if(AnyPreviousDefrostWasAbnormal(instance) || FreezerCompartmentWasTooWarmOnPowerUp(instance))
         {
            if(FreezerCompartmentWasTooWarmOnPowerUp(instance))
            {
               ResetFreezerCompartmentWasTooWarmOnPowerUp(instance);
            }
            Hsm_Transition(hsm, State_HeaterOnEntry);
         }
         else
         {
            Hsm_Transition(hsm, State_PrechillPrep);
         }
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

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_PrechillPrep);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Prechill(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   Defrost_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, DefrostHsmState_Prechill);
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
   instance->_private.evaporatorParametricData = PersonalityParametricData_Get(dataModel)->evaporatorData;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, InitialState(instance));

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);
}
