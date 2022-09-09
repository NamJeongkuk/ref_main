/*!
 * @file
 * @brief adjusted setpoint parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADJUSTEDSETPOINTDATA_H
#define ADJUSTEDSETPOINTDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t coldOffsetInDegFx100;
   TemperatureDegFx100_t midOffsetInDegFx100;
   TemperatureDegFx100_t warmOffsetInDegFx100;
} SetpointOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t maxShiftInDegFx100;
   TemperatureDegFx100_t minShiftInDegFx100;
   TemperatureDegFx100_t upperAdjustmentLimitInDegFx100;
   TemperatureDegFx100_t lowerAdjustmentLimitInDegFx100;
   TemperatureDegFx100_t postDefrostShiftInDegFx100;
} ShiftOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t cabinetOffsetInDegFx100;
   int16_t crossAmbientSlopeCoefficient;
   TemperatureDegFx100_t pulldownOffsetInDegFx100;
   const SetpointOffsetData_t *setpointOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} FreshFoodAdjustedSetpointData_t;

typedef struct
{
   const FreshFoodAdjustedSetpointData_t *freshFoodAdjustedSetpointData;
} AdjustedSetpointData_t;

#endif
