/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SensorFilteredReading.h"
#include "DataModelErdPointerAccess.h"
#include "Filter_ExponentiallyWeightedMovingAverage.h"
#include "Mapper_SmallLookupTable.h"
#include "Constants_Time.h"
#include "TemperatureDegFx100.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "TimerModule_TestDouble.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

#define And
#define AND ,
#define Then
#define For ,
#define Periods(x) x

enum
{
   AdcChannel0,
   AdcChannel1,
   NumberOfAdcChannels,

   AlphaNum = 9, // alpha = 1 - alpha
   AlphaDenom = 10, // applCommon filter alpha defined inversely
   WindowSize = 10,
   SampleIsSigned = IS_SIGNED(int32_t),
   SampleWidth = sizeof(int32_t),
   slewRatePerSecondx100 = 10,
   SensorFiltering_GoodReadingMax = 100,
   SensorFiltering_BadReadingMax = 100,

   BadFilterValue = 30000,
   BadTempValue = BadFilterValue,
   FreshFoodFallbackDegFx100 = -500,
   FreezerFallbackDegFx100 = 500,

   AdcRawCount1 = 127,
   AdcRawCount2 = 255,
   AdcRawCount3 = 470,
   AdcRawCount4 = 505,
   AdcRawCount5 = 1023,
   AdcCountForBadMapping = 2047,

   MappedValue1 = 200,
   MappedValue2 = 500,
   MappedValue3 = 1004,
   MappedValue4 = 1086,
   MappedValue5 = 2000,

   ExponentiallyFilteredValueFromMappedValue1To2WithClamping = ((MappedValue1 * WindowSize * AlphaNum) + (WindowSize * ((AlphaDenom - AlphaNum) * (MappedValue1 + (slewRatePerSecondx100 * AlphaDenom)))) + (AlphaDenom - 1)) / (AlphaDenom * WindowSize),
   ExponentiallyFilteredValueFromMappedValue1To5WithClamping = ((MappedValue1 * WindowSize * AlphaNum) + (WindowSize * ((AlphaDenom - AlphaNum) * (MappedValue1 + (slewRatePerSecondx100 * AlphaDenom)))) + (AlphaDenom - 1)) / (AlphaDenom * WindowSize),
   ExponentiallyFilteredValueFromMappedValue1To3WithClamping = ((ExponentiallyFilteredValueFromMappedValue1To2WithClamping * WindowSize * AlphaNum) + (WindowSize * ((AlphaDenom - AlphaNum) * (ExponentiallyFilteredValueFromMappedValue1To2WithClamping + (slewRatePerSecondx100 * AlphaDenom)))) + (AlphaDenom - 1)) / (AlphaDenom * WindowSize),
   ExponentiallyFilteredValueFromMappedValue2To1WithClamping = ((MappedValue2 * WindowSize * AlphaNum) + (WindowSize * ((AlphaDenom - AlphaNum) * (MappedValue2 - (slewRatePerSecondx100 * AlphaDenom)))) + (AlphaDenom - 1)) / (AlphaDenom * WindowSize),
   ExponentiallyFilteredValueFromMappedValue5To1WithClamping = ((MappedValue5 * WindowSize * AlphaNum) + (WindowSize * ((AlphaDenom - AlphaNum) * (MappedValue5 - (slewRatePerSecondx100 * AlphaDenom)))) + (AlphaDenom - 1)) / (AlphaDenom * WindowSize),
   ExponentiallyFilteredValueFromMappedValue2To3WithClamping = (((MappedValue2)*WindowSize * AlphaNum) + (WindowSize * ((AlphaDenom - AlphaNum) * (MappedValue2 + (slewRatePerSecondx100 * AlphaDenom)))) + (AlphaDenom - 1)) / (AlphaDenom * WindowSize),
   ExponentiallyFilteredValueFromMappedValue3To4 = ((MappedValue3 * WindowSize * AlphaNum) + (WindowSize * ((AlphaDenom - AlphaNum) * MappedValue4)) + (AlphaDenom - 1)) / (AlphaDenom * WindowSize),

   Erd_RawAdcCount1,
   Erd_RawAdcCount2,

   Erd_UnfilteredTemp1,
   Erd_UnfilteredTemp2,

   Erd_FilteredTemp1,
   Erd_FilteredTemp2,

   Erd_TimerModuleDouble,

