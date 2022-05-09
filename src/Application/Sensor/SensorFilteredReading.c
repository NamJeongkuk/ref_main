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

enum
{
   ScalingOption_Downscale,
   ScalingOption_Upscale
};

static int32_t GetAdjustedSlewRateByFilterAlphaValue(SensorFilteredReadingChannelData_t *channelData)
{
   return channelData->clampData.slewRatePerSecondx100 * channelData->filterAlphax100;
}

static int32_t GetAdjustedValueUsingPrecisionScalingFactor(uint8_t scalingOption, int32_t unadjustedValue)
{
   if(scalingOption == ScalingOption_Downscale)
   {
      return unadjustedValue / PrecisionScalingFactor;
   }
   else // ScalingOption_Upscale
   {
      return unadjustedValue * PrecisionScalingFactor;
   }
}

static bool PreviousFilterValueIsValid(SensorFilteredReadingChannelData_t *channelData, int32_t unfilteredValue)
{
   return unfilteredValue != channelData->filterInvalidValue;
}

static int32_t GetUpdatedFilteredValueFromFilter(I_Filter_t *filter)
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

static void DownscaleValueUsingPrecisionScalingFactorThenWriteToErd(I_DataModel_t *dataModel, Erd_t erd, int32_t value)
{
   int16_t downscaledValue = (int16_t)GetAdjustedValueUsingPrecisionScalingFactor(ScalingOption_Downscale, value);

   DataModel_Write(
      dataModel,
      erd,
      &downscaledValue);
}

static int32_t ReadErdValueThenUpscaleUsingPrecisionScalingFactor(I_DataModel_t *dataModel, Erd_t erd)
{
   int16_t value;

   DataModel_Read(
      dataModel,
      erd,
      &value);

   return GetAdjustedValueUsingPrecisionScalingFactor(ScalingOption_Upscale, (int16_t)value);
}

static int32_t SlewRateFilter(SensorFilteredReadingChannelData_t *channelData, int32_t newUnfilteredValue, int32_t previousFilteredValue)
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

static void WriteFallbackDegFx100ValueToFilteredErd(I_DataModel_t *dataModel, SensorFilteredReadingChannelData_t *channelData)
{
   DataModel_Write(
      dataModel,
      channelData->erds.filteredOutputErd,
      &channelData->fallbackData.fallbackValue);
}

static void WriteUpdatedFilteredDegFx100ValueToFilteredErdUsingSlewAndEWMAFilters(
   I_DataModel_t *dataModel,
   SensorFilteredReadingChannelData_t *channelData,
   int32_t unfilteredValue,
   int32_t previousFilteredValue)
{
   int32_t slewFilteredValue = SlewRateFilter(channelData, unfilteredValue, previousFilteredValue);
   UpdateFilter(channelData, slewFilteredValue);
   int32_t ewmaFilteredValue = GetUpdatedFilteredValueFromFilter(channelData->filter);
   DownscaleValueUsingPrecisionScalingFactorThenWriteToErd(dataModel, channelData->erds.filteredOutputErd, ewmaFilteredValue);
}

static void WriteUnfilteredDegFx100ValueToFilteredErd(I_DataModel_t *dataModel, SensorFilteredReadingChannelData_t *channelData, int32_t unfilteredValue)
{
   DownscaleValueUsingPrecisionScalingFactorThenWriteToErd(dataModel, channelData->erds.filteredOutputErd, unfilteredValue);
}

