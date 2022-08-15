/*!
 * @file
 * @brief definition files for compressor speed and speed tables.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSPEEDS_H
#define COMPRESSORSPEEDS_H

#include <stdint.h>
#include "CoolingModeDependentCompressorSpeeds.h"
#include "CoolingModeIndependentCompressorSpeeds.h"

typedef uint16_t CompressorRpm_t;

typedef struct
{
   bool coolingModeDependent;
   uint8_t startupSpeedFrequencyInHz;
   uint8_t superLowSpeedFrequencyInHz;
   uint8_t superHighSpeedFrequencyInHz;

   union
   {
      CoolingModeDependentCompressorSpeeds_t coolingModeDependentCompressorSpeeds;
      CoolingModeIndependentCompressorSpeeds_t coolingModeIndependentCompressorSpeeds;
   };
} CompressorSpeeds_t;

#endif
