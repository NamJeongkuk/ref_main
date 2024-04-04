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
   uint8_t gridInvalidSecondDimensionThermistorRow;
   uint8_t gridInvalidFirstDimensionThermistorColumn;
   TemperatureDegFx100_t freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
   uint16_t freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF;
   uint16_t gridPeriodicRunRateInMSec;
} GridData_t;

#endif
