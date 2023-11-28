/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FanFaultHandler.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Vote.h"
#include "FanId.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   NonZeroRpm = 800,
   AnotherNonZeroRpm = 600,
   NonZeroDutyCyclePercentage = 80,
   AnotherNonZeroDutyCyclePercentage = 25
};

static FanControl_t fanControl = { .type = FanControlType_Rpm, .rpm = NonZeroRpm };

static const FanSpeedData_t speedData = {
   .superLowSpeed = fanControl,
   .lowSpeed = fanControl,
   .mediumSpeed = fanControl,
   .highSpeed = fanControl,
   .superHighSpeed = fanControl
};

static PidControllerGains_t gains = {
   .kpValue = { .value = 1, .qBits = 2 },
   .kiValue = { .value = 2, .qBits = 3 },
   .kdValue = { .value = 3, .qBits = 4 }
};

static const FanData_t fanData = {
   .careAboutHighAmbientTemperature = false,
   .fanId = FanId_Condenser,
   .pulsesPerRevolution = 2,
   .careAboutCoolingMode = false,
   .powerUsageInWatts = 2,
   .gains = gains,
   .lowerLimit = 0,
   .upperLimit = 0xFFFF,
   .cannotReachTargetFaultTimeoutInMinutes = 5,
   .missingFeedbackWhenOnFaultTimeoutInMinutes = 6,
   .feedbackPresentWhenFanOffTimeoutInMinutes = 7,
   .speedData = &speedData,
   .superLowAtHighAmbientHumidityAndHighTemperature = fanControl,
   .superLowAtHighAmbientHumidityAndLowTemperature = fanControl,
   .highAmbientTriggerTemperatureInDegFx100 = 75,
   .highAmbientTriggerHumidityInPercentx100 = 82
};

static const FanFaultHandlerConfiguration_t fanConfig = {
   .calculatedFanControlErd = Erd_CalculatedCondenserFanControl,
   .fanActualRpmErd = Erd_CondenserFan_ActualRpm,
   .fanPwmErd = Erd_CondenserFan_Pwm,
   .fanMissingFeedbackWhenOnFaultErd = Erd_CondenserFanMissingFanFeedbackWhenFanIsRunningFault,
   .fanFeedbackPresentWhenFanOffFaultErd = Erd_CondenserFanFeedbackPresentWhenFanIsOffFault,
   .fanCannotReachTargetRpmFaultErd = Erd_CondenserFanCannotReachTargetRpmFault,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(FanFaultHandler)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   FanFaultHandler_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenModuleIsInitialized()
   {
      FanFaultHandler_Init(
         &instance,
         dataModel,
         &fanConfig,
         &fanData);
   }

   void GivenMissingFanFeedbackWhenFanIsRunningFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CondenserFanMissingFanFeedbackWhenFanIsRunningFault, &state);
   }

   void MissingFanFeedbackWhenFanIsRunningFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CondenserFanMissingFanFeedbackWhenFanIsRunningFault, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFanFeedbackPresentWhenFanIsOffFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CondenserFanFeedbackPresentWhenFanIsOffFault, &state);
   }

   void FanFeedbackPresentWhenFanIsOffFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CondenserFanFeedbackPresentWhenFanIsOffFault, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFanCannotReachTargetRpmFaultIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CondenserFanCannotReachTargetRpmFault, &state);
   }

   void FanCannotReachTargetRpmFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CondenserFanCannotReachTargetRpmFault, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFanControlIs(FanControl_t fanControl)
   {
      DataModel_Write(dataModel, Erd_CalculatedCondenserFanControl, &fanControl);
   }

   void WhenFanControlIs(FanControl_t fanControl)
   {
      GivenFanControlIs(fanControl);
   }

   void GivenFanActualRpmIs(FanRpm_t rpm)
   {
      DataModel_Write(dataModel, Erd_CondenserFan_ActualRpm, &rpm);
   }

   void WhenFanActualRpmIs(FanRpm_t rpm)
   {
      GivenFanActualRpmIs(rpm);
   }

   void GivenFanPwmIs(PwmDutyCycle_t pwm)
   {
      DataModel_Write(dataModel, Erd_CondenserFan_Pwm, &pwm);
   }

   void WhenFanPwmIs(PwmDutyCycle_t pwm)
   {
      GivenFanPwmIs(pwm);
   }

   void GivenMissingFanFeedbackWhenFanIsRunningFaultIsSet()
   {
      GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
      GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
      GivenFanActualRpmIs(0);
      GivenModuleIsInitialized();

      After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
      MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

      After(1);
      MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
   }

   void GivenFanFeedbackPresentWhenFanIsOffFaultIsSet()
   {
      GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
      GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
      GivenFanActualRpmIs(NonZeroRpm);
      GivenModuleIsInitialized();

      After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
      FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

      After(1);
      FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
   }

   void GivenFanCannotReachTargetRpmFaultIsSet()
   {
      GivenFanCannotReachTargetRpmFaultIs(CLEAR);
      GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
      GivenFanActualRpmIs(700);
      GivenFanPwmIs(PwmDutyCycle_Max);
      GivenModuleIsInitialized();

      After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
      FanCannotReachTargetRpmFaultShouldBe(CLEAR);

      After(1);
      FanCannotReachTargetRpmFaultShouldBe(SET);
   }

   void WhenFanCannotReachTargetRpmFaultIsCleared()
   {
      WhenFanPwmIs((PwmDutyCycle_Max * 94) / 100);

      After(MSEC_PER_MIN - 1);
      FanCannotReachTargetRpmFaultShouldBe(SET);

      After(1);
      FanCannotReachTargetRpmFaultShouldBe(CLEAR);
   }
};

