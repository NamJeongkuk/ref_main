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
#include "IceMakerStateMachineState.h"
#include "WaterValveVotedState.h"
#include "DispensingRequest.h"
#include "PercentageDutyCycleVote.h"

#define FullIceBucketWaitPeriodMsec (instance->_private.parametric->harvestData.fullBucketWaitPeriodMinutes * MSEC_PER_MIN)
#define DelayToHarvestAfterDoorOpens (instance->_private.parametric->harvestData.delayToHarvestAfterDoorOpensMinutes * MSEC_PER_MIN)
#define MaxHarvestTemperatureInDegFx100 (instance->_private.parametric->freezeData.maximumHarvestTemperatureInDegFx100)
#define RetryMotorInitializeMsec ((instance->_private.parametric->harvestData.motorErrorRetryInitializeMinutes) * MSEC_PER_MIN)
#define FullIceBucketDoorOpenCheckPeriodMsec ((instance->_private.parametric->harvestData.fullBucketDoorOpenCheckTimeInMinutes) * MSEC_PER_MIN)

enum
{
   Signal_Entry = Fsm_Entry,
   Signal_Exit = Fsm_Exit,
   Signal_DispenserOn = FSM_USER_SIGNAL_START,
   Signal_IceHasStoppedDispensing,
   Signal_MotorActionResultHarvested,
   Signal_MotorActionResultBucketWasFull,
   Signal_MotorActionResultMotorError,
   Signal_MotorActionResultHomed,
   Signal_TrayFilled,
   Signal_FullIceBucketWaitTimeElapsed,
   Signal_SabbathModeEnabled,
   Signal_SabbathModeDisabled,
   Signal_HarvestDoorDelayElapsed,
   Signal_TestRequest_Fill,
   Signal_TestRequest_Harvest,
   Signal_IceMakerThermistorIsInvalid,
   Signal_IceMakerThermistorIsValid,
   Signal_IceMakerFilteredTemperatureChanged,
   Signal_FillTubeHeaterTimerExpired,
   Signal_IceMakerIsEnabled,
   Signal_IceMakerIsDisabled,
   Signal_DispensingActive,
   Signal_DispensingInactive,
   Signal_MotorErrorRetryTimerExpired,
   Signal_HarvestCountIsReadyToHarvest,
   Signal_CoolingSystemIsTurnedOff,
   Signal_CoolingSystemIsTurnedOn,
   Signal_IceRateNoLongerActive,
   Signal_FreezerDoorOpened,
   Signal_FreezerDoorClosed,
   Signal_DispensingIsNotInhibitedByRfid,
   Signal_IceMakerFillIsInhibited,
   Signal_IceMakerFillIsNotInhibited,

   Idle = IceMakerMotorState_Off,
   Home = IceMakerMotorState_Homing,
   Harvest = IceMakerMotorState_Run,

   Homed = IceMakerMotorActionResult_Homed,
   BucketWasFull = IceMakerMotorActionResult_BucketWasFull,
   Harvested = IceMakerMotorActionResult_Harvested,
   MotorError = IceMakerMotorActionResult_MotorError,

