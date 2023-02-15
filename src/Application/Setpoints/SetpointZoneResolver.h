/*!
 * @file
 * @brief set a zone for a compartment dependent on the voted temperature
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTZONERESOLVER_H
#define SETPOINTZONERESOLVER_H

#include "I_DataModel.h"
#include "SetpointData.h"
#include "SetpointZone.h"
#include "PersonalityParametricData.h"

typedef struct
{
   Erd_t setpointVotedTemperatureErd; // SetpointVotedTemperature_t
   Erd_t setpointZoneErd; // SetpointZone_t
   Erd_t setpointZoneTemperatureBoundsErd; // SetpointZoneTemperatureBounds_t
} SetpointZoneResolverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const SetpointZoneResolverConfig_t *config;
      EventSubscription_t dataModelOnChangedSubscription;
   } _private;
} SetpointZoneResolver_t;

/*!
 * @param instance
 * @param config
 * @param dataModel
 */
void SetpointZoneResolver_Init(
   SetpointZoneResolver_t *instance,
   const SetpointZoneResolverConfig_t *config,
   I_DataModel_t *dataModel);
#endif
