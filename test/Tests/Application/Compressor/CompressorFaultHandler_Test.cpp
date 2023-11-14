/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorFaultHandler.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   ExcessiveCompressorRunTimeInMinutes = 1440,
   ExcessiveCompressorRunTimeInMsec = ExcessiveCompressorRunTimeInMinutes * MSEC_PER_MIN,
   AVeryExcessiveCompressorRunTimeInMsec = 5 * ExcessiveCompressorRunTimeInMsec,

   CompressorOffCoolingOnMaxAllowedTimeInMinutes = 180,
   CompressorOffCoolingOnMaxAllowedTimeInMsec = CompressorOffCoolingOnMaxAllowedTimeInMinutes * MSEC_PER_MIN,
};

static const CompressorFaultHandlerConfig_t config = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingOffStatusErd = Erd_CoolingOffStatus,
   .excessiveCompressorRunTimeFaultErd = Erd_ExcessiveCompressorRunTimeFault,
   .compressorOffWhileCoolingSystemOnForMaxTimeFaultErd = Erd_CompressorOffWhileCoolingSystemOnForMaxTimeFault,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(CompressorFaultHandler)
{
   CompressorFaultHandler_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   CompressorTimes_t compressorTimes;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      memset(&compressorTimes, 0, sizeof(CompressorTimes_t));
      compressorTimes.excessiveRunTimeInMinutes = ExcessiveCompressorRunTimeInMinutes;
      compressorTimes.compressorOffCoolingOnMaxAllowedTimeInMinutes = CompressorOffCoolingOnMaxAllowedTimeInMinutes;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenInitialization()
   {
      CompressorFaultHandler_Init(
         &instance,
         dataModel,
         &config,
         &compressorTimes);
   }

   void WhenCompressorTurns(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, &state);
   }

   void GivenInitialCycleCompleted()
   {
      WhenCompressorTurns(ON);
      WhenCompressorTurns(OFF);
   }

   void WhenInitialCycleCompletes()
   {
      WhenCompressorTurns(ON);
      WhenCompressorTurns(OFF);
   }

   void ExcessiveCompressorRunTimeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_ExcessiveCompressorRunTimeFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenExcessiveRunTimeFaultIsSet()
   {
      GivenInitialCycleCompleted();
      WhenCompressorTurns(ON);
      After(ExcessiveCompressorRunTimeInMsec);
      ExcessiveCompressorRunTimeFaultShouldBe(SET);
   }

   void GivenCoolingSystemIs(bool status)
   {
      status = !status;
      DataModel_Write(dataModel, Erd_CoolingOffStatus, &status);
   }

   void WhenCoolingSystemTurns(bool status)
   {
      GivenCoolingSystemIs(status);
   }

   void GivenCompressorIs(bool status)
   {
      WhenCompressorTurns(status);
   }

   void CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorOffWhileCoolingSystemOnForMaxTimeFault, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void GivenCompressorOffWhileCoolingSystemOnForMaxTimeFaultIsSet()
   {
      WhenCoolingSystemTurns(ON);
      After(CompressorOffCoolingOnMaxAllowedTimeInMsec);
      CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(SET);
   }
};

TEST(CompressorFaultHandler, ShouldSetExcessiveCompressorRunTimeFaultWhenInitialCycleHasRunAndCompressorIsOnLongerThanExcessiveRunTime)
{
   GivenInitialization();
   WhenInitialCycleCompletes();

   WhenCompressorTurns(ON);
   After(ExcessiveCompressorRunTimeInMsec - 1);
   ExcessiveCompressorRunTimeFaultShouldBe(CLEAR);

   After(1);
   ExcessiveCompressorRunTimeFaultShouldBe(SET);
}