static int32_t GetAdcMappedTemperatureDegFx100Value(
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

static SensorFilteredReadingChannelData_t *GetChannelDataAndUpdateUnfilteredErdWithNewMappedValue(SensorFilteredReading_t *instance, uint8_t index)
{
   SensorFilteredReadingChannelData_t *channelData = &instance->_private.configuration->channelData[index];
   int32_t unfilteredValue = GetAdjustedValueUsingPrecisionScalingFactor(
      ScalingOption_Upscale,
      GetAdcMappedTemperatureDegFx100Value(
         instance,
         channelData));

   channelData->previousFilterValueIsValid = PreviousFilterValueIsValid(
      channelData,
      GetAdjustedValueUsingPrecisionScalingFactor(
         ScalingOption_Downscale,
         unfilteredValue));

   DownscaleValueUsingPrecisionScalingFactorThenWriteToErd(
      instance->_private.dataModel,
      channelData->erds.unfilteredOutputErd,
      unfilteredValue);

   return channelData;
}

static void UpdateFilteredValues(SensorFilteredReadingChannelData_t *channelData, I_DataModel_t *dataModel, uint8_t writeToFilteredErd)
{
   int32_t unfilteredValue;
   int32_t previousFilteredValue;

   previousFilteredValue = ReadErdValueThenUpscaleUsingPrecisionScalingFactor(dataModel, channelData->erds.filteredOutputErd);
   unfilteredValue = ReadErdValueThenUpscaleUsingPrecisionScalingFactor(dataModel, channelData->erds.unfilteredOutputErd);

   if(writeToFilteredErd == UpdateFilteredErdUsingFilter)
   {
      WriteUpdatedFilteredDegFx100ValueToFilteredErdUsingSlewAndEWMAFilters(dataModel, channelData, unfilteredValue, previousFilteredValue);
   }
   else if(writeToFilteredErd == UpdateFilteredErdWithFallback)
   {
      WriteFallbackDegFx100ValueToFilteredErd(dataModel, channelData);
   }
   else if(writeToFilteredErd == ForceUpdateFilteredErd)
   {
      ResetFilter(channelData);
      UpdateFilter(channelData, unfilteredValue);
      WriteUnfilteredDegFx100ValueToFilteredErd(dataModel, channelData, unfilteredValue);
   }
}

static void SetFilteredErdFlagAndSensorStateBasedOnGoodReadingCounter(SensorFilteredReadingChannelData_t *channelData, uint8_t *writeToFilteredErd)
{
   channelData->fallbackData.badReadingCounter = 0;

   if(channelData->fallbackData.sensorState == SensorIsGood)
   {
      *writeToFilteredErd = UpdateFilteredErdUsingFilter;
   }
   else
   {
      if(channelData->fallbackData.goodReadingCounter < channelData->fallbackData.goodReadingMaxValue - 1)
      {
         channelData->fallbackData.goodReadingCounter += 1;
         *writeToFilteredErd = UpdateFilteredErdWithFallback;
      }
      else
      {
         *writeToFilteredErd = ForceUpdateFilteredErd;
         channelData->fallbackData.sensorState = SensorIsGood;
      }
   }
}

static void SetFilteredErdFlagAndSensorStateBasedOnBadReadingCounter(SensorFilteredReadingChannelData_t *channelData, uint8_t *writeToFilteredErd)
{
   channelData->fallbackData.goodReadingCounter = 0;

   if(channelData->fallbackData.sensorState == SensorIsGood)
   {
      if(channelData->fallbackData.badReadingCounter < channelData->fallbackData.badReadingMaxValue - 1)
      {
         channelData->fallbackData.badReadingCounter += 1;
      }
      else
      {
         channelData->fallbackData.sensorState = SensorIsBad;
         *writeToFilteredErd = UpdateFilteredErdWithFallback;
      }
   }
   else
   {
      *writeToFilteredErd = UpdateFilteredErdWithFallback;
   }
}

static void UpdateSensorValues(void *context)
{
   REINTERPRET(instance, context, SensorFilteredReading_t *);
   SensorFilteredReadingChannelData_t *channelData;
   uint8_t writeToFilteredErd;

   for(uint8_t index = 0; index < instance->_private.configuration->channelDataCount; index++)
   {
      channelData = GetChannelDataAndUpdateUnfilteredErdWithNewMappedValue(instance, index);
      writeToFilteredErd = DontUpdateFilteredErd;

      if(channelData->previousFilterValueIsValid)
      {
         SetFilteredErdFlagAndSensorStateBasedOnGoodReadingCounter(channelData, &writeToFilteredErd);
      }
      else
      {
         SetFilteredErdFlagAndSensorStateBasedOnBadReadingCounter(channelData, &writeToFilteredErd);
      }

      UpdateFilteredValues(channelData, instance->_private.dataModel, writeToFilteredErd);
   }
}

static void SetConvertibleCompartmentCabinetThermistorFallbackValue(SensorFilteredReading_t *instance, const uint8_t convertibleCompartmentCabinetState)
{
   if(convertibleCompartmentCabinetState == ConvertibleCompartmentCabinetState_FreshFood)
   {
      instance->_private.configuration->channelData[instance->_private.configuration->convertibleCompartmentCabinetIndex].fallbackData.fallbackValue = instance->_private.configuration->convertibleCompartmentSensorData->freshFoodFallbackValueDegFx100;
   }
   else // ConvertibleCompartmentCabinetState_Freezer
   {
      instance->_private.configuration->channelData[instance->_private.configuration->convertibleCompartmentCabinetIndex].fallbackData.fallbackValue = instance->_private.configuration->convertibleCompartmentSensorData->freezerFallbackValueDegFx100;
   }
}

static void ConvertibleCompartmentCabinetStateChangedCallback(void *context, const void *args)
{
   REINTERPRET(instance, context, SensorFilteredReading_t *);
   REINTERPRET(convertibleCompartmentCabinetState, args, const ConvertibleCompartmentCabinetStateType_t *);

   SetConvertibleCompartmentCabinetThermistorFallbackValue(instance, *convertibleCompartmentCabinetState);
}

static void InitializeFilter(SensorFilteredReading_t *instance)
{
   SensorFilteredReadingChannelData_t *channelData;

   for(uint8_t index = 0; index < instance->_private.configuration->channelDataCount; index++)
   {
      channelData = GetChannelDataAndUpdateUnfilteredErdWithNewMappedValue(instance, index);
      channelData->clampData.slewRatePerSecondx100 = GetAdjustedValueUsingPrecisionScalingFactor(ScalingOption_Upscale, GetAdjustedSlewRateByFilterAlphaValue(channelData));
      channelData->filterHasBeenSeeded = false;

      if(channelData->previousFilterValueIsValid)
      {
         int32_t unfilteredValue = ReadErdValueThenUpscaleUsingPrecisionScalingFactor(instance->_private.dataModel, channelData->erds.unfilteredOutputErd);
         UpdateFilter(channelData, unfilteredValue);
         DownscaleValueUsingPrecisionScalingFactorThenWriteToErd(instance->_private.dataModel, channelData->erds.filteredOutputErd, unfilteredValue);
      }
      else
      {
         channelData->fallbackData.sensorState = SensorIsBad;
         WriteFallbackDegFx100ValueToFilteredErd(instance->_private.dataModel, channelData);
      }
   }
}

static void InitializeConvertibleCompartmentCabinetFallbackValue(SensorFilteredReading_t *instance)
{
   ConvertibleCompartmentCabinetStateType_t convertibleCompartmentCabinetState;
   DataModel_Read(instance->_private.dataModel, instance->_private.configuration->convertibleCompartmentCabinetStateErd, &convertibleCompartmentCabinetState);
   SetConvertibleCompartmentCabinetThermistorFallbackValue(instance, convertibleCompartmentCabinetState);
}

void SensorFilteredReading_Init(
   SensorFilteredReading_t *instance,
   I_DataModel_t *dataModel,
   SensorFilteredReadingConfiguration_t *config)
{
   instance->_private.configuration = config;
   instance->_private.dataModel = dataModel;

   InitializeConvertibleCompartmentCabinetFallbackValue(instance);
   InitializeFilter(instance);

   EventSubscription_Init(
      &instance->_private.onConvertibleCompartmentCabinetStateChanged,
      instance,
      ConvertibleCompartmentCabinetStateChangedCallback);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.configuration->convertibleCompartmentCabinetStateErd,
      &instance->_private.onConvertibleCompartmentCabinetStateChanged);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(dataModel, instance->_private.configuration->timerModule),
      &instance->_private.filterTimer,
      instance->_private.configuration->periodicCheckRateInMsec,
      UpdateSensorValues,
      instance);
}
