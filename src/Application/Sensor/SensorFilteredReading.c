/*!
 * @file
 * @brief Implementation of the generic sensor filtering module
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "SensorFilteredReading.h"
#include "DataModelErdPointerAccess.h"
#include "TemperatureDegFx100.h"

enum
{
   DontUpdateFilteredErd,
   UpdateFilteredErdWithFallback,
   UpdateFilteredErdUsingFilter,
   ForceUpdateFilteredErd
};

enum
{
   PrecisionScalingFactor = UINT8_MAX
};

static int32_t AdjustedSlewRateByFilterAlphaValue(SensorFilteredReadingChannelData_t *channelData)
{
   return channelData->clampData.slewRatePerSecondx100 * channelData->filterAlphax100;
}

static bool ValueIsValid(
   SensorFilteredReadingChannelData_t *channelData,
   int32_t value)
{
   return value != channelData->filterInvalidValue;
}

static int32_t FilteredValue(I_Filter_t *filter)
{
   int32_t filteredValue;
   Filter_Read(filter, &filteredValue);

   return filteredValue;
}

static void UpdateFilter(
   SensorFilteredReadingChannelData_t *channelData,
   int32_t mappedAdcValue)
{
   if(channelData->filterHasBeenSeeded)
   {
      Filter_Feed(channelData->filter, &mappedAdcValue);
   }
   else
   {
      Filter_Seed(channelData->filter, &mappedAdcValue);
      channelData->filterHasBeenSeeded = true;
   }
}

static void ResetFilter(SensorFilteredReadingChannelData_t *channelData)
{
   Filter_Reset(channelData->filter);
   channelData->filterHasBeenSeeded = false;
}

static int16_t DownscaledByPrecisionScalingFactor(int32_t value)
{
   return (int16_t)(value / PrecisionScalingFactor);
}

static int32_t UpscaledByPrecisionScalingFactor(int32_t value)
{
   return ((int16_t)value) * PrecisionScalingFactor;
}

static int32_t SlewRateLimited(
   SensorFilteredReadingChannelData_t *channelData,
   int32_t newUnfilteredValue,
   int32_t previousFilteredValue)
{
   if(channelData->clampData.slewRateEnabled)
   {
      if((newUnfilteredValue - previousFilteredValue) > channelData->clampData.slewRatePerSecondx100)
      {
         newUnfilteredValue = previousFilteredValue + channelData->clampData.slewRatePerSecondx100;
      }
      else if((previousFilteredValue - newUnfilteredValue) > channelData->clampData.slewRatePerSecondx100)
      {
         newUnfilteredValue = previousFilteredValue - channelData->clampData.slewRatePerSecondx100;
      }
   }

   return newUnfilteredValue;
}

static int32_t AdcMappedTemperatureDegFx100Value(
   SensorFilteredReading_t *instance,
   SensorFilteredReadingChannelData_t *channelData)
{
   AdcCounts_t rawAdcValue;
   DataModel_Read(
      instance->_private.dataModel,
      channelData->erds.rawAdcCountErd,
      &rawAdcValue);

   int32_t mappedAdcValue = (int32_t)Mapper_GetMappedValue(
      channelData->adcMapper,
      rawAdcValue);

   return mappedAdcValue;
}

static uint8_t FilteredErdUpdateMethodFlagBasedOnGoodReadingCounter(
   SensorFilteredReadingChannelData_t *channelData)
{
   uint8_t writeToFilteredErd;

   channelData->fallbackData.badReadingCounter = 0;

   if(channelData->fallbackData.sensorState == SensorIsGood)
   {
      writeToFilteredErd = UpdateFilteredErdUsingFilter;
   }
   else
   {
      if(channelData->fallbackData.goodReadingCounter < channelData->fallbackData.goodReadingMaxValue - 1)
      {
         channelData->fallbackData.goodReadingCounter += 1;
         writeToFilteredErd = UpdateFilteredErdWithFallback;
      }
      else
      {
         channelData->fallbackData.sensorState = SensorIsGood;
         writeToFilteredErd = ForceUpdateFilteredErd;
      }
   }

   return writeToFilteredErd;
}

static uint8_t FilteredErdUpdateMethodFlagBasedOnBadReadingCounter(
   SensorFilteredReadingChannelData_t *channelData)
{
   uint8_t writeToFilteredErd;

   channelData->fallbackData.goodReadingCounter = 0;

   if(channelData->fallbackData.sensorState == SensorIsGood)
   {
      if(channelData->fallbackData.badReadingCounter < channelData->fallbackData.badReadingMaxValue - 1)
      {
         channelData->fallbackData.badReadingCounter += 1;
         writeToFilteredErd = DontUpdateFilteredErd;
      }
      else
      {
         channelData->fallbackData.sensorState = SensorIsBad;
         writeToFilteredErd = UpdateFilteredErdWithFallback;
      }
   }
   else
   {
      writeToFilteredErd = UpdateFilteredErdWithFallback;
   }

   return writeToFilteredErd;
}

static void UpdateSensorValues(void *context)
{
   SensorFilteredReading_t *instance = context;

   for(uint8_t index = 0; index < instance->_private.configuration->channelDataCount; index++)
   {
      uint8_t writeToFilteredErd;

      SensorFilteredReadingChannelData_t *channelData = &instance->_private.configuration->channelData[index];

      int32_t unfilteredValue = AdcMappedTemperatureDegFx100Value(
         instance,
         channelData);
      DataModel_Write(
         instance->_private.dataModel,
         channelData->erds.unfilteredOutputErd,
         &unfilteredValue);

      if(ValueIsValid(channelData, unfilteredValue))
      {
         writeToFilteredErd = FilteredErdUpdateMethodFlagBasedOnGoodReadingCounter(channelData);
      }
      else
      {
         writeToFilteredErd = FilteredErdUpdateMethodFlagBasedOnBadReadingCounter(channelData);
      }

      if(writeToFilteredErd == UpdateFilteredErdUsingFilter)
      {
         int32_t previousFilteredValue;
         DataModel_Read(
            instance->_private.dataModel,
            channelData->erds.filteredOutputErd,
            &previousFilteredValue);

         int32_t slewRateLimited = SlewRateLimited(
            channelData,
            UpscaledByPrecisionScalingFactor(unfilteredValue),
            UpscaledByPrecisionScalingFactor(previousFilteredValue));

         UpdateFilter(channelData, slewRateLimited);

         int16_t downscaledFilteredValue = DownscaledByPrecisionScalingFactor(
            FilteredValue(channelData->filter));
         DataModel_Write(
            instance->_private.dataModel,
            channelData->erds.filteredOutputErd,
            &downscaledFilteredValue);
      }
      else if(writeToFilteredErd == UpdateFilteredErdWithFallback)
      {
         DataModel_Write(
            instance->_private.dataModel,
            channelData->erds.filteredOutputErd,
            &channelData->fallbackData.fallbackValue);
      }
      else if(writeToFilteredErd == ForceUpdateFilteredErd)
      {
         ResetFilter(channelData);
         UpdateFilter(channelData, UpscaledByPrecisionScalingFactor(unfilteredValue));

         DataModel_Write(
            instance->_private.dataModel,
            channelData->erds.filteredOutputErd,
            &unfilteredValue);
      }
   }
}

static void SetWhichConvertibleCompartmentThermistorFallbackValueToUseDependingOnState(
   SensorFilteredReading_t *instance,
   const ConvertibleCompartmentStateType_t convertibleCompartmentState)
{
   if(convertibleCompartmentState == ConvertibleCompartmentStateType_FreshFood)
   {
      instance->_private.configuration->channelData[instance->_private.configuration->convertibleCompartmentIndex].fallbackData.fallbackValue =
         instance->_private.configuration->convertibleCompartmentSensorData->freshFoodFallbackValueDegFx100;
   }
   else if(convertibleCompartmentState == ConvertibleCompartmentStateType_Freezer)
   {
      instance->_private.configuration->channelData[instance->_private.configuration->convertibleCompartmentIndex].fallbackData.fallbackValue =
         instance->_private.configuration->convertibleCompartmentSensorData->freezerFallbackValueDegFx100;
   }
}

static void ConvertibleCompartmentStateChangedCallback(void *context, const void *args)
{
   SensorFilteredReading_t *instance = context;
   const ConvertibleCompartmentStateType_t *convertibleCompartmentState = args;

   SetWhichConvertibleCompartmentThermistorFallbackValueToUseDependingOnState(instance, *convertibleCompartmentState);
}

static void SetSlewRatePerSecondx100PerChannel(SensorFilteredReading_t *instance)
{
   for(uint8_t index = 0; index < instance->_private.configuration->channelDataCount; index++)
   {
      SensorFilteredReadingChannelData_t *channelData = &instance->_private.configuration->channelData[index];

      channelData->clampData.slewRatePerSecondx100 = UpscaledByPrecisionScalingFactor(
         AdjustedSlewRateByFilterAlphaValue(channelData));
   }
}

static void SetFilterHasNotBeenSeededForEachChannel(SensorFilteredReading_t *instance)
{
   for(uint8_t index = 0; index < instance->_private.configuration->channelDataCount; index++)
   {
      SensorFilteredReadingChannelData_t *channelData = &instance->_private.configuration->channelData[index];

      channelData->filterHasBeenSeeded = false;
   }
}

static void InitializeFilter(SensorFilteredReading_t *instance)
{
   for(uint8_t index = 0; index < instance->_private.configuration->channelDataCount; index++)
   {
      SensorFilteredReadingChannelData_t *channelData = &instance->_private.configuration->channelData[index];

      int32_t unfilteredValue = AdcMappedTemperatureDegFx100Value(
         instance,
         channelData);
      DataModel_Write(
         instance->_private.dataModel,
         channelData->erds.unfilteredOutputErd,
         &unfilteredValue);

      if(ValueIsValid(channelData, unfilteredValue))
      {
         UpdateFilter(channelData, UpscaledByPrecisionScalingFactor(unfilteredValue));

         DataModel_Write(
            instance->_private.dataModel,
            channelData->erds.filteredOutputErd,
            &unfilteredValue);
      }
      else
      {
         channelData->fallbackData.sensorState = SensorIsBad;

         DataModel_Write(
            instance->_private.dataModel,
            channelData->erds.filteredOutputErd,
            &channelData->fallbackData.fallbackValue);
      }
   }
}

void SensorFilteredReading_Init(
   SensorFilteredReading_t *instance,
   I_DataModel_t *dataModel,
   SensorFilteredReadingConfiguration_t *config)
{
   instance->_private.configuration = config;
   instance->_private.dataModel = dataModel;

   ConvertibleCompartmentStateType_t convertibleCompartmentState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.configuration->convertibleCompartmentStateErd,
      &convertibleCompartmentState);
   SetWhichConvertibleCompartmentThermistorFallbackValueToUseDependingOnState(instance, convertibleCompartmentState);

   SetSlewRatePerSecondx100PerChannel(instance);
   SetFilterHasNotBeenSeededForEachChannel(instance);
   InitializeFilter(instance);

   EventSubscription_Init(
      &instance->_private.onConvertibleCompartmentStateChanged,
      instance,
      ConvertibleCompartmentStateChangedCallback);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.configuration->convertibleCompartmentStateErd,
      &instance->_private.onConvertibleCompartmentStateChanged);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         dataModel,
         instance->_private.configuration->timerModule),
      &instance->_private.filterTimer,
      instance->_private.configuration->periodicCheckRateInMsec,
      UpdateSensorValues,
      instance);
}
