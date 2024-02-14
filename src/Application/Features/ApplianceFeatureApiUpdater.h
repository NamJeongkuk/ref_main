/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLIANCEFEATUREAPIUPDATER_H
#define APPLIANCEFEATUREAPIUPDATER_H

#include "I_DataModel.h"

typedef struct
{
   const uint32_t *featureBits;
   const uint8_t *apiVersions;
   const Erd_t *boolFeatureErds;
   const uint8_t numberOfElements;
} ApplianceFeatureApiUpdaterTable_t;

typedef struct
{
   const ApplianceFeatureApiUpdaterTable_t boolToFeatureMap;
} ApplianceFeatureApiUpdaterConfig_t;

typedef struct
{
   struct
   {
      const ApplianceFeatureApiUpdaterConfig_t *config;
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
   } _private;
} ApplianceFeatureApiUpdater_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void ApplianceFeatureApiUpdater_Init(
   ApplianceFeatureApiUpdater_t *instance,
   I_DataModel_t *dataModel,
   const ApplianceFeatureApiUpdaterConfig_t *config);

#endif
