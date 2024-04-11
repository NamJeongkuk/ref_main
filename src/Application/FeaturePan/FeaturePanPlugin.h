/*!
 * @file
 * @brief Plugin for four door feature pan modules
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANPLUGIN_H
#define FEATUREPANPLUGIN_H

#include "ConvertibleCompartmentModeSetpointResolver.h"
#include "I_DataModel.h"
#include "FeaturePanAsConvertibleCompartmentDualEvapFanVoting.h"
#include "FeaturePanHeaterDisablingVoting.h"
#include "FeaturePanPulldownVoting.h"
#include "FeaturePanWarmupSlopeVoting.h"

typedef struct
{
   struct
   {
      ConvertibleCompartmentModeSetpointResolver_t convertibleCompartmentModeSetpointResolver;
      FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t featurePanAsConvertibleCompartmentDualEvapFanVoting;
      FeaturePanHeaterDisablingVoting_t featurePanHeaterDisablingVoting;
      FeaturePanPulldownVoting_t featurePanPulldownVoting;
      FeaturePanWarmupSlopeVoting_t featurePanWarmupSlopeVoting;
   } _private;
} FeaturePanPlugin_t;

void FeaturePanPlugin_Init(
   FeaturePanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
