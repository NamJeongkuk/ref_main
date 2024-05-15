/*!
 * @file
 * @brief Parametric data structure for Grid
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDDATA_H
#define GRIDDATA_H

#include "DeltaGridLines.h"
#include "GridId.h"

typedef struct
{
   GridId_t gridId;
   uint8_t gridInvalidSecondDimensionThermistorRow;
   uint8_t gridInvalidFirstDimensionThermistorColumn;
   uint16_t gridPeriodicRunRateInMSec;
} GridData_t;

#endif
