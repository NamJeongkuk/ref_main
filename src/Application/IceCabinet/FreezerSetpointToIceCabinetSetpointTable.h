/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERSETPOINTTOICECABINETSETPOINTTABLE_H
#define FREEZERSETPOINTTOICECABINETSETPOINTTABLE_H

#include "TemperatureDegFx100.h"
#include <stdint.h>

typedef struct
{
   TemperatureDegFx100_t freezerResolvedSetpointInDegFx100;
   TemperatureDegFx100_t iceCabinetSetpointInDegFx100;
} FreezerSetpointToIceCabinetSetpointTableEntry_t;

typedef struct
{
   const FreezerSetpointToIceCabinetSetpointTableEntry_t *entries;
   uint8_t numEntries;
} FreezerSetpointToIceCabinetSetpointTable_t;

#endif
