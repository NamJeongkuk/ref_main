/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLIANCEFEATUREAPIUPDATERPLUGIN_H
#define APPLIANCEFEATUREAPIUPDATERPLUGIN_H

#include "ApplianceFeatureApiUpdater.h"
#include "BoardFeaturesToApplianceApiFeaturesUpdater.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      ApplianceFeatureApiUpdater_t applianceFeatureApiUpdater;
      BoardFeaturesToApplianceApiFeaturesUpdater_t dispenserFeatureUpdater;
   } _private;
} ApplianceFeatureApiUpdaterPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void ApplianceFeatureApiUpdaterPlugin_Init(
   ApplianceFeatureApiUpdaterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
