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
#include "Constants_Binary.h"

enum
{
   Signal_StartUpTimerComplete = Hsm_UserSignalStart,
   Signal_RemainOffAfterValveMoveTimerComplete,
   Signal_MinimumOnTimerComplete,
   Signal_MinimumOffTimerComplete,
   Signal_ValvePositionChanged,
   Signal_ResolvedSpeedVoteChanged,
   Signal_SabbathDelayTimerComplete,
   Signal_MinimumRunTimerComplete,
   Signal_MinimumTimesEnabled,
   Signal_MinimumTimesDisabled
};

static bool State_Startup(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_On(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_OnAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_MinimumOnTime(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_OffAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_MinimumOffTime(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_RemainOffAfterValveMove(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_SabbathDelay(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_VariableSpeedMinimumRunTime(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_On, HSM_NO_PARENT },
   { State_Startup, State_On },
   { State_Off, HSM_NO_PARENT },
   { State_OnAndReadyToChange, State_On },
   { State_MinimumOnTime, State_On },
   { State_OffAndReadyToChange, State_Off },
   { State_MinimumOffTime, State_Off },
   { State_RemainOffAfterValveMove, State_Off },
   { State_SabbathDelay, State_Off },
   { State_VariableSpeedMinimumRunTime, State_On }
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
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorStateErd,
      &state);
}

static void StartupTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;

   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_StartUpTimerComplete,
      NULL);
}

static void SabbathDelayTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;

   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_SabbathDelayTimerComplete,
      NULL);
}

static void MinimumOnTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;

   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_MinimumOnTimerComplete,
      NULL);
}

static void MinimumRunTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;

   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_MinimumRunTimerComplete,
      NULL);
}

static void MinimumOffTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;

   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_MinimumOffTimerComplete,
      NULL);
}

static void RemainOffAfterValveMoveTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;

   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_RemainOffAfterValveMoveTimerComplete,
      NULL);
}

static void StartStartupTimer(CompressorSpeedController_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.startupTimer,
      instance->_private.compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC,
      StartupTimerCompleted,
      instance);
}

static void StopTimer(CompressorSpeedController_t *instance, Timer_t *timer)
{
   TimerModule_Stop(instance->_private.timerModule, timer);
}

static void StartSabbathDelayTimer(CompressorSpeedController_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.sabbathDelayTimer,
      instance->_private.config->sabbathDelayTimeInSeconds * MSEC_PER_SEC,
      SabbathDelayTimerCompleted,
      instance);
}

static void StartMinimumOnTimer(CompressorSpeedController_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.compressorTimer,
      instance->_private.compressorData->compressorTimes.minimumOnTimeInMinutes * MSEC_PER_MIN,
      MinimumOnTimerCompleted,
      instance);
}

static void StartMinimumOffTimerIfNotAlreadyRunning(CompressorSpeedController_t *instance)
{
   if(!TimerModule_IsRunning(instance->_private.timerModule, &instance->_private.compressorTimer))
   {
      TimerModule_StartOneShot(
         instance->_private.timerModule,
         &instance->_private.compressorTimer,
         instance->_private.compressorData->compressorTimes.minimumOffTimeInMinutes * MSEC_PER_MIN,
         MinimumOffTimerCompleted,
         instance);
   }
}

static void StartMinimumRunTimer(CompressorSpeedController_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.minimumRunTimer,
      instance->_private.compressorData->compressorTimes.variableSpeedMinimumRunTimeInMinutes * MSEC_PER_MIN,
      MinimumRunTimerCompleted,
      instance);
}

static TemperatureDegFx100_t FilteredAmbientTemperature(CompressorSpeedController_t *instance)
{
   TemperatureDegFx100_t actualFilteredAmbientTemperature;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->filteredAmbientTemperatureInDegFx100Erd,
      &actualFilteredAmbientTemperature);

   return actualFilteredAmbientTemperature;
}

static void StartRemainOffAfterValveMoveTimer(CompressorSpeedController_t *instance)
{
   TimerTicks_t remainOffAfterValveMoveTime = instance->_private.compressorData->compressorTimes.remainOffAfterValveMoveTimeInMinutes * MSEC_PER_MIN;

   if(FilteredAmbientTemperature(instance) <= instance->_private.compressorData->lowAmbientTriggerTemperatureInDegFx100)
   {
      remainOffAfterValveMoveTime = instance->_private.compressorData->compressorTimes.remainOffAfterValveMoveLowAmbientTimeInMinutes * MSEC_PER_MIN;
   }

   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.remainOffAfterValveMoveTimer,
      remainOffAfterValveMoveTime,
      RemainOffAfterValveMoveTimerCompleted,
      instance);
}

static void RequestCompressorSpeed(CompressorSpeedController_t *instance, CompressorSpeed_t request)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedRequestErd,
      &request);
}

static CompressorSpeed_t ResolvedCompressorVotedSpeed(CompressorSpeedController_t *instance)
{
   CompressorVotedSpeed_t speedVote;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedResolvedVoteErd,
      &speedVote);

   return speedVote.speed;
}

