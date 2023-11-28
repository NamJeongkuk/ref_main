/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"
#include "TddPersonality.h"

enum
{
   NonZeroInputCapture = 800
};

TEST_GROUP(FanFaultIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const CombinedFanData_t *fanData;
   FanData_t condenserFanData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      fanData = PersonalityParametricData_Get(dataModel)->fanData;
      condenserFanData = fanData->condenserFan;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenCondenserFanInputCaptureTimeIs(InputCaptureMicroSeconds_t microSeconds)
   {
      DataModel_Write(dataModel, Erd_CondenserFan_InputCaptureTime, &microSeconds);
   }

   void WhenCondenserFanInputCaptureTimeIs(InputCaptureMicroSeconds_t microSeconds)
   {
      GivenCondenserFanInputCaptureTimeIs(microSeconds);
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

   void GivenFanFactoryVotedSpeedIs(FanVotedSpeed_t fanVotedSpeed)
   {
      DataModel_Write(dataModel, Erd_CondenserFanSpeed_FactoryServiceVote, &fanVotedSpeed);
   }

   void WhenFanFactoryVotedSpeedIs(FanVotedSpeed_t fanVotedSpeed)
   {
      GivenFanFactoryVotedSpeedIs(fanVotedSpeed);
   }

   void FanFactoryVotedSpeedShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_FactoryServiceVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }

   void GivenFanPwmIs(PwmDutyCycle_t pwm)
   {
      DataModel_Write(dataModel, Erd_CondenserFan_Pwm, &pwm);
   }

   void WhenFanPwmIs(PwmDutyCycle_t pwm)
   {
      GivenFanPwmIs(pwm);
   }

   void FanPwmShouldBe(PwmDutyCycle_t expected)
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_CondenserFan_Pwm, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenMissingFanFeedbackWhenFanIsRunningFaultIsSet()
   {
      GivenApplicationHasBeenInitialized();
      GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
      GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
      GivenCondenserFanInputCaptureTimeIs(0);

      After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
      MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

      After(1);
      MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
   }

   void GivenFanFeedbackPresentWhenFanIsOffFaultIsSet()
   {
      GivenApplicationHasBeenInitialized();
      GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
      GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
      GivenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

      After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
      FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

      After(1);
      FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
   }

   void GivenFanCannotReachTargetRpmFaultIsSet()
   {
      GivenApplicationHasBeenInitialized();
      GivenFanCannotReachTargetRpmFaultIs(CLEAR);
      GivenCondenserFanInputCaptureTimeIs(UINT32_MAX);
      FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

      WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
      After(MSEC_PER_MIN);
      FanCannotReachTargetRpmFaultShouldBe(CLEAR);

      After(condenserFanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
      FanCannotReachTargetRpmFaultShouldBe(CLEAR);

      FanPwmShouldBe(PwmDutyCycle_Max);
      After(1);
      FanCannotReachTargetRpmFaultShouldBe(SET);
   }
};

// Missing Fan Feedback When Fan Is Running Fault

TEST(FanFaultIntegration, ShouldSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfFanVotedSpeedRpmIsNonZeroAndFanInputCaptureIsZero)
{
   GivenApplicationHasBeenInitialized();
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

TEST(FanFaultIntegration, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfFanVotedSpeedRpmIsZeroAndFanInputCaptureIsZero)
{
   GivenApplicationHasBeenInitialized();
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfFanVotedSpeedRpmIsNonZeroAndFanInputCaptureIsZero)
{
   GivenApplicationHasBeenInitialized();
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_SuperHigh, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfFanVotedSpeedRpmIsNonZeroAndFanInputCaptureIsNonZero)
{
   GivenApplicationHasBeenInitialized();
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotSetMissingFanFeedbackWhenFanIsRunningFaultAfterMissingFeedbackWhenOnFaultTimeoutInMinutesIfFanVotedSpeedRpmIsNonZeroAndFanInputCaptureIsZeroAfterFanVotedSpeedChangesToOff)
{
   GivenApplicationHasBeenInitialized();
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldClearMissingFanFeedbackWhenFanIsRunningFaultWhenInputCaptureChangesToNonZeroValueWhileFanIsOnAfterOneMinute)
{
   GivenMissingFanFeedbackWhenFanIsRunningFaultIsSet();

   WhenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotSetMissingFanFeedbackFaultUntilItReachesConsecutiveMissingFeedbackWhenOnNumberOfMinutesWithZeroInputCaptureAndFanVotedSpeedOfNonZeroRpmAndInputCaptureWasNonZeroForOneOfThePeriodicChecks)
{
   GivenApplicationHasBeenInitialized();
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);
   After(MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

TEST(FanFaultIntegration, ShouldNotSetMissingFanFeedbackFaultUntilItReachesConsecutiveMissingFeedbackWhenOnNumberOfMinutesWithZeroInputCaptureAndFanVotedSpeedOfNonZeroRpmAndFanVotedSpeedRpmWasZeroForOneOfThePeriodicChecks)
{
   GivenApplicationHasBeenInitialized();
   GivenMissingFanFeedbackWhenFanIsRunningFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   After(MSEC_PER_MIN);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });

   After(condenserFanData.missingFeedbackWhenOnFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(CLEAR);

   After(1);
   MissingFanFeedbackWhenFanIsRunningFaultShouldBe(SET);
}

// Feedback Present When Fan Is Off

TEST(FanFaultIntegration, ShouldSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfFanVotedSpeedIsOffAndInputCaptureIsNonZero)
{
   GivenApplicationHasBeenInitialized();
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
}

TEST(FanFaultIntegration, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfFanVotedSpeedIsOffAndInputCaptureIsZero)
{
   GivenApplicationHasBeenInitialized();
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(0);

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfFanVotedSpeedIsNonZeroAndInputCaptureIsNonZero)
{
   GivenApplicationHasBeenInitialized();
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotSetFeedbackPresentWhenFanIsOffFaultAfterFeedbackPresentWhenFanIsOffTimeoutInMinutesIfFanVotedSpeedIsOffAndInputCaptureIsNonZeroAfterFanVotedSpeedChangesToNonZero)
{
   GivenApplicationHasBeenInitialized();
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldClearFeedbackPresentWhenFanIsOffFaultWhenInputCaptureChangesToZeroWhileFanIsOffAfterOneMinute)
{
   GivenFanFeedbackPresentWhenFanIsOffFaultIsSet();

   WhenCondenserFanInputCaptureTimeIs(0);

   After(MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotSetFeedbackPresentWhenFanIsOffFaultUntilItReachesConsecutiveFeedbackPresentWhenOffNumberOfMinutesWithNonZeroInputCaptureAndFanVotedSpeedOffAndInputCaptureWasZeroForOneOfThePeriodicChecks)
{
   GivenApplicationHasBeenInitialized();
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenCondenserFanInputCaptureTimeIs(0);
   After(MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
}

TEST(FanFaultIntegration, ShouldNotSetFeedbackPresentWhenFanIsOffFaultUntilItReachesConsecutiveFeedbackPresentWhenOffNumberOfMinutesWithNonZeroInputCaptureAndFanVotedSpeedOffAndFanVotedSpeedWasNonZeroForOneOfThePeriodicChecks)
{
   GivenApplicationHasBeenInitialized();
   GivenFanFeedbackPresentWhenFanIsOffFaultIs(CLEAR);
   GivenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });
   GivenCondenserFanInputCaptureTimeIs(NonZeroInputCapture);

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   After(MSEC_PER_MIN);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_Care });

   After(condenserFanData.feedbackPresentWhenFanOffTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(CLEAR);

   After(1);
   FanFeedbackPresentWhenFanIsOffFaultShouldBe(SET);
}

// Cannot Reach Target RPM

TEST(FanFaultIntegration, ShouldSetCannotReachTargetRpmFaultWhenPwmIs100ForCannotReachTargetFaultTimeoutInMinutesAndFanVotedSpeedIsNonZeroAndANonZeroInputCapture)
{
   GivenApplicationHasBeenInitialized();
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenCondenserFanInputCaptureTimeIs(UINT32_MAX);
   FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   After(MSEC_PER_MIN);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(condenserFanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   FanPwmShouldBe(PwmDutyCycle_Max);
   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultIntegration, ShouldSetCannotReachTargetRpmFaultWhenPwmIs100ForCannotReachTargetFaultTimeoutInMinutesAndFanVotedSpeedIsNonZeroAndZeroInputCapture)
{
   GivenApplicationHasBeenInitialized();
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenCondenserFanInputCaptureTimeIs(0);
   FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   After(MSEC_PER_MIN);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(condenserFanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   FanPwmShouldBe(PwmDutyCycle_Max);
   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}

TEST(FanFaultIntegration, ShouldNotSetCannotReachTargetRpmFaultWhenPwmIsPwmDutyCycle_MaxAndVotedFanSpeedIsDutyCycleWithANonZeroDutyCycle)
{
   GivenApplicationHasBeenInitialized();
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenCondenserFanInputCaptureTimeIs(UINT32_MAX);
   FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   After(MSEC_PER_MIN);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_SuperHigh, .care = Vote_Care });

   After(condenserFanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   FanPwmShouldBe(PwmDutyCycle_Max);
   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldClearCannotReachTargetRpmFaultWhenPwmDropsBelow95PercentAfterOneMinute)
{
   GivenApplicationHasBeenInitialized();
   GivenFanCannotReachTargetRpmFaultIs(CLEAR);
   GivenCondenserFanInputCaptureTimeIs(UINT32_MAX);
   FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Medium, .care = Vote_Care });
   After(MSEC_PER_MIN);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   After(condenserFanData.cannotReachTargetFaultTimeoutInMinutes * MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);

   FanPwmShouldBe(PwmDutyCycle_Max);
   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   WhenCondenserFanInputCaptureTimeIs(USEC_PER_MIN / (condenserFanData.careAboutCoolingModeSpeedData->careAboutSetpointData.setpointSpeeds.mediumSpeedFreezer.rpm * condenserFanData.pulsesPerRevolution) - 10000);

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   FanPwmShouldBe(0); // it drops to 0 over 1 minute b/c the input capture time is < time necessary for the rpm it's trying to reach
   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldClearCannotReachTargetRpmFaultWhenFanVotedSpeedRpmChangesAfterOneMinute)
{
   GivenApplicationHasBeenInitialized();
   GivenFanCannotReachTargetRpmFaultIs(SET);
   FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_High, .care = Vote_Care });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldClearCannotReachTargetRpmFaultWhenFanVotedSpeedRpmChangesToADutyCycleAfterOneMinute)
{
   GivenApplicationHasBeenInitialized();
   GivenFanCannotReachTargetRpmFaultIs(SET);
   FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_SuperHigh, .care = Vote_Care });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(CLEAR);
}

TEST(FanFaultIntegration, ShouldNotClearCannotReachTargetRpmFaultWhenFanVotedSpeedDutyCycleDoesNotChangeAfterOneMinute)
{
   GivenApplicationHasBeenInitialized();
   GivenFanCannotReachTargetRpmFaultIs(SET);
   FanFactoryVotedSpeedShouldBe({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   WhenFanFactoryVotedSpeedIs({ .speed = FanSpeed_Off, .care = Vote_DontCare });

   After(MSEC_PER_MIN - 1);
   FanCannotReachTargetRpmFaultShouldBe(SET);

   After(1);
   FanCannotReachTargetRpmFaultShouldBe(SET);
}
