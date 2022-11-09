/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SensorFiltering.h"
#include "SensorData.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "TddPersonality.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "PersonalityParametricDataTestDouble.h"
#include "uassert_test.h"

enum
{
   InvalidAdcCount = 5375,
   InvalidTemperature = 30000,
   ValidAdcCount1 = 22656,
   MappedValidTemperature1 = 1314,
   ValidAdcCount2 = 22700,
   MappedValidTemperature2 = 1323,
   ValidAdcCount3 = 26432,
   MappedValidTemperature3 = 2137,
   ValidAdcCount4 = 26470,
   MappedValidTemperature4 = 2145,
   ValidAdcCount5 = 30336,
   MappedValidTemperature5 = 2977,

   Erd_AmbientThermistor_AdcCount = 0xF000,
   Erd_Ambient_UnfilteredTemperature,
   Erd_Ambient_FilteredTemperature,
   Erd_Ambient_ThermistorIsValid,
   Erd_TimerModule,
   Erd_FreezerEvapThermistor_AdcCount,
   Erd_FreezerEvap_UnfilteredTemperature,
   Erd_FreezerEvap_FilteredTemperature,
   Erd_FreezerEvaporatorThermistorIsValid,
};

static const DataModel_TestDoubleConfigurationEntry_t erdsEntries[] = {
   { Erd_AmbientThermistor_AdcCount, sizeof(AdcCounts_t) },
   { Erd_Ambient_UnfilteredTemperature, sizeof(TemperatureDegFx100_t) },
   { Erd_Ambient_FilteredTemperature, sizeof(TemperatureDegFx100_t) },
   { Erd_Ambient_ThermistorIsValid, sizeof(bool) },
   { Erd_TimerModule, sizeof(TimerModule_t *) },
   { Erd_FreezerEvapThermistor_AdcCount, sizeof(AdcCounts_t) },
   { Erd_FreezerEvap_UnfilteredTemperature, sizeof(TemperatureDegFx100_t) },
   { Erd_FreezerEvap_FilteredTemperature, sizeof(TemperatureDegFx100_t) },
   { Erd_FreezerEvaporatorThermistorIsValid, sizeof(bool) },
};

static const SensorFilteringConfig_t sensorConfigWithSlewRateFilterEnabled = {
   .sensorAdcCountErd = Erd_AmbientThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_Ambient_UnfilteredTemperature,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_Ambient_FilteredTemperature,
   .sensorIsValidErd = Erd_Ambient_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t sensorConfigWithSlewRateDisabled = {
   .sensorAdcCountErd = Erd_FreezerEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_FreezerEvap_UnfilteredTemperature,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_FreezerEvap_FilteredTemperature,
   .sensorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(SensorFiltering_SlewEnabled)
{
   DataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleTestDouble;
   SensorFiltering_t instance;
   const SensorData_t *sensorData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelDouble, erdsEntries, NUM_ELEMENTS(erdsEntries));
      dataModel = dataModelDouble.dataModel;
      TimerModule_TestDouble_Init(&timerModuleTestDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble.timerModule);

      sensorData = ((PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator))->sensorData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenSensorWithSlewRateFilterEnabledIsInitialized()
   {
      SensorFiltering_Init(
         &instance,
         dataModel,
         &sensorConfigWithSlewRateFilterEnabled,
         sensorData->ambientThermistor,
         sensorData->periodicUpdateRateInMs);
   }

   void GivenInitialAdcCountsAre(AdcCounts_t counts)
   {
      DataModel_Write(dataModel, Erd_AmbientThermistor_AdcCount, &counts);
   }

   void FilteredTemperatureShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Ambient_FilteredTemperature, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenAdcCountChangesTo(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_AmbientThermistor_AdcCount, &count);
   }

   void UnfilteredTemperatureShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_Ambient_UnfilteredTemperature, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenThermistorWasSeededWithInvalidValue()
   {
      GivenInitialAdcCountsAre(InvalidAdcCount);
      GivenSensorWithSlewRateFilterEnabledIsInitialized();

      FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);
   }

   void GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100()
   {
      GivenInitialAdcCountsAre(ValidAdcCount1);
      GivenSensorWithSlewRateFilterEnabledIsInitialized();

      FilteredTemperatureShouldBe(MappedValidTemperature1);
   }

   void WhenBadReadingCounterMaxNumberOfBadReadingsHappen()
   {
      for(uint8_t i = 0; i < sensorData->ambientThermistor->badReadingCounterMax; i++)
      {
         WhenAdcCountChangesTo(InvalidAdcCount);
         After(sensorData->periodicUpdateRateInMs);
      }
   }

   void AfterFilteredTemperatureIsSnappedToTheUnfilteredTemperatureAfterBecomingValidAgain()
   {
      for(uint8_t i = 0; i < sensorData->ambientThermistor->goodReadingCounterMax - 1; i++)
      {
         WhenAdcCountChangesTo(ValidAdcCount3);

         After(sensorData->periodicUpdateRateInMs - 1);
         FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);

         After(1);
         FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);
      }

      WhenAdcCountChangesTo(ValidAdcCount3);

      After(sensorData->periodicUpdateRateInMs - 1);
      FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);

      After(1);
      UnfilteredTemperatureShouldBe(MappedValidTemperature3);
      FilteredTemperatureShouldBe(MappedValidTemperature3);
   }
};

