/*!
 * @file
 * @brief Compressor speed definition.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef COMPRESSORVOTEDSPEED_H
#define COMPRESSORVOTEDSPEED_H

#include <stdint.h>

enum
{
   CompressorSpeed_Off,
   CompressorSpeed_SuperLow,
   CompressorSpeed_Low,
   CompressorSpeed_Medium,
   CompressorSpeed_High,
   CompressorSpeed_SuperHigh,
   CompressorSpeed_Startup,
   CompressorSpeed_MaxNumberOfSpeeds
};
typedef uint8_t CompressorSpeed_t;

typedef struct
{
   CompressorSpeed_t speed;
   bool care;
} CompressorVotedSpeed_t;

#endif
