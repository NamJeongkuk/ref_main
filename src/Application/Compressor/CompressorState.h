/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSTATE_H
#define COMPRESSORSTATE_H

#include <stdint.h>

enum
{
   CompressorState_Off,
   CompressorState_On,
   CompressorState_MinimumOnTime,
   CompressorState_MinimumOffTime,
   CompressorState_MinimumRunTime,
   CompressorState_Max,
};
typedef uint8_t CompressorState_t;

#endif
