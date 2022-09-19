/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COOLINGMODEINDEPENDENTCOMPRESSORSPEEDS_H
#define COOLINGMODEINDEPENDENTCOMPRESSORSPEEDS_H

#include "CompressorSpeedFrequencyInHz.h"

typedef struct
{
   CompressorSpeedFrequencyInHz_t lowSpeedFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedFrequencyInHz;
   CompressorSpeedFrequencyInHz_t highSpeedFrequencyInHz;
} CoolingModeIndependentCompressorSpeeds_t;

#endif
