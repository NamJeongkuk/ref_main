/*!
 * @file
 * @brief Definition for the calculated grid lines
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef GRIDLINES_H
#define GRIDLINES_H

#include <stdint.h>

typedef struct
{
   uint8_t numberOfLines;
   uint16_t *gridLinesDegFx100;
} SingleDimensionGridLines_t;

typedef struct
{
   uint8_t dimensions;
   SingleDimensionGridLines_t *gridLines;
} GridLines_t;

typedef GridLines_t CalculatedGridLines_t;

#endif