   OPEN = true,
   CLOSED = false,
   DOOR_OPEN = true,
   DOOR_CLOSED = false
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
   ENTRY(State_IdleFill)

FSM_STATE_TABLE(FSM_STATE_EXPAND_AS_FUNCTION_DEFINITION)

#define open enabled
#define closed disabled

static void FullIceBucketWaitTimeElapsed(void *context);
static void HarvestDoorDelayElapsed(void *context);

static TwistTrayIceMaker_t *InstanceFrom(Fsm_t *fsm)
{
   return CONTAINER_OF(TwistTrayIceMaker_t, _private.fsm, fsm);
}

static void UpdateFsmState(TwistTrayIceMaker_t *instance, IceMakerStateMachineState_t newState)
{
   DataSource_Write(instance->_private.dataSource, instance->_private.config->fsmStateErd, &newState);
}

static void RequestMotorState(TwistTrayIceMaker_t *instance, IceMakerMotorState_t newMotorState)
{
   IceMakerMotorVotedState_t motorVote;
   motorVote.state = newMotorState;
   motorVote.care = (newMotorState == Idle) ? Vote_DontCare : Vote_Care;

   DataSource_Write(instance->_private.dataSource, instance->_private.config->motorIceMakerVoteErd, &motorVote);
}

static bool ItIsSabbathMode(TwistTrayIceMaker_t *instance)
{
   bool sabbathMode;
   bool enhancedSabbathMode;

   DataSource_Read(instance->_private.dataSource, instance->_private.config->sabbathModeErd, &sabbathMode);
   DataSource_Read(instance->_private.dataSource, instance->_private.config->enhancedSabbathModeErd, &enhancedSabbathMode);

   return (sabbathMode || enhancedSabbathMode);
}

static bool IceMakerIsEnabled(TwistTrayIceMaker_t *instance)
{
   bool iceMakerIsEnabled;
   DataSource_Read(instance->_private.dataSource, instance->_private.config->iceMakerEnabledResolvedErd, &iceMakerIsEnabled);

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

   DataSource_Write(instance->_private.dataSource, instance->_private.config->waterValveIceMakerVoteErd, &vote);
   DataSource_Write(instance->_private.dataSource, instance->_private.config->isolationWaterValveVoteErd, &vote);
}

static void SendFreezerIceRateSignal(TwistTrayIceMaker_t *instance)
{
   Signal_SendViaErd(
      instance->_private.dataSource,
      instance->_private.config->freezerIceRateTriggerSignalErd);
}

static void SetWaterFillMonitoringRequestTo(TwistTrayIceMaker_t *instance, IceMakerWaterFillMonitoringRequest_t request)
{
   DataSource_Write(
      instance->_private.dataSource,
      instance->_private.config->waterFillMonitoringRequestErd,
      &request);
}

static void VoteForFillTubeHeater(TwistTrayIceMaker_t *instance, PercentageDutyCycle_t dutyCycle)
{
   PercentageDutyCycleVote_t vote = {
      .percentageDutyCycle = dutyCycle,
      .care = Vote_Care
   };

   DataSource_Write(instance->_private.dataSource, instance->_private.config->fillTubeHeaterVoteErd, &vote);
}

static void VoteForFillTubeHeaterOffAndDontCare(TwistTrayIceMaker_t *instance)
{
   PercentageDutyCycleVote_t vote = {
      .percentageDutyCycle = OFF,
      .care = Vote_DontCare
   };

   DataSource_Write(instance->_private.dataSource, instance->_private.config->fillTubeHeaterVoteErd, &vote);
}

static void FillTubeHeaterTimerExpired(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_FillTubeHeaterTimerExpired, NULL);
}

static void StartFillTubeHeaterTimer(TwistTrayIceMaker_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.fillTubeHeaterTimer,
      instance->_private.parametric->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC,
      FillTubeHeaterTimerExpired,
      instance);
}

static bool FillTubeHeaterTimerHasExpired(TwistTrayIceMaker_t *instance)
{
   return !TimerModule_IsRunning(instance->_private.timerModule, &instance->_private.fillTubeHeaterTimer);
}

static void StopFillTubeHeaterTimer(TwistTrayIceMaker_t *instance)
{
   TimerModule_Stop(instance->_private.timerModule, &instance->_private.fillTubeHeaterTimer);
}

static bool HarvestDoorDelayHasElapsed(TwistTrayIceMaker_t *instance)
{
   return instance->_private.doorHarvestDelayHasElapsed;
}

static bool IceMakerThermistorIsValid(TwistTrayIceMaker_t *instance)
{
   bool thermistorIsValid;
   DataSource_Read(instance->_private.dataSource, instance->_private.config->thermistorIsValidResolvedErd, &thermistorIsValid);

   return thermistorIsValid;
}

