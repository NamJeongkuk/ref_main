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
#include "DispensingRequest.h"

#define WaterFillPeriod (instance->_private.parametric->fillData.waterFillTimeSecX10 * 100)
#define MinimumFreezePeriodMinutes (instance->_private.parametric->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN)
#define FullIceBucketWaitPeriodMinutes (instance->_private.parametric->harvestData.fullBucketWaitPeriodMinutes * MSEC_PER_MIN)
#define DelayToHarvestAfterDoorCloses (instance->_private.parametric->harvestData.delayToHarvestAfterDoorClosesSeconds * MSEC_PER_SEC)
#define FreezingPointDegFx100 (instance->_private.parametric->freezeData.startIntegrationTemperatureInDegFx100)
#define TargetFreezeIntegrationSum (instance->_private.parametric->freezeData.targetFreezeIntegrationSum)
#define MaxHarvestTemperature (instance->_private.parametric->freezeData.maximumHarvestTemperatureInDegFx100)

enum
{
   Signal_Entry = Fsm_Entry,
   Signal_Exit = Fsm_Exit,
   Signal_DispenserOn = FSM_USER_SIGNAL_START,
   Signal_IceHasStoppedDispensing,
   Signal_MinimumFreezeTimeReached,
   Signal_IceTemperatureIntegrationIntervalElapsed,
   Signal_MotorActionResultHarvested,
   Signal_MotorActionResultBucketWasFull,
   Signal_MotorActionResultMotorError,
   Signal_MotorActionResultHomed,
   Signal_TrayFilled,
   Signal_FullIceBucketWaitTimeElapsed,
   Signal_SabbathModeEnabled,
   Signal_SabbathModeDisabled,
   Signal_DoorClosedForLongEnough,
   Signal_ForcedHarvest,
   Signal_IceMakerThermistorIsInvalid,
   Signal_IceMakerThermistorIsValid,
   Signal_IceMakerFilteredTemperatureChanged,
   Signal_IceMakerIsEnabled,
   Signal_IceMakerIsDisabled,

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
   ENTRY(State_MotorError)           \
   ENTRY(State_ThermistorFault)      \
   ENTRY(State_IdleFreeze)

FSM_STATE_TABLE(FSM_STATE_EXPAND_AS_FUNCTION_DEFINITION)

#define open enabled
#define closed disabled

static void MinimumFreezeTimeReached(void *context);
static void IceTemperatureIntegrationIntervalElapsed(void *context);
static void FillingTrayPeriodElapsed(void *context);
static void FullIceBucketWaitTimeElapsed(void *context);

static TwistTrayIceMaker_t *InstanceFrom(Fsm_t *fsm)
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

static bool IceMakerIsEnabled(TwistTrayIceMaker_t *instance)
{
   bool iceMakerIsEnabled;
   DataSource_Read(instance->_private.dataSource, Erd_IceMakerEnabledOverrideResolved, &iceMakerIsEnabled);

   return iceMakerIsEnabled;
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

static void SendFreezerIceRateSignal(TwistTrayIceMaker_t *instance)
{
   Signal_SendViaErd(
      instance->_private.dataSource,
      Erd_FreezerIceRateTriggerSignal);
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

static bool IceMakerThermistorIsValid(TwistTrayIceMaker_t *instance)
{
   bool thermistorIsValid;
   DataSource_Read(instance->_private.dataSource, Erd_TwistTrayIceMaker_ThermistorIsValid, &thermistorIsValid);

   return thermistorIsValid;
}

static bool HarvestConditionsHaveBeenMet(TwistTrayIceMaker_t *instance)
{
   return (
      instance->_private.minimumFreezeTimeReached &&
      instance->_private.freezeIntegrationSumReached &&
      instance->_private.maximumHarvestTemperatureReached);
}

static void State_Homing(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_Homing);
         RequestMotorAction(instance, Home);
         break;

      case Signal_MotorActionResultHomed:
         if(!IceMakerThermistorIsValid(instance))
         {
            Fsm_Transition(fsm, State_ThermistorFault);
         }
         else if(IceMakerIsEnabled(instance))
         {
            Fsm_Transition(fsm, State_Freeze);
         }
         else
         {
            Fsm_Transition(fsm, State_IdleFreeze);
         }
         break;

      case Signal_MotorActionResultMotorError:
         Fsm_Transition(fsm, State_MotorError);
         break;

      case Fsm_Exit:
         RequestMotorAction(instance, Idle);
         break;
   }
}

static void State_Freeze(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   TemperatureDegFx100_t iceTrayTempx100;
   uint32_t freezeIntegrationSum;
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_Freeze);

