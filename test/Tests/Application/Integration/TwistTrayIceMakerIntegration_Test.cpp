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
#include "TwistTrayIceMakerHighLevelState.h"
#include "TwistTrayIceMakerOperationState.h"
#include "TwistTrayIceMakerMotorState.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataSource_Gpio.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "IceMakerData.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "GpioGroup_TestDouble.h"
#include "Interrupt_TestDouble.h"
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
   Valid = true
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
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      timerModuleTestDouble = (TimerModule_TestDouble_t *)DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);
      gpioGroupTestDouble = (GpioGroup_TestDouble_t *)DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroupInterface);
      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt);

      twistTrayIceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->twistTrayIceMakerData;
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
      DataModel_Write(dataModel, Erd_TwistTrayIceMakerThermistor_AdcCount, &count);
   }

   void WhenTheIceMakerThermistorAdcCountIs(AdcCounts_t count)
   {
      GivenTheIceMakerThermistorAdcCountIs(count);
   }

   void GivenTheIceMakerIsEnabled()
   {
      DataModel_Write(dataModel, Erd_IceMaker0EnableStatus, set);
   }

   void GivenTheIceMakerIsDisabled()
   {
      DataModel_Write(dataModel, Erd_IceMaker0EnableStatus, clear);
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, clear);
   }

   void WhenTheIceMakerBecomesDisabled()
   {
      GivenTheIceMakerIsDisabled();
   }

   void WhenTheIceMakerBecomesEnabled()
   {
      GivenTheIceMakerIsEnabled();
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

   void HighLevelStateShouldBe(TwistTrayIceMakerHighLevelState_t expected)
   {
      TwistTrayIceMakerHighLevelState_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_HighLevelState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void OperationStateShouldBe(TwistTrayIceMakerOperationState_t expected)
   {
      TwistTrayIceMakerHighLevelState_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_OperationState, &actual);
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

   void TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_t expected)
   {
      TwistTrayIceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_MotorActionResult, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_t expected)
   {
      TwistTrayIceMakerMotorOperationState_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_MotorOperationState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheMotorSwitchIsDebouncedHigh(void)
   {
      DataModel_Write(dataModel, Erd_Gpio_GPIO_IN_02, clear);
   }

   void WhenTheMotorSwitchIsDebouncedHigh(void)
   {
      GivenTheMotorSwitchIsDebouncedHigh();
   }

   void GivenTheMotorSwitchIsDebouncedLow(void)
   {
      DataModel_Write(dataModel, Erd_Gpio_GPIO_IN_02, set);
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathMode,
         &state);
   }

   void WhenSabbathModeBecomes(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void TheIceMakerWaterValveShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMakerWaterValveRelay, &actual);
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
      DataModel_Read(dataModel, Erd_FillTubeHeater, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheFillTubeHeaterResolvedVoteShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FillTubeHeater_ResolvedVote, &vote);
      CHECK_EQUAL(expectedDutyCycle, vote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, vote.care);
   }

   void TheFillTubeHeaterWinningErdShouldBe(WinningVoteErd_t expected)
   {
      WinningVoteErd_t actual;
      DataModel_Read(
         dataModel,
         Erd_FillTubeHeater_WinningVoteErd,
         &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTheMotorSwitchIsDebouncedLow(void)
   {
      GivenTheMotorSwitchIsDebouncedLow();
   }

   void GivenTheApplicationIsInitializedAndTheMotorIsHomed(void)
   {
      GivenApplicationHasBeenInitialized();
      GivenTheMotorSwitchIsDebouncedHigh();
      WhenMotorDriveIs(ENABLED);

      AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedLow();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      AfterNInterrupts(MotorBrakingDurationInMsec);

      After(MotorControllerPollingTimeInMsec);
      TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homed);
   }

   void WhenTheThermistorAdcCountsAre(AdcCounts_t adcCount)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMakerThermistor_AdcCount, &adcCount);
   }

   void TwistTrayIceMakerThermistorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMakerThermistor_IsValidResolved, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenTheApplicationIsInitializedAndIceMakerIsInFreeze(void)
   {
      GivenTheApplicationIsInitializedAndTheMotorIsHomed();

      OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   }

   void GivenTheApplicationIsInitializedAndIceMakerIsInHarvest(void)
   {
      GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

      WhenTheThermistorAdcCountsAre(BelowFreezingAdcCounts);
      After(MSEC_PER_SEC);
      After(twistTrayIceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - 1);
      OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

      After(1);
      OperationStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
   }

   void GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested(void)
   {
      GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

      WhenMotorDriveIs(ENABLED);
      After(MotorControllerPollingTimeInMsec);

      TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);
      TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

      AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
      After(MotorControllerPollingTimeInMsec);
      TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

      AfterNInterrupts(MotorBrakingDurationInMsec);
      After(MotorControllerPollingTimeInMsec);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
      TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_Untwisting);

      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      After(MotorControllerPollingTimeInMsec);
      TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_ReadyToLandInHomePosition);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

      AfterNInterrupts(MotorBrakingDurationInMsec);
      TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

      After(MotorControllerPollingTimeInMsec);
      TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_Idle);
      TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvested);

      After(twistTrayIceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
      TheFillTubeHeaterWinningErdShouldBe(Erd_FillTubeHeater_NonHarvestVote);
   }

   void TheMotorControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorControlRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheMotorStateShouldBe(TwistTrayIceMakerMotorState_t expected)
   {
      switch(expected)
      {
         case TwistTrayIceMakerMotorState_Coasting:
            CHECK_EQUAL(false, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_00));
            CHECK_EQUAL(false, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_01));
            break;

         case TwistTrayIceMakerMotorState_Untwisting:
            CHECK_EQUAL(false, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_00));
            CHECK_EQUAL(true, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_01));
            break;

         case TwistTrayIceMakerMotorState_Twisting:
            CHECK_EQUAL(true, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_00));
            CHECK_EQUAL(false, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_01));
            break;

         case TwistTrayIceMakerMotorState_Braking:
            CHECK_EQUAL(true, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_00));
            CHECK_EQUAL(true, GpioGroup_TestDouble_ReadOutputState(gpioGroupTestDouble, Erd_BspGpio_MTR_DRV_01));
            break;

         default:
            CHECK_FALSE("Invalid State");
      };
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

   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldInitializeInThermistorFaultStateIfThermistorIsInvalidThenHomeWhenTheThermistorBecomesValid)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(InvalidAdcCount);
   GivenApplicationHasBeenInitialized();
   OperationStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   TwistTrayIceMakerThermistorShouldBe(Invalid);

   WhenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   After(sensorData->twistTrayIceMakerMoldThermistor->goodReadingCounterMax * sensorData->periodicUpdateRateInMs);

   TwistTrayIceMakerThermistorShouldBe(Valid);
   HighLevelStateShouldBe(TwistTrayIceMakerHighLevelState_NormalRun);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Homing);
}