static bool HarvestConditionsHaveBeenMet(TwistTrayIceMaker_t *instance)
{
   TemperatureDegFx100_t iceTrayTempx100;
   DataSource_Read(
      instance->_private.dataSource,
      instance->_private.config->filteredTemperatureResolvedInDegFx100Erd,
      &iceTrayTempx100);

   bool harvestCountIsReadyToHarvest;
   DataSource_Read(
      instance->_private.dataSource,
      instance->_private.config->harvestCountIsReadyToHarvestErd,
      &harvestCountIsReadyToHarvest);

   DispensingInhibitedReasonBitmap_t dispensingInhibitedBitmap;
   DataSource_Read(
      instance->_private.dataSource,
      instance->_private.config->dispensingInhibitedReasonErd,
      &dispensingInhibitedBitmap);

   return (iceTrayTempx100 < MaxHarvestTemperatureInDegFx100) &&
      harvestCountIsReadyToHarvest &&
      HarvestDoorDelayHasElapsed(instance) &&
      !BITMAP_STATE(dispensingInhibitedBitmap.bitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
}

static void RequestHarvestCountCalculation(TwistTrayIceMaker_t *instance)
{
   DataSource_Write(
      instance->_private.dataSource,
      instance->_private.config->harvestCountCalculationRequestErd,
      set);
}

static void StopHarvestCountCalculation(TwistTrayIceMaker_t *instance)
{
   DataSource_Write(
      instance->_private.dataSource,
      instance->_private.config->harvestCountCalculationRequestErd,
      clear);
}

static bool CoolingSystemIsOff(TwistTrayIceMaker_t *instance)
{
   bool coolingSystemIsOff;
   DataSource_Read(
      instance->_private.dataSource,
      instance->_private.config->coolingOffStatusErd,
      &coolingSystemIsOff);

   return coolingSystemIsOff;
}

static bool SabbathModeIsDisabledAndIceMakerIsEnabledAndCoolingSystemIsOn(TwistTrayIceMaker_t *instance)
{
   return (!ItIsSabbathMode(instance) && IceMakerIsEnabled(instance) && !CoolingSystemIsOff(instance));
}

static bool FreezerIceRateIsActive(TwistTrayIceMaker_t *instance)
{
   bool iceRateIsActive;
   DataSource_Read(
      instance->_private.dataSource,
      instance->_private.config->freezerIceRateIsActiveErd,
      &iceRateIsActive);

   return iceRateIsActive;
}

static void DoorOpenCheckTimeElapsed(void *context)
{
   TwistTrayIceMaker_t *instance = context;
   instance->_private.doorOpenCheckTimeElapsed = true;
}

static bool ThermistorTemperatureIsGreaterThanFullToFreezeThreshold(TwistTrayIceMaker_t *instance)
{
   int16_t thermistorTemperatureInDegFx100;
   DataSource_Read(
      instance->_private.dataSource,
      instance->_private.config->filteredTemperatureResolvedInDegFx100Erd,
      &thermistorTemperatureInDegFx100);

   return (thermistorTemperatureInDegFx100 > instance->_private.parametric->harvestData.fullBucketToFreezeStateTemperatureInDegFx100);
}

static bool IceMakerFillBitmapIsSet(IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReasonBitmap)
{
   for(uint8_t i = 0; i < IceMakerFillInhibitedReasonBitmap_NumberOfBytes; i++)
   {
      if(!!(iceMakerFillInhibitedReasonBitmap.bitmap[i]))
      {
         return true;
      }
   }
   return false;
}

static bool IceMakerFillIsInhibited(TwistTrayIceMaker_t *instance)
{
   IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReasonBitmap;
   DataSource_Read(
      instance->_private.dataSource,
      instance->_private.config->iceMakerFillInhibitedReasonErd,
      &iceMakerFillInhibitedReasonBitmap);

   return IceMakerFillBitmapIsSet(iceMakerFillInhibitedReasonBitmap);
}

static void UpdateIceMakerFullStatus(TwistTrayIceMaker_t *instance, bool fullStatus)
{
   DataSource_Write(
      instance->_private.dataSource,
      instance->_private.config->iceMakerFullStatusErd,
      &fullStatus);
}

static void State_Homing(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateFsmState(instance, IceMakerStateMachineState_Homing);
         RequestMotorState(instance, Home);
         break;

      case Signal_MotorActionResultHomed:
         if(!IceMakerThermistorIsValid(instance))
         {
            Fsm_Transition(fsm, State_ThermistorFault);
         }
         else
         {
            Fsm_Transition(fsm, State_Freeze);
         }
         break;

      case Signal_MotorActionResultMotorError:
         Fsm_Transition(fsm, State_MotorError);
         break;

      case Signal_DispensingActive:
         instance->_private.delayFillMonitoring = true;
         break;

      case Signal_DispensingInactive:
         instance->_private.delayFillMonitoring = false;
         break;

      case Fsm_Exit:
         RequestMotorState(instance, Idle);
         break;
   }
}

