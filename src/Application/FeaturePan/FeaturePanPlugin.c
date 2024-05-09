/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanMode.h"
#include "FeaturePanPlugin.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

static const FeaturePanModeSetpointResolverConfigurationEntry_t entries[] = {
   { .setpointRequestErd = Erd_FeaturePanMode1_Request,
      .setpointStatusErd = Erd_FeaturePanMode1_Status,
      .setpointRangeErd = Erd_FeaturePanMode1_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode2_Request,
      .setpointStatusErd = Erd_FeaturePanMode2_Status,
      .setpointRangeErd = Erd_FeaturePanMode2_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode3_Request,
      .setpointStatusErd = Erd_FeaturePanMode3_Status,
      .setpointRangeErd = Erd_FeaturePanMode3_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode4_Request,
      .setpointStatusErd = Erd_FeaturePanMode4_Status,
      .setpointRangeErd = Erd_FeaturePanMode4_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode5_Request,
      .setpointStatusErd = Erd_FeaturePanMode5_Status,
      .setpointRangeErd = Erd_FeaturePanMode5_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode6_Request,
      .setpointStatusErd = Erd_FeaturePanMode6_Status,
      .setpointRangeErd = Erd_FeaturePanMode6_SetpointRangeData },
   { .setpointRequestErd = Erd_FeaturePanMode7_Request,
      .setpointStatusErd = Erd_FeaturePanMode7_Status,
      .setpointRangeErd = Erd_FeaturePanMode7_SetpointRangeData },
};

STATIC_ASSERT(FeaturePanMode_NumberOfFeaturePanModes == NUM_ELEMENTS(entries));

static const FeaturePanModeSetpointResolverConfiguration_t convertibleCompartmentModeSetpointResolverConfig = {
   .entries = entries,
   .numberOfEntries = NUM_ELEMENTS(entries)
};

static const Erd_t resolvedModeSetpointErds[] = {
   Erd_FeaturePanMode1_Status,
   Erd_FeaturePanMode2_Status,
   Erd_FeaturePanMode3_Status,
   Erd_FeaturePanMode4_Status,
   Erd_FeaturePanMode5_Status,
   Erd_FeaturePanMode6_Status,
   Erd_FeaturePanMode7_Status
};

static const FeaturePanUserSetpointVoterConfiguration_t featurePanUserSetpointVoterConfig = {
   .featurePanCurrentModeErd = Erd_FeaturePanCurrentModeStatus,
   .userSetpointVoteErd = Erd_FeaturePanSetpoint_UserVote,
   .resolvedModeSetpointErds = resolvedModeSetpointErds,
   .numberOfErds = NUM_ELEMENTS(resolvedModeSetpointErds)
};

static const FeaturePanSetpointOffsetUpdaterConfiguration_t featurePanSetpointOffsetUpdaterConfig = {
   .featurePanCurrentModeErd = Erd_FeaturePanCurrentModeStatus,
   .setpointOffsetErd = Erd_FeaturePan_SetpointOffsetInDegFx100
};

static const FeaturePanWarmupSlopeVotingConfig_t featurePanWarmupSlopeVotingConvertibleCompartmentConfig = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .featurePanTemperatureDegFx100Erd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
   .heaterVotedErd = Erd_ConvertibleCompartmentHeater_WarmupSlopeVote,
};

static const FeaturePanWarmupSlopeVotingConfig_t featurePanWarmupSlopeVotingDeliPanConfig = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .featurePanTemperatureDegFx100Erd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
   .heaterVotedErd = Erd_DeliPanHeater_WarmupSlopeVote,
};

static const FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t dualEvapVotingConfig = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .evapFanVote = Erd_FreezerEvapFanSpeed_FeaturePanVote
};

static const FeaturePanHeaterDisablingVotingConfig_t heaterDisablingVotingConvertibleCompartmentConfig = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .heaterVotedErd = Erd_ConvertibleCompartmentHeater_DisableForAmbientTemperatureVote,
   .ambientTemperatureDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100
};

