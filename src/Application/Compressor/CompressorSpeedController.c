/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorSpeedController.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "CompressorSpeedFrequencyInHz.h"
#include "CompressorState.h"

enum
{
   Signal_StartUpTimerComplete = Hsm_UserSignalStart,
   Signal_RemainOffAfterValveMoveTimerComplete,
};

static bool State_StartUp(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_On(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_OnAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_MinimumOnTime(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_OffAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_MinimumOffTime(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_RemainOffAfterValveMove(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_On, HSM_NO_PARENT },
   { State_StartUp, State_On },
   { State_Off, HSM_NO_PARENT },
   { State_OnAndReadyToChange, State_On },
   { State_MinimumOnTime, State_On },
   { State_OffAndReadyToChange, State_Off },
   { State_MinimumOffTime, State_Off },
   { State_RemainOffAfterValveMove, State_Off },
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static CompressorSpeedController_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(CompressorSpeedController_t, _private.hsm, hsm);
}

static void SetHsmStateTo(CompressorSpeedController_t *instance, CompressorState_t state)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->compressorStateErd, &state);
}

static bool TimerIsRunning(CompressorSpeedController_t *instance, Timer_t *timer)
{
   return TimerModule_IsRunning(instance->_private.timerModule, timer);
}

static void RemainOffAfterValveMoveTimerExpired(void *context)
{
   CompressorSpeedController_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_RemainOffAfterValveMoveTimerComplete, NULL);
}

static CompressorTimes_t GetCompressorTimes(CompressorSpeedController_t *instance)
{
   return instance->_private.compressorData->compressorTimes;
}

static bool State_StartUp(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_Startup);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_On(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_On);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_Off);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_OnAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_OnAndReadyToChange);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_MinimumOnTime(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_MinimumOnTime);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_OffAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_OffAndReadyToChange);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_MinimumOffTime(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_MinimumOffTime);
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_RemainOffAfterValveMove(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_RemainOffAfterValveMove);
         if(!TimerIsRunning(instance, &instance->_private.remainOffAfterValveMoveTimer))
         {
            TimerModule_StartOneShot(
               instance->_private.timerModule,
               &instance->_private.remainOffAfterValveMoveTimer,
               GetCompressorTimes(instance).remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN,
               RemainOffAfterValveMoveTimerExpired,
               instance);
         }
         break;

      case Signal_RemainOffAfterValveMoveTimerComplete:
         if(TimerIsRunning(instance, &instance->_private.minimumOffTimer))
         {
            Hsm_Transition(&instance->_private.hsm, State_MinimumOffTime);
         }
         else
         {
            Hsm_Transition(&instance->_private.hsm, State_OffAndReadyToChange);
         }
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

void CompressorSpeedController_Init(
   CompressorSpeedController_t *instance,
   I_DataModel_t *dataModel,
   const CompressorSpeedControllerConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.timerModule = DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);
   instance->_private.compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   instance->_private.config = config;

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_StartUp);
}
