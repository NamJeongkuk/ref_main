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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "SensorData_TestDouble.h"
#include "uassert_test.h"

enum
{
   ValidAdcCount = 22656,
   MappedValidTemperature = 1314,
   InvalidAdcCount = 5375,
   InvalidTemperature = 30000,
   AnotherValidAdcCount = 22780,
   AnotherMappedValidTemperature = 1320,
   SomeOtherValidAdcCount = 26432,
   SomeOtherMappedValidTemperature = 2137
};

static const SensorFilteringConfig_t sensorConfigWithSlewRateFilterEnabled = {
   .sensorAdcCountErd = Erd_AmbientThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_Ambient_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_Ambient_FilteredTemperature,
   .sensorIsValidErd = Erd_Ambient_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t sensorConfigWithSlewRateDisabled = {
   .sensorAdcCountErd = Erd_FreezerEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureErd = Erd_FreezerEvap_UnfilteredTemperature,
   .sensorFilteredTemperatureErd = Erd_FreezerEvap_FilteredTemperature,
   .sensorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

TEST_GROUP(SensorFiltering_SlewEnabled)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   SensorFiltering_t instance;

   SensorData_t sensorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      SensorData_TestDouble_Init(&sensorData);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetSensors(&personalityParametricData, &sensorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenSensorWithSlewRateFilterEnabledIsInitialized()
   {
      SensorFiltering_Init(
         &instance,
         dataModel,
         &sensorConfigWithSlewRateFilterEnabled,
         sensorData.ambientThermistor,
         sensorData.periodicUpdateRateInMs);
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

      FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);
   }

   void GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100()
   {
      GivenInitialAdcCountsAre(ValidAdcCount);
      GivenSensorWithSlewRateFilterEnabledIsInitialized();

      FilteredTemperatureShouldBe(MappedValidTemperature);
   }

   void WhenBadReadingCounterMaxNumberOfBadReadingsHappen()
   {
      for(uint8_t i = 0; i < sensorData.ambientThermistor->badReadingCounterMax; i++)
      {
         WhenAdcCountChangesTo(InvalidAdcCount);
         After(sensorData.periodicUpdateRateInMs);
      }
   }

   void AfterFilteredTemperatureIsSnappedToTheUnfilteredTemperatureAfterBecomingValidAgain()
   {
      for(uint8_t i = 0; i < sensorData.ambientThermistor->goodReadingCounterMax - 1; i++)
      {
         WhenAdcCountChangesTo(AnotherValidAdcCount);

         After(sensorData.periodicUpdateRateInMs - 1);
         FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);

         After(1);
         FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);
      }

      WhenAdcCountChangesTo(AnotherValidAdcCount);

      After(sensorData.periodicUpdateRateInMs - 1);
      FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);

      After(1);
      UnfilteredTemperatureShouldBe(AnotherMappedValidTemperature);
      FilteredTemperatureShouldBe(AnotherMappedValidTemperature);
   }
};

TEST(SensorFiltering_SlewEnabled, ShouldInitialize)
{
   GivenSensorWithSlewRateFilterEnabledIsInitialized();
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteUnfilteredTemperatureToFilteredTemperatureWhenSeededWithValidValue)
{
   GivenInitialAdcCountsAre(ValidAdcCount);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   FilteredTemperatureShouldBe(MappedValidTemperature);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteUnfilteredTemperatureToUnfilteredTemperatureErdOnInit)
{
   GivenInitialAdcCountsAre(ValidAdcCount);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   UnfilteredTemperatureShouldBe(MappedValidTemperature);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteTheFallbackTemperatureAsTheFilteredTemperatureWhenSeededWithInvalidValue)
{
   GivenInitialAdcCountsAre(InvalidAdcCount);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteTheInvalidMappedTemperatureToUnfilteredTemperatureErdWhenSeededWithInvalidValue)
{
   GivenInitialAdcCountsAre(InvalidAdcCount);
   GivenSensorWithSlewRateFilterEnabledIsInitialized();

   UnfilteredTemperatureShouldBe(sensorData.ambientThermistor->lookupTable->mappings[sensorData.ambientThermistor->lookupTable->mappingCount - 1].y);
}

TEST(SensorFiltering_SlewEnabled, ShouldWriteUnfilteredTemperatureToFilteredTemperatureAfterFinallySeedingWithValidValue)
{
   GivenThermistorWasSeededWithInvalidValue();

   WhenAdcCountChangesTo(ValidAdcCount);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);

   After(1);
   UnfilteredTemperatureShouldBe(MappedValidTemperature);
   FilteredTemperatureShouldBe(MappedValidTemperature);
}

TEST(SensorFiltering_SlewEnabled, ShouldSlewTheFilteredTemperatureValueIfItChangesMoreThanTheSlewLimitInEitherDirection)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenAdcCountChangesTo(44352);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   WhenAdcCountChangesTo(45248);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature + sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 2 * sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   WhenAdcCountChangesTo(44352);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 2 * sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 3 * sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   WhenAdcCountChangesTo(19072);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 3 * sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 4 * sensorData.ambientThermistor->clampData.slewRatePerSecondx100);
}

