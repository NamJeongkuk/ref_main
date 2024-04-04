/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SideBySideDefrostPlugin.h"
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
   WaitingToDefrostUpdatePeriodInSeconds = 60,
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

static TwoDimensionalCalculatedGridLines_t calcGridLines = {
   .firstDimensionGridLines = freshFoodCalcAxis,
   .secondDimensionGridLines = freezerCalcAxis,
   .numberOfGridLinesPerDimension = NUM_ELEMENTS(freshFoodCalcAxis.gridLinesDegFx100)
};

TEST_GROUP(DefrostTimerIntegration)
{
   SideBySideDefrostPlugin_t instance;

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

   void CalculatedGridLinesAre(TwoDimensionalCalculatedGridLines_t gridLines)
   {
      DataModel_Write(dataModel, Erd_FreshFoodAndFreezerGrid_CalculatedGridLines, &gridLines);
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
         Erd_AmbientTemperaturePluginReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_FreshFoodAndFreezerGridPluginReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_PeriodicNvUpdaterReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_SabbathPluginReady,
         set);

      DataModel_Write(
         dataModel,
         Erd_FreezerThermistor_IsValidResolved,
         set);

      PeriodicNvUpdaterPlugin_Init(&periodicNvUpdaterPlugin, dataModel);
      SideBySideDefrostPlugin_Init(&instance, dataModel);
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

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
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

   void CompressorIsOff()
   {
      DataModel_Write(dataModel, Erd_CompressorIsOn, off);
   }

   void ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_t expected)
   {
      ReadyToDefrostHsmState_t actual;
      DataModel_Read(dataModel, Erd_ReadyToDefrostHsmState, &actual);

      CHECK_EQUAL(expected, actual);
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

   void BoardHasBeenRunningForPeriodicNvUpdateTimeInMinutes()
   {
      Given DefrostStateIs(DefrostState_Idle);
      And CompressorIsOn();

      When PluginsAreInitialized();

      DefrostHsmStateShouldBe(DefrostHsmState_Idle);
      DefrostStateShouldBe(DefrostState_Idle);
      ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

      After(systemMonitorData->periodicNvUpdateInMinutes * MSEC_PER_MIN);
      When CompressorIsOff();
      CompressorOnTimeInSecondsShouldBe((systemMonitorData->periodicNvUpdateInMinutes * SECONDS_PER_MINUTE));
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
      CompressorOnTimeInSecondsShouldBe((systemMonitorData->periodicNvUpdateInMinutes * SECONDS_PER_MINUTE) - 1);
      EepromCompressorOnTimeInSecondsShouldBe(0);

      After(1);
      CompressorOnTimeInSecondsShouldBe((systemMonitorData->periodicNvUpdateInMinutes * SECONDS_PER_MINUTE));
      EepromCompressorOnTimeInSecondsShouldBe((systemMonitorData->periodicNvUpdateInMinutes * SECONDS_PER_MINUTE) - 1);
   }

   void CompressorIsOnForLessThanPeriodicNvUpdateTimeInMinutes()
   {
      When CompressorIsOn();
      After((systemMonitorData->periodicNvUpdateInMinutes) * MSEC_PER_MIN - 1);
      When CompressorIsOff();

      CompressorOnTimeInSecondsShouldBe((systemMonitorData->periodicNvUpdateInMinutes) * SECONDS_PER_MINUTE - 1);
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
   ReadyToDefrostHsmStateShouldBe(ReadyToDefrostHsmState_WaitingForMinimumTimeBetweenDefrosts);

   After(systemMonitorData->periodicNvUpdateInMinutes * MSEC_PER_MIN - 1);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   When CompressorIsOff();
   CompressorOnTimeInSecondsShouldBe(systemMonitorData->periodicNvUpdateInMinutes * SECONDS_PER_MINUTE - 1);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   After(1);
   CompressorOnTimeInSecondsShouldBe(systemMonitorData->periodicNvUpdateInMinutes * SECONDS_PER_MINUTE - 1);
   EepromCompressorOnTimeInSecondsShouldBe(systemMonitorData->periodicNvUpdateInMinutes * SECONDS_PER_MINUTE - 1);
}

TEST(DefrostTimerIntegration, ShouldResetCountWhenBoardResetsIntoDefrostStateIdleAndFreezerTemperatureIsTooWarm)
{
   Given BoardHasBeenRunningForPeriodicNvUpdateTimeInMinutes();

   BoardResetsAndFreezerIsTooWarm();
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0);
}

TEST(DefrostTimerIntegration, ShouldResetEepromErdToZeroWhenRamErdIsResetToZero)
{
   Given BoardHasBeenRunningForPeriodicNvUpdateTimeInMinutes();

   BoardResetsAndFreezerIsTooWarm();
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   CompressorIsOnForLessThanPeriodicNvUpdateTimeInMinutes();

   BoardResetsAndFreezerIsNormalTemperature();
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0);
}