static void State_Freeze(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateFsmState(instance, IceMakerStateMachineState_Freeze);
         RequestHarvestCountCalculation(instance);

         if((!instance->_private.firstFreezeTransition) &&
            SabbathModeIsDisabledAndIceMakerIsEnabledAndCoolingSystemIsOn(instance))
         {
            SendFreezerIceRateSignal(instance);
         }
         break;

      case Signal_FreezerDoorOpened:
         instance->_private.doorHarvestDelayHasElapsed = false;
         TimerModule_StartOneShot(
            instance->_private.timerModule,
            &instance->_private.doorHarvestDelayTimer,
            DelayToHarvestAfterDoorOpens,
            HarvestDoorDelayElapsed,
            instance);
         break;

      case Signal_IceRateNoLongerActive:
      case Signal_SabbathModeDisabled:
      case Signal_IceMakerIsEnabled:
      case Signal_CoolingSystemIsTurnedOn:
      case Signal_IceMakerFilteredTemperatureChanged:
      case Signal_HarvestCountIsReadyToHarvest:
      case Signal_HarvestDoorDelayElapsed:
      case Signal_DispensingIsNotInhibitedByRfid:
         if(SabbathModeIsDisabledAndIceMakerIsEnabledAndCoolingSystemIsOn(instance))
         {
            if(HarvestConditionsHaveBeenMet(instance))
            {
               Fsm_Transition(fsm, State_Harvesting);
            }
            else if(!FreezerIceRateIsActive(instance) &&
               !instance->_private.firstFreezeTransition)
            {
               SendFreezerIceRateSignal(instance);
            }
         }
         break;

      case Signal_IceMakerThermistorIsInvalid:
         Fsm_Transition(fsm, State_ThermistorFault);
         break;

      case Signal_TestRequest_Fill:
         Fsm_Transition(fsm, State_FillingTrayWithWater);
         break;

      case Signal_TestRequest_Harvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Signal_DispensingActive:
         instance->_private.delayFillMonitoring = true;
         break;

      case Signal_DispensingInactive:
         instance->_private.delayFillMonitoring = false;
         break;

      case Fsm_Exit:
         instance->_private.firstFreezeTransition = false;
         StopHarvestCountCalculation(instance);
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.doorHarvestDelayTimer);
         instance->_private.doorHarvestDelayHasElapsed = true;
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
         UpdateFsmState(instance, IceMakerStateMachineState_Harvest);
         RequestMotorState(instance, Harvest);
         VoteForFillTubeHeater(instance, instance->_private.parametric->harvestData.fillTubeHeaterDutyCyclePercentage);
         StartFillTubeHeaterTimer(instance);
         break;

      case Signal_FillTubeHeaterTimerExpired:
         VoteForFillTubeHeaterOffAndDontCare(instance);

         if(instance->_private.motorHarvested)
         {
            if(!IceMakerThermistorIsValid(instance))
            {
               Fsm_Transition(fsm, State_ThermistorFault);
            }
            else
            {
               Fsm_Transition(fsm, State_FillingTrayWithWater);
            }
         }
         break;

      case Signal_MotorActionResultHarvested:
         instance->_private.motorHarvested = true;
         RequestMotorState(instance, Idle);
         UpdateIceMakerFullStatus(instance, CLEAR);
         if(FillTubeHeaterTimerHasExpired(instance))
         {
            if(!IceMakerThermistorIsValid(instance))
            {
               Fsm_Transition(fsm, State_ThermistorFault);
            }
            else
            {
               Fsm_Transition(fsm, State_FillingTrayWithWater);
            }
         }
         break;

      case Signal_MotorActionResultBucketWasFull:
         if(!IceMakerThermistorIsValid(instance))
         {
            Fsm_Transition(fsm, State_ThermistorFault);
         }
         else if(IceMakerIsEnabled(instance) && !CoolingSystemIsOff(instance))
         {
            Fsm_Transition(fsm, State_BucketIsFull);
         }
         else
         {
            Fsm_Transition(fsm, State_Freeze);
         }
         break;

      case Signal_MotorActionResultMotorError:
         if(!IceMakerThermistorIsValid(instance))
         {
            Fsm_Transition(fsm, State_ThermistorFault);
         }
         else
         {
            Fsm_Transition(fsm, State_MotorError);
         }
         break;

      case Signal_DispensingActive:
         instance->_private.delayFillMonitoring = true;
         break;

      case Signal_DispensingInactive:
         instance->_private.delayFillMonitoring = false;
         break;

      case Signal_TestRequest_Fill:
         if(instance->_private.motorHarvested)
         {
            Fsm_Transition(fsm, State_FillingTrayWithWater);
         }
         break;

      case Fsm_Exit:
         VoteForFillTubeHeaterOffAndDontCare(instance);
         StopFillTubeHeaterTimer(instance);
         instance->_private.motorHarvested = false;
         RequestMotorState(instance, Idle);
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
         UpdateFsmState(instance, IceMakerStateMachineState_Fill);

         if(instance->_private.pauseFillMonitoring)
         {
            SetWaterFillMonitoringRequestTo(instance, IceMakerWaterFillMonitoringRequest_Resume);
            instance->_private.pauseFillMonitoring = false;
         }
         else if(instance->_private.delayFillMonitoring || IceMakerFillIsInhibited(instance))
         {
            instance->_private.delayFillMonitoring = false;
            Fsm_Transition(&instance->_private.fsm, State_IdleFill);
         }
         else
         {
            UpdateWaterValve(instance, OPEN);
            SetWaterFillMonitoringRequestTo(instance, IceMakerWaterFillMonitoringRequest_Start);
         }
         break;

      case Signal_TrayFilled:
         if(!IceMakerThermistorIsValid(instance))
         {
            Fsm_Transition(fsm, State_ThermistorFault);
         }
         else
         {
            Fsm_Transition(fsm, State_Freeze);
         }
         break;

      case Signal_DispensingActive:
      case Signal_IceMakerFillIsInhibited:
         instance->_private.pauseFillMonitoring = true;
         Fsm_Transition(fsm, State_IdleFill);
         break;

      case Signal_TestRequest_Harvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Fsm_Exit:
         if(instance->_private.pauseFillMonitoring)
         {
            SetWaterFillMonitoringRequestTo(instance, IceMakerWaterFillMonitoringRequest_Pause);
         }
         else
         {
            SetWaterFillMonitoringRequestTo(instance, IceMakerWaterFillMonitoringRequest_Stop);
         }
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
         UpdateFsmState(instance, IceMakerStateMachineState_BucketIsFull);
         UpdateIceMakerFullStatus(instance, SET);

         TimerModule_StartOneShot(
            instance->_private.timerModule,
            &instance->_private.waitingTimer,
            FullIceBucketWaitPeriodMsec,
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

      case Signal_FreezerDoorOpened:
         TimerModule_StartOneShot(
            instance->_private.timerModule,
            &instance->_private.doorOpenCheckTimer,
            FullIceBucketDoorOpenCheckPeriodMsec,
            DoorOpenCheckTimeElapsed,
            instance);
         break;

      case Signal_FreezerDoorClosed:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.doorOpenCheckTimer);
         if(instance->_private.doorOpenCheckTimeElapsed)
         {
            Fsm_Transition(fsm, State_Harvesting);
         }
         break;

      case Signal_FullIceBucketWaitTimeElapsed:
      case Signal_TestRequest_Harvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Signal_IceMakerFilteredTemperatureChanged:
         if(ThermistorTemperatureIsGreaterThanFullToFreezeThreshold(instance))
         {
            Fsm_Transition(fsm, State_Freeze);
         }
         break;

      case Signal_IceMakerThermistorIsInvalid:
         Fsm_Transition(fsm, State_ThermistorFault);
         break;

      case Signal_TestRequest_Fill:
         Fsm_Transition(fsm, State_FillingTrayWithWater);
         break;

      case Signal_SabbathModeEnabled:
      case Signal_CoolingSystemIsTurnedOff:
      case Signal_IceMakerIsDisabled:
         Fsm_Transition(fsm, State_Freeze);
         break;

      case Signal_DispensingActive:
         instance->_private.delayFillMonitoring = true;
         break;

      case Signal_DispensingInactive:
         instance->_private.delayFillMonitoring = false;
         break;

      case Fsm_Exit:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.waitingTimer);
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.doorOpenCheckTimer);
         instance->_private.doorOpenCheckTimeElapsed = false;
         break;
   }
}

