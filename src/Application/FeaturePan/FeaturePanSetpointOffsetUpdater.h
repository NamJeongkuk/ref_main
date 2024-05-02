/*!
 * @file
 * @brief Updates the Feature Pan Setpoint Offset ERD based on the currently selected Feature Pan mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANSETPOINTOFFSETUPDATER_H
#define FEATUREPANSETPOINTOFFSETUPDATER_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t featurePanCurrentModeErd; // FeaturePanCurrentMode_t
   Erd_t setpointOffsetErd; // TemperatureDegFx100_t
} FeaturePanSetpointOffsetUpdaterConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const FeaturePanSetpointOffsetUpdaterConfiguration_t *config;
      I_DataModel_t *dataModel;
   } _private;
} FeaturePanSetpointOffsetUpdater_t;

void FeaturePanSetpointOffsetUpdater_Init(
   FeaturePanSetpointOffsetUpdater_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanSetpointOffsetUpdaterConfiguration_t *config);

#endif
