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
   GridDelta_FirstDimension,
   GridDelta_SecondDimension,

   GridDelta_FeaturePan = GridDelta_FirstDimension,
   GridDelta_FreshFood = GridDelta_FirstDimension,
   GridDelta_Freezer = GridDelta_SecondDimension,
};

enum
{
   GridLine_1,
   GridLine_2,
   GridLine_3,
   GridLine_4,
   GridLine_5,
   GridLine_6,
   GridLine_7,

   GridLine_FeaturePanSuperLow = GridLine_1,
   GridLine_FeaturePanLow = GridLine_2,
   GridLine_FeaturePanLowHyst = GridLine_3,
   GridLine_SetpointDelta = GridLine_4,
   GridLine_FeaturePanHighHyst = GridLine_5,
   GridLine_FeaturePanHigh = GridLine_6,
   GridLine_FeaturePanSuperHigh = GridLine_7,

   GridLine_IceCabinetSuperLow = GridLine_1,
   GridLine_IceCabinetLow = GridLine_2,
   GridLine_IceCabinetLowHyst = GridLine_3,
   GridLine_IceCabinetSetpointDelta = GridLine_4,
   GridLine_IceCabinetHighHyst = GridLine_5,
   GridLine_IceCabinetHigh = GridLine_6,
   GridLine_IceCabinetSuperHigh = GridLine_7,

   GridLine_Nfl = GridLine_1,
   GridLine_FreshFoodLowHyst = GridLine_2,
   GridLine_FreshFoodSetpointDelta = GridLine_3,
   GridLine_FreshFoodHighHyst = GridLine_4,
   GridLine_FreshFoodExtraHigh = GridLine_5,
   GridLine_FreshFoodSuperHigh = GridLine_6,

   GridLine_FreezerLowHyst = GridLine_1,
   GridLine_FreezerSetpointDelta = GridLine_2,
   GridLine_FreezerHighHyst = GridLine_3,
   GridLine_FreezerExtraHigh = GridLine_4,
   GridLine_FreezerSuperHigh = GridLine_5,
   GridLine_FreezerExtremeHigh = GridLine_6,
};

enum
{
   DeltaGridLinesCorrection_Offset,
   DeltaGridLinesCorrection_AdjustedSetpoint
};
typedef uint8_t DeltaGridLinesCorrection_t;

typedef struct
{
   const TemperatureDegFx100_t multiplier;
   const TemperatureDegFx100_t divider;
} CrossAmbientHysteresisAdjustmentData_t;

typedef struct
{
   const TemperatureDegFx100_t gridLinesDegFx100;
   const DeltaGridLinesCorrection_t correction;
   const CrossAmbientHysteresisAdjustmentData_t *crossAmbientAdjustment;
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
