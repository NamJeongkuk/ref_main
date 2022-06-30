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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "DefrostData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   INVALID = false,
   VALID = true
};

enum
{
   SixGridLines = 6,
   TwoDimensional = 2,
};

enum
{
   DontCare = false,
   Care = true
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
   SomeAdjustedFreezerSetpointDegFx100 = 100
};

static const SabbathData_t sabbathData = {
   .maxTimeBetweenDefrostsInMinutes = 16 * MINUTES_PER_HOUR
};

static const DeltaGridLineData_t freshFoodGridLineData[] = {
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b0010,
   },
   {
      .gridLinesDegFx100 = -450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 150,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 450,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 950,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 1150,
      .bitMapping = 0b1000,
   },
};

static const DeltaGridLineData_t freezerGridLineData[] = {
   {
      .gridLinesDegFx100 = -250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 0,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 250,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 600,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 750,
      .bitMapping = 0b1000,
   },
   {
      .gridLinesDegFx100 = 5500,
      .bitMapping = 0b0010,
   },
};

static const DeltaAxisGridLines_t freshFoodAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freshFoodGridLineData
};

static const DeltaAxisGridLines_t freezerAxis = {
   .numberOfLines = SixGridLines,
   .gridLineData = freezerGridLineData
};

static DeltaAxisGridLines_t parametricGrid[] = { freshFoodAxis, freezerAxis };
static DeltaGridLines_t deltaGrid = {
   .dimensions = TwoDimensional,
   .gridLines = parametricGrid
};

static const GridData_t gridData = {
   .gridId = 0,
   .deltaGridLines = &deltaGrid,
   .gridPeriodicRunRateInMSec = 1 * MSEC_PER_SEC
};

static const EvaporatorData_t singleEvaporatorData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvaporatorData = {
   .numberOfEvaporators = 2
};

#define PowerUpDelayInMs 5 * gridData.gridPeriodicRunRateInMSec

static TemperatureDegFx100_t freshFoodCalcAxisGridLines[] = { 0, -450, 150, 450, 950, 1150 };
static TemperatureDegFx100_t freezerCalcAxisGridLines[] = { -250, 0, 250, 600, 750, 5500 };

static CalculatedAxisGridLines_t freshFoodCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freshFoodCalcAxisGridLines
};

static CalculatedAxisGridLines_t freezerCalcAxis = {
   .numberOfLines = SixGridLines,
   .gridLinesDegFx100 = freezerCalcAxisGridLines
};

static CalculatedAxisGridLines_t calcGrid[] = { freshFoodCalcAxis, freezerCalcAxis };

static CalculatedGridLines_t calcGridLines = {
   .dimensions = TwoDimensional,
   .gridLines = calcGrid
};

static SystemMonitorData_t systemMonitorData = {
   .periodicNvUpdateInMinutes = 60
};

TEST_GROUP(DefrostTimerIntegration)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;
   DefrostData_t defrostData;

   PeriodicNvUpdaterPlugin_t periodicNvUpdaterPlugin;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      DefrostData_TestDouble_Init(&defrostData);
      DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(&defrostData, 60);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetSabbath(&personalityParametricData, &sabbathData);
      PersonalityParametricData_TestDouble_SetGrid(&personalityParametricData, &gridData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &singleEvaporatorData);
      PersonalityParametricData_TestDouble_SetSystemMonitor(&personalityParametricData, &systemMonitorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
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

      PeriodicNvUpdaterPlugin_Init(&periodicNvUpdaterPlugin, dataModel);
      DefrostPlugin_Init(dataModel);
   }

   void FilteredFreezerCabinetTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolved, &temperature);
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

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void PluginsAreInitializedAndDefrostHsmStateIsIdle()
   {
      Given DefrostStateIs(DefrostState_Idle);
      And PluginsAreInitialized();

      After(PowerUpDelayInMs - 1);
      DefrostHsmStateShouldBe(DefrostHsmState_PowerUp);

      After(1);
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
};