TEST(TwistTrayIceMakerIntegration, ShouldInitializeInHighLevelStateNormalRunAndOperationStateHoming)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenApplicationHasBeenInitialized();
   HighLevelStateShouldBe(TwistTrayIceMakerHighLevelState_NormalRun);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Homing);
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

   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homing);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);
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
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HomingJumpingOutOfHome);
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
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HomingReadyToLandInHomePosition);
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
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homed);
   TheMotorControlRequestShouldBe(CLEAR);
}

TEST(TwistTrayIceMakerIntegration, ShouldGoToFreezeAfterHoming)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndTheMotorIsHomed();

   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldCompleteFreezeAndEnterHarvest)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

   WhenTheThermistorAdcCountsAre(BelowFreezingAdcCounts);
   After(MSEC_PER_SEC);
   After(twistTrayIceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - 1);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   After(1);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMakerIntegration, ShouldRunMotorHarvestCycle)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   WhenMotorDriveIs(ENABLED);
   After(MotorControllerPollingTimeInMsec);

   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   After(MotorControllerPollingTimeInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_Untwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_ReadyToLandInHomePosition);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvested);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptHarvestAndShouldGoToFillAfterHarvestWhenIceMakerBecomesDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   WhenMotorDriveIs(ENABLED);
   After(MotorControllerPollingTimeInMsec);

   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   After(MotorControllerPollingTimeInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_Untwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_ReadyToLandInHomePosition);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

   WhenTheIceMakerBecomesDisabled();
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvested);

   After(twistTrayIceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptHarvestAndShouldGoToFillAfterHarvestWhenSabbathModeBecomesEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenSabbathModeIs(DISABLED);
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   After(MotorControllerPollingTimeInMsec);
   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestCheckingIfBucketIsFull);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Twisting);

   AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   After(MotorControllerPollingTimeInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_Untwisting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationStateHarvest_ReadyToLandInHomePosition);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Untwisting);

   WhenSabbathModeBecomes(ENABLED);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);

   WhenTheMotorSwitchIsDebouncedLow();
   WhenTheMotorSwitchIsDebouncedHigh();
   AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Braking);

   AfterNInterrupts(MotorBrakingDurationInMsec);
   TheMotorStateShouldBe(TwistTrayIceMakerMotorState_Coasting);

   After(MotorControllerPollingTimeInMsec);
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvested);

   After((twistTrayIceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC) - (5 * MotorControllerPollingTimeInMsec));
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldFillAfterHarvest)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldRemainInStateFreezeWhenIceMakerEnabledStateChanges)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenTheIceMakerBecomesDisabled();
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   WhenTheIceMakerBecomesEnabled();
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptFillWhenIceMakerBecomesDisabled)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   WhenTheIceMakerBecomesDisabled();
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldNotInterruptFillWhenSabbathModeBecomesEnabled)
{
   GivenTheIceMakerIsEnabled();
   GivenSabbathModeIs(DISABLED);
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   WhenSabbathModeBecomes(ENABLED);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   After(twistTrayIceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldTransitionToHomingThenFreezeWhenIceMakerIsDisabledInThermistorFaultState)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(InvalidAdcCount);
   GivenApplicationHasBeenInitialized();
   OperationStateShouldBe(TwistTrayIceMakerOperationState_ThermistorFault);
   TwistTrayIceMakerThermistorShouldBe(Invalid);

   WhenTheIceMakerBecomesDisabled();
   WhenTheIceMakerThermistorAdcCountIs(ValidAdcCount);

   After(sensorData->twistTrayIceMakerMoldThermistor->goodReadingCounterMax * sensorData->periodicUpdateRateInMs);
   TwistTrayIceMakerThermistorShouldBe(Valid);
   HighLevelStateShouldBe(TwistTrayIceMakerHighLevelState_NormalRun);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Homing);

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
   TheMotorOperationStateShouldBe(TwistTrayIceMakerMotorOperationState_Idle);
   TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homed);
   TheMotorControlRequestShouldBe(CLEAR);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldTurnOnFillTubeHeaterWhenEnteringHarvest)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInHarvest();

   TheFillTubeHeaterResolvedVoteShouldBe(twistTrayIceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
   TheFillTubeHeaterWinningErdShouldBe(Erd_FillTubeHeater_TwistTrayIceMakerVote);
   After(RelayDelay);
   TheFillTubeHeaterRelayShouldBe(ON);

   After((twistTrayIceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC) - RelayDelay - 1);
   TheFillTubeHeaterResolvedVoteShouldBe(twistTrayIceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
   TheFillTubeHeaterWinningErdShouldBe(Erd_FillTubeHeater_TwistTrayIceMakerVote);

   After(1);
   TheFillTubeHeaterResolvedVoteShouldBe(twistTrayIceMakerData->fillTubeHeaterData.nonHarvestFillTubeHeaterDutyCyclePercentage, Vote_Care);
   TheFillTubeHeaterWinningErdShouldBe(Erd_FillTubeHeater_NonHarvestVote);
   TheFillTubeHeaterRelayShouldBe(ON);
}

TEST(TwistTrayIceMakerIntegration, ShouldTransitionFromFreezeToHarvestingOnceHarvestCountIsReachedWhenStartingCountingWithABelowStartIntegrationTemperature)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(BelowFreezingAdcCounts);
   GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

   After(twistTrayIceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - 1);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   After(1);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMakerIntegration, ShouldTransitionFromFreezeToHarvestingOnceHarvestCountIsReachedWhenStartingCountingAfterTemperatureDecreasesBelowStartIntegrationTemperature)
{
   GivenTheIceMakerIsEnabled();
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

   WhenTheIceMakerThermistorAdcCountIs(BelowFreezingAdcCounts);

   After(MSEC_PER_SEC);
   After(twistTrayIceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN - 1);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);

   After(1);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}
