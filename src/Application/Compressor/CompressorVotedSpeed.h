/*!
 * @file
 * @brief Compressor speed definition.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef COMPRESSORVOTEDSPEED_H
#define COMPRESSORVOTEDSPEED_H

#include <stdint.h>
#include "Vote.h"

enum
{
   CompressorSpeed_Off,
   CompressorSpeed_SuperLow,
   CompressorSpeed_Low,
   CompressorSpeed_Medium,
   CompressorSpeed_High,
   CompressorSpeed_SuperHigh,
   CompressorSpeed_MaxNumberOfSpeeds
};
typedef uint8_t CompressorSpeed_t;

typedef struct
{
   CompressorSpeed_t speed;
   Vote_t care;
} CompressorVotedSpeed_t;

#endif
