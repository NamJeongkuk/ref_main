/*!
 * @file
 * @brief Definition for the calculated grid lines
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef DELTAGRIDLINES_H
#define DELTAGRIDLINES_H

#include <stdint.h>
#include "TemperatureDegFx100.h"

enum
{
   GridDelta_FreshFood,
   GridDelta_Freezer,
};

enum
{
   GridLine_Nfl,
   GridLine_FreshFoodLowHyst,
   GridLine_FreshFoodSetpointDelta,
   GridLine_FreshFoodHighHyst,
   GridLine_FreshFoodExtraHigh,
   GridLine_FreshFoodSuperHigh
};

enum
{
   GridLine_FreezerLowHyst,
   GridLine_FreezerSetpointDelta,
   GridLine_FreezerHighHyst,
   GridLine_FreezerExtraHigh,
   GridLine_FreezerSuperHigh,
   GridLine_FreezerExtremeHigh
};

enum
{
   DeltaGridLinesCorrection_Offset,
   DeltaGridLinesCorrection_AdjustedSetpoint
};
typedef uint8_t DeltaGridLinesCorrection_t;

typedef struct
{
   const TemperatureDegFx100_t gridLinesDegFx100;
   const DeltaGridLinesCorrection_t correction;
} DeltaGridLineData_t;

typedef struct
{
   const uint8_t numberOfLines;
   const DeltaGridLineData_t *gridLineData;
} DeltaAxisGridLines_t;

typedef struct
{
   const uint8_t dimensions;
   const DeltaAxisGridLines_t *gridLines;
} DeltaGridLines_t;

#endif
