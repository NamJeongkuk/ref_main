/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideEnhancedSabbathPlugin.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

static const EnhancedSabbathAverageTemperatureConfig_t freshFoodEnhancedSabbathAverageTemperatureConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureInDegFx100,
   .unfilteredTemperatureInDegFx100Erd = Erd_FreshFood_UnfilteredTemperatureInDegFx100,
   .overrideRequestErd = Erd_FreshFood_AveragedTemperatureOverrideRequest,
   .overrideValueErd = Erd_FreshFood_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeEnable,
   .timerModuleErd = Erd_TimerModule,
};

static const EnhancedSabbathAverageTemperatureConfig_t freezerEnhancedSabbathAverageTemperatureConfig = {
   .filteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureInDegFx100,
   .unfilteredTemperatureInDegFx100Erd = Erd_Freezer_UnfilteredTemperatureInDegFx100,
   .overrideRequestErd = Erd_Freezer_AveragedTemperatureOverrideRequest,
   .overrideValueErd = Erd_Freezer_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeEnable,
   .timerModuleErd = Erd_TimerModule,
};

static const Erd_t lightVotes[] = {
   Erd_FreshFoodBackWallLight_EnhancedSabbathVote,
   Erd_FreshFoodTopLight_EnhancedSabbathVote,
   Erd_FreezerBackWallLight_EnhancedSabbathVote,
   Erd_FreezerTopLight_EnhancedSabbathVote,
};

static const ErdList_t lightVoteErdList = {
   .erds = lightVotes,
   .numberOfErds = NUM_ELEMENTS(lightVotes)
};

static const EnhancedSabbathModeConfig_t enhancedSabbathModeConfig = {
   .disableMinimumCompressorTimesVoteErd = Erd_DisableMinimumCompressorTimes_EnhancedSabbathVote,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_EnhancedSabbathVote,
   .condenserFanVoteErd = Erd_CondenserFanSpeed_SabbathVote,
   .freezerEvapFanVoteErd = Erd_FreezerEvapFanSpeed_SabbathVote,
   .damperPositionVoteErd = Erd_FreshFoodDamperPosition_EnhancedSabbathVote,
   .enhancedSabbathModeStatusErd = Erd_EnhancedSabbathModeEnable,
   .regularSabbathModeStatusErd = Erd_SabbathModeEnable,
   .freshFoodSetpointVoteErd = Erd_FreshFoodSetpoint_EnhancedSabbathVote,
   .freezerSetpointVoteErd = Erd_FreezerSetpoint_EnhancedSabbathVote,
   .freshFoodAverageCabinetTemperatureErd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freezerAverageCabinetTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .freshFoodCabinetSetpointErd = Erd_FreshFood_AdjustedSetpointWithoutShiftInDegFx100,
   .freezerCabinetSetpointErd = Erd_Freezer_AdjustedSetpointWithoutShiftInDegFx100,
   .overrideIceMakerEnabledRequestErd = Erd_IceMakerEnabledEnhancedSabbathOverrideRequest,
   .overrideIceMakerEnabledValueErd = Erd_IceMakerEnabledEnhancedSabbathOverrideValue,
   .dispensingDisabledErd = Erd_DispensingDisabled,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .gridOverrideEnabledErd = Erd_GridOverrideEnable,
   .coolingModeErd = Erd_CoolingMode,
   .hsmStateErd = Erd_EnhancedSabbathModeHsmState,
   .defrostingErd = Erd_Defrosting,
   .enhancedSabbathIsRequestingDefrostErd = Erd_EnhancedSabbathIsRequestingDefrost,
   .enhancedSabbathStageFreshFoodCoolingIsActiveErd = Erd_EnhancedSabbathStageFreshFoodCoolingIsActive,
   .enhancedSabbathStageFreezerCoolingIsActiveErd = Erd_EnhancedSabbathStageFreezerCoolingIsActive,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .lightVoteErdList = lightVoteErdList
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

   EnhancedSabbathMode_Init(
      &instance->_private.enhancedSabbathMode,
      dataModel,
      DataModelErdPointerAccess_GetTimerModule(
         dataModel,
         Erd_TimerModule),
      PersonalityParametricData_Get(dataModel)->enhancedSabbathData,
      &enhancedSabbathModeConfig);
}
