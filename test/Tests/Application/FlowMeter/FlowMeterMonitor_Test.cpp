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
   .flowMeterCountsErd = Erd_FlowMeter_InputCaptureCount,
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

   void GivenTheMonitoringRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FlowMeterMonitoringRequest, &state);
   }

   void WhenTheMonitoringRequestIs(bool state)
   {
      GivenTheMonitoringRequestIs(state);
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
   GivenTheMonitoringRequestIs(CLEAR);
   GivenTheOuncesDispensedAre(SomeOunces);
   GivenTheModuleIsInitialized();
   TheOuncesDispensedShouldBe(0);
}

TEST(FlowMeterMonitor, ShouldNotUpdateOuncesDispensedWhenMonitoringNotRequested)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(CLEAR);

   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed - 1);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);
}

TEST(FlowMeterMonitor, ShouldNotUpdateOuncesDispensedWhenMonitoringRequestIsSetButNotEnoughPulsesHaveBeenObserved)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(SET);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed - 1);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);
}

TEST(FlowMeterMonitor, ShouldUpdateOuncesDispensedWhenMonitoringRequestIsSetAndEnoughPulsesHaveBeenObserved)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(SET);
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

TEST(FlowMeterMonitor, ShouldUpdateOuncesDispensedWithEnoughPrecisionWhenMonitoringRequestIsSetAndManyPulsesHaveBeenObservedOverSomeTime)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(SET);
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

TEST(FlowMeterMonitor, ShouldUpdateOuncesDispensedWhenMonitoringRequestIsSetOnInit)
{
   GivenTheMonitoringRequestIs(SET);
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
   WhenTheMonitoringRequestIs(SET);
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

TEST(FlowMeterMonitor, ShouldClearAndStopUpdatingOuncesDispensedWhenMonitoringRequestIsCleared)
{
   GivenTheModuleIsInitialized();

   WhenTheFlowMeterCountsAre(SomeCounts);
   WhenTheMonitoringRequestIs(SET);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(MinimumNumberOfPulsesBeforePublishingOuncesDispensed * flowMeterData->flowMeterOzPerPulseX100000 / 1000 +
      flowMeterData->flowMeterOffsetOzX100);

   WhenTheMonitoringRequestIs(CLEAR);
   TheOuncesDispensedShouldBe(0);

   WhenTheFlowMeterCountsAre(SomeCounts + 2 * MinimumNumberOfPulsesBeforePublishingOuncesDispensed);
   After(PollingPeriodInMsec);
   TheOuncesDispensedShouldBe(0);
}
