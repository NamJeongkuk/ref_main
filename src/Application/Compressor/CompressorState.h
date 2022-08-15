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
   CompressorState_OffAndReadyToChange,
   CompressorState_OnAndReadyToChange,
   CompressorState_MinimumOnTime,
   CompressorState_MinimumOffTime,
   CompressorState_VariableSpeedMinimumRunTime,
   CompressorState_RemainOffAfterValveMove,
   CompressorState_StartupOnTime,
   CompressorState_Max
};
typedef uint8_t CompressorState_t;

#endif
