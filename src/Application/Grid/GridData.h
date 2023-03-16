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
   DeltaGridLines_t *deltaGridLines;
   uint8_t grid_invalid_freezer_thermistor_row;
   uint8_t grid_invalid_fresh_food_thermistor_column;
   uint16_t gridPeriodicRunRateInMSec;
} GridData_t;

#endif
