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

typedef struct
{
   uint8_t numberOfLines;
   TemperatureDegFx100_t *gridLinesDegFx100;
} CalculatedAxisGridLines_t;

typedef struct
{
   uint8_t dimensions;
   CalculatedAxisGridLines_t *gridLines;
} CalculatedGridLines_t;

#endif
