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

typedef CalculatedAxisGridLines_t FreshFoodCalculatedAxisGridLines_t;
typedef CalculatedAxisGridLines_t FreezerCalculatedAxisGridLines_t;

typedef struct
{
   FreshFoodCalculatedAxisGridLines_t freshFoodGridLine;
   FreezerCalculatedAxisGridLines_t freezerGridLine;
} CalculatedGridLines_t;

#endif