static void MotorErrorRetryTimerExpired(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_MotorErrorRetryTimerExpired, NULL);
}

static void State_MotorError(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         TimerModule_StartOneShot(
            instance->_private.timerModule,
            &instance->_private.retryMotorInitTimer,
            RetryMotorInitializeMsec,
            MotorErrorRetryTimerExpired,
            instance);

         UpdateFsmState(instance, IceMakerStateMachineState_MotorError);

         RequestMotorState(instance, Idle);
         DataSource_Write(instance->_private.dataSource, instance->_private.config->motorFaultActiveErd, set);
         break;

      case Signal_TestRequest_Fill:
         Fsm_Transition(fsm, State_FillingTrayWithWater);
         break;

      case Signal_TestRequest_Harvest:
         Fsm_Transition(fsm, State_Harvesting);
         break;

      case Signal_MotorErrorRetryTimerExpired:
         Fsm_Transition(&instance->_private.fsm, State_Homing);
         break;

      case Signal_DispensingActive:
         instance->_private.delayFillMonitoring = true;
         break;

      case Signal_DispensingInactive:
         instance->_private.delayFillMonitoring = false;
         break;

      case Fsm_Exit:
         TimerModule_Stop(instance->_private.timerModule, &instance->_private.retryMotorInitTimer);
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
         UpdateFsmState(instance, IceMakerStateMachineState_ThermistorFault);
         break;

      case Signal_IceMakerThermistorIsValid:
         Fsm_Transition(fsm, State_Homing);
         break;

      case Signal_DispensingActive:
         instance->_private.delayFillMonitoring = true;
         break;

      case Signal_DispensingInactive:
         instance->_private.delayFillMonitoring = false;
         break;
   }
}

