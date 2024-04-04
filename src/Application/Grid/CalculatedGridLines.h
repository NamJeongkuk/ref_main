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
   CalculatedGridLines_MaxGridDimensionsCount = 2,
   CalculatedGridLines_MaxGridLinesCount = 7
};

typedef struct
{
   TemperatureDegFx100_t gridLinesDegFx100[CalculatedGridLines_MaxGridLinesCount];
} CalculatedAxisGridLines_t;

typedef CalculatedAxisGridLines_t TwoDimensionalFirstDimensionCalculatedAxisGridLines_t;
typedef CalculatedAxisGridLines_t TwoDimensionalSecondDimensionCalculatedAxisGridLines_t;
typedef CalculatedAxisGridLines_t OneDimensionalCalculatedAxisGridLines_t;

#define SWAP_DEFINITION_TwoDimensionalFirstDimensionCalculatedAxisGridLines_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, gridLinesDegFx100),

#define SWAP_DEFINITION_TwoDimensionalSecondDimensionCalculatedAxisGridLines_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, gridLinesDegFx100),

#define SWAP_DEFINITION_OneDimensionalCalculatedAxisGridLines_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, gridLinesDegFx100),

typedef struct
{
   TwoDimensionalFirstDimensionCalculatedAxisGridLines_t firstDimensionGridLines;
   TwoDimensionalSecondDimensionCalculatedAxisGridLines_t secondDimensionGridLines;
   uint8_t numberOfGridLinesPerDimension;
} TwoDimensionalCalculatedGridLines_t;

#define SWAP_DEFINITION_TwoDimensionalCalculatedGridLines_t(_name, _type)  \
   SWAPPED_FIELD(_name, _type, firstDimensionGridLines.gridLinesDegFx100), \
      SWAPPED_FIELD(_name, _type, secondDimensionGridLines.gridLinesDegFx100),

typedef struct
{
   OneDimensionalCalculatedAxisGridLines_t gridLines;
   uint8_t numberOfGridLines;
} OneDimensionalCalculatedGridLines_t;

#define SWAP_DEFINITION_OneDimensionalCalculatedGridLines_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, gridLines.gridLinesDegFx100),

#endif