   FilterSamplingTimeInMsec = 1 * MSEC_PER_SEC,

};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_RawAdcCount1, sizeof(AdcCounts_t) },
   { Erd_UnfilteredTemp1, sizeof(TemperatureDegFx100_t) },
   { Erd_FilteredTemp1, sizeof(TemperatureDegFx100_t) },
   { Erd_RawAdcCount2, sizeof(AdcCounts_t) },
   { Erd_UnfilteredTemp2, sizeof(TemperatureDegFx100_t) },
   { Erd_FilteredTemp2, sizeof(TemperatureDegFx100_t) },
   { Erd_TimerModuleDouble, sizeof(TimerModule_t *) }
};

static const Mapper_SmallLookupTableMapping_t mappingsForFilter[] = {
   { 0, BadFilterValue },
   { AdcRawCount1 - 1, BadFilterValue },
   { AdcRawCount1, MappedValue1 },
   { AdcRawCount2, MappedValue2 },
   { AdcRawCount3, MappedValue3 },
   { AdcRawCount4, MappedValue4 },
   { AdcRawCount5, MappedValue5 },
   { AdcRawCount5 + 1, BadFilterValue },
   { AdcCountForBadMapping, BadFilterValue }
};

static const Mapper_SmallLookupTableConfiguration_t configurationForMappings = {
   mappingsForFilter,
   NUM_ELEMENTS(mappingsForFilter)
};

TEST_GROUP(SensorFilteredReading)
{
   SensorFilteredReading_t instance;
   Filter_ExponentiallyWeightedMovingAverage_t filter[NumberOfAdcChannels];
   Mapper_SmallLookupTable_t mapper;
   SensorFilteredReadingChannelData_t channelData[NumberOfAdcChannels];
   SensorFilteredReadingConfiguration_t config;

   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleTestDouble;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erdTable, NUM_ELEMENTS(erdTable));
      dataModel = &dataModelTestDouble.interface;
      TimerModule_TestDouble_Init(&timerModuleTestDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModuleDouble, TimerModule_TestDouble_GetTimerModule(&timerModuleTestDouble));

      for(uint8_t channel = 0; channel < NumberOfAdcChannels; channel++)
      {
         Filter_ExponentiallyWeightedMovingAverage_Init(
            &filter[channel],
            WindowSize,
            SampleIsSigned,
            SampleWidth,
            AlphaNum,
            AlphaDenom);

         channelData[channel].filter = &filter[channel].interface;
         channelData[channel].filterInvalidValue = BadFilterValue;
         channelData[channel].clampData.clampingEnabled = true;
         channelData[channel].clampData.slewRatePerSecondx100 = slewRatePerSecondx100;
         channelData[channel].fallbackData.sensorState = SensorIsGood;
         channelData[channel].adcMapper = &mapper.interface;
         channelData[channel].filterAlphax100 = 100 - ((AlphaNum * 100) / AlphaDenom);
         channelData[channel].fallbackData.goodReadingMaxValue = SensorFiltering_GoodReadingMax;
         channelData[channel].fallbackData.badReadingMaxValue = SensorFiltering_BadReadingMax;
      }

      channelData[0].erds.rawAdcCountErd = Erd_RawAdcCount1;
      channelData[0].erds.unfilteredOutputErd = Erd_UnfilteredTemp1;
      channelData[0].erds.filteredOutputErd = Erd_FilteredTemp1;
      channelData[0].fallbackData.fallbackValue = FreshFoodFallbackDegFx100;

      channelData[1].erds.rawAdcCountErd = Erd_RawAdcCount2;
      channelData[1].erds.unfilteredOutputErd = Erd_UnfilteredTemp2;
      channelData[1].erds.filteredOutputErd = Erd_FilteredTemp2;
      channelData[1].fallbackData.fallbackValue = FreezerFallbackDegFx100;

      Mapper_SmallLookupTable_Init(&mapper, &configurationForMappings);
      config.channelData = channelData;
      config.channelDataCount = NUM_ELEMENTS(channelData);
      config.timerModule = Erd_TimerModuleDouble;
      config.periodicCheckRateInMsec = FilterSamplingTimeInMsec;
   }

   void WhenTheModuleIsInitialized()
   {
      SensorFilteredReading_Init(
         &instance,
         dataModel,
         &config);
   }

   void NothingShouldHappen()
   {
   }

   void GivenRawAdcCountErdIs(Erd_t erd, AdcCounts_t adcCounts)
   {
      DataModel_Write(dataModel, erd, &adcCounts);
   }

   void TheFilteredOutputErdShouldBe(Erd_t erd, TemperatureDegFx100_t temperature)
   {
      TemperatureDegFx100_t realTemp;
      DataModel_Read(dataModel, erd, &realTemp);
      CHECK_EQUAL(temperature, realTemp);
   }

   void TheUnfilteredErdShouldBe(Erd_t erd, TemperatureDegFx100_t temperature)
   {
      TemperatureDegFx100_t realTemp;
      DataModel_Read(dataModel, erd, &realTemp);
      CHECK_EQUAL(temperature, realTemp);
   }

   void GivenTheSensorsAreBad()
   {
      for(uint8_t channel = 0; channel < NumberOfAdcChannels; channel++)
      {
         channelData[channel].fallbackData.sensorState = SensorIsBad;
      }
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleTestDouble, ticks);
   }

   void TheFilteredTemperatureShouldBeClampedForSeconds(Erd_t erd, uint16_t seconds, int16_t startPoint)
   {
      for(uint16_t index = 0; index < seconds; index++)
      {
         After(1);
         TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, startPoint + slewRatePerSecondx100);

         After(FilterSamplingTimeInMsec - 1);
         startPoint += slewRatePerSecondx100;
      }
   }

   void TheFilteredAndUnfilteredTemperaturesShouldBe(TemperatureDegFx100_t filteredTemp, TemperatureDegFx100_t unfilteredTemp, uint16_t seconds)
   {
      for(uint16_t index = 0; index < seconds; index++)
      {
         After(FilterSamplingTimeInMsec);
         TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, unfilteredTemp);
         TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, filteredTemp);
         TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, unfilteredTemp);
         TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, filteredTemp);
      }
   }

   void TheFilteredTemperaturesShouldBeAtFallbackForPeriods(uint16_t seconds)
   {
      for(uint16_t index = 0; index < seconds; index++)
      {
         After(FilterSamplingTimeInMsec);
         TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
         TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
      }
   }
};