static void MinimumTimesDisabled(CompressorSpeedController_t *instance)
{
   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_MinimumTimesDisabled,
      NULL);
}

static void MinimumTimesEnabled(CompressorSpeedController_t *instance)
{
   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_MinimumTimesEnabled,
      NULL);
}

static void CacheCompressorVotedSpeed(CompressorSpeedController_t *instance)
{
   instance->_private.cacheSpeedFromResolvedVote = ResolvedCompressorVotedSpeed(instance);
}

static CompressorSpeed_t CachedVotedSpeed(CompressorSpeedController_t *instance)
{
   return instance->_private.cacheSpeedFromResolvedVote;
}

static bool CompressorIsVariableSpeed(CompressorSpeedController_t *instance)
{
   return !instance->_private.compressorData->compressorIsSingleSpeed;
}

static bool MinimumTimesAreEnabled(CompressorSpeedController_t *instance)
{
   bool minimumTimesDisabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->disableMinimumCompressorTimeErd,
      &minimumTimesDisabled);

   return !minimumTimesDisabled;
}

static HsmState_t InitializationCompressorState(CompressorSpeedController_t *instance)
{
   CompressorSpeed_t resolvedSpeed = ResolvedCompressorVotedSpeed(instance);
   if(MinimumTimesAreEnabled(instance) && resolvedSpeed != CompressorSpeed_Off)
   {
      return State_SabbathDelay;
   }
   else if(resolvedSpeed == CompressorSpeed_Off)
   {
      return State_OffAndReadyToChange;
   }
   else
   {
      return State_OnAndReadyToChange;
   }
}

