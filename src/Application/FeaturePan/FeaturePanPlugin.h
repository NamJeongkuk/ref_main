/*!
 * @file
 * @brief Plugin for four door feature pan modules
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANPLUGIN_H
#define FEATUREPANPLUGIN_H

#include "ConvertibleCompartmentHeaterVotingFrameworkPlugin.h"
#include "FeaturePanAsConvertibleCompartmentDualEvapFanVoting.h"
#include "FeaturePanAsDeliPanEvapFanVoting.h"
#include "FeaturePanConvertibleFreezerEvapFanDependencyVoting.h"
#include "FeaturePanDeliDeliFanDependencyVoting.h"
#include "FeaturePanDeliFreshFoodEvapFanDependencyVoting.h"
#include "FeaturePanGridPlugin.h"
#include "FeaturePanHeaterDisablingVoting.h"
#include "FeaturePanModeRequestHandler.h"
#include "FeaturePanModeSetpointResolver.h"
#include "FeaturePanPulldownVoting.h"
#include "FeaturePanSetpointOffsetUpdater.h"
#include "FeaturePanUserSetpointVoter.h"
#include "FeaturePanWarmupSlopeVoting.h"

typedef struct
{
   struct
   {
      ConvertibleCompartmentHeaterVotingFrameworkPlugin_t convertibleCompartmentHeaterVotingFrameworkPlugin;
      FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t featurePanAsConvertibleCompartmentDualEvapFanVoting;
      FeaturePanAsDeliPanEvapFanVoting_t featurePanAsDeliPanEvapFanVoting;
      FeaturePanConvertibleFreezerEvapFanDependencyVoting_t featurePanConvertibleFreezerEvapFanDependencyVoting;
      FeaturePanDeliDeliFanDependencyVoting_t featurePanDeliDeliFanDependencyVoting;
      FeaturePanDeliFreshFoodEvapFanDependencyVoting_t featurePanDeliFreshFoodEvapFanDependencyVoting;
      FeaturePanGridPlugin_t featurePanGridPlugin;
      FeaturePanHeaterDisablingVoting_t featurePanHeaterDisablingVoting;
      FeaturePanModeRequestHandler_t featurePanModeRequestHandler;
      FeaturePanModeSetpointResolver_t convertibleCompartmentModeSetpointResolver;
      FeaturePanPulldownVoting_t featurePanPulldownVoting;
      FeaturePanSetpointOffsetUpdater_t featurePanSetpointOffsetUpdater;
      FeaturePanUserSetpointVoter_t featurePanUserSetpointVoter;
      FeaturePanWarmupSlopeVoting_t featurePanWarmupSlopeVoting;
   } _private;
} FeaturePanPlugin_t;

void FeaturePanPlugin_Init(FeaturePanPlugin_t *instance, I_DataModel_t *dataModel);

#endif
