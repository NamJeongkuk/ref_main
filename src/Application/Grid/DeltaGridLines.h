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
   GridLine_FreshFoodLowHystDelta,
   GridLine_FreshFoodHighHyst,
   GridLine_FreshFoodExtraHigh,
   GridLine_FreshFoodSuperHigh
};

enum
{
   GridLine_FreezerLowHyst,
   GridLine_FreezerDelta,
   GridLine_FreezerHighHyst,
   GridLine_FreezerExtraHigh,
   GridLine_FreezerSuperHigh,
   GridLine_FreezerExtremeHigh
};

enum
{
   DeltaGridLines_BitMapping_SetpointBitMask = 0x01,
   DeltaGridLines_BitMapping_OffsetBitMask = 0x02,
   DeltaGridLines_BitMapping_ShiftBitMask = 0x04,
   DeltaGridLines_BitMapping_AdjSetpointBitMask = 0x08
};
typedef uint8_t DeltaGridLines_BitMapping_t;

typedef struct
{
   const TemperatureDegFx100_t gridLinesDegFx100;
   const DeltaGridLines_BitMapping_t bitMapping;
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
