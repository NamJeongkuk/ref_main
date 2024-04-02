/*!
 * @file
 * @brief Plugin for four door feature pan modules
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANPLUGIN_H
#define FEATUREPANPLUGIN_H

#include "I_DataModel.h"
#include "FeaturePanAsConvertibleCompartmentDualEvapFanVoting.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      FeaturePanAsConvertibleCompartmentDualEvapFanVoting_t featurePanAsConvertibleCompartmentDualEvapFanVoting;
   } _private;
} FeaturePanPlugin_t;

void FeaturePanPlugin_Init(
   FeaturePanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
