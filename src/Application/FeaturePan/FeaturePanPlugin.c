/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FeaturePanPlugin.h"
#include "SystemErds.h"

const FeaturePanAsConvertibleCompartmentDualEvapFanVotingConfig_t dualEvapVotingConfig = {
   .featurePanModeErd = Erd_FeaturePanMode,
   .evapFanVote = Erd_FreezerEvapFanSpeed_ResolvedVote
};

void FeaturePanPlugin_Init(
   FeaturePanPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   FeaturePanAsConvertibleCompartmentDualEvapFanVoting_Init(
      &instance->_private.featurePanAsConvertibleCompartmentDualEvapFanVoting,
      dataModel,
      &dualEvapVotingConfig);
}
