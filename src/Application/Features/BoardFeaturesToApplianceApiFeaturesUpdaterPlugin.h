/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BOARDFEATURESTOAPPLIANCEAPIFEATURESUPDATERPLUGIN_H
#define BOARDFEATURESTOAPPLIANCEAPIFEATURESUPDATERPLUGIN_H

#include "I_DataModel.h"
#include "BoardFeaturesToApplianceApiFeaturesUpdater.h"

typedef struct
{
   struct
   {
      BoardFeaturesToApplianceApiFeaturesUpdater_t dispenserFeatureUpdater;
   } _private;
} BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_t;

void BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_Init(
   BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