static void State_IdleFill(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   TwistTrayIceMaker_t *instance = InstanceFrom(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         UpdateFsmState(instance, IceMakerStateMachineState_IdleFill);
         break;

      case Signal_DispensingInactive:
      case Signal_IceMakerFillIsNotInhibited:
         Fsm_Transition(fsm, State_FillingTrayWithWater);
         break;
   }
}

static void FullIceBucketWaitTimeElapsed(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_FullIceBucketWaitTimeElapsed, NULL);
}

static void HarvestDoorDelayElapsed(void *context)
{
   REINTERPRET(instance, context, TwistTrayIceMaker_t *);
   instance->_private.doorHarvestDelayHasElapsed = true;
   Fsm_SendSignal(&instance->_private.fsm, Signal_HarvestDoorDelayElapsed, NULL);
}

static bool IceIsSelected(DispensingRequestStatus_t dispensingRequestStatus)
{
   return (dispensingRequestStatus.selection == DispensingRequestSelection_CrushedIce ||
      dispensingRequestStatus.selection == DispensingRequestSelection_CubedIce);
}

static bool WaterOrAutofillIsSelected(DispensingRequestStatus_t dispensingRequestStatus)
{
   return (dispensingRequestStatus.selection == DispensingRequestSelection_Water ||
      dispensingRequestStatus.selection == DispensingRequestSelection_Autofill);
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

   if(onChangeArgs->erd == instance->_private.config->dispensingRequestStatusErd)
   {
      const DispensingRequestStatus_t *dispensingRequestStatus = onChangeArgs->data;

      if(WaterOrAutofillIsSelected(*dispensingRequestStatus))
      {
         if(dispensingRequestStatus->status == DispenseStatus_Dispensing)
         {
            Fsm_SendSignal(&instance->_private.fsm, Signal_DispensingActive, NULL);
         }
         else
         {
            Fsm_SendSignal(&instance->_private.fsm, Signal_DispensingInactive, NULL);
         }
      }

      if(IceIsSelected(*dispensingRequestStatus))
      {
         if(dispensingRequestStatus->status == DispenseStatus_Dispensing)
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
   }
   else if((onChangeArgs->erd == instance->_private.config->sabbathModeErd) ||
      (onChangeArgs->erd == instance->_private.config->enhancedSabbathModeErd))
   {
      if(!ItIsSabbathMode(instance))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_SabbathModeDisabled, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_SabbathModeEnabled, NULL);
      }
   }
   else if(onChangeArgs->erd == instance->_private.config->leftSideFreezerDoorStatusResolvedErd)
   {
      const bool *doorIsOpen = onChangeArgs->data;
      if(*doorIsOpen)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_FreezerDoorOpened, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_FreezerDoorClosed, NULL);
      }
   }
   else if(onChangeArgs->erd == instance->_private.config->testRequestErd)
   {
      const IceMakerTestRequest_t *testRequest = onChangeArgs->data;

      if(*testRequest != IceMakerTestRequest_None)
      {
         switch(*testRequest)
         {
            case IceMakerTestRequest_Fill:
               Fsm_SendSignal(&instance->_private.fsm, Signal_TestRequest_Fill, NULL);
               break;

            case IceMakerTestRequest_Harvest:
               Fsm_SendSignal(&instance->_private.fsm, Signal_TestRequest_Harvest, NULL);
               break;
         }

         IceMakerTestRequest_t testRequestValue = IceMakerTestRequest_None;
         DataSource_Write(
            instance->_private.dataSource,
            instance->_private.config->testRequestErd,
            &testRequestValue);
      }
   }
   else if(onChangeArgs->erd == instance->_private.config->thermistorIsValidResolvedErd)
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
   else if(onChangeArgs->erd == instance->_private.config->motorActionResultErd)
   {
      const IceMakerMotorActionResult_t *motorActionResult = onChangeArgs->data;

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
   else if(onChangeArgs->erd == instance->_private.config->filteredTemperatureResolvedInDegFx100Erd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerFilteredTemperatureChanged, NULL);
   }
   else if(onChangeArgs->erd == instance->_private.config->iceMakerEnabledResolvedErd)
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
   else if(onChangeArgs->erd == instance->_private.config->stopFillSignalErd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_TrayFilled, NULL);
   }
   else if(onChangeArgs->erd == instance->_private.config->harvestCountIsReadyToHarvestErd)
   {
      const bool *state = onChangeArgs->data;
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_HarvestCountIsReadyToHarvest, NULL);
      }
   }
   else if(onChangeArgs->erd == instance->_private.config->coolingOffStatusErd)
   {
      const bool *state = onChangeArgs->data;
      if(*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_CoolingSystemIsTurnedOff, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_CoolingSystemIsTurnedOn, NULL);
      }
   }
   else if(onChangeArgs->erd == instance->_private.config->freezerIceRateIsActiveErd)
   {
      const bool *state = onChangeArgs->data;
      if(!*state)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceRateNoLongerActive, NULL);
      }
   }
   else if(onChangeArgs->erd == instance->_private.config->dispensingInhibitedReasonErd)
   {
      const DispensingInhibitedReasonBitmap_t *dispensingInhibitedBitmap = onChangeArgs->data;

      if(!BITMAP_STATE(dispensingInhibitedBitmap->bitmap, DispensingInhibitedReason_WaterDueToRfidFilter))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_DispensingIsNotInhibitedByRfid, NULL);
      }
   }
   else if(onChangeArgs->erd == instance->_private.config->iceMakerFillInhibitedReasonErd)
   {
      const IceMakerFillInhibitedReasonBitmap_t *iceMakerFillInhibitedReasonBitmap = onChangeArgs->data;

      if(IceMakerFillBitmapIsSet(*iceMakerFillInhibitedReasonBitmap))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerFillIsInhibited, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerFillIsNotInhibited, NULL);
      }
   }
}