// Clear Cannot Reach Target RPM When Calculated Fan Control Changes

TEST(FanFaultHandler, ShouldClearCannotReachTargetRpmFaultWhenCalculatedFanControlRpmChangesAfterOneMinuteAndFanPwmIs95Percent)
{
   GivenFanCannotReachTargetRpmFaultIs(SET);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanPwmIs((PwmDutyCycle_Max * 95) / 100);
   GivenModuleIsInitialized();

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = AnotherNonZeroRpm });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldClearCannotReachTargetRpmFaultWhenCalculatedFanControlRpmChangesToADutyCycleAfterOneMinuteAndFanPwmIs95Percent)
{
   GivenFanCannotReachTargetRpmFaultIs(SET);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanPwmIs((PwmDutyCycle_Max * 95) / 100);
   GivenModuleIsInitialized();

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = AnotherNonZeroDutyCyclePercentage });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldClearCannotReachTargetRpmFaultWhenCalculatedFanControlRpmChangesDutyCyclesAfterOneMinuteAndFanPwmIs95Percent)
{
   GivenFanCannotReachTargetRpmFaultIs(SET);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });
   GivenFanPwmIs((PwmDutyCycle_Max * 95) / 100);
   GivenModuleIsInitialized();

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = AnotherNonZeroDutyCyclePercentage });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotClearCannotReachTargetRpmFaultWhenCalculatedFanControlRpmDoesNotChangeAfterOneMinuteAndFanPwmIsEqualTo95Percent)
{
   GivenFanCannotReachTargetRpmFaultIs(SET);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanPwmIs((PwmDutyCycle_Max * 95) / 100);
   GivenModuleIsInitialized();

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotClearCannotReachTargetRpmFaultWhenCalculatedFanControlRpmDoesNotChangeAfterOneMinuteAndFanPwmIsGreaterThan95Percent)
{
   GivenFanCannotReachTargetRpmFaultIs(SET);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanPwmIs((PwmDutyCycle_Max * 96) / 100);
   GivenModuleIsInitialized();

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotClearCannotReachTargetRpmFaultWhenCalculatedFanControlDutyCycleDoesNotChangeAfterOneMinute)
{
   GivenFanCannotReachTargetRpmFaultIs(SET);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });
   GivenModuleIsInitialized();

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

// Missing Fan Feedback When Fan Is Running Fault

