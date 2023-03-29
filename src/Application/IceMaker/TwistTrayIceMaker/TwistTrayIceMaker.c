/*!
 * @file
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMaker.h"
#include "utils.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "TwistTrayIceMakerMotorController.h"
#include "SystemErds.h"
#include "TwistTrayIceMakerMotorAction.h"
#include "TwistTrayIceMakerHighLevelState.h"
#include "TwistTrayIceMakerOperationState.h"
#include "WaterValveVotedState.h"

#define WaterFillPeriod (instance->_private.parametric->fillData.waterFillTimeSecX10 * 100)
#define MinimumFreezePeriodMinutes (instance->_private.parametric->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN)
#define FullIceBucketWaitPeriodMinutes (instance->_private.parametric->harvestData.fullBucketWaitPeriodMinutes * MSEC_PER_MIN)
#define DelayToHarvestAfterDoorCloses (instance->_private.parametric->harvestData.delayToHarvestAfterDoorClosesSeconds * MSEC_PER_SEC)
#define FreezingPointDegFx100 (instance->_private.parametric->freezeData.freezingTemperatureDegreesFx100)
#define TargetFreezeIntegrationSum (instance->_private.parametric->freezeData.targetFreezeIntegrationSum)

enum
{
   Signal_Entry = Fsm_Entry,
   Signal_Exit = Fsm_Exit,
   Signal_DispenserOn = FSM_USER_SIGNAL_START,
   Signal_DispenserOff,
   Signal_MinimumFreezeTimeReached,
   Signal_IceTemperatureIntegrationIntervalElapsed,
   Signal_MotorActionResultChanged,
   Signal_TrayFilled,
   Signal_FullIceBucketWaitTimeElapsed,
   Signal_IceTemperaturePollingIntervalElapsed,
   Signal_SabbathModeEnabled,
   Signal_SabbathModeDisabled,
   Signal_DoorClosedForLongEnough,
   Signal_ForcedHarvest,

   Idle = TwistTrayIceMakerMotorAction_Idle,
   Home = TwistTrayIceMakerMotorAction_RunHomingRoutine,
   Harvest = TwistTrayIceMakerMotorAction_RunCycle,

   Homed = TwistTrayIceMakerMotorActionResult_Homed,
   BucketWasFull = TwistTrayIceMakerMotorActionResult_BucketWasFull,
   Harvested = TwistTrayIceMakerMotorActionResult_Harvested,
   MotorError = TwistTrayIceMakerMotorActionResult_MotorError,

   OPEN = true,
   CLOSED = false,
   DOOR_OPEN = true,
   DOOR_CLOSED = false,

   IceTemperaturePollPeriodInMsec = 3 * MSEC_PER_SEC,
   IntegrationIntervalPeriodInMsec = 1 * MSEC_PER_SEC,
};

#define FSM_STATE_EXPAND_AS_FUNCTION_DEFINITION(Name) \
   static void Name(Fsm_t *fsm, FsmSignal_t signal, const void *data);

#define FSM_STATE_TABLE(ENTRY)       \
   ENTRY(State_Homing)               \
   ENTRY(State_Freeze)               \
   ENTRY(State_Harvesting)           \
   ENTRY(State_FillingTrayWithWater) \
   ENTRY(State_BucketIsFull)         \
   ENTRY(State_MotorError)

FSM_STATE_TABLE(FSM_STATE_EXPAND_AS_FUNCTION_DEFINITION)

#define open enabled
#define closed disabled

static void MinimumFreezeTimeReached(void *context);
static void IceTemperatureIntegrationIntervalElapsed(void *context);
static void FillingTrayPeriodElapsed(void *context);
static void FullIceBucketWaitTimeElapsed(void *context);
static void IceTemperaturePollingIntervalElapsed(void *context);

static TwistTrayIceMaker_t *InterfaceFrom(Fsm_t *fsm)
{
   return CONTAINER_OF(TwistTrayIceMaker_t, _private.fsm, fsm);
}

static void UpdateHighLevelState(TwistTrayIceMaker_t *instance, TwistTrayIceMakerHighLevelState_t newState)
{
   DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMaker_HighLevelState, &newState);
}

static void UpdateOperationState(TwistTrayIceMaker_t *instance, TwistTrayIceMakerOperationState_t newState)
{
   DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMaker_OperationState, &newState);
}

static void RequestMotorAction(TwistTrayIceMaker_t *instance, TwistTrayIceMakerMotorAction_t newMotorAction)
{
   TwistTrayIceMakerMotorVotedAction_t motorVote;
   motorVote.action = newMotorAction;
   motorVote.care = (newMotorAction == TwistTrayIceMakerMotorAction_Idle) ? Vote_DontCare : Vote_Care;

   DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMakerMotor_IceMakerVote, &motorVote);
}

static bool ItIsSabbathMode(TwistTrayIceMaker_t *instance)
{
   bool sabbathMode;
   bool enhancedSabbathMode;

   DataSource_Read(instance->_private.dataSource, Erd_SabbathMode, &sabbathMode);
   DataSource_Read(instance->_private.dataSource, Erd_EnhancedSabbathMode, &enhancedSabbathMode);

   return sabbathMode || enhancedSabbathMode;
}

static void UpdateWaterValve(TwistTrayIceMaker_t *instance, bool newState)
{
   WaterValveVotedState_t vote;

   if(newState)
   {
      vote.care = Vote_Care;
      vote.state = WaterValveState_On;
   }
   else
   {
      vote.care = Vote_DontCare;
      vote.state = WaterValveState_Off;
   }

   DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMakerWaterValve_IceMakerVote, &vote);
   DataSource_Write(instance->_private.dataSource, Erd_IsolationWaterValve_TwistTrayIceMakerVote, &vote);
}

static void ClearFreezeIntegrationSum(TwistTrayIceMaker_t *instance)
{
   uint32_t freezeIntegrationSum = 0;
   DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMaker_FreezeIntegrationCount, &freezeIntegrationSum);
}

static uint32_t IncreasedFreezeIntegrationSum(TwistTrayIceMaker_t *instance)
{
   TemperatureDegFx100_t iceTrayTempx100;
   uint32_t freezeIntegrationSum;

   DataSource_Read(instance->_private.dataSource, Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100, &iceTrayTempx100);
   DataSource_Read(instance->_private.dataSource, Erd_TwistTrayIceMaker_FreezeIntegrationCount, &freezeIntegrationSum);

   freezeIntegrationSum +=
      TRUNCATE_UNSIGNED_SUBTRACTION(FreezingPointDegFx100, iceTrayTempx100);

   DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMaker_FreezeIntegrationCount, &freezeIntegrationSum);

   return freezeIntegrationSum;
}

static void StartTemperaturePollingTimer(TwistTrayIceMaker_t *instance)
{
   TimerModule_StartPeriodic(
      instance->_private.timerModule,
      &instance->_private.pollingTimer,
      IceTemperaturePollPeriodInMsec,
      IceTemperaturePollingIntervalElapsed,
      instance);
}

static void StopTemperaturePollingTimer(TwistTrayIceMaker_t *instance)
{
   TimerModule_Stop(
      instance->_private.timerModule,
      &instance->_private.pollingTimer);
}

static void StartFreezingTimers(TwistTrayIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.minimumFreezeTimer,
      MinimumFreezePeriodMinutes,
      MinimumFreezeTimeReached,
      instance);

   TimerModule_StartPeriodic(
      instance->_private.timerModule,
      &instance->_private.integrationTimer,
      IntegrationIntervalPeriodInMsec,
      IceTemperatureIntegrationIntervalElapsed,
      instance);
}

static void StopFreezeTimers(TwistTrayIceMaker_t *instance)
{
   TimerModule_Stop(instance->_private.timerModule, &instance->_private.minimumFreezeTimer);
   TimerModule_Stop(instance->_private.timerModule, &instance->_private.integrationTimer);
}

static void StartFillTrayTimer(TwistTrayIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.waitingTimer,
      WaterFillPeriod,
      FillingTrayPeriodElapsed,
      instance);
}

static void StartHarvestingIfConditionsAreMet(TwistTrayIceMaker_t *instance)
{
   if(!ItIsSabbathMode(instance) && instance->_private.doorHasBeenClosedForLongEnough)
   {
      RequestMotorAction(instance, TwistTrayIceMakerMotorAction_RunCycle);
   }
}

static void State_Homing(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InterfaceFrom(fsm);
   const TwistTrayIceMakerMotorActionResult_t *motorActionResult = data;

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_Homing);

         RequestMotorAction(instance, Home);
         break;

      case Signal_MotorActionResultChanged:
         if(Homed == *motorActionResult)
         {
            Fsm_Transition(fsm, State_Freeze);
         }
         else if(MotorError == *motorActionResult)
         {
            Fsm_Transition(fsm, State_MotorError);
         }
         break;

      case Fsm_Exit:
         RequestMotorAction(instance, Idle);
         break;
   }
}

static void State_Freeze(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InterfaceFrom(fsm);
   TemperatureDegFx100_t iceTrayTempx100;
   uint32_t freezeIntegrationSum;
   bool temperatureIsValid;
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_Freeze);

         instance->_private.minimumFreezeTimeReached = false;
         instance->_private.freezeIntegrationSumReached = false;

         ClearFreezeIntegrationSum(instance);

         StartFreezingTimers(instance);
         StartTemperaturePollingTimer(instance);
         break;

      case Signal_IceTemperaturePollingIntervalElapsed:
         DataSource_Read(instance->_private.dataSource, Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100, &iceTrayTempx100);
         DataSource_Read(instance->_private.dataSource, Erd_TwistTrayIceMaker_ThermistorIsValid, &temperatureIsValid);

         if((iceTrayTempx100 > FreezingPointDegFx100) || !temperatureIsValid)
         {
            instance->_private.minimumFreezeTimeReached = false;
            instance->_private.freezeIntegrationSumReached = false;

            ClearFreezeIntegrationSum(instance);
            StopFreezeTimers(instance);
         }
         else
         {
            if(!TimerModule_IsRunning(instance->_private.timerModule, &instance->_private.minimumFreezeTimer) &&
               !TimerModule_IsRunning(instance->_private.timerModule, &instance->_private.integrationTimer))
            {
               StartFreezingTimers(instance);
            }
         }
         break;

      case Signal_MinimumFreezeTimeReached:
         instance->_private.minimumFreezeTimeReached = true;

         if(instance->_private.minimumFreezeTimeReached && instance->_private.freezeIntegrationSumReached)
         {
            Fsm_Transition(fsm, State_Harvesting);
         }
         break;

      case Signal_IceTemperatureIntegrationIntervalElapsed:
         freezeIntegrationSum = IncreasedFreezeIntegrationSum(instance);

         if(freezeIntegrationSum >= TargetFreezeIntegrationSum)
         {
            instance->_private.freezeIntegrationSumReached = true;
            TimerModule_Stop(instance->_private.timerModule, &instance->_private.integrationTimer);

            if(instance->_private.minimumFreezeTimeReached && instance->_private.freezeIntegrationSumReached)
            {
               Fsm_Transition(fsm, State_Harvesting);
            }
         }
         break;

      case Signal_ForcedHarvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Fsm_Exit:
         StopFreezeTimers(instance);
         StopTemperaturePollingTimer(instance);

         instance->_private.minimumFreezeTimeReached = false;
         instance->_private.freezeIntegrationSumReached = false;
         break;
   }
}

static void State_Harvesting(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InterfaceFrom(fsm);
   const TwistTrayIceMakerMotorActionResult_t *motorActionResult = data;

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_Harvesting);
         StartHarvestingIfConditionsAreMet(instance);
         break;

      case Signal_DoorClosedForLongEnough:
      case Signal_SabbathModeDisabled:
         StartHarvestingIfConditionsAreMet(instance);
         break;

      case Signal_MotorActionResultChanged:
         if(Harvested == *motorActionResult)
         {
            Fsm_Transition(fsm, State_FillingTrayWithWater);
         }
         else if(BucketWasFull == *motorActionResult)
         {
            Fsm_Transition(fsm, State_BucketIsFull);
         }
         else if(MotorError == *motorActionResult)
         {
            Fsm_Transition(fsm, State_MotorError);
         }
         break;

      case Fsm_Exit:
         RequestMotorAction(instance, Idle);
         break;
   }
}

static void State_FillingTrayWithWater(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_FillingTrayWithWater);

         if(!ItIsSabbathMode(instance))
         {
            UpdateWaterValve(instance, OPEN);
            StartFillTrayTimer(instance);
         }
         break;

      case Signal_SabbathModeDisabled:
         UpdateWaterValve(instance, OPEN);
         StartFillTrayTimer(instance);
         break;

      case Signal_TrayFilled:
         Fsm_Transition(fsm, State_Freeze);
         break;

      case Fsm_Exit:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.waitingTimer);
         UpdateWaterValve(instance, CLOSED);
         break;
   }
}

static void State_BucketIsFull(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_BucketIsFull);

         TimerModule_StartOneShot(
            instance->_private.timerModule,
            &instance->_private.waitingTimer,
            FullIceBucketWaitPeriodMinutes,
            FullIceBucketWaitTimeElapsed,
            instance);
         break;

      case Signal_DispenserOff:
      case Signal_FullIceBucketWaitTimeElapsed:
      case Signal_ForcedHarvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Fsm_Exit:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.waitingTimer);
         break;
   }
}

static void State_MotorError(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InterfaceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_MotorError);
         UpdateHighLevelState(instance, TwistTrayIceMakerHighLevelState_Fault);

         RequestMotorAction(instance, Idle);
         DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMaker_MotorFaultActive, set);
         break;
   }
}

static void MinimumFreezeTimeReached(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_MinimumFreezeTimeReached, NULL);
}

static void IceTemperatureIntegrationIntervalElapsed(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_IceTemperatureIntegrationIntervalElapsed, NULL);
}

static void FillingTrayPeriodElapsed(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_TrayFilled, NULL);
}

static void FullIceBucketWaitTimeElapsed(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_FullIceBucketWaitTimeElapsed, NULL);
}

static void IceTemperaturePollingIntervalElapsed(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_IceTemperaturePollingIntervalElapsed, NULL);
}

static void DoorClosedForLongEnough(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   instance->_private.doorHasBeenClosedForLongEnough = true;
   Fsm_SendSignal(&instance->_private.fsm, Signal_DoorClosedForLongEnough, NULL);
}

static void DataSourceChanged(void *context, const void *data)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   REINTERPRET(onChangeArgs, data, const DataSourceOnDataChangeArgs_t *);

   if(onChangeArgs->erd == Erd_TwistTrayIceMaker_IceDispenserState)
   {
      REINTERPRET(dispenserStatus, onChangeArgs->data, const bool *);
      if(*dispenserStatus)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DispenserOn, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DispenserOff, NULL);
      }
   }
   else if(onChangeArgs->erd == Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeRequest)
   {
      TimerTicks_t remainingTime =
         TimerModule_RemainingTicks(instance->_private.timerModule, &instance->_private.minimumFreezeTimer);
      DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMaker_MinimumFreezeTimerRemainingTimeInMsec, &remainingTime);
   }
   else if((onChangeArgs->erd == Erd_SabbathMode) ||
      (onChangeArgs->erd == Erd_EnhancedSabbathMode))
   {
      if(ItIsSabbathMode(instance))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_SabbathModeEnabled, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_SabbathModeDisabled, NULL);
      }
   }
   else if(onChangeArgs->erd == Erd_FreezerDoorIsOpen)
   {
      REINTERPRET(doorIsOpen, onChangeArgs->data, const bool *);

      if(*doorIsOpen)
      {
         instance->_private.doorHasBeenClosedForLongEnough = false;
         TimerModule_Stop(
            instance->_private.timerModule,
            &instance->_private.doorClosedTimer);
      }
      else
      {
         TimerModule_StartOneShot(
            instance->_private.timerModule,
            &instance->_private.doorClosedTimer,
            DelayToHarvestAfterDoorCloses,
            DoorClosedForLongEnough,
            instance);
      }
   }
   else if(onChangeArgs->erd == Erd_TwistTrayIceMaker_ForceHarvest)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_ForcedHarvest, NULL);
      DataSource_Write(instance->_private.dataSource, Erd_TwistTrayIceMaker_ForceHarvest, clear);
   }
   else if(onChangeArgs->erd == Erd_TwistTrayIceMaker_MotorActionResult)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_MotorActionResultChanged, onChangeArgs->data);
   }
}

void TwistTrayIceMaker_Init(
   TwistTrayIceMaker_t *instance,
   TimerModule_t *timerModule,
   I_DataSource_t *dataSource,
   const TwistTrayIceMakerData_t *parametric)
{
   instance->_private.timerModule = timerModule;
   instance->_private.dataSource = dataSource;
   instance->_private.parametric = parametric;
   instance->_private.doorHasBeenClosedForLongEnough = true;

   EventSubscription_Init(&instance->_private.dataSourceChangeEventSubcription, instance, DataSourceChanged);
   Event_Subscribe(dataSource->OnDataChange, &instance->_private.dataSourceChangeEventSubcription);

   UpdateHighLevelState(instance, TwistTrayIceMakerHighLevelState_NormalRun);

   Fsm_Init(&instance->_private.fsm, State_Homing);
}
