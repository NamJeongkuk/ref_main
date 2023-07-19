/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideEnhancedSabbathPlugin.h"
#include "SystemErds.h"

static const EnhancedSabbathAverageTemperatureConfig_t freshFoodEnhancedSabbathAverageTemperatureConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureInDegFx100,
   .unfilteredTemperatureInDegFx100Erd = Erd_FreshFood_UnfilteredTemperatureInDegFx100,
   .overrideRequestErd = Erd_FreshFood_AveragedTemperatureOverrideRequest,
   .overrideValueErd = Erd_FreshFood_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeStatus,
   .timerModuleErd = Erd_TimerModule,
};

static const EnhancedSabbathAverageTemperatureConfig_t freezerEnhancedSabbathAverageTemperatureConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureInDegFx100,
   .unfilteredTemperatureInDegFx100Erd = Erd_Freezer_UnfilteredTemperatureInDegFx100,
   .overrideRequestErd = Erd_Freezer_AveragedTemperatureOverrideRequest,
   .overrideValueErd = Erd_Freezer_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeStatus,
   .timerModuleErd = Erd_TimerModule,
};

static void InitializeEnhancedSabbathAverageTemperatureFiltering(
   SideBySideEnhancedSabbathPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const EnhancedSabbathData_t *enhancedSabbathData = PersonalityParametricData_Get(dataModel)->enhancedSabbathData;

   Filter_WindowAverage_Init(
      &instance->_private.freshFoodEnhancedSabbathWindowAverageFilter,
      instance->_private.freshFoodFilterI16,
      EnhancedSabbathAverageTemperature_TotalNumberOfReadsPerSample,
      true,
      sizeof(int16_t));

   Filter_WindowAverage_Init(
      &instance->_private.freezerEnhancedSabbathWindowAverageFilter,
      instance->_private.freezerFilterI16,
      EnhancedSabbathAverageTemperature_TotalNumberOfReadsPerSample,
      true,
      sizeof(int16_t));

   Input_EnhancedSabbathRandomAdditionalMinute_Init(
      &instance->_private.freshFoodEnhancedSabbathRandomAdditionalMinuteInput);

   Input_EnhancedSabbathRandomAdditionalMinute_Init(
      &instance->_private.freezerEnhancedSabbathRandomAdditionalMinuteInput);

   EnhancedSabbathAverageTemperature_Init(
      &instance->_private.freshFoodEnhancedSabbathAverageTemperature,
      dataModel,
      &instance->_private.freshFoodEnhancedSabbathWindowAverageFilter.interface,
      &instance->_private.freshFoodEnhancedSabbathRandomAdditionalMinuteInput.interface,
      enhancedSabbathData->minTimeBetweenTemperatureAveragingInMinutes,
      &freshFoodEnhancedSabbathAverageTemperatureConfig);

   EnhancedSabbathAverageTemperature_Init(
      &instance->_private.freezerEnhancedSabbathAverageTemperature,
      dataModel,
      &instance->_private.freezerEnhancedSabbathWindowAverageFilter.interface,
      &instance->_private.freezerEnhancedSabbathRandomAdditionalMinuteInput.interface,
      enhancedSabbathData->minTimeBetweenTemperatureAveragingInMinutes,
      &freezerEnhancedSabbathAverageTemperatureConfig);
}

void SideBySideEnhancedSabbathPlugin_Init(
   SideBySideEnhancedSabbathPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeEnhancedSabbathAverageTemperatureFiltering(instance, dataModel);
}