         instance->_private.minimumFreezeTimeReached = false;
         instance->_private.freezeIntegrationSumReached = false;
         instance->_private.maximumHarvestTemperatureReached = false;

         ClearFreezeIntegrationSum(instance);
         StartFreezingTimers(instance);

         if(!instance->_private.firstFreezeTransition)
         {
            SendFreezerIceRateSignal(instance);
         }
         else
         {
            instance->_private.firstFreezeTransition = false;
         }
         break;

      case Signal_IceMakerFilteredTemperatureChanged:
      case Signal_IceMakerThermistorIsInvalid:
         DataSource_Read(instance->_private.dataSource, Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100, &iceTrayTempx100);

         if((iceTrayTempx100 > FreezingPointDegFx100) || !IceMakerThermistorIsValid(instance))
         {
            instance->_private.minimumFreezeTimeReached = false;
            instance->_private.freezeIntegrationSumReached = false;
            instance->_private.maximumHarvestTemperatureReached = false;

            ClearFreezeIntegrationSum(instance);
            StopFreezeTimers(instance);

            if(!IceMakerThermistorIsValid(instance))
            {
               Fsm_Transition(fsm, State_ThermistorFault);
            }
         }
         else
         {
            if(!TimerModule_IsRunning(instance->_private.timerModule, &instance->_private.minimumFreezeTimer) &&
               !TimerModule_IsRunning(instance->_private.timerModule, &instance->_private.integrationTimer))
            {
               StartFreezingTimers(instance);
            }

            instance->_private.maximumHarvestTemperatureReached = (iceTrayTempx100 < MaxHarvestTemperature);

            if(HarvestConditionsHaveBeenMet(instance))
            {
               Fsm_Transition(fsm, State_Harvesting);
            }
         }
         break;

      case Signal_MinimumFreezeTimeReached:
         instance->_private.minimumFreezeTimeReached = true;

         if(HarvestConditionsHaveBeenMet(instance))
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

            if(HarvestConditionsHaveBeenMet(instance))
            {
               Fsm_Transition(fsm, State_Harvesting);
            }
         }
         break;

      case Signal_ForcedHarvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Signal_IceMakerIsDisabled:
         Fsm_Transition(fsm, State_IdleFreeze);
         break;

      case Fsm_Exit:
         StopFreezeTimers(instance);

         instance->_private.minimumFreezeTimeReached = false;
         instance->_private.freezeIntegrationSumReached = false;
         instance->_private.maximumHarvestTemperatureReached = false;
         break;
   }
}

static void State_Harvesting(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

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

      case Signal_MotorActionResultHarvested:
         if(IceMakerIsEnabled(instance))
         {
            Fsm_Transition(fsm, State_FillingTrayWithWater);
         }
         else
         {
            Fsm_Transition(fsm, State_IdleFreeze);
         }
         break;

      case Signal_MotorActionResultBucketWasFull:
         if(IceMakerIsEnabled(instance))
         {
            Fsm_Transition(fsm, State_BucketIsFull);
         }
         else
         {
            Fsm_Transition(fsm, State_IdleFreeze);
         }
         break;

      case Signal_MotorActionResultMotorError:
         Fsm_Transition(fsm, State_MotorError);
         break;

      case Fsm_Exit:
         RequestMotorAction(instance, Idle);
         break;
   }
}

static void State_FillingTrayWithWater(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
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
         if(!IceMakerThermistorIsValid(instance))
         {
            Fsm_Transition(fsm, State_ThermistorFault);
         }
         else if(IceMakerIsEnabled(instance))
         {
            Fsm_Transition(fsm, State_Freeze);
         }
         else
         {
            Fsm_Transition(fsm, State_IdleFreeze);
         }
         break;

      case Fsm_Exit:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.waitingTimer);
         UpdateWaterValve(instance, CLOSED);
         break;
   }
}

static void State_BucketIsFull(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
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

      case Signal_IceHasStoppedDispensing:
         if(instance->_private.iceDispensedLongEnoughToCheckHarvest)
         {
            instance->_private.iceDispensedLongEnoughToCheckHarvest = false;
            Fsm_Transition(fsm, State_Harvesting);
         }
         break;

      case Signal_FullIceBucketWaitTimeElapsed:
      case Signal_ForcedHarvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Signal_IceMakerThermistorIsInvalid:
         Fsm_Transition(fsm, State_ThermistorFault);
         break;

      case Signal_IceMakerIsDisabled:
         Fsm_Transition(fsm, State_IdleFreeze);
         break;

      case Fsm_Exit:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.waitingTimer);
         break;
   }
}

