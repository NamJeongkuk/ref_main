/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COOLINGMODEDEPENDENTCOMPRESSORSPEEDS_H
#define COOLINGMODEDEPENDENTCOMPRESSORSPEEDS_H

#include <stdint.h>

typedef struct
{
   uint8_t lowSpeedFreshFoodWarmFrequencyInHz;
   uint8_t lowSpeedFreshFoodMidFrequencyInHz;
   uint8_t lowSpeedFreshFoodColdFrequencyInHz;
   uint8_t lowSpeedFreezerWarmFrequencyInHz;
   uint8_t lowSpeedFreezerMidFrequencyInHz;
   uint8_t lowSpeedFreezerColdFrequencyInHz;
   uint8_t lowSpeedConvertibleCompartmentWarmFrequencyInHz;
   uint8_t lowSpeedConvertibleCompartmentMidFrequencyInHz;
   uint8_t lowSpeedConvertibleCompartmentColdFrequencyInHz;
   uint8_t mediumSpeedFreshFoodWarmFrequencyInHz;
   uint8_t mediumSpeedFreshFoodMidFrequencyInHz;
   uint8_t mediumSpeedFreshFoodColdFrequencyInHz;
   uint8_t mediumSpeedFreezerWarmFrequencyInHz;
   uint8_t mediumSpeedFreezerMidFrequencyInHz;
   uint8_t mediumSpeedFreezerColdFrequencyInHz;
   uint8_t mediumSpeedConvertibleCompartmentWarmFrequencyInHz;
   uint8_t mediumSpeedConvertibleCompartmentMidFrequencyInHz;
   uint8_t mediumSpeedConvertibleCompartmentColdFrequencyInHz;
   uint8_t highSpeedFreshFoodFrequencyInHz;
   uint8_t highSpeedFreezerFrequencyInHz;
   uint8_t highSpeedConvertibleCompartmentFrequencyIn_Hz;
} CoolingModeDependentCompressorSpeeds_t;

#endif
