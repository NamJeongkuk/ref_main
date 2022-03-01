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
   DeltaGridLines_BitMapping_SetpointBitMask = 0x01,
   DeltaGridLines_BitMapping_OffsetBitMask = 0x02,
   DeltaGridLines_BitMapping_ShiftBitMask = 0x04,
   DeltaGridLines_BitMapping_AdjSetpointBitMask = 0x08
};
typedef uint8_t DeltaGridLines_BitMapping_t;

typedef struct
{
   const uint8_t numberOfLines;
   const TemperatureDegFx100_t *gridLinesDegFx100;
   const DeltaGridLines_BitMapping_t *bitMappings;
} DeltaAxisGridLines_t;

typedef struct
{
   const uint8_t dimensions;
   const DeltaAxisGridLines_t *gridLines;
} DeltaGridLines_t;

#endif
