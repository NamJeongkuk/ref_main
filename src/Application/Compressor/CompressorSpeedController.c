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
   Signal_DisableMinimumTimeSetRequest,
   Signal_DisableMinimumTimeClearRequest,
   Signal_RemainOffAfterValveMoveTimerComplete,
   Signal_MinimumOnTimerComplete,
   Signal_MinimumOffTimerComplete,
   Signal_ValveVotedPositionChanged,
   Signal_ResolvedSpeedVoteChanged,
   Signal_SabbathDelayTimerComplete
};

static bool State_StartUp(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_On(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_OnAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_MinimumOnTime(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_OffAndReadyToChange(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_MinimumOffTime(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_RemainOffAfterValveMove(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_SabbathDelay(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_On, HSM_NO_PARENT },
   { State_StartUp, State_On },
   { State_Off, HSM_NO_PARENT },
   { State_OnAndReadyToChange, State_On },
   { State_MinimumOnTime, State_On },
   { State_OffAndReadyToChange, State_Off },
   { State_MinimumOffTime, State_Off },
   { State_RemainOffAfterValveMove, State_Off },
   { State_SabbathDelay, State_Off },
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

static void StartupTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;
   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_StartUpTimerComplete,
      NULL);
}

static CompressorSpeed_t ResolvedCompressorSpeedIs(CompressorSpeedController_t *instance)
{
   CompressorVotedSpeed_t actual;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedResolvedVoteErd,
      &actual);
   return actual.speed;
}

static void SabbathDelayTimerCompleted(void *context)
{
   CompressorSpeedController_t *instance = context;

   Hsm_SendSignal(
      &instance->_private.hsm,
      Signal_SabbathDelayTimerComplete,
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

static void StartSabbathDelayTimer(CompressorSpeedController_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.sabbathTimer,
      instance->_private.config->sabbathDelayTimeInSeconds * MSEC_PER_SEC,
      SabbathDelayTimerCompleted,
      instance);
}

static void RequestCompressorSpeed(CompressorSpeedController_t *instance, CompressorSpeed_t request)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedRequestErd,
      &request);
}

static void CacheCompressorVotedSpeed(CompressorSpeedController_t *instance)
{
   CompressorVotedSpeed_t speedVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorSpeedResolvedVoteErd,
      &speedVote);

   instance->_private.cacheSpeedFromResolvedVote = speedVote.speed;
}

static CompressorSpeed_t CachedVotedSpeed(CompressorSpeedController_t *instance)
{
   return instance->_private.cacheSpeedFromResolvedVote;
}

static bool State_StartUp(Hsm_t *hsm, HsmSignal_t signal, const void *data)
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
         RequestCompressorSpeed(instance, CachedVotedSpeed(instance));
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
         if(*newResolvedVotedSpeed > CompressorSpeed_Off)
         {
            Hsm_Transition(hsm, State_SabbathDelay);
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
         Hsm_Transition(hsm, State_StartUp);
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
}

static HsmState_t InitializationState(CompressorSpeedController_t *instance)
{
   if(ResolvedCompressorSpeedIs(instance) > CompressorSpeed_Off)
   {
      return State_SabbathDelay;
   }

   return State_OffAndReadyToChange;
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
      InitializationState(instance));
}