static void State_MotorError(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
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

static void State_ThermistorFault(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_ThermistorFault);
         break;

      case Signal_IceMakerThermistorIsValid:
         Fsm_Transition(fsm, State_Homing);
         break;
   }
}

static void State_IdleFreeze(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateOperationState(instance, TwistTrayIceMakerOperationState_IdleFreeze);
         break;

      case Signal_IceMakerIsEnabled:
         Fsm_Transition(fsm, State_Freeze);
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

static void DoorClosedForLongEnough(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   instance->_private.doorHasBeenClosedForLongEnough = true;
   Fsm_SendSignal(&instance->_private.fsm, Signal_DoorClosedForLongEnough, NULL);
}

static bool IceIsDispensing(DispensingRequestStatus_t dispensingRequestStatus)
{
   return (dispensingRequestStatus.status == DispenseStatus_Dispensing &&
      (dispensingRequestStatus.selection == DispensingRequestSelection_CrushedIce ||
         dispensingRequestStatus.selection == DispensingRequestSelection_CubedIce));
}

static void IceDispensedLongEnoughToCheckForHarvestAfterDispensingHasEnded(void *context)
{
   TwistTrayIceMaker_t *instance = context;
   instance->_private.iceDispensedLongEnoughToCheckHarvest = true;
}

static void DataSourceChanged(void *context, const void *data)
{
   TwistTrayIceMaker_t *instance = context;
   const DataSourceOnDataChangeArgs_t *onChangeArgs = data;

   if(onChangeArgs->erd == Erd_DispensingRequestStatus)
   {
      const DispensingRequestStatus_t *dispensingRequestStatus = onChangeArgs->data;
      if(IceIsDispensing(*dispensingRequestStatus))
      {
         TimerModule_StartOneShot(
            instance->_private.timerModule,
            &instance->_private.dispensingIceTimer,
            instance->_private.parametric->harvestData.fullBucketDispenseCheckTimeInSeconds * MSEC_PER_SEC,
            IceDispensedLongEnoughToCheckForHarvestAfterDispensingHasEnded,
            instance);
      }
      else
      {
         TimerModule_Stop(
            instance->_private.timerModule,
            &instance->_private.dispensingIceTimer);
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceHasStoppedDispensing, NULL);
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
   else if(onChangeArgs->erd == Erd_TwistTrayIceMaker_ThermistorIsValid)
   {
      const bool *state = onChangeArgs->data;
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerThermistorIsValid, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerThermistorIsInvalid, NULL);
      }
   }
   else if(onChangeArgs->erd == Erd_TwistTrayIceMaker_MotorActionResult)
   {
      const TwistTrayIceMakerMotorActionResult_t *motorActionResult = onChangeArgs->data;

      switch(*motorActionResult)
      {
         case Harvested:
            Fsm_SendSignal(&instance->_private.fsm, Signal_MotorActionResultHarvested, NULL);
            break;

         case BucketWasFull:
            Fsm_SendSignal(&instance->_private.fsm, Signal_MotorActionResultBucketWasFull, NULL);
            break;

         case MotorError:
            Fsm_SendSignal(&instance->_private.fsm, Signal_MotorActionResultMotorError, NULL);
            break;

         case Homed:
            Fsm_SendSignal(&instance->_private.fsm, Signal_MotorActionResultHomed, NULL);
            break;
      }
   }
   else if(onChangeArgs->erd == Erd_TwistTrayIceMaker_FilteredTemperatureInDegFx100)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerFilteredTemperatureChanged, NULL);
   }
   else if(onChangeArgs->erd == Erd_IceMakerEnabledOverrideResolved)
   {
      const bool *state = onChangeArgs->data;
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerIsEnabled, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerIsDisabled, NULL);
      }
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
   instance->_private.firstFreezeTransition = true;
   instance->_private.iceDispensedLongEnoughToCheckHarvest = false;

   EventSubscription_Init(&instance->_private.dataSourceChangeEventSubscription, instance, DataSourceChanged);
   Event_Subscribe(dataSource->OnDataChange, &instance->_private.dataSourceChangeEventSubscription);

   UpdateHighLevelState(instance, TwistTrayIceMakerHighLevelState_NormalRun);

   IceMakerThermistorIsValid(instance) ? Fsm_Init(&instance->_private.fsm, State_Homing) : Fsm_Init(&instance->_private.fsm, State_ThermistorFault);
}
