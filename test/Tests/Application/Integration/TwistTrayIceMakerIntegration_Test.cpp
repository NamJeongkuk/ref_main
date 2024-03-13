/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "TwistTrayIceMakerData.h"
#include "DataModelErdPointerAccess.h"
#include "IceMakerStateMachineState.h"
#include "TwistTrayIceMakerMotorState.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataSource_Gpio.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "IceMakerData.h"
#include "StepperPositionRequest.h"
#include "EventQueueInterruptSafePlugin.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "GpioGroup_TestDouble.h"
#include "Interrupt_TestDouble.h"
#include "TwistTrayMotorOutput_TestDouble.h"
#include "uassert_test.h"

enum
{
   MotorControllerPollingTimeInMsec = 150,
   MotorBrakingDurationInMsec = 1 * MSEC_PER_SEC,
   BelowFreezingAdcCounts = 44800,
   InvalidAdcCount = 5375,
   ValidAdcCount = 30336,
   RelayDelay = 2 * MSEC_PER_SEC,
   Invalid = false,
   Valid = true,
   Harvested = IceMakerMotorActionResult_Harvested
};

TEST_GROUP(TwistTrayIceMakerIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   const TwistTrayIceMakerData_t *twistTrayIceMakerData;
   const IceMakerFillMonitorData_t *twistTrayIceMakerFillMonitorData;
   const SensorData_t *sensorData;
   const IceMakerData_t *iceMakerData;
   GpioGroup_TestDouble_t *gpioGroupTestDouble;
   Interrupt_TestDouble_t *interruptTestDouble;
   Interrupt_TestDouble_t *fastInterruptTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   TwistTrayMotorOutput_TestDouble_t *twistTrayMotorOutputTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      timerModuleTestDouble = (TimerModule_TestDouble_t *)DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);
      gpioGroupTestDouble = (GpioGroup_TestDouble_t *)DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroupInterface);
      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt);
      fastInterruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_FastTickInterrupt);
      twistTrayMotorOutputTestDouble = (TwistTrayMotorOutput_TestDouble_t *)DataModelErdPointerAccess_GetOutput(dataModel, Erd_IceMaker0_TwistTrayMotorOutput);

      twistTrayIceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot0Data->twistTrayData;
      twistTrayIceMakerFillMonitorData = twistTrayIceMakerData->fillData.iceMakerFillMonitorData;
      sensorData = PersonalityParametricData_Get(dataModel)->sensorData;
      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData;
   }

   void GivenApplicationHasBeenInitialized(void)
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);

      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, set);
   };

   void GivenTheIceMakerThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_MoldThermistor_AdcCount, &count);
   }

   void WhenTheIceMakerThermistorAdcCountIs(AdcCounts_t count)
   {
      GivenTheIceMakerThermistorAdcCountIs(count);
   }

   void GivenTheIceMakerIsEnabledAndNotInSabbath()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_EnableStatus, set);
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, set);

      GivenSabbathModeIs(DISABLED);
   }

   void GivenTheIceMakerIsDisabled()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_EnableStatus, clear);
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, clear);
   }

   void WhenTheIceMakerBecomesDisabled()
   {
      GivenTheIceMakerIsDisabled();
   }

   void WhenTheIceMakerBecomesEnabled()
   {
      GivenTheIceMakerIsEnabledAndNotInSabbath();
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
   }

   void TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_t expected)
   {
      IceMakerStateMachineState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_StateMachineState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void IceMakerMotorControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorControlRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenMotorDriveIs(bool state)
   {
      DataModel_Write(dataModel, Erd_TwistIceMakerMotorDriveEnable, &state);
   }

   void TheMotorActionResultShouldBe(IceMakerMotorActionResult_t expected)
   {
      IceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorActionResult, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheMotorOperationStateShouldBe(IceMakerMotorOperationState_t expected)
   {
      IceMakerMotorOperationState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorOperationState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheMotorSwitchIsDebouncedHigh(void)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_TwistMotorSwitchState, clear);
   }

   void WhenTheMotorSwitchIsDebouncedHigh(void)
   {
      GivenTheMotorSwitchIsDebouncedHigh();
   }

   void GivenTheMotorSwitchIsDebouncedLow(void)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_TwistMotorSwitchState, set);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathModeEnable,
         &state);
   }

   void WhenSabbathModeBecomes(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void TheIceMakerWaterValveShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0_WaterValveRelay, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheIsolationValveShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IsolationValveRelay, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheFillTubeHeaterRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0_FillTubeHeater, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheFillTubeHeaterResolvedVoteShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_IceMaker0_FillTubeHeater_ResolvedVote, &vote);
      CHECK_EQUAL(expectedDutyCycle, vote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, vote.care);
   }

   void TheFillTubeHeaterWinningErdShouldBe(WinningVoteErd_t expected)
   {
      WinningVoteErd_t actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_FillTubeHeater_WinningVoteErd,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTheMotorSwitchIsDebouncedLow(void)
   {
      GivenTheMotorSwitchIsDebouncedLow();
   }

   uint16_t DamperStepsRemaining(void)
   {
      StepperPositionRequest_t request;
      DataModel_Read(dataModel, Erd_DamperStepperMotorPositionRequest, &request);
      return request.stepsToMove;
   }

   void AfterTheEventQueueIsRun()
   {
      EventQueue_InterruptSafe_Run(EventQueueInterruptSafePlugin_GetInterruptSafeEventQueue());
   }

   void GivenTheDamperIsDoneMoving(void)
   {
      while(DamperStepsRemaining() > 0)
      {
         Interrupt_TestDouble_TriggerInterrupt(fastInterruptTestDouble);
         AfterTheEventQueueIsRun();
      }
   }

   void GivenTheApplicationIsInitializedAndTheMotorIsHomed(void)
   {
      GivenApplicationHasBeenInitialized();
      GivenTheDamperIsDoneMoving();
      GivenTheMotorSwitchIsDebouncedHigh();
      WhenMotorDriveIs(ENABLED);

      AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedLow();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      AfterNInterrupts(MotorBrakingDurationInMsec);

      After(MotorControllerPollingTimeInMsec);
      TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homed);
   }

   void WhenTheThermistorAdcCountsAre(AdcCounts_t adcCount)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_MoldThermistor_AdcCount, &adcCount);
   }

   void TwistTrayIceMakerThermistorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MoldThermistor_IsValidResolved, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheApplicationIsInitializedAndIceMakerIsInFreeze(void)
   {
      GivenTheApplicationIsInitializedAndTheMotorIsHomed();

      TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenTheApplicationIsInitializedAndIceMakerIsInHarvest(void)
   {
      GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

      WhenTheThermistorAdcCountsAre(BelowFreezingAdcCounts);
      After(MSEC_PER_SEC);
      After(twistTrayIceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN - 1);
      TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

      After(1);
      TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
   }

   void GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested(void)
   {
      GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

      WhenMotorDriveIs(ENABLED);
      After(MotorControllerPollingTimeInMsec);

      TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);
      TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

      AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
      After(MotorControllerPollingTimeInMsec);
      TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

      AfterNInterrupts(MotorBrakingDurationInMsec);
      After(MotorControllerPollingTimeInMsec);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
      TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestUntwisting);

      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      After(MotorControllerPollingTimeInMsec);
      TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

      AfterNInterrupts(MotorBrakingDurationInMsec);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

      After(MotorControllerPollingTimeInMsec);
      TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
      TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvested);

      After(twistTrayIceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
      TheFillTubeHeaterWinningErdShouldBe(Erd_IceMaker0_FillTubeHeater_NonHarvestVote);
   }

   void TheMotorControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorControlRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheMotorStateShouldBe(TwistTrayIceMakerMotorState_t expected)
   {
      CHECK_EQUAL(expected, TwistTrayMotorOutput_TestDouble_GetTwistTrayMotorOutputState(twistTrayMotorOutputTestDouble));
   }

   void WhenTheMotorActionResultIs(IceMakerMotorActionResult_t motorActionResult)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_MotorActionResult,
         &motorActionResult);
   }

   void WhenTheTestRequestIs(IceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_TestRequest,
         &request);
   }
};