static bool State_Startup(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_Startup);
         StartStartupTimer(instance);
         RequestCompressorSpeed(instance, CompressorSpeed_Startup);
         break;

      case Signal_StartUpTimerComplete:
         Hsm_Transition(hsm, State_MinimumOnTime);
         break;

      case Signal_MinimumTimesDisabled:
         StopTimer(instance, &instance->_private.sabbathDelayTimer);
         if(ResolvedCompressorVotedSpeed(instance) == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
         else
         {
            Hsm_Transition(hsm, State_OnAndReadyToChange);
         }
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
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
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
   IGNORE(instance);

   switch(signal)
   {
      case Hsm_Entry:
         RequestCompressorSpeed(instance, CompressorSpeed_Off);
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

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_OnAndReadyToChange);
         CompressorSpeed_t currentResolvedSpeedVote = ResolvedCompressorVotedSpeed(instance);

         if(!MinimumTimesAreEnabled(instance))
         {
            RequestCompressorSpeed(instance, currentResolvedSpeedVote);
         }
         break;

      case Signal_ResolvedSpeedVoteChanged: {
         const CompressorSpeed_t *votedSpeed = data;
         if(*votedSpeed == CompressorSpeed_Off)
         {
            if(MinimumTimesAreEnabled(instance))
            {
               Hsm_Transition(hsm, State_MinimumOffTime);
            }
            else
            {
               Hsm_Transition(hsm, State_OffAndReadyToChange);
            }
         }
         else if(CompressorIsVariableSpeed(instance))
         {
            if(MinimumTimesAreEnabled(instance))
            {
               Hsm_Transition(hsm, State_VariableSpeedMinimumRunTime);
            }
            else
            {
               RequestCompressorSpeed(instance, *votedSpeed);
            }
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

static bool State_MinimumOnTime(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_MinimumOnTime);
         RequestCompressorSpeed(instance, CachedVotedSpeed(instance));
         StartMinimumOnTimer(instance);
         break;

      case Signal_MinimumOnTimerComplete: {
         CompressorSpeed_t currentVotedSpeed = ResolvedCompressorVotedSpeed(instance);
         if(currentVotedSpeed == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_MinimumOffTime);
         }
         else
         {
            Hsm_Transition(hsm, State_OnAndReadyToChange);
         }
      }
      break;

      case Signal_MinimumTimesDisabled: {
         StopTimer(instance, &instance->_private.compressorTimer);
         CompressorSpeed_t resolvedVotedSpeed = ResolvedCompressorVotedSpeed(instance);
         if(resolvedVotedSpeed == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
         else
         {
            Hsm_Transition(hsm, State_OnAndReadyToChange);
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

static bool State_OffAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_OffAndReadyToChange);
         break;

      case Signal_ResolvedSpeedVoteChanged: {
         const CompressorSpeed_t *newResolvedVotedSpeed = data;
         if(*newResolvedVotedSpeed != CompressorSpeed_Off)
         {
            if(MinimumTimesAreEnabled(instance))
            {
               Hsm_Transition(hsm, State_SabbathDelay);
            }
            else
            {
               RequestCompressorSpeed(instance, *newResolvedVotedSpeed);
               Hsm_Transition(hsm, State_OnAndReadyToChange);
            }
         }
      }
      break;

      case Signal_ValvePositionChanged:
         if(MinimumTimesAreEnabled(instance))
         {
            Hsm_Transition(hsm, State_RemainOffAfterValveMove);
         }
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
         StartMinimumOffTimerIfNotAlreadyRunning(instance);
         break;

      case Signal_MinimumOffTimerComplete: {
         CompressorSpeed_t currentResolvedVote = ResolvedCompressorVotedSpeed(instance);
         if(currentResolvedVote != CompressorSpeed_Off)
         {
            if(MinimumTimesAreEnabled(instance))
            {
               Hsm_Transition(hsm, State_SabbathDelay);
            }
            else
            {
               Hsm_Transition(hsm, State_OnAndReadyToChange);
            }
         }
         else
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
      }
      break;

      case Signal_ValvePositionChanged:
         Hsm_Transition(hsm, State_RemainOffAfterValveMove);
         break;

      case Signal_MinimumTimesDisabled:
         StopTimer(instance, &instance->_private.compressorTimer);
         if(ResolvedCompressorVotedSpeed(instance) == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
         else
         {
            Hsm_Transition(hsm, State_OnAndReadyToChange);
         }
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
         StartRemainOffAfterValveMoveTimer(instance);
         break;

      case Signal_RemainOffAfterValveMoveTimerComplete:
         if(TimerModule_IsRunning(instance->_private.timerModule, &instance->_private.compressorTimer))
         {
            Hsm_Transition(hsm, State_MinimumOffTime);
         }
         else
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
         break;

      case Signal_MinimumTimesDisabled:
         StopTimer(instance, &instance->_private.remainOffAfterValveMoveTimer);
         if(ResolvedCompressorVotedSpeed(instance) == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
         else
         {
            Hsm_Transition(hsm, State_OnAndReadyToChange);
         }
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_SabbathDelay(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         CacheCompressorVotedSpeed(instance);
         SetHsmStateTo(instance, CompressorState_SabbathDelay);
         StartSabbathDelayTimer(instance);
         break;

      case Signal_SabbathDelayTimerComplete:
         Hsm_Transition(hsm, State_Startup);
         break;

      case Signal_MinimumTimesDisabled:
         StopTimer(instance, &instance->_private.startupTimer);
         CompressorSpeed_t resolvedVotedSpeed = ResolvedCompressorVotedSpeed(instance);
         if(resolvedVotedSpeed == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
         else
         {
            Hsm_Transition(hsm, State_OnAndReadyToChange);
         }
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static bool State_VariableSpeedMinimumRunTime(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   CompressorSpeedController_t *instance = InstanceFromHsm(hsm);
   IGNORE(data);

   switch(signal)
   {
      case Hsm_Entry:
         SetHsmStateTo(instance, CompressorState_VariableSpeedMinimumRunTime);
         RequestCompressorSpeed(instance, ResolvedCompressorVotedSpeed(instance));
         StartMinimumRunTimer(instance);
         break;

      case Signal_MinimumRunTimerComplete:
         if(ResolvedCompressorVotedSpeed(instance) == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_MinimumOffTime);
         }
         else
         {
            Hsm_Transition(hsm, State_OnAndReadyToChange);
         }
         break;

      case Signal_MinimumTimesDisabled:
         StopTimer(instance, &instance->_private.minimumRunTimer);
         CompressorSpeed_t currentCompressorSpeed = ResolvedCompressorVotedSpeed(instance);
         if(currentCompressorSpeed == CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_OffAndReadyToChange);
         }
         else
         {
            RequestCompressorSpeed(instance, currentCompressorSpeed);
            Hsm_Transition(hsm, State_OnAndReadyToChange);
         }
         break;

      case Hsm_Exit:
         break;

      default:
         return HsmSignalDeferred;
   }
   return HsmSignalConsumed;
}

static void DataModelChanged(void *context, const void *args)
{
   CompressorSpeedController_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeArgs = args;

   if(onChangeArgs->erd == instance->_private.config->compressorSpeedResolvedVoteErd)
   {
      const CompressorVotedSpeed_t *votedSpeed = onChangeArgs->data;
      Hsm_SendSignal(
         &instance->_private.hsm,
         Signal_ResolvedSpeedVoteChanged,
         &votedSpeed->speed);
   }
   else if(onChangeArgs->erd == instance->_private.config->valvePositionResolvedVoteErd)
   {
      const ValveVotedPosition_t *resolvedValvePosition = onChangeArgs->data;

      Hsm_SendSignal(
         &instance->_private.hsm,
         Signal_ValvePositionChanged,
         &resolvedValvePosition->position);
   }
   else if(onChangeArgs->erd == instance->_private.config->disableMinimumCompressorTimeErd)
   {
      const bool *minimumTimeDisableRequest = onChangeArgs->data;
      if(*minimumTimeDisableRequest)
      {
         MinimumTimesDisabled(instance);
      }
      else
      {
         MinimumTimesEnabled(instance);
      }
   }
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

   EventSubscription_Init(
      &instance->_private.dataModelChangedSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelChangedSubscription);

   Hsm_Init(
      &instance->_private.hsm,
      &hsmConfiguration,
      InitializationCompressorState(instance));
}