static FsmState_t InitialState(TwistTrayIceMaker_t *instance)
{
   if(IceMakerThermistorIsValid(instance))
   {
      return State_Homing;
   }
   else
   {
      return State_ThermistorFault;
   }
}

void TwistTrayIceMaker_Init(
   TwistTrayIceMaker_t *instance,
   TimerModule_t *timerModule,
   I_DataSource_t *dataSource,
   const TwistTrayIceMakerConfiguration_t *config,
   const TwistTrayIceMakerData_t *parametric)
{
   instance->_private.timerModule = timerModule;
   instance->_private.dataSource = dataSource;
   instance->_private.config = config;
   instance->_private.parametric = parametric;
   instance->_private.doorHarvestDelayHasElapsed = true;
   instance->_private.firstFreezeTransition = true;
   instance->_private.iceDispensedLongEnoughToCheckHarvest = false;
   instance->_private.pauseFillMonitoring = false;
   instance->_private.delayFillMonitoring = false;
   instance->_private.doorOpenCheckTimeElapsed = false;
   instance->_private.motorHarvested = false;

   EventSubscription_Init(
      &instance->_private.dataSourceChangeEventSubscription,
      instance,
      DataSourceChanged);
   Event_Subscribe(
      dataSource->OnDataChange,
      &instance->_private.dataSourceChangeEventSubscription);

   Fsm_Init(&instance->_private.fsm, InitialState(instance));
}