TEST(TwistTrayIceMakerIntegration, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(TwistTrayIceMakerIntegration, ShouldGoToFreezeAfterHomingWhileIceMakerIsDisabled)
{
   GivenTheIceMakerIsDisabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndTheMotorIsHomed();

   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldInitializeInThermistorFaultStateIfThermistorIsInvalidThenHomeWhenTheThermistorBecomesValid)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(InvalidAdcCount);
   GivenApplicationHasBeenInitialized();
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
   TwistTrayIceMakerThermistorShouldBe(Invalid);

   WhenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   After(sensorData->iceMaker0MoldThermistor->goodReadingCounterMax * sensorData->periodicUpdateRateInMs);

   TwistTrayIceMakerThermistorShouldBe(Valid);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Homing);
}

TEST(TwistTrayIceMakerIntegration, ShouldInitializeInFsmStateHoming)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenApplicationHasBeenInitialized();
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Homing);
}

TEST(TwistTrayIceMakerIntegration, ShouldRequestMotorControlRequest)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenApplicationHasBeenInitialized();

   IceMakerMotorControlRequestShouldBe(true);
}

TEST(TwistTrayIceMakerIntegration, ShouldBeginHomingMotorOnInitializationIfControlIsGranted)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenApplicationHasBeenInitialized();
   TheMotorControlRequestShouldBe(SET);

   WhenMotorDriveIs(ENABLED);
   After(MotorControllerPollingTimeInMsec);

   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homing);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
}

