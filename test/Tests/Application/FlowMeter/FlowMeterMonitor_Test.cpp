/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FlowMeterMonitor.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"

enum
{
   PollingPeriodInMsec = 100,
   MinimumNumberOfPulsesBeforePublishingOuncesDispensed = 2,
   SomeCounts = 10,
   ManyCounts = 99,
   SomeOunces = 3
};

static const FlowMeterMonitorConfig_t config = {
   .flowMeterMonitoringRequest = Erd_FlowMeterMonitoringRequest,
   .flowMeterInputCaptureCountsErd = Erd_FlowMeter_InputCaptureCount,
   .flowMeterWaterDispensedOzX100Erd = Erd_FlowMeterWaterDispensedOzX100
};

TEST_GROUP(FlowMeterMonitor)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t *personalityParametricData;
   const FlowMeterData_t *flowMeterData;
   FlowMeterMonitor_t instance;
   TimerModule_t *timerModule;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      DataModel_Read(
         dataModel,
         Erd_PersonalityParametricData,
         &personalityParametricData);
      flowMeterData = personalityParametricData->flowMeterData;
   }

   void GivenTheModuleIsInitialized()
   {
      FlowMeterMonitor_Init(
         &instance,
         dataModel,
         &config,
         flowMeterData);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void GivenTheOuncesDispensedAre(uint32_t ounces)
   {
      DataModel_Write(dataModel, Erd_FlowMeterWaterDispensedOzX100, &ounces);
   }

   void GivenTheFlowMeterCountsAre(InputCaptureCounts_t counts)
   {
      DataModel_Write(dataModel, Erd_FlowMeter_InputCaptureCount, &counts);
   }

   void WhenTheFlowMeterCountsAre(InputCaptureCounts_t counts)
   {
      GivenTheFlowMeterCountsAre(counts);
   }

   void GivenTheMonitoringRequestIs(FlowMeterMonitoringRequest_t request)
   {
      DataModel_Write(dataModel, Erd_FlowMeterMonitoringRequest, &request);
   }

   void WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_t request)
   {
      GivenTheMonitoringRequestIs(request);
   }

   void TheOuncesDispensedShouldBe(uint32_t expected)
   {
      uint32_t actual;
      DataModel_Read(dataModel, Erd_FlowMeterWaterDispensedOzX100, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(FlowMeterMonitor, ShouldClearOuncesDispensedOnInitWhenMonitoringNotRequested)
{
   GivenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Stop);
   GivenTheOuncesDispensedAre(SomeOunces);
   GivenTheModuleIsInitialized();
   TheOuncesDispensedShouldBe(0);
}

TEST(FlowMeterMonitor, ShouldNotUpdateOuncesDispensedWhenMonitoringNotRequested)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Stop);

   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed - 1);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);
}

TEST(FlowMeterMonitor, ShouldNotUpdateOuncesDispensedWhenMonitoringIsStartedButNotEnoughPulsesHaveBeenObserved)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed - 1);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);
}

TEST(FlowMeterMonitor, ShouldUpdateOuncesDispensedWhenMonitoringIsStartedAndEnoughPulsesHaveBeenObserved)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed - 1);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed + 1);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe((MinimumNumberOfPulsesBeforePublishingOuncesDispensed + 1) * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);
}

TEST(FlowMeterMonitor, ShouldUpdateOuncesDispensedWithEnoughPrecisionWhenMonitoringIsStartedAndManyPulsesHaveBeenObservedOverSomeTime)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   for(uint8_t i = 1; i < 10; i++)
   {
      WhenTheFlowMeterCountsAre(SomeCounts + i * ManyCounts);
      After(PollingPeriodInMsec);
      TheOuncesDispensedShouldBe(i * ManyCounts * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
         flowMeterData->flowMeterOffsetOzX100);
   }
}

TEST(FlowMeterMonitor, ShouldUpdateOuncesDispensedWhenMonitoringIsStartedOnInit)
{
   GivenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);
   GivenTheFlowMeterCountsAre(SomeCounts);
   GivenTheModuleIsInitialized();

   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);
}

TEST(FlowMeterMonitor, ShouldUpdateOuncesDispensedCorrectlyWhenPulsesOverflows)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(UINT32_MAX - 1);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(UINT32_MAX);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(1);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);
}

TEST(FlowMeterMonitor, ShouldClearAndStopUpdatingOuncesDispensedWhenMonitoringIsStopped)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);

   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Stop);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + 2 * MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);
}

TEST(FlowMeterMonitor, ShouldNotClearAndPauseUpdatingOuncesDispensedWhenMonitoringRequestIsPaused)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);

   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Pause);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);

   WhenTheFlowMeterCountsAre(SomeCounts + 2 * MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);
}

TEST(FlowMeterMonitor, ShouldResumeUpdatingOuncesDispenseWhenMonitoringRequestChangesToResumeWhilePaused)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Start);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec - 1);
   TheOuncesDispensedShouldBe(0);

   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Pause);
   After(1);
   TheOuncesDispensedShouldBe(0);

   WhenTheMonitoringRequestIs(FlowMeterMonitoringRequest_Resume);
   After(1);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);
}