TEST(SensorFiltering_SlewEnabled, ShouldInitialize)
{
   GivenSensorWithSlewRateFilterEnabledIsInitialized();
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteUnfilteredTemperatureToFilteredTemperatureWhenSeededWithValidValue)
{
   GivenInitialAdcCountsAre(ValidAdcCount1);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   FilteredTemperatureShouldBe(MappedValidTemperature1);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteUnfilteredTemperatureToUnfilteredTemperatureErdOnInit)
{
   GivenInitialAdcCountsAre(ValidAdcCount1);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   UnfilteredTemperatureShouldBe(MappedValidTemperature1);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteTheFallbackTemperatureAsTheFilteredTemperatureWhenSeededWithInvalidValue)
{
   GivenInitialAdcCountsAre(InvalidAdcCount);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteTheInvalidMappedTemperatureToUnfilteredTemperatureErdWhenSeededWithInvalidValue)
{
   GivenInitialAdcCountsAre(InvalidAdcCount);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   UnfilteredTemperatureShouldBe(sensorData->ambientThermistor->lookupTable->mappings[sensorData->ambientThermistor->lookupTable->mappingCount - 1].y);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteUnfilteredTemperatureToFilteredTemperatureAfterFinallySeedingWithValidValue)
{
   GivenThermistorWasSeededWithInvalidValue();

   WhenAdcCountChangesTo(ValidAdcCount1);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);

   After(1);
   UnfilteredTemperatureShouldBe(MappedValidTemperature1);
   FilteredTemperatureShouldBe(MappedValidTemperature1);
}

TEST(SensorFiltering_SlewEnabled, ShouldSlewTheFilteredTemperatureValueIfItChangesMoreThanTheSlewLimitInEitherDirection)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenAdcCountChangesTo(44352);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + sensorData->ambientThermistor->clampData.slewRatePerSecondx100);

   WhenAdcCountChangesTo(45248);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + sensorData->ambientThermistor->clampData.slewRatePerSecondx100);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 2 * sensorData->ambientThermistor->clampData.slewRatePerSecondx100);

   WhenAdcCountChangesTo(44352);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 2 * sensorData->ambientThermistor->clampData.slewRatePerSecondx100);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 3 * sensorData->ambientThermistor->clampData.slewRatePerSecondx100);

   WhenAdcCountChangesTo(19072);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 3 * sensorData->ambientThermistor->clampData.slewRatePerSecondx100);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 4 * sensorData->ambientThermistor->clampData.slewRatePerSecondx100);
}

TEST(SensorFiltering_SlewEnabled, ShouldUpdateUnfilteredTemperatureAsAdcCountsChangeToMappedTemperature)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenAdcCountChangesTo(44352);

   After(sensorData->periodicUpdateRateInMs - 1);
   UnfilteredTemperatureShouldBe(1314);

   After(1);
   UnfilteredTemperatureShouldBe(6270);

   WhenAdcCountChangesTo(26432);

   After(sensorData->periodicUpdateRateInMs - 1);
   UnfilteredTemperatureShouldBe(6270);

   After(1);
   UnfilteredTemperatureShouldBe(2137);
}

TEST(SensorFiltering_SlewEnabled, ShouldSetTheFallbackValueAsTheFilteredTemperatureAndTheInvalidMappedTemperatureToTheUnfilteredTemperatureAfterBadReadingCounterMaxNumberOfInvalidValues)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   for(uint8_t i = 0; i < sensorData->ambientThermistor->badReadingCounterMax - 1; i++)
   {
      WhenAdcCountChangesTo(InvalidAdcCount);

      After(sensorData->periodicUpdateRateInMs - 1);
      FilteredTemperatureShouldBe(MappedValidTemperature1);

      After(1);
      FilteredTemperatureShouldBe(MappedValidTemperature1);
   }

   WhenAdcCountChangesTo(InvalidAdcCount);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1);

   After(1);
   FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);
   UnfilteredTemperatureShouldBe(sensorData->ambientThermistor->lookupTable->mappings[sensorData->ambientThermistor->lookupTable->mappingCount - 1].y);
}

