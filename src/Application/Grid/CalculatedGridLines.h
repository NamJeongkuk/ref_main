/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CALCULATEDGRIDLINES_H
#define CALCULATEDGRIDLINES_H

#include <stdint.h>
#include "TemperatureDegFx100.h"

enum
{
   FreshFoodGridLineDimension = 0,
   FreezerGridLineDimension = 1,
   NumberGridDimensions,
   NumberOfGridLinesPerAxis = 6,
};

typedef struct
{
   TemperatureDegFx100_t gridLinesDegFx100[NumberOfGridLinesPerAxis];
} CalculatedAxisGridLines_t;

#define SWAP_DEFINITION_CalculatedAxisGridLines_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, gridLinesDegFx100[0]),           \
      SWAPPED_FIELD(_name, _type, gridLinesDegFx100[1]),        \
      SWAPPED_FIELD(_name, _type, gridLinesDegFx100[2]),        \
      SWAPPED_FIELD(_name, _type, gridLinesDegFx100[3]),        \
      SWAPPED_FIELD(_name, _type, gridLinesDegFx100[4]),        \
      SWAPPED_FIELD(_name, _type, gridLinesDegFx100[5]),

typedef CalculatedAxisGridLines_t FreshFoodCalculatedAxisGridLines_t;
typedef CalculatedAxisGridLines_t FreezerCalculatedAxisGridLines_t;

typedef struct
{
   FreshFoodCalculatedAxisGridLines_t freshFoodGridLine;
   FreezerCalculatedAxisGridLines_t freezerGridLine;
} CalculatedGridLines_t;

#define SWAP_DEFINITION_CalculatedGridLines_t(_name, _type)                                             \
   SWAPPED_FIELD(_name, _type, freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl]),                      \
      SWAPPED_FIELD(_name, _type, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst]),      \
      SWAPPED_FIELD(_name, _type, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHystDelta]), \
      SWAPPED_FIELD(_name, _type, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodHighHyst]),     \
      SWAPPED_FIELD(_name, _type, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodExtraHigh]),    \
      SWAPPED_FIELD(_name, _type, freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodSuperHigh]),    \
      SWAPPED_FIELD(_name, _type, freezerGridLine.gridLinesDegFx100[GridLine_FreezerLowHyst]),          \
      SWAPPED_FIELD(_name, _type, freezerGridLine.gridLinesDegFx100[GridLine_FreezerDelta]),            \
      SWAPPED_FIELD(_name, _type, freezerGridLine.gridLinesDegFx100[GridLine_FreezerHighHyst]),         \
      SWAPPED_FIELD(_name, _type, freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtraHigh]),        \
      SWAPPED_FIELD(_name, _type, freezerGridLine.gridLinesDegFx100[GridLine_FreezerSuperHigh]),        \
      SWAPPED_FIELD(_name, _type, freezerGridLine.gridLinesDegFx100[GridLine_FreezerExtremeHigh]),

#endif
