/*!
 * @file
 * @brief definition files for compressor speed and speed tables.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORSPEEDS_H
#define COMPRESSORSPEEDS_H

#include <stdbool.h>
#include "CoolingModeDependentCompressorSpeeds.h"
#include "CoolingModeIndependentCompressorSpeeds.h"
#include "CompressorSpeedFrequencyInHz.h"

typedef struct
{
   bool coolingModeDependent;
   CompressorSpeedFrequencyInHz_t startupSpeedFrequencyInHz;
   CompressorSpeedFrequencyInHz_t superLowSpeedFrequencyInHz;
   CompressorSpeedFrequencyInHz_t superHighSpeedFrequencyInHz;
   CompressorSpeedFrequencyInHz_t delayedConvertibleCompartmentSpeedFrequencyInHz;

   union
   {
      CoolingModeDependentCompressorSpeeds_t coolingModeDependentCompressorSpeeds;
      CoolingModeIndependentCompressorSpeeds_t coolingModeIndependentCompressorSpeeds;
   };
} CompressorSpeeds_t;

#endif
