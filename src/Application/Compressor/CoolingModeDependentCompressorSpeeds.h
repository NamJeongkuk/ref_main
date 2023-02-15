/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COOLINGMODEDEPENDENTCOMPRESSORSPEEDS_H
#define COOLINGMODEDEPENDENTCOMPRESSORSPEEDS_H

#include "CompressorSpeedFrequencyInHz.h"

typedef struct
{
   CompressorSpeedFrequencyInHz_t lowSpeedFreshFoodWarmFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedFreshFoodMidFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedFreshFoodColdFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedFreezerWarmFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedFreezerMidFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedFreezerColdFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedConvertibleCompartmentWarmFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedConvertibleCompartmentMidFrequencyInHz;
   CompressorSpeedFrequencyInHz_t lowSpeedConvertibleCompartmentColdFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedFreshFoodWarmFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedFreshFoodMidFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedFreshFoodColdFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedFreezerWarmFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedFreezerMidFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedFreezerColdFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedConvertibleCompartmentWarmFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedConvertibleCompartmentMidFrequencyInHz;
   CompressorSpeedFrequencyInHz_t mediumSpeedConvertibleCompartmentColdFrequencyInHz;
   CompressorSpeedFrequencyInHz_t highSpeedFreshFoodFrequencyInHz;
   CompressorSpeedFrequencyInHz_t highSpeedFreezerFrequencyInHz;
   CompressorSpeedFrequencyInHz_t highSpeedConvertibleCompartmentFrequencyInHz;
} CoolingModeDependentCompressorSpeeds_t;

#endif