TEST(SensorFiltering_SlewEnabled, ShouldSetTheUnfilteredValueAsTheFilteredTemperatureAfterGoodReadingCounterMaxNumberOfValidValuesWhenTheThermistorIsInvalid)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenBadReadingCounterMaxNumberOfBadReadingsHappen();

   for(uint8_t i = 0; i < sensorData->ambientThermistor->goodReadingCounterMax - 1; i++)
   {
      WhenAdcCountChangesTo(ValidAdcCount3);

      After(sensorData->periodicUpdateRateInMs - 1);
      FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);

      After(1);
      FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);
   }

   WhenAdcCountChangesTo(ValidAdcCount3);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature3);
   UnfilteredTemperatureShouldBe(MappedValidTemperature3);
}

TEST(SensorFiltering_SlewEnabled, ShouldSlewFromUnfilteredAndFilteredValueAfterFilteredTemperatureIsSnappedToUnfilteredTemperature)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenBadReadingCounterMaxNumberOfBadReadingsHappen();
   FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);

   AfterFilteredTemperatureIsSnappedToTheUnfilteredTemperatureAfterBecomingValidAgain();

   WhenAdcCountChangesTo(ValidAdcCount5);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature3);
   UnfilteredTemperatureShouldBe(MappedValidTemperature3);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature3 + sensorData->ambientThermistor->clampData.slewRatePerSecondx100);
   UnfilteredTemperatureShouldBe(MappedValidTemperature5);
}

TEST(SensorFiltering_SlewEnabled, ShouldDoTheExponentialWeightedMovingAverage)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenAdcCountChangesTo(ValidAdcCount2);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 1);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 1);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 2);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 2);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature1 + 3);

   After(sensorData->periodicUpdateRateInMs * 75);
   FilteredTemperatureShouldBe(MappedValidTemperature2);
}

TEST(SensorFiltering_SlewEnabled, ShouldDoOnlyExponentialWeightedMovingAverageAfterFilteredTemperatureIsSnappedToUnfilteredTemperatureWhenNewChangeIsLessThanSlewRateThenFinallyReachTheUnfilteredTemperature)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenBadReadingCounterMaxNumberOfBadReadingsHappen();
   FilteredTemperatureShouldBe(sensorData->ambientThermistor->fallbackValueDegFx100);

   AfterFilteredTemperatureIsSnappedToTheUnfilteredTemperatureAfterBecomingValidAgain();

   WhenAdcCountChangesTo(ValidAdcCount4);
   UnfilteredTemperatureShouldBe(MappedValidTemperature3);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature3);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature3);

   After(sensorData->periodicUpdateRateInMs * 10);
   FilteredTemperatureShouldBe(MappedValidTemperature3 + 5);

   After(sensorData->periodicUpdateRateInMs * 10);
   FilteredTemperatureShouldBe(MappedValidTemperature3 + 7);

   After(sensorData->periodicUpdateRateInMs * 57);
   FilteredTemperatureShouldBe(MappedValidTemperature4);
}

TEST_GROUP(SensorFiltering_SlewDisabled)
{
   DataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleTestDouble;
   SensorFiltering_t instance;

   const SensorData_t *sensorData;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelDouble, erdsEntries, NUM_ELEMENTS(erdsEntries));
      dataModel = dataModelDouble.dataModel;
      TimerModule_TestDouble_Init(&timerModuleTestDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble.timerModule);

      sensorData = ((PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator))->sensorData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenSensorWithSlewRateDisabledIsInitializedWithSlewRateOf(int16_t slewRatePerSecondx100)
   {
      sensorData->freezerEvapThermistor->clampData.slewRatePerSecondx100 = slewRatePerSecondx100;

      SensorFiltering_Init(
         &instance,
         dataModel,
         &sensorConfigWithSlewRateDisabled,
         sensorData->freezerEvapThermistor,
         sensorData->periodicUpdateRateInMs);
   }

   void GivenInitialAdcCountsAre(AdcCounts_t counts)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_AdcCount, &counts);
   }

   void FilteredTemperatureShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvap_FilteredTemperature, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenAdcCountChangesTo(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_FreezerEvapThermistor_AdcCount, &count);
   }

   void GivenThermistorWasSeededWithValidValueAndSlewRateOf(int16_t slewRatePerSecondx100)
   {
      GivenInitialAdcCountsAre(22656);
      GivenSensorWithSlewRateDisabledIsInitializedWithSlewRateOf(slewRatePerSecondx100);

      FilteredTemperatureShouldBe(1314);
   }

   void UnfilteredTemperatureShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvap_UnfilteredTemperature, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(SensorFiltering_SlewDisabled, ShouldNotLimitSlewRateWhichAllowsFilteredTemperatureToChangeGreaterThanSlewRate)
{
   GivenInitialAdcCountsAre(22656);
   GivenSensorWithSlewRateDisabledIsInitializedWithSlewRateOf(2);

   FilteredTemperatureShouldBe(1314);

   WhenAdcCountChangesTo(44352);

   After(sensorData->periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(1314);

   After(1);
   FilteredTemperatureShouldBe(1809);
}
