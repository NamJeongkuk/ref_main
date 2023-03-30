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
   IntegrationPeriod = 1 * MSEC_PER_SEC,
   BelowFreezingAdcCounts = 44800
};

TEST_GROUP(TwistTrayIceMakerIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   const TwistTrayIceMakerData_t *twistTrayIceMakerData;
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
   }

   void GivenApplicationHasBeenInitialized(void)
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   };

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
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

   void GivenTheApplicationIsInitializedAndIceMakerIsInFreeze(void)
   {
      GivenTheApplicationIsInitializedAndTheMotorIsHomed();

      OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
   }

   void GivenTheApplicationIsInitializedAndIceMakerIsInHarvest(void)
   {
      GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

      WhenTheThermistorAdcCountsAre(BelowFreezingAdcCounts);
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

TEST(TwistTrayIceMakerIntegration, ShouldInitializeInHighLevelStateNormalRunAndOperationStateHoming)
{
   GivenApplicationHasBeenInitialized();
   HighLevelStateShouldBe(TwistTrayIceMakerHighLevelState_NormalRun);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Homing);
}

TEST(TwistTrayIceMakerIntegration, ShouldRequestMotorControlRequest)
{
   GivenApplicationHasBeenInitialized();

   IceMakerMotorControlRequestShouldBe(true);
}

TEST(TwistTrayIceMakerIntegration, ShouldBeginHomingMotorOnInitializationIfControlIsGranted)
{
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
   GivenTheApplicationIsInitializedAndTheMotorIsHomed();

   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}

TEST(TwistTrayIceMakerIntegration, ShouldCompleteFreezeAndEnterHarvest)
{
   GivenTheApplicationIsInitializedAndIceMakerIsInFreeze();

   WhenTheThermistorAdcCountsAre(BelowFreezingAdcCounts);
   After(twistTrayIceMakerData->freezeData.minimumFreezeTimeMinutes * MSEC_PER_MIN);

   OperationStateShouldBe(TwistTrayIceMakerOperationState_Harvesting);
}

TEST(TwistTrayIceMakerIntegration, ShouldRunMotorHarvestCycle)
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
}

TEST(TwistTrayIceMakerIntegration, ShouldFillAfterHarvest)
{
   GivenTheApplicationIsInitializedAndIceMakerHasJustHarvested();

   After(MotorControllerPollingTimeInMsec);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);
   TheIceMakerWaterValveShouldBe(ON);
   TheIsolationValveShouldBe(ON);

   After(twistTrayIceMakerData->fillData.waterFillTimeSecX10 * 100);
   TheIceMakerWaterValveShouldBe(OFF);
   TheIsolationValveShouldBe(OFF);
   OperationStateShouldBe(TwistTrayIceMakerOperationState_Freeze);
}