TEST(SensorFilteredReading, ShouldInitWithFallbackTempsForFilteredAndBadTempsForUnfilteredWhenBadAdcReading)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, 0);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, 0);
   WhenTheModuleIsInitialized();

   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
}

TEST(SensorFilteredReading, ShouldInitWithGoodTempsForFilteredAndUnfilteredWhenGoodAdcReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount2);
   WhenTheModuleIsInitialized();

   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue2);

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, MappedValue2);
}

TEST(SensorFilteredReading, ShouldFilterSampleWithoutClampingAfter1PeriodOfGoodAdcReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount3);
   WhenTheModuleIsInitialized();

   After(FilterSamplingTimeInMsec - 1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue3);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue3);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount4);
   After(1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue4);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue3To4);
}

TEST(SensorFilteredReading, ShoulBringAverageCloseToSampleAfterMultipleOfTheSameReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount3);
   WhenTheModuleIsInitialized();

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue3);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount4);
   After(FilterSamplingTimeInMsec - 1);

   After(1);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue3To4);

   After(FilterSamplingTimeInMsec * 150);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue4);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue4);
}

TEST(SensorFilteredReading, ShouldApplyClampingThenEwmaFilterOnFilteredTempErdAfter1SecondWhenNewSampleAverageIncreasedGreaterThanSlewRateTimesAlpha)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   WhenTheModuleIsInitialized();

   After(FilterSamplingTimeInMsec - 1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue1);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount5);
   After(1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue5);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue1To5WithClamping);
}

TEST(SensorFilteredReading, ShouldClampFilteredTempErdAfter1SecondWhenNewSampleAverageDecreasedGreaterThanSlewRateTimesAlpha)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount5);
   WhenTheModuleIsInitialized();

   After(FilterSamplingTimeInMsec - 1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue5);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue5);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   After(1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue1);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue5To1WithClamping);
}

TEST(SensorFilteredReading, ShouldStillClampFilteredTempErdAfter2SecondWhenNewSampleAverageIncreasesGreaterThanSlewRateTimesAlpha)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   WhenTheModuleIsInitialized();

   After(FilterSamplingTimeInMsec - 1);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount2);
   After(1);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue2);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue1To2WithClamping);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount3);
   After(FilterSamplingTimeInMsec);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue3);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue1To3WithClamping);
}

TEST(SensorFilteredReading, ShouldNotChangeFilteredTempErdWhenGoodReadingFollowingByBadReading)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   WhenTheModuleIsInitialized();

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);

   After(FilterSamplingTimeInMsec - 1);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue1);

   After(1);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
}

