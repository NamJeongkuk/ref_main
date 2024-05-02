/*!
 * @file
 * @brief Module that handles writing the resolved feature pan setpoint based on the setpoint range
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANMODESETPOINTRESOLVER_H
#define FEATUREPANMODESETPOINTRESOLVER_H

#include "EventSubscription.h"
#include "I_DataModel.h"
#include "TemperatureDegFx100.h"

typedef struct
{
   Erd_t setpointRequestErd; // TemperatureDegFx100_t
   Erd_t setpointStatusErd; // TemperatureDegFx100_t
   Erd_t setpointRangeErd; // FeaturePanModeRange_t
} FeaturePanModeSetpointResolverConfigurationEntry_t;

typedef struct
{
   const FeaturePanModeSetpointResolverConfigurationEntry_t *entries;
   const uint8_t numberOfEntries;
} FeaturePanModeSetpointResolverConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const FeaturePanModeSetpointResolverConfiguration_t *config;
      I_DataModel_t *dataModel;
   } _private;
} FeaturePanModeSetpointResolver_t;

void FeaturePanModeSetpointResolver_Init(
   FeaturePanModeSetpointResolver_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanModeSetpointResolverConfiguration_t *config);

#endif
