/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETDATA_H
#define ICECABINETDATA_H

#include "FreezerSetpointToIceCabinetSetpointTable.h"
#include "GammaTable.h"
#include "IceCabinetFanStartupData.h"

typedef struct
{
   const GammaTable_t *gammaTable;
   const IceCabinetFanStartupData_t *iceCabinetFanStartupData;
   const FreezerSetpointToIceCabinetSetpointTable_t *freezerSetpointToIceCabinetSetpointTable;
} IceCabinetData_t;

#endif
