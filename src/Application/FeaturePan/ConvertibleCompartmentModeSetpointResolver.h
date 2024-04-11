/*!
 * @file
 * @brief Module that handles writing the resolved convertible compartment setpoint based on the setpoint range
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTMODESETPOINTRESOLVER_H
#define CONVERTIBLECOMPARTMENTMODESETPOINTRESOLVER_H

#include "EventSubscription.h"
#include "I_DataModel.h"
#include "TemperatureDegFx100.h"

typedef struct
{
   Erd_t setpointRequestErd; // TemperatureDegFx100_t
   Erd_t setpointStatusErd; // TemperatureDegFx100_t
   Erd_t setpointRangeErd; // FeaturePanModeRange_t
} ConvertibleCompartmentModeSetpointResolverConfigurationEntry_t;

typedef struct
{
   const ConvertibleCompartmentModeSetpointResolverConfigurationEntry_t *entries;
   const uint8_t numberOfEntries;
} ConvertibleCompartmentModeSetpointResolverConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const ConvertibleCompartmentModeSetpointResolverConfiguration_t *config;
      I_DataModel_t *dataModel;
   } _private;
} ConvertibleCompartmentModeSetpointResolver_t;

void ConvertibleCompartmentModeSetpointResolver_Init(
   ConvertibleCompartmentModeSetpointResolver_t *instance,
   I_DataModel_t *dataModel,
   const ConvertibleCompartmentModeSetpointResolverConfiguration_t *config);

#endif