TEST(CompressorFaultHandler, ShouldClearExcessiveCompressorRunTimeFaultWhenCompressorTurnsOffAfterTheFaultIsSet)
{
   GivenInitialization();
   GivenExcessiveRunTimeFaultIsSet();

   WhenCompressorTurns(OFF);
   ExcessiveCompressorRunTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldNotSetExcessiveCompressorRunTimeFaultWhenInitialCycleRunsForLongerThanExcessiveRunTime)
{
   GivenInitialization();

   WhenCompressorTurns(ON);
   After(ExcessiveCompressorRunTimeInMsec - 1);
   ExcessiveCompressorRunTimeFaultShouldBe(CLEAR);

   After(1);
   ExcessiveCompressorRunTimeFaultShouldBe(CLEAR);

   After(AVeryExcessiveCompressorRunTimeInMsec);
   ExcessiveCompressorRunTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldNotSetExcessiveRunTimeFaultWhenTheCompressorTurnsOffWithinTheExcessiveRunTimeTimeout)
{
   GivenInitialization();
   GivenInitialCycleCompleted();

   WhenCompressorTurns(ON);
   After(ExcessiveCompressorRunTimeInMsec - 1);
   ExcessiveCompressorRunTimeFaultShouldBe(CLEAR);

   WhenCompressorTurns(OFF);
   After(1);
   ExcessiveCompressorRunTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenCompressorIsOffWhileCoolingSystemIsOnForMoreThanMaxAllowedTime)
{
   GivenCoolingSystemIs(ON);
   GivenCompressorIs(ON);
   GivenInitialization();

   WhenCompressorTurns(OFF);
   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(SET);
}

TEST(CompressorFaultHandler, ShouldNotSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenCompressorTurnsOffWhileCoolingSystemIsOff)
{
   GivenCoolingSystemIs(OFF);
   GivenCompressorIs(ON);
   GivenInitialization();

   WhenCompressorTurns(OFF);
   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldNotSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCompressorTurnsOnWhileTheCoolingSystemIsOnWithinTheMaxAllowedTime)
{
   GivenCoolingSystemIs(ON);
   GivenCompressorIs(ON);
   GivenInitialization();

   WhenCompressorTurns(OFF);
   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   WhenCompressorTurns(ON);
   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldNotSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCoolingSystemTurnsOffWithinTheMaxAllowedTime)
{
   GivenCoolingSystemIs(ON);
   GivenCompressorIs(ON);
   GivenInitialization();

   WhenCompressorTurns(OFF);
   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   WhenCoolingSystemTurns(OFF);
   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCompressorIsOffAndTheCoolingSystemTurnsOnForMoreThanMaxAllowedTime)
{
   GivenCompressorIs(OFF);
   GivenCoolingSystemIs(OFF);
   GivenInitialization();

   WhenCoolingSystemTurns(ON);
   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(SET);
}

TEST(CompressorFaultHandler, ShouldClearCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCompressorTurnsOn)
{
   GivenCoolingSystemIs(OFF);
   GivenInitialization();

   GivenCompressorOffWhileCoolingSystemOnForMaxTimeFaultIsSet();
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(SET);

   WhenCompressorTurns(ON);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldNotClearTheCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCoolingSystemTurnsOff)
{
   GivenCoolingSystemIs(OFF);
   GivenInitialization();

   GivenCompressorOffWhileCoolingSystemOnForMaxTimeFaultIsSet();
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(SET);

   WhenCoolingSystemTurns(OFF);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(SET);
}

TEST(CompressorFaultHandler, ShouldNotSetTheCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCompressorIsOnAndTheCoolingSystemTurnsOn)
{
   GivenCoolingSystemIs(OFF);
   GivenCompressorIs(ON);
   GivenInitialization();

   WhenCoolingSystemTurns(ON);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   After(CompressorOffCoolingOnMaxAllowedTimeInMsec);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCompressorIsOffAndTheCoolingSystemIsOnForMoreThanMaxAllowedTimeOnInit)
{
   GivenCompressorIs(OFF);
   GivenCoolingSystemIs(ON);
   GivenInitialization();

   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(SET);
}

TEST(CompressorFaultHandler, ShouldNotSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCompressorIsOnAndTheCoolingSystemIsOnForMoreThanMaxAllowedTimeOnInit)
{
   GivenCompressorIs(ON);
   GivenCoolingSystemIs(ON);
   GivenInitialization();

   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);
}

TEST(CompressorFaultHandler, ShouldNotSetCompressorOffWhileCoolingSystemOnForMaxTimeFaultWhenTheCompressorIsOffAndTheCoolingSystemIsOffForMoreThanMaxAllowedTimeOnInit)
{
   GivenCompressorIs(OFF);
   GivenCoolingSystemIs(OFF);
   GivenInitialization();

   After(CompressorOffCoolingOnMaxAllowedTimeInMsec - 1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);

   After(1);
   CompressorOffWhileCoolingSystemOnForMaxTimeFaultShouldBe(CLEAR);
}