static const FeaturePanHeaterDisablingVotingConfig_t heaterDisablingVotingDeliPanConfig = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .heaterVotedErd = Erd_DeliPanHeater_DisableForAmbientTemperatureVote,
   .ambientTemperatureDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100
};

static const FeaturePanPulldownVotingConfig_t featurePanPulldownVotingConvertibleCompartmentConfig = {
   .featurePanTemperatureErd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
   .featurePanDamperPositionVoteErd = Erd_ConvertibleCompartmentDamperPosition_PulldownVote,
   .featurePanFanVoteErd = Erd_ConvertibleCompartmentEvapFanSpeed_PulldownVote,
   .featurePanHeaterVoteErd = Erd_ConvertibleCompartmentHeater_PulldownVote
};

static const FeaturePanPulldownVotingConfig_t featurePanPulldownVotingDeliPanConfig = {
   .featurePanTemperatureErd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
   .featurePanDamperPositionVoteErd = Erd_DeliDamperPosition_PulldownVote,
   .featurePanFanVoteErd = Erd_DeliFanSpeed_PulldownVote,
   .featurePanHeaterVoteErd = Erd_DeliPanHeater_PulldownVote
};

static const FeaturePanDeliFreshFoodEvapFanDependencyVotingConfig_t featurePanDeliFreshFoodEvapFanDependencyVotingConfig = {
   .evapFanResolvedVoteErd = Erd_FreshFoodEvapFanSpeed_ResolvedVote,
   .fanVoteErd = Erd_DeliFanSpeed_FreshFoodFanDependencyVote,
   .damperVoteErd = Erd_DeliDamperPosition_FreshFoodFanDependencyVote
};

static const FeaturePanAsDeliPanEvapFanVotingConfig_t featurePanAsDeliPanEvapFanVotingConfig = {
   .featurePanCoolingModeErd = Erd_FeaturePanCoolingMode,
   .fanEvapVoteErd = Erd_FreshFoodEvapFanSpeed_DeliPanSpeedVote,
   .fanEvapGridVoteErd = Erd_FreshFoodEvapFanSpeed_GridVote
};

static const FeaturePanConvertibleFreezerEvapFanDependencyVotingConfiguration_t featurePanConvertibleFreezerEvapFanDependencyVotingConfig = {
   .freezerEvapFanResolvedVoteErd = Erd_FreezerEvapFanSpeed_ResolvedVote,
   .convertibleCompartmentDamperVoteErd = Erd_ConvertibleCompartmentDamperPosition_FreezerEvapFanDependencyVote
};

static const FeaturePanDeliDeliFanDependencyVotingConfig_t featurePanDeliDeliFanDepenencyVotingConfig = {
   .deliFanActualRpmErd = Erd_DeliFan_ActualRpm,
   .deliHeaterVotedErd = Erd_DeliPanHeater_DeliFanDependencyVote
};

static void WriteSetpointRangeDataToErds(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *personalityData = PersonalityParametricData_Get(dataModel);

   for(uint8_t i = 0; i < NUM_ELEMENTS(entries); i++)
   {
      DataModel_Write(
         dataModel,
         entries[i].setpointRangeErd,
         &personalityData->setpointData->userSetpointData->featurePanUserSetpointData->featurePanModeUserSetpointData[i]);
   }
}