TEST(FanFaultHandler, ShouldSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfCalculatedFanControlRpmIsNonZeroAndFanActualRpmIsZero)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfCalculatedFanControlRpmIsZeroAndFanActualRpmIsZero)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = 0 });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfCalculatedFanControlDutyCycleIsZeroAndFanActualRpmIsZero)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfCalculatedFanControlDutyCycleIsNonZeroAndFanActualRpmIsZero)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfCalculatedFanControlRpmIsNonZeroAndFanActualRpmIsNonZero)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfCalculatedFanControlRpmIsNonZeroAndFanActualRpmIsZeroAfterCalculatedFanControlChangesToZeroDutyCycle)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldClearMissingFanFeedbackWhenFanIsRunningFaultWhenActualRpmChangesToNonZeroValueWhileFanIsOnAfterOneMinute)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIsSet();

   WhenFanActualRpmIs(NonZeroRpm);

   After(MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackFaultUntilItReachesConsecutiveMissingFeedbackWhenOnNumberOfMinutesWithZeroActualRpmAndFanControlWithNonZeroRpmAndActualRpmWasNonZeroForOneOfThePeriodicChecks)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenFanActualRpmIs(NonZeroRpm);
   After(MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenFanActualRpmIs(0);

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackFaultUntilItReachesConsecutiveMissingFeedbackWhenOnNumberOfMinutesWithZeroActualRpmAndFanControlWithNonZeroRpmAndFanControlWasZeroDutyCycleForOneOfThePeriodicChecks)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   After(MSEC_PER_MIN);

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackFaultUntilItReachesConsecutiveMissingFeedbackWhenOnNumberOfMinutesWithZeroActualRpmAndFanControlWithNonZeroRpmAndFanControlWasNonZeroDutyCycleForOneOfThePeriodicChecks)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });
   After(MSEC_PER_MIN);

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = AnotherNonZeroRpm });

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetMissingFanFeedbackFaultUntilItReachesConsecutiveMissingFeedbackWhenOnNumberOfMinutesWithZeroActualRpmAndFanControlWithNonZeroRpmAndFanControlWasZeroRpmForOneOfThePeriodicChecks)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   // this time is ignored, it's not considered the fan being turned off
   // when fan is voted off, it's always duty cycle of 0%
   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = 0 });
   After(MSEC_PER_MIN);

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   After(MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

// Feedback Present When Fan Is Off

TEST(FanFaultHandler, ShouldSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfCalculatedFanControlIsZeroDutyCycleAndFanActualRpmIsNonZero)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfCalculatedFanControlIsZeroDutyCycleAndFanActualRpmIsZero)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfCalculatedFanControlIsNonZeroDutyCycleAndFanActualRpmIsZero)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });
   GivenFanActualRpmIs(0);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfCalculatedFanControlIsRpmTypeAndNonZeroDutyCycleAndFanActualRpmIsNonZero)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfCalculatedFanControlRpmIsNonZeroAndFanActualRpmIsNonZero)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfCalculatedFanControlIsZeroDutyCycleAndFanActualRpmIsNonZeroAfterCalculatedFanControlChangesToNonZeroRpm)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldClearFeedbackPresentWhenFanIsOffFaultWhenActualRpmChangesToZeroValueWhileFanIsOffAfterOneMinute)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIsSet();

   WhenFanActualRpmIs(0);

   After(MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetFeedbackPresentWhenFanIsOffFaultUntilItReachesConsecutiveFeedbackPresentWhenOffNumberOfMinutesWithNonZeroActualRpmAndFanControlWithZeroDutyCycleAndActualRpmWasZeroForOneOfThePeriodicChecks)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanActualRpmIs(0);
   After(MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanActualRpmIs(NonZeroRpm);

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetFeedbackPresentWhenFanIsOffFaultUntilItReachesConsecutiveFeedbackPresentWhenOffNumberOfMinutesWithNonZeroActualRpmAndFanControlWithZeroDutyCycleAndFanControlWasNonZeroRpmForOneOfThePeriodicChecks)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   After(MSEC_PER_MIN);

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldSetFeedbackPresentWhenFanIsOffFaultUntilItReachesConsecutiveFeedbackPresentWhenOffNumberOfMinutesWithNonZeroActualRpmAndFanControlWithZeroDutyCycleAndFanControlWasZeroRpmForOneOfThePeriodicChecksThatDidNotCount)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenModuleIsInitialized();

   After(fanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   // this time is ignored, it's not considered the fan being turned off
   // when fan is voted off, it's always duty cycle of 0%
   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = 0 });
   After(MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = 0 });
   After(MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
}

