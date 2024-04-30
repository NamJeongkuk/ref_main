/*!
 * @file
 * @brief Converts the Freezer Resolved Setpoint to an ice cabinet setpoint
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERSETPOINTTOICECABINETSETPOINTMAPPER_H
#define FREEZERSETPOINTTOICECABINETSETPOINTMAPPER_H

#include "I_DataModel.h"
#include "FreezerSetpointToIceCabinetSetpointTable.h"

typedef struct
{
   Erd_t freezerResolvedSetpoint; // SetpointVotedTemperature_t
   Erd_t iceCabinetSetpoint; // TemperatureDegFx100_t
} FreezerSetpointToIceCabinetSetpointMapperConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t freezerSetpointSubscription;
      I_DataModel_t *dataModel;
      const FreezerSetpointToIceCabinetSetpointTable_t *mappingTable;
      const FreezerSetpointToIceCabinetSetpointMapperConfig_t *config;
   } _private;
} FreezerSetpointToIceCabinetSetpointMapper_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param mappingTable
 */
void FreezerSetpointToIceCabinetSetpointMapper_Init(
   FreezerSetpointToIceCabinetSetpointMapper_t *instance,
   I_DataModel_t *dataModel,
   const FreezerSetpointToIceCabinetSetpointMapperConfig_t *config,
   const FreezerSetpointToIceCabinetSetpointTable_t *mappingTable);

#endif
