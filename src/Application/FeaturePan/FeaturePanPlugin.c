/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanPlugin.h"
#include "PersonalityParametricData.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

static const FeaturePanWarmupSlopeVotingConfig_t featurePanWarmupSlopeVotingConvertibleCompartmentConfig = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .featurePanTemperatureDegFx100Erd = Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100,
   .heaterVotedErd = Erd_ConvertibleCompartmentHeater_WarmupSlopeVote,
};

static const FeaturePanWarmupSlopeVotingConfig_t featurePanWarmupSlopeVotingDeliPanConfig = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .featurePanTemperatureDegFx100Erd = Erd_DeliPan_FilteredTemperatureResolvedInDegFx100,
   .heaterVotedErd = Erd_DeliPanHeater_WarmupSlopeVote,
};

static const FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t dualEvapVotingConfig = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .evapFanVote = Erd_FreezerEvapFanSpeed_ResolvedVote
};

static const FeaturePanHeaterDisablingVotingConfig_t heaterDisablingVotingConvertibleCompartmentConfig = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .heaterVotedErd = Erd_ConvertibleCompartmentHeater_DisableForAmbientTemperatureVote,
   .ambientTemperatureDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100
};

static const FeaturePanHeaterDisablingVotingConfig_t heaterDisablingVotingDeliPanConfig = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .heaterVotedErd = Erd_DeliPanHeater_DisableForAmbientTemperatureVote,
   .ambientTemperatureDegFx100Erd = Erd_Ambient_FilteredTemperatureResolvedInDegFx100
};

static const FeaturePanPulldownVotingConfig_t featurePanPulldownVotingConvertibleCompartmentConfig = {
   .featurePanTemperatureErd = Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100,
   .featurePanDamperPositionVoteErd = Erd_ConvertibleCompartmentDamperPosition_PulldownVote,
   .featurePanFanVoteErd = Erd_ConvertibleCompartmentEvapFanSpeed_PulldownVote,
   .featurePanHeaterVoteErd = Erd_ConvertibleCompartmentHeater_PulldownVote
};

static const FeaturePanPulldownVotingConfig_t featurePanPulldownVotingDeliPanConfig = {
   .featurePanTemperatureErd = Erd_DeliPan_FilteredTemperatureResolvedInDegFx100,
   .featurePanDamperPositionVoteErd = Erd_DeliDamperPosition_FeaturePanVote,
   .featurePanFanVoteErd = Erd_DeliFanSpeed_FeaturePanVote,
   .featurePanHeaterVoteErd = Erd_DeliPanHeater_PulldownVote
};

void FeaturePanPlugin_Init(
   FeaturePanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
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
   }

   FeaturePanAsConvertibleCompartmentDualEvapFanVoting_Init(
      &instance->_private.featurePanAsConvertibleCompartmentDualEvapFanVoting,
      dataModel,
      &dualEvapVotingConfig);
}
