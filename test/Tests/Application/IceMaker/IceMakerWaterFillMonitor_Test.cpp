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
#include "FlowMeterMonitoringRequest.h"
#include "IceMakerWaterFillMonitoringRequest.h"
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

   TimedIceMakerFillMaxTimeInSecx10 = 31,
   PreciseIceMakerFillMaxTimeInSecx10 = 100,

   TimedIceMakerFillMaxTimeInMsec = TimedIceMakerFillMaxTimeInSecx10 * MSEC_PER_SEC / 10,
   PreciseIceMakerFillMaxTimeInMsec = PreciseIceMakerFillMaxTimeInSecx10 * MSEC_PER_SEC / 10,

   MaxAmountOfWaterDispensedInOzx100 = 213,
};

static const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_StopIceMakerFillSignal, sizeof(Signal_t) },
   { Erd_FlowMeterWaterDispensedOzX100, sizeof(uint32_t) },
   { Erd_IceMakerWaterFillMonitoringRequest, sizeof(IceMakerWaterFillMonitoringRequest_t) },
   { Erd_FlowMeterMonitoringRequest, sizeof(FlowMeterMonitoringRequest_t) },
   { Erd_TimerModule, sizeof(TimerModule_t *) },
};

static const IceMakerFillMonitorData_t iceMakerFillMonitorData = {
   .timedIceMakerFillInSecondsx10 = TimedIceMakerFillMaxTimeInSecx10,
   .preciseIceMakerFillMaxTimeInSecondsx10 = PreciseIceMakerFillMaxTimeInSecx10,
   .preciseIceMakerFillOuncesInOzx100 = MaxAmountOfWaterDispensedInOzx100
};

static const IceMakerWaterFillMonitorConfig_t config = {
   .stopIceMakerFillSignalErd = Erd_StopIceMakerFillSignal,
   .flowMeterWaterDispensedOzx100Erd = Erd_FlowMeterWaterDispensedOzX100,
   .waterFillMonitoringRequestErd = Erd_IceMakerWaterFillMonitoringRequest,
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

   void GivenNoOuncesWereDispensedAndWaterFillMonitoringHasPaused()
   {
      GivenInitialization();
      GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);
      TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);

      WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Pause);
      TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
   }

   void GivenSomeOuncesWereDispensedAndWaterFillMonitoringHasPaused()
   {
      GivenInitialization();
      GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

      WhenTheFlowMeterWaterDispensedIncrements();

      WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Pause);
      TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
   }

   void GivenSomeOuncesWereDispensedAndWaterFillMonitoringHasResumed()
   {
      GivenSomeOuncesWereDispensedAndWaterFillMonitoringHasPaused();

      WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Resume);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleDouble, ticks, 1000);
   }

   void GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_t request)
   {
      DataModel_Write(dataModel, Erd_IceMakerWaterFillMonitoringRequest, &request);
   }

   void WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_t request)
   {
      GivenTheIceMakerFillMonitoringRequestIs(request);
   }

   void TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_t expected)
   {
      FlowMeterMonitoringRequest_t actual;
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

TEST(IceMakerWaterFillMonitor, ShouldStopFlowMeterMonitoringAndSendStopSignalOncePreciseMaxFillTimeHasBeenElapsedWhenNotEnoughOuncesDispensed)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   WhenTheFlowMeterWaterDispensedIncrements();

   After(PreciseIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldStopFlowMeterMonitoringAndSendStopSignalOnceNonFlowMeterTimeHasBeenElapsedWhenNoOuncesAreDispensed)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   After(TimedIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldStopFlowMeterMonitoringAndSendStopSignalOncePreciseFillFlowMeterWaterHasBeenDispensed)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100 - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldNotStopFlowMeterMonitoringAndNotSendStopRequestUntilAfterTimedIceMakerFillIfIceMakerFillMonitoringRequestIsClearedAfterItWasSet)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Stop);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(TimedIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldNotStopFlowMeterMonitoringAndNotSendStopRequestUntilAfterMaxTimedIceMakerFillIfIceMakerFillMonitoringRequestIsClearedAfterItWasSet)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Stop);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();

   After(PreciseIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldNotStopFlowMeterMonitoringAndNotSendStopRequestUntilAfterPreciseFillFlowMeterWaterHasBeenDispensedIfIceMakerFillMonitoringRequestIsClearedAfterItWasSet)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Stop);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100 - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldStopFlowMeterMonitoringAndSendStopSignalOncePreciseMaxFillTimeHasBeenElapsedWhenNotEnoughOuncesDispensedAndNotNonFlowMeterTimeIfDispensedWaterIsDetected)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   After(TimedIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();
   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(PreciseIceMakerFillMaxTimeInMsec - TimedIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldNotSendMultipleStopMonitoringRequestsInFlowMeterWaterDispensedOncesReachesPreciseIceMakerFillOuncesAfterMaxTimer)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   After(TimedIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIncrements();
   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(PreciseIceMakerFillMaxTimeInMsec - TimedIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100 - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldPauseFlowMeterMonitoringWhenFillMonitoringIsPaused)
{
   GivenInitialization();

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Pause);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
}

TEST(IceMakerWaterFillMonitor, ShouldNotStopFlowMeterMonitoringAndSendStopSignalOnceNonFlowMeterFillMaxTimeHasBeenElapsedWhenNoOuncesAreDispensedAndFillMonitoringIsPaused)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Start);

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Pause);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);

   After(TimedIceMakerFillMaxTimeInMsec);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
   TheStopIceMakerFillSignalShouldBe(0);
}