TEST(DefrostTimerIntegration, ShouldInitialize)
{
   PluginsAreInitialized();
}

TEST(DefrostTimerIntegration, ShouldResetEepromCompressorOnTimeToZeroWhenRamErdIsSetToZeroSoOldCompressorOnTimeValueIsNotHeldOnToIfMainboardResetsMultipleTimes)
{
   Given EepromCompressorOnTimeInSecondsIs(0);
   And FilteredFreezerCabinetTemperatureIs(1000);
   And CompressorIsOn();
   And PluginsAreInitializedAndDefrostHsmStateIsIdle();

   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   After(60 * MSEC_PER_MIN - 1);
   EepromCompressorOnTimeInSecondsShouldBe(3594);
   CompressorOnTimeInSecondsShouldBe(3599);

   After(1);
   EepromCompressorOnTimeInSecondsShouldBe(3594); // off by 6 b/c periodic NV starts its timer before defrost starts counting (defrost power up delay - that will be removed later)
   CompressorOnTimeInSecondsShouldBe(3600);

   // Reset board and now FZ cabinet is too warm
   FilteredFreezerCabinetTemperatureIs(freezerCalcAxisGridLines[GridLine_FreezerExtremeHigh] + 1);
   PluginsAreInitializedAndDefrostHsmStateIsIdle();
   // EEPROM is copied to RAM and RAM is 3594

   CompressorOnTimeInSecondsShouldBe(0); // Compressor on time writes RAM to 0 b/c it's too warm
   EepromCompressorOnTimeInSecondsShouldBe(0); // when RAM ERD is set to 0, it'll update EEPROM ERD to 0 immediately
   // (if this didn't reset EEPROM to 0 too, it would be 3594)

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);

   // RAM ERD starts counting from 0
   After(50 * MSEC_PER_MIN - 1);
   CompressorOnTimeInSecondsShouldBe(2999);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   After(1);
   CompressorOnTimeInSecondsShouldBe(3000);
   EepromCompressorOnTimeInSecondsShouldBe(0);

   // Resets before 60 minutes where the periodic NV ERD transferer would've transferred RAM to EEPROM
   // FZ cabinet is normal temperature
   FilteredFreezerCabinetTemperatureIs(1000);
   PluginsAreInitializedAndDefrostHsmStateIsIdle();

   // If it hadn't reset the EEPROM ERD to 0 it'd be 3594 for both RAM and EEPROM ERDs, then RAM would have been reset to 0 b/c FZ is too warm
   CompressorOnTimeInSecondsShouldBe(0);
   EepromCompressorOnTimeInSecondsShouldBe(0); // when RAM ERD is set to 0, it'll update EEPROM ERD to 0 immediately

   DefrostHsmStateShouldBe(DefrostHsmState_Idle);
   DefrostCompressorOnTimeCounterFsmStateShouldBe(DefrostCompressorOnTimeCounterFsmState_Run);

   // Would have started from 3594 instead of 0 if EEPROM value hadn't been reset to 0
   After(50 * MSEC_PER_MIN - 1);
   EepromCompressorOnTimeInSecondsShouldBe(0);
   CompressorOnTimeInSecondsShouldBe(2999);

   After(1);
   EepromCompressorOnTimeInSecondsShouldBe(0);
   CompressorOnTimeInSecondsShouldBe(3000);

   // It eventually updates the EEPROM value
   // Timing is off between the two b/c the NV periodic transfer timer starts ~ 5 seconds before the defrost counter
   // Defrost delay will be removed with rewrite
   After(10 * MSEC_PER_MIN - 1);
   EepromCompressorOnTimeInSecondsShouldBe(3594);
   CompressorOnTimeInSecondsShouldBe(3599);

   After(1);
   EepromCompressorOnTimeInSecondsShouldBe(3594);
   CompressorOnTimeInSecondsShouldBe(3600);
}
