/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "WaterVolumeUsageCalculator.h"
#include "RfidTypes.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "uassert_test.h"
#include "utils.h"

enum
{
   OneSecond = 1,
   TenthOfASecondInMsec = 100,
   SomeTimeInSeconds = 37372,
   SomeOtherTimeInSeconds = 862111,
   SomeRandomTimeInSeconds = 1303297
};

enum
{
   SomeWaterValveFlowRateInOuncesPerSecondX100 = 53,
   MaxWaterValveFlowRateInOuncesPerSecondX100 = 255
};

enum
{
   Erd_WaterValveRelay,
   Erd_WaterVolumeUsageInOuncesX100,
   Erd_TimerModuleDouble = Erd_TimerModule
};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_WaterValveRelay, sizeof(bool) },
   { Erd_WaterVolumeUsageInOuncesX100, sizeof(VolumeInOuncesX100_t) },
   { Erd_TimerModuleDouble, sizeof(TimerModule_t *) }
};

static const WaterVolumeUsageCalculatorConfig_t config = {
   .waterValveRelayErd = Erd_WaterValveRelay,
   .waterVolumeUsageInOuncesX100Erd = Erd_WaterVolumeUsageInOuncesX100
};

TEST_GROUP(WaterVolumeUsageCalculator)
{
   WaterVolumeUsageCalculator_t instance;

   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleTestDouble;

   void setup()
   {
      DataModel_TestDouble_Init(
         &dataModelTestDouble,
         erdTable,
         NUM_ELEMENTS(erdTable));
      dataModel = dataModelTestDouble.dataModel;

      TimerModule_TestDouble_Init(&timerModuleTestDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModuleDouble, &timerModuleTestDouble.timerModule);
   }

   void GivenTheModuleIsInitializedForSomeWaterValveFlowRateInOuncesPerSecondX100()
   {
      WaterVolumeUsageCalculator_Init(
         &instance,
         dataModel,
         &config,
         SomeWaterValveFlowRateInOuncesPerSecondX100);
   }

   void GivenTheModuleIsInitializedForMaxWaterValveFlowRateInOuncesPerSecondX100()
   {
      WaterVolumeUsageCalculator_Init(
         &instance,
         dataModel,
         &config,
         MaxWaterValveFlowRateInOuncesPerSecondX100);
   }

   void GivenTheWaterValveRelayTurns(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_WaterValveRelay,
         &state);
   }

   void WhenTheWaterValveRelayTurns(bool state)
   {
      GivenTheWaterValveRelayTurns(state);
   }

   void WaterVolumeUsageInOuncesX100ShouldBe(VolumeInOuncesX100_t expected)
   {
      VolumeInOuncesX100_t actual;
      DataModel_Read(
         dataModel,
         Erd_WaterVolumeUsageInOuncesX100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks);
   }
};

TEST(WaterVolumeUsageCalculator, ShouldWriteATenthOfSomeWaterValveFlowRateInOuncesX100WhenUsingSomeWaterValveFlowRateInOuncesPerSecondX100AndWaterValveRelayTurnsOnForATenthOfASecond)
{
   GivenTheModuleIsInitializedForSomeWaterValveFlowRateInOuncesPerSecondX100();
   GivenTheWaterValveRelayTurns(ON);

   After(TenthOfASecondInMsec);

   WhenTheWaterValveRelayTurns(OFF);
   WaterVolumeUsageInOuncesX100ShouldBe(SomeWaterValveFlowRateInOuncesPerSecondX100 * OneSecond / 10);
}

TEST(WaterVolumeUsageCalculator, ShouldWriteZeroToWaterVolumeUsageInOuncesX100WhenWaterValveRelayTurnsOn)
{
   GivenTheModuleIsInitializedForSomeWaterValveFlowRateInOuncesPerSecondX100();
   GivenTheWaterValveRelayTurns(ON);

   After(OneSecond * MSEC_PER_SEC);

   WhenTheWaterValveRelayTurns(OFF);
   WaterVolumeUsageInOuncesX100ShouldBe(SomeWaterValveFlowRateInOuncesPerSecondX100 * OneSecond);

   WhenTheWaterValveRelayTurns(ON);
   WaterVolumeUsageInOuncesX100ShouldBe(0);
}

TEST(WaterVolumeUsageCalculator, ShouldWriteTheCorrectWaterVolumeUsageInOuncesX100WhenUsingSomeWaterValveFlowRateInOuncesPerSecondX100AndWaterValveRelayTurnsOnForSomeTimeInSeconds)
{
   GivenTheModuleIsInitializedForSomeWaterValveFlowRateInOuncesPerSecondX100();
   GivenTheWaterValveRelayTurns(ON);

   After(SomeTimeInSeconds * MSEC_PER_SEC);

   WhenTheWaterValveRelayTurns(OFF);
   WaterVolumeUsageInOuncesX100ShouldBe(SomeWaterValveFlowRateInOuncesPerSecondX100 * SomeTimeInSeconds);
}

TEST(WaterVolumeUsageCalculator, ShouldWriteTheCorrectWaterVolumeUsageInOuncesX100WhenUsingSomeWaterValveFlowRateInOuncesPerSecondX100AndWaterValveRelayTurnsOnForSomeOtherTimeInSeconds)
{
   GivenTheModuleIsInitializedForSomeWaterValveFlowRateInOuncesPerSecondX100();
   GivenTheWaterValveRelayTurns(ON);

   After(SomeOtherTimeInSeconds * MSEC_PER_SEC);

   WhenTheWaterValveRelayTurns(OFF);
   WaterVolumeUsageInOuncesX100ShouldBe(SomeWaterValveFlowRateInOuncesPerSecondX100 * SomeOtherTimeInSeconds);
}

TEST(WaterVolumeUsageCalculator, ShouldWriteTheCorrectWaterVolumeUsageInOuncesX100WhenUsingMaxWaterValveFlowRateInOuncesPerSecondX100AndWaterValveRelayTurnsOnForSomeRandomTimeInSeconds)
{
   GivenTheModuleIsInitializedForMaxWaterValveFlowRateInOuncesPerSecondX100();
   GivenTheWaterValveRelayTurns(ON);

   After(SomeRandomTimeInSeconds * MSEC_PER_SEC);

   WhenTheWaterValveRelayTurns(OFF);
   WaterVolumeUsageInOuncesX100ShouldBe(MaxWaterValveFlowRateInOuncesPerSecondX100 * SomeRandomTimeInSeconds);
}