TEST(IceMakerWaterFillMonitor, ShouldResumeFlowMeterMonitoringWhenFillMonitoringIsResumed)
{
   GivenInitialization();

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Resume);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Resume);
}

TEST(IceMakerWaterFillMonitor, ShouldStopFlowMeterMonitoringAndSendStopSignalWhenNoOuncesAreDispensedAndFillMonitoringIsResumedWhilePaused)
{
   GivenNoOuncesWereDispensedAndWaterFillMonitoringHasPaused();
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Resume);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Resume);

   After(TimedIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Resume);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldNotStopFlowMeterMonitoringAndSendStopSignalOncePreciseMaxFillTimeHasBeenElapsedWhenNotEnoughOuncesDispensedAndFillMonitoringRequestIsPaused)
{
   GivenInitialization();
   GivenTheIceMakerFillMonitoringRequestIs(IceMakerWaterFillMonitoringRequest_Start);

   WhenTheFlowMeterWaterDispensedIncrements();

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Pause);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
   TheStopIceMakerFillSignalShouldBe(0);

   After(PreciseIceMakerFillMaxTimeInMsec);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
   TheStopIceMakerFillSignalShouldBe(0);
}

TEST(IceMakerWaterFillMonitor, ShouldStopFlowMeterMonitoringAndSendStopSignalAfterPreciseFillMaxTimeWhenFillMonitoringIsResumedIfNotEnoughOuncesWereDispensedWhenFillMonitoringWasPaused)
{
   GivenSomeOuncesWereDispensedAndWaterFillMonitoringHasPaused();
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);

   WhenTheIceMakerFillMonitoringRequestChangesTo(IceMakerWaterFillMonitoringRequest_Resume);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Resume);

   After(PreciseIceMakerFillMaxTimeInMsec - 1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Resume);
   TheStopIceMakerFillSignalShouldBe(0);

   After(1);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}

TEST(IceMakerWaterFillMonitor, ShouldNotStopFlowMeterMonitoringAndSendStopSignalWhenTheMaxAmountOfWaterDispensedWhileFillMonitoringIsPaused)
{
   GivenNoOuncesWereDispensedAndWaterFillMonitoringHasPaused();
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Pause);
   TheStopIceMakerFillSignalShouldBe(0);
}

TEST(IceMakerWaterFillMonitor, ShouldStopFlowMeterMonitoringAndSendStopSignalWhenTheMaxAmountOfWaterDispensedAfterFillMonitoringIsResumed)
{
   GivenSomeOuncesWereDispensedAndWaterFillMonitoringHasResumed();
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Resume);
   TheStopIceMakerFillSignalShouldBe(0);

   WhenTheFlowMeterWaterDispensedIsSetTo(MaxAmountOfWaterDispensedInOzx100);
   TheFlowMeterMonitoringRequestShouldBe(FlowMeterMonitoringRequest_Stop);
   TheStopIceMakerFillSignalShouldBe(1);
}
