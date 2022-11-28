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
   uint16_t gridConfigWord;
   DeltaGridLines_t *deltaGridLines;
   uint16_t gridPeriodicRunRateInMSec;
} GridData_t;

#endif