// Cannot Reach Target RPM

TEST(FanFaultHandler, ShouldSetCannotReachTargetRpmFaultWhenPwmIsPwmDutyCycle_MaxForCannotReachTargetFaultTimeoutInMinutesAndFanControlTypeIsRpmWithANonZeroRpmAndANonZeroActualRpm)
{
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenFanPwmIs(PwmDutyCycle_Max);
   GivenModuleIsInitialized();

   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetCannotReachTargetRpmFaultWhenPwmIsPwmDutyCycle_MaxForCannotReachTargetFaultTimeoutInMinutesAndFanControlTypeIsRpmWithZeroRpmAndANonZeroActualRpm)
{
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = 0 });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenFanPwmIs(PwmDutyCycle_Max);
   GivenModuleIsInitialized();

   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldSetCannotReachTargetRpmFaultWhenPwmIsPwmDutyCycle_MaxForCannotReachTargetFaultTimeoutInMinutesAndFanControlTypeIsRpmWithANonZeroRpmAndAZeroActualRpm)
{
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(0);
   GivenFanPwmIs(PwmDutyCycle_Max);
   GivenModuleIsInitialized();

   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldClearCannotReachTargetRpmFaultWhenFanPwmIsLessThan95AfterOneMinute)
{
   GivenFanCannotReachTargetRpmFaultIsSet();

   WhenFanPwmIs((PwmDutyCycle_Max * 94) / 100);

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldSetCannotReachTargetRpmFaultWhenPwmIsPwmDutyCycle_MaxForCannotReachTargetFaultTimeoutInMinutesAndFanControlTypeIsRpmWithANonZeroRpmAndANonZeroActualRpmASecondTimeWhenFanPwmIsPwmDutyCycle_MaxAgain)
{
   GivenFanCannotReachTargetRpmFaultIsSet();

   WhenFanCannotReachTargetRpmFaultIsCleared();

   WhenFanPwmIs(PwmDutyCycle_Max);
   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldNotSetCannotReachTargetRpmFaultWhenPwmIsPwmDutyCycle_MaxAndFanControlTypeIsDutyCycleWithANonZeroDutyCycle)
{
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_DutyCycle, .dutyCycle = NonZeroDutyCyclePercentage });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenFanPwmIs(PwmDutyCycle_Max);
   GivenModuleIsInitialized();

   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetCannotReachTargetRpmFaultWhenFanPwmDropsBelow95BeforeCannotReachTargetFaultTimeoutInMinutesIsReached)
{
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenFanPwmIs(PwmDutyCycle_Max);
   GivenModuleIsInitialized();

   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   WhenFanPwmIs((PwmDutyCycle_Max * 94) / 100);
   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultHandler, ShouldNotSetCannotReachTargetRpmFaultUntilItReachesConsecutiveCannotReachTargetRpmNumberOfMinutesWithFanPwmOfPwmDutyCycle_MaxAndFanControlWithNonZeroRpmWithFanBelow95ForOneOfThePeriodicChecks)
{
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenFanControlIs({ .type = FanControlType_Rpm, .rpm = NonZeroRpm });
   GivenFanActualRpmIs(NonZeroRpm);
   GivenFanPwmIs(PwmDutyCycle_Max);
   GivenModuleIsInitialized();

   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   WhenFanPwmIs((PwmDutyCycle_Max * 94) / 100);
   After(MSEC_PER_MIN);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   WhenFanPwmIs(PwmDutyCycle_Max);
   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultHandler, ShouldBeAbleToClearCannotReachTargetRpmFaultWhenFanControlChangesRpmAndSetAgainIfPwmIsStillMax)
{
   GivenFanCannotReachTargetRpmFaultIsSet();

   WhenFanControlIs({ .type = FanControlType_Rpm, .rpm = AnotherNonZeroRpm });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(fanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}