TEST(TwistTrayIceMakerIntegration, ShouldMakeSureTrayIsHome)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenApplicationHasBeenInitialized();
   GivenTheMotorSwitchIsDebouncedHigh();

   WhenMotorDriveIs(ENABLED);

   After(MotorControllerPollingTimeInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingJumpingOutOfHome);
}

TEST(TwistTrayIceMakerIntegration, ShouldJumpOutOfHomeAfterBeingHome)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenApplicationHasBeenInitialized();
   GivenTheMotorSwitchIsDebouncedHigh();
   WhenMotorDriveIs(ENABLED);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
   WhenTheMotorSwitchIsDebouncedLow();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HomingReadyToLandInHomePosition);
}

TEST(TwistTrayIceMakerIntegration, ShouldLandBackInHomePositionAfterJumpingOutOfHome)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenApplicationHasBeenInitialized();
   GivenTheMotorSwitchIsDebouncedHigh();
   WhenMotorDriveIs(ENABLED);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
   WhenTheMotorSwitchIsDebouncedLow();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);
   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homed);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerIntegration, ShouldGoToFreezeAfterHoming)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndTheMotorIsHomed();

   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldCompleteFreezeAndEnterHarvest)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

   WhenTheThermistorAdcCountsAre(BelowFreezingAdcCounts);
   After(MSEC_PER_SEC);
   After(twistTrayIceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   After(1);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(TwistTrayIceMakerIntegration, ShouldRunMotorHarvestCycle)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   WhenMotorDriveIs(ENABLED);
   After(MotorControllerPollingTimeInMsec);

   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   After(MotorControllerPollingTimeInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestUntwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvested);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptHarvestAndShouldGoToFillAfterHarvestWhenIceMakerBecomesDisabled)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   WhenMotorDriveIs(ENABLED);
   After(MotorControllerPollingTimeInMsec);

   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   After(MotorControllerPollingTimeInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestUntwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvested);

   After(twistTrayIceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptHarvestAndShouldGoToFillAfterHarvestWhenSabbathModeBecomesEnabled)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenSabbathModeIs(DISABLED);
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   After(MotorControllerPollingTimeInMsec);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvesting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   After(MotorControllerPollingTimeInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestUntwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_HarvestReadyToLandInHomePosition);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

   WhenSabbathModeBecomes(ENABLED);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Harvested);

   After((twistTrayIceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC) - (5 * MotorControllerPollingTimeInMsec));
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldFillAfterHarvest)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldRemainInStateFreezeWhenIceMakerEnabledStateChanges)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenTheIceMakerBecomesEnabled();
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptFillWhenIceMakerBecomesDisabled)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   WhenTheIceMakerBecomesDisabled();
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptFillWhenSabbathModeBecomesEnabled)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenSabbathModeIs(DISABLED);
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   WhenSabbathModeBecomes(ENABLED);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldTransitionToHomingThenFreezeWhenIceMakerIsDisabledInThermistorFaultState)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(InvalidAdcCount);
   GivenApplicationHasBeenInitialized();
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
   TwistTrayIceMakerThermistorShouldBe(Invalid);

   WhenTheIceMakerBecomesDisabled();
   WhenTheIceMakerThermistorAdcCountIs(ValidAdcCount);

   After(sensorData->iceMaker0MoldThermistor->goodReadingCounterMax * sensorData->periodicUpdateRateInMs);
   TwistTrayIceMakerThermistorShouldBe(Valid);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Homing);

   GivenTheMotorSwitchIsDebouncedHigh();
   WhenMotorDriveIs(ENABLED);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
   WhenTheMotorSwitchIsDebouncedLow();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);
   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(IceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(IceMakerMotorActionResult_Homed);
   TheMotorControlRequestShouldBe(CLEAR);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldTurnOnFillTubeHeaterWhenEnteringHarvest)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   TheFillTubeHeaterResolvedVoteShouldBe(twistTrayIceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
   TheFillTubeHeaterWinningErdShouldBe(Erd_IceMaker0_FillTubeHeater_IceMakerVote);
   After(RelayDelay);
   TheFillTubeHeaterRelayShouldBe(ON);

   After((twistTrayIceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC) - RelayDelay - 1);
   TheFillTubeHeaterResolvedVoteShouldBe(twistTrayIceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
   TheFillTubeHeaterWinningErdShouldBe(Erd_IceMaker0_FillTubeHeater_IceMakerVote);

   After(1);
   TheFillTubeHeaterResolvedVoteShouldBe(iceMakerData->iceMakerSlots->slot0Data->nonHarvestFillTubeHeaterData->offDutyCyclePercentage, Vote_Care);
   TheFillTubeHeaterRelayShouldBe(OFF);
}

TEST(TwistTrayIceMakerIntegration, ShouldTransitionFromFreezeToHarvestingOnceHarvestCountIsReachedWhenStartingCountingWithABelowStartIntegrationTemperature)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(BelowFreezingAdcCounts);
   GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

   After(twistTrayIceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   After(1);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(TwistTrayIceMakerIntegration, ShouldTransitionFromFreezeToHarvestingOnceHarvestCountIsReachedWhenStartingCountingAfterTemperatureDecreasesBelowStartIntegrationTemperature)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

   WhenTheIceMakerThermistorAdcCountIs(BelowFreezingAdcCounts);

   After(MSEC_PER_SEC);
   After(twistTrayIceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN - 1);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   After(1);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(TwistTrayIceMakerIntegration, ShouldTransitionToFillStateWhenTestRequestIsFillInHarvestingStateAndHarvestIsCompleted)
{
   GivenTheIceMakerIsEnabledAndNotInSabbath();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   WhenTheMotorActionResultIs(Harvested);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheTestRequestIs(IceMakerTestRequest_Fill);
   TwistTrayIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}