TEST(SensorFilteredReading, ShouldNotChangeFilteredTempErdWhenGoodReadingFollowingBy99BadReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount1);
   WhenTheModuleIsInitialized();

   After(FilterSamplingTimeInMsec);
   TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue1);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   TheFilteredAndUnfilteredTemperaturesShouldBe(MappedValue1 AND BadFilterValue For Periods(98));

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
}

TEST(SensorFilteredReading, ShouldChangeFilteredTempErdToFallbackWhenGoodReadingFollowingBy100BadReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount1);
   WhenTheModuleIsInitialized();

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue1);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, MappedValue1);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   TheFilteredAndUnfilteredTemperaturesShouldBe(MappedValue1 AND BadFilterValue For Periods(99));

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);
}

TEST(SensorFilteredReading, ShouldKeepFilteredTempErdAtFallbackValueWhenSensorIsInitedWithBadReadingAndThenFollowedByGoodReading)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   WhenTheModuleIsInitialized();

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);

   Then GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount1);

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue1);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue1);
}

TEST(SensorFilteredReading, ShouldKeepFilteredTempErdAtFallbackValueWhenSensorIsInitedWithBadReadingAndThenFollowedBy99GoodReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   WhenTheModuleIsInitialized();

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount1);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount1);
   TheFilteredTemperaturesShouldBeAtFallbackForPeriods(98);

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue1);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue1);
}

TEST(SensorFilteredReading, ShouldChangeFilteredTempToMostRecentGoodReadingWhenSensorIsInitedWithBadReadingAndThenFollowedBy100GoodReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   WhenTheModuleIsInitialized();

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount2);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount2);
   TheFilteredTemperaturesShouldBeAtFallbackForPeriods(99);

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue2);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, MappedValue2);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue2);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue2);
}

TEST(SensorFilteredReading, ShouldFilterFilteredTempWhenSensorIsInitedWithBadReadingAndThenFollowedBy101GoodReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   WhenTheModuleIsInitialized();

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount2);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount2);
   TheFilteredTemperaturesShouldBeAtFallbackForPeriods(99);

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue2);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, MappedValue2);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue2);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue2);

   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount3);
   GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount3);
   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue2To3WithClamping);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, ExponentiallyFilteredValueFromMappedValue2To3WithClamping);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue3);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue3);
}

TEST(SensorFilteredReading, WhenInitedWithBadSensorsShouldOnlySwitchToGoodFilteredTempAfter100ConsecutiveGoodReadings)
{
   WhenTheModuleIsInitialized();
   GivenTheSensorsAreBad();

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);

   Then GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount2);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount2);
   TheFilteredTemperaturesShouldBeAtFallbackForPeriods(99);

   Then GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);

   Then GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount2);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount2);
   TheFilteredTemperaturesShouldBeAtFallbackForPeriods(99);

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue2);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, MappedValue2);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue2);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue2);
}

TEST(SensorFilteredReading, WhenInitedWithGoodSensorsShouldOnlySwitchToFallbackFilteredTempAfter100ConsecutiveBadReadings)
{
   GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount3);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount3);
   WhenTheModuleIsInitialized();

   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, MappedValue3);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, MappedValue3);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue3);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue3);

   Then GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   TheFilteredAndUnfilteredTemperaturesShouldBe(MappedValue3 AND BadTempValue For Periods(99));

   Then GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcRawCount4);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcRawCount4);
   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, ExponentiallyFilteredValueFromMappedValue3To4);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, ExponentiallyFilteredValueFromMappedValue3To4);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, MappedValue4);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, MappedValue4);

   Then GivenRawAdcCountErdIs(Erd_RawAdcCount1, AdcCountForBadMapping);
   And GivenRawAdcCountErdIs(Erd_RawAdcCount2, AdcCountForBadMapping);
   TheFilteredAndUnfilteredTemperaturesShouldBe(ExponentiallyFilteredValueFromMappedValue3To4 AND BadTempValue For Periods(99));

   After(FilterSamplingTimeInMsec);
   TheFilteredOutputErdShouldBe(Erd_FilteredTemp1, FreshFoodFallbackDegFx100);
   And TheFilteredOutputErdShouldBe(Erd_FilteredTemp2, FreezerFallbackDegFx100);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp1, BadTempValue);
   And TheUnfilteredErdShouldBe(Erd_UnfilteredTemp2, BadTempValue);
}