void FeaturePanPlugin_Init(FeaturePanPlugin_t *instance, I_DataModel_t *dataModel)
{
   WriteSetpointRangeDataToErds(dataModel);

   FeaturePanModeRequestHandler_Init(
      &instance->_private.featurePanModeRequestHandler,
      dataModel,
      PersonalityParametricData_Get(dataModel)->featurePanData);

   FeaturePanModeSetpointResolver_Init(
      &instance->_private.convertibleCompartmentModeSetpointResolver,
      dataModel,
      &convertibleCompartmentModeSetpointResolverConfig);

   FeaturePanUserSetpointVoter_Init(
      &instance->_private.featurePanUserSetpointVoter,
      dataModel,
      &featurePanUserSetpointVoterConfig);

   FeaturePanSetpointOffsetUpdater_Init(
      &instance->_private.featurePanSetpointOffsetUpdater,
      dataModel,
      &featurePanSetpointOffsetUpdaterConfig);

   const PlatformData_t *platformData = PersonalityParametricData_Get(dataModel)->platformData;

   if(BITMAP_STATE(platformData->compartmentBitmap.bitmap, Compartment_Convertible))
   {
      FeaturePanWarmupSlopeVoting_Init(
         &instance->_private.featurePanWarmupSlopeVoting,
         dataModel,
         DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
         &featurePanWarmupSlopeVotingConvertibleCompartmentConfig,
         PersonalityParametricData_Get(dataModel)->featurePanData);

      FeaturePanHeaterDisablingVoting_Init(
         &instance->_private.featurePanHeaterDisablingVoting,
         dataModel,
         &heaterDisablingVotingConvertibleCompartmentConfig,
         PersonalityParametricData_Get(dataModel)->featurePanData);

      FeaturePanPulldownVoting_Init(
         &instance->_private.featurePanPulldownVoting,
         dataModel,
         &featurePanPulldownVotingConvertibleCompartmentConfig,
         PersonalityParametricData_Get(dataModel)->featurePanData);

      FeaturePanAsConvertibleCompartmentDualEvapFanVoting_Init(
         &instance->_private.featurePanAsConvertibleCompartmentDualEvapFanVoting,
         dataModel,
         &dualEvapVotingConfig);

      FeaturePanConvertibleFreezerEvapFanDependencyVoting_Init(
         &instance->_private.featurePanConvertibleFreezerEvapFanDependencyVoting,
         dataModel,
         &featurePanConvertibleFreezerEvapFanDependencyVotingConfig);
   }
   else if(BITMAP_STATE(platformData->compartmentBitmap.bitmap, Compartment_DeliPan))
   {
      FeaturePanWarmupSlopeVoting_Init(
         &instance->_private.featurePanWarmupSlopeVoting,
         dataModel,
         DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
         &featurePanWarmupSlopeVotingDeliPanConfig,
         PersonalityParametricData_Get(dataModel)->featurePanData);

      FeaturePanHeaterDisablingVoting_Init(
         &instance->_private.featurePanHeaterDisablingVoting,
         dataModel,
         &heaterDisablingVotingDeliPanConfig,
         PersonalityParametricData_Get(dataModel)->featurePanData);

      FeaturePanPulldownVoting_Init(
         &instance->_private.featurePanPulldownVoting,
         dataModel,
         &featurePanPulldownVotingDeliPanConfig,
         PersonalityParametricData_Get(dataModel)->featurePanData);

      FeaturePanAsDeliPanEvapFanVoting_Init(
         &instance->_private.featurePanAsDeliPanEvapFanVoting,
         dataModel,
         &featurePanAsDeliPanEvapFanVotingConfig,
         PersonalityParametricData_Get(dataModel)->featurePanData);

      FeaturePanDeliFreshFoodEvapFanDependencyVoting_Init(
         &instance->_private.featurePanDeliFreshFoodEvapFanDependencyVoting,
         dataModel,
         &featurePanDeliFreshFoodEvapFanDependencyVotingConfig);

      FeaturePanDeliDeliFanDependencyVoting_Init(
         &instance->_private.featurePanDeliDeliFanDependencyVoting,
         dataModel,
         &featurePanDeliDeliFanDepenencyVotingConfig);
   }

   FeaturePanGridPlugin_Init(
      &instance->_private.featurePanGridPlugin,
      dataModel);
}
