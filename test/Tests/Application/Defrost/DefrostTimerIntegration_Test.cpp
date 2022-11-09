/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostPlugin.h"
#include "PeriodicNvUpdaterPlugin.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

#define PowerUpDelayInMs(_gridPeriodicRunRateInMSec) (5 * _gridPeriodicRunRateInMSec)

enum
{
   INVALID = false,
   VALID = true
};

enum
{
   PeriodicTimerTicksInMs = 1 * MSEC_PER_SEC
};

enum
{
   TemperatureLessThanPrechillFreezerSetpoint = -700,
   TemperatureGreaterThanPrechillFreezerSetpoint = -500,
   SomeMaxPrechillTimeInMinutes = 5,
   SomeAdjustedFreezerSetpointDegFx100 = 100,
};

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .gridLinesDegFx100 = { 0, -450, 150, 450, 950, 1150 }
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .gridLinesDegFx100 = { -250, 0, 250, 600, 750, 5500 }
};

static CalculatedGridLines_t calcGridLines = {
   .freshFoodGridLine = freshFoodCalcAxis,
   .freezerGridLine = freezerCalcAxis
};

TEST_GROUP(DefrostTimerIntegration)
{
   DefrostPlugin_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;

   PeriodicNvUpdaterPlugin_t periodicNvUpdaterPlugin;
   const SystemMonitorData_t *systemMonitorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      systemMonitorData = PersonalityParametricData_Get(dataModel)->systemMonitorData;
   }

   void CalculatedGridLinesAre(CalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_Grid_CalculatedGridLines, &gridLines);
   }

   void PluginsAreInitialized()
   {
      CalculatedGridLinesAre(calcGridLines);

      DataModel_Write(
         dataModel,
         Erd_SensorsReadyToBeRead,
         set);

      DataModel_Write(
         dataModel,
         Erd_SetpointResolverReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_ConvertibleCompartmentStateResolverReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_OverrideArbiterReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_GridPluginReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_PeriodicNvUpdaterReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_SabbathPluginReady,
         set);

      PeriodicNvUpdaterPlugin_Init(&periodicNvUpdaterPlugin, dataModel);
      DefrostPlugin_Init(&instance, dataModel);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void DefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void DefrostHsmStateShouldBe(DefrostHsmState_t expectedState)
   {
      DefrostHsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DefrostStateShouldBe(DefrostState_t expectedState)
   {
      DefrostState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void PluginsAreInitializedAndDefrostHsmStateIsIdle()
   {
      Given DefrostStateIs(DefrostState_Idle);
      And PluginsAreInitialized();

      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   }

   void CompressorIsOn()
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, on);
   }

   void DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_t expectedState)
   {
      DefrostCompressorOnTimeCounterFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeCounterFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void CompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void CompressorOnTimeInSecondsShouldBe(uint32_t expectedSeconds)
   {
      uint32_t actualSeconds;
      DataModel_Read(dataModel, Erd_DefrostCompressorOnTimeInSeconds, &actualSeconds);

      CHECK_EQUAL(expectedSeconds, actualSeconds);
   }

   void EepromCompressorOnTimeInSecondsIs(uint32_t seconds)
   {
      DataModel_Write(dataModel, Erd_Eeprom_DefrostCompressorOnTimeInSeconds, &seconds);
   }

   void EepromCompressorOnTimeInSecondsShouldBe(uint32_t expectedSeconds)
   {
      uint32_t actualSeconds;
      DataModel_Read(dataModel, Erd_Eeprom_DefrostCompressorOnTimeInSeconds, &actualSeconds);

      CHECK_EQUAL(expectedSeconds, actualSeconds);
   }

   void BoardHasBeenRunningForOneHour()
   {
      Given DefrostStateIs(DefrostState_Idle);
      And CompressorIsOn();

      When PluginsAreInitialized();

      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
      DefrostStateShouldBe(DefrostState_Idle);
      DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);

      After(systemMonitorData->periodicNvUpdateInMinutes * MSEC_PER_MIN - 1);
      CompressorOnTimeInSecondsShouldBe(1799);

      After(1);
      CompressorOnTimeInSecondsShouldBe(1800);
   }

   void BoardResetsAndFreezerIsTooWarm()
   {
      FilteredFreezerCabinetTemperatureIs(freezerCalcAxis.gridLinesDegFx100[GridLine_FreezerExtremeHigh] + 1);
      PluginsAreInitializedAndDefrostHsmStateIsIdle();
   }

   void BoardResetsAndFreezerIsNormalTemperature()
   {
      FilteredFreezerCabinetTemperatureIs(1000);
      PluginsAreInitializedAndDefrostHsmStateIsIdle();
   }

   void CompressorIsOnForThirtyMinutes()
   {
      After(systemMonitorData->periodicNvUpdateInMinutes * MSEC_PER_MIN - 1);
      CompressorOnTimeInSecondsShouldBe(1799);
      EepromCompressorOnTimeInSecondsShouldBe(0);

      After(1);
      CompressorOnTimeInSecondsShouldBe(1800);
      EepromCompressorOnTimeInSecondsShouldBe(1799);
   }

   void CompressorIsOnForLessThanThirtyMinutes()
   {
      // Before the 30 minute copy of RAM to NV
      After((systemMonitorData->periodicNvUpdateInMinutes - 10) * MSEC_PER_MIN - 1);
      CompressorOnTimeInSecondsShouldBe(1199);
      EepromCompressorOnTimeInSecondsShouldBe(0);

      After(1);
      CompressorOnTimeInSecondsShouldBe(1200);
      EepromCompressorOnTimeInSecondsShouldBe(0);
   }
};

TEST(DefrostTimerIntegration, ShouldInitialize)
{
   PluginsAreInitialized();
}

TEST(DefrostTimerIntegration, ShouldCountWhenDefrostStateIsIdleOnStartUp)
{
   Given DefrostStateIs(DefrostState_Idle);
   And CompressorIsOn();

   When PluginsAreInitialized();

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostStateShouldBe(DefrostState_Idle);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);

   After(systemMonitorData->periodicNvUpdateInMinutes * MSEC_PER_MIN - 1);
   CompressorOnTimeInSecondsShouldBe(1799);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   After(1);
   CompressorOnTimeInSecondsShouldBe(1800);
   EepromCompressorOnTimeInSecondsShouldBe(1799);
}

TEST(DefrostTimerIntegration, ShouldResetCountWhenBoardResetsIntoDefrostStateIdleAndFreezerTemperatureIsTooWarm)
{
   Given BoardHasBeenRunningForOneHour();

   BoardResetsAndFreezerIsTooWarm();
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0);
}

TEST(DefrostTimerIntegration, ShouldResetEepromErdToZeroWhenRamErdIsResetToZeroToPreventCarryOverBetweenResets)
{
   Given BoardHasBeenRunningForOneHour();

   BoardResetsAndFreezerIsTooWarm();
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   CompressorIsOnForLessThanThirtyMinutes();

   BoardResetsAndFreezerIsNormalTemperature();
   // If it hadn't reset the EEPROM ERD to 0 it'd be 1799 for both RAM and EEPROM ERDs, then RAM would have been reset to 0 b/c FZ is too warm
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0); // when RAM ERD is set to 0, it'll update EEPROM ERD to 0 immediately
}