TEST(SensorFiltering_SlewEnabled, ShouldUpdateUnfilteredTemperatureAsAdcCountsChangeToMappedTemperature)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenAdcCountChangesTo(44352);

   After(sensorData.periodicUpdateRateInMs - 1);
   UnfilteredTemperatureShouldBe(1314);

   After(1);
   UnfilteredTemperatureShouldBe(6270);

   WhenAdcCountChangesTo(26432);

   After(sensorData.periodicUpdateRateInMs - 1);
   UnfilteredTemperatureShouldBe(6270);

   After(1);
   UnfilteredTemperatureShouldBe(2137);
}

TEST(SensorFiltering_SlewEnabled, ShouldSetTheFallbackValueAsTheFilteredTemperatureAndTheInvalidMappedTemperatureToTheUnfilteredTemperatureAfterBadReadingCounterMaxNumberOfInvalidValues)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   for(uint8_t i = 0; i < sensorData.ambientThermistor->badReadingCounterMax - 1; i++)
   {
      WhenAdcCountChangesTo(InvalidAdcCount);

      After(sensorData.periodicUpdateRateInMs - 1);
      FilteredTemperatureShouldBe(MappedValidTemperature);

      After(1);
      FilteredTemperatureShouldBe(MappedValidTemperature);
   }

   WhenAdcCountChangesTo(InvalidAdcCount);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature);

   After(1);
   FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);
   UnfilteredTemperatureShouldBe(sensorData.ambientThermistor->lookupTable->mappings[sensorData.ambientThermistor->lookupTable->mappingCount - 1].y);
}

TEST(SensorFiltering_SlewEnabled, ShouldSetTheUnfilteredValueAsTheFilteredTemperatureAfterGoodReadingCounterMaxNumberOfValidValuesWhenTheThermistorIsInvalid)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenBadReadingCounterMaxNumberOfBadReadingsHappen();

   for(uint8_t i = 0; i < sensorData.ambientThermistor->goodReadingCounterMax - 1; i++)
   {
      WhenAdcCountChangesTo(AnotherValidAdcCount);

      After(sensorData.periodicUpdateRateInMs - 1);
      FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);

      After(1);
      FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);
   }

   WhenAdcCountChangesTo(AnotherValidAdcCount);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);

   After(1);
   FilteredTemperatureShouldBe(AnotherMappedValidTemperature);
   UnfilteredTemperatureShouldBe(AnotherMappedValidTemperature);
}

TEST(SensorFiltering_SlewEnabled, ShouldDoTheExponentialWeightedMovingAverage)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenAdcCountChangesTo(AnotherValidAdcCount);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 1);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 1);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 1);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 1);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 2);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 2);

   After(1);
   FilteredTemperatureShouldBe(MappedValidTemperature + 2);

   After(sensorData.periodicUpdateRateInMs * 75);
   FilteredTemperatureShouldBe(AnotherMappedValidTemperature);
}

TEST(SensorFiltering_SlewEnabled, ShouldDoTheExponentialWeightedMovingAverageAfterFilteredTemperatureIsSnappedToUnfilteredTemperature)
{
   GivenThermistorWasSeededWithValidValueAndSlewRateOfTenDegFx100();

   WhenBadReadingCounterMaxNumberOfBadReadingsHappen();
   FilteredTemperatureShouldBe(sensorData.ambientThermistor->fallbackValueDegFx100);

   AfterFilteredTemperatureIsSnappedToTheUnfilteredTemperatureAfterBecomingValidAgain();

   WhenAdcCountChangesTo(SomeOtherValidAdcCount);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(AnotherMappedValidTemperature);

   After(1);
   FilteredTemperatureShouldBe(AnotherMappedValidTemperature + sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(AnotherMappedValidTemperature + sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   After(1);
   FilteredTemperatureShouldBe(AnotherMappedValidTemperature + 2 * sensorData.ambientThermistor->clampData.slewRatePerSecondx100);

   After(sensorData.periodicUpdateRateInMs * 120);
   FilteredTemperatureShouldBe(SomeOtherMappedValidTemperature);
}

TEST_GROUP(SensorFiltering_SlewDisabled)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   SensorFiltering_t instance;

   SensorData_t sensorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      SensorData_TestDouble_Init(&sensorData);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetSensors(&personalityParametricData, &sensorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenSensorWithSlewRateDisabledIsInitializedWithSlewRateOf(int16_t slewRatePerSecondx100)
   {
      sensorData.freezerEvapThermistor->clampData.slewRatePerSecondx100 = slewRatePerSecondx100;

      SensorFiltering_Init(
         &instance,
         dataModel,
         &sensorConfigWithSlewRateDisabled,
         sensorData.freezerEvapThermistor,
         sensorData.periodicUpdateRateInMs);
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

   After(sensorData.periodicUpdateRateInMs - 1);
   FilteredTemperatureShouldBe(1314);

   After(1);
   FilteredTemperatureShouldBe(1809);
}
