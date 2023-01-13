/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceMakerWaterFillMonitor.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "Signal.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"

enum
{
   Erd_StopIceMakerFillSignal,
   Erd_FlowMeterWaterDispensedOzX100,
   Erd_IceMakerWaterFillMonitoringRequest,
   Erd_FlowMeterMonitoringRequest,
   Erd_TimerModule,

   SomeTimedIceMakerFillInSec = 31,
   SomePreciseIceMakerFillMaxTimeInSec = 100,

   SomeTimedIceMakerFillInMsec = SomeTimedIceMakerFillInSec * MSEC_PER_SEC / 10,
   SomePreciseIceMakerFillMaxTimeInMsec = SomePreciseIceMakerFillMaxTimeInSec * MSEC_PER_SEC / 10,

   MaxAmountOfWaterDispensedInOzx100 = 213,
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_StopIceMakerFillSignal, sizeof(Signal_t) },
   { Erd_FlowMeterWaterDispensedOzX100, sizeof(uint32_t) },
   { Erd_IceMakerWaterFillMonitoringRequest, sizeof(bool) },
   { Erd_FlowMeterMonitoringRequest, sizeof(bool) },
   { Erd_TimerModule, sizeof(TimerModule_t *) },
};

static const IceMakerFillMonitorData_t iceMakerFillMonitorData = {
   .timedIceMakerFillInSecondsx10 = SomeTimedIceMakerFillInSec,
   .preciseIceMakerFillMaxTimeInSecondsx10 = SomePreciseIceMakerFillMaxTimeInSec,
   .preciseIceMakerFillOuncesInOzx100 = MaxAmountOfWaterDispensedInOzx100
};

static const IceMakerWaterFillMonitorConfig_t config = {
   .stopIceMakerFillSignalErd = Erd_StopIceMakerFillSignal,
   .flowMeterWaterDispensedOzx100Erd = Erd_FlowMeterWaterDispensedOzX100,
   .iceMakerWaterFillMonitoringRequestErd = Erd_IceMakerWaterFillMonitoringRequest,
   .flowMeterMonitoringRequestErd = Erd_FlowMeterMonitoringRequest,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(IceMakerWaterFillMonitor)
{
   IceMakerWaterFillMonitor_t instance;
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleDouble;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

      TimerModule_TestDouble_Init(&timerModuleDouble);
      DataModel_WritePointer(dataModel, Erd_TimerModule, &timerModuleDouble.timerModule);
   }

   void GivenInitialization()
   {
      IceMakerWaterFillMonitor_Init(
         &instance,
         dataModel,
         &config,
         &iceMakerFillMonitorData);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleDouble, ticks, 1000);
   }

   void GivenTheIceMakerFillMonitoringRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMakerWaterFillMonitoringRequest, &state);
   }

   void WhenTheIceMakerFillMonitoringRequestChangesTo(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMakerWaterFillMonitoringRequest, &state);
   }

   void TheFlowMeterMonitoringRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FlowMeterMonitoringRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTheFlowMeterWaterDispensedIncrements()
   {
      uint32_t waterDispensed;
      DataModel_Read(dataModel, Erd_FlowMeterWaterDispensedOzX100, &waterDispensed);
      waterDispensed++;
      DataModel_Write(dataModel, Erd_FlowMeterWaterDispensedOzX100, &waterDispensed);
   }

   void WhenTheFlowMeterWaterDispensedIsSetTo(uint32_t someAmountDispensedInOzx100)
   {
      DataModel_Write(dataModel, Erd_FlowMeterWaterDispensedOzX100, &someAmountDispensedInOzx100);
   }

   void TheStopIceMakerFillSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_StopIceMakerFillSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(IceMakerWaterFillMonitor, ShouldClearFlowMeterMonitoringRequestAndSendStopSignalOncePreciseMaxFillTimeHasBeenElapsedWhenNotEnoughOuncesDispensed)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   WhenTheFlowMeterWaterDispensedIncrements();

   After(SomePreciseIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldClearFlowMeterMonitoringRequestAndSendStopSignalOnceNonFlowMeterTimeHasBeenElapsedWhenNoOuncesAreDispensed)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   After(SomeTimedIceMakerFillInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldClearFlowMeterMonitoringRequestAndSendStopSignalOncePreciseFillFlowMeterWaterHasBeenDispensed)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100 - 1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldClearFlowMeterMonitoringRequestAndNotSendAdditionalStopRequestsAfterTimedIceMakerFillIfIceMakerFillMonitoringRequestIsClearedAfterItWasSet)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   WhenTheIceMakerFillMonitoringRequestChangesTo(DISABLED);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);

   After(SomeTimedIceMakerFillInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldClearFlowMeterMonitoringRequestAndNotSendAdditionalStopRequestsAfterMaxTimedIceMakerFillIfIceMakerFillMonitoringRequestIsClearedAfterItWasSet)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   WhenTheIceMakerFillMonitoringRequestChangesTo(DISABLED);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);

   WhenTheFlowMeterWaterDispensedIncrements();

   After(SomePreciseIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldClearFlowMeterMonitoringRequestAndNotSendAdditionalStopRequestsAfterPreciseFillFlowMeterWaterHasBeenDispensedIfIceMakerFillMonitoringRequestIsClearedAfterItWasSet)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   WhenTheIceMakerFillMonitoringRequestChangesTo(DISABLED);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100 - 1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);

   WhenTheFlowMeterWaterDispensedIncrements();
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldClearFlowMeterMonitoringRequestAndSendStopSignalOncePreciseMaxFillTimeHasBeenElapsedWhenNotEnoughOuncesDispensedAndNotNonFlowMeterTimeIfDispensedWaterIsDetected)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   After(SomeTimedIceMakerFillInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();
   After(1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   After(SomePreciseIceMakerFillMaxTimeInMsec - SomeTimedIceMakerFillInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldNotSendMultipleStopMonitoringRequestsInFlowMeterWaterDispensedOncesReachesPreciseIceMakerFillOuncesAfterMaxTimer)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(ENABLED);

   After(SomeTimedIceMakerFillInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();
   After(1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   After(SomePreciseIceMakerFillMaxTimeInMsec - SomeTimedIceMakerFillInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(ENABLED);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100 - 1);
   TheFlowMeterMonitoringRequestShouldBe(DISABLED);
   TheStopIceMakerFillSignalShouldBe(1);
}
