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
   CompressorSpeed_On,
   CompressorSpeed_MaxNumberOfSpeedsInSingleType,
   CompressorSpeed_SuperLow = CompressorSpeed_On,
   CompressorSpeed_Low,
   CompressorSpeed_Mid,
   CompressorSpeed_High,
   CompressorSpeed_SuperHigh,
   CompressorSpeed_MaxNumberOfSpeedsInVariableType,
   CompressorSpeed_MaxNumberOfSpeeds = CompressorSpeed_MaxNumberOfSpeedsInVariableType,
};
typedef uint8_t CompressorSpeed_t;

typedef struct
{
   CompressorSpeed_t speed;
   bool care;
} CompressorVotedSpeed_t;

#endif
