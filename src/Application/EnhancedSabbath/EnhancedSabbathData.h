/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ENHANCEDSABBATHDATA_H
#define ENHANCEDSABBATHDATA_H

#include <stdint.h>
#include "TemperatureDegFx100.h"

typedef struct
{
   uint16_t maxTimeInEnhancedSabbathModeInMinutes;
   TemperatureDegFx100_t freshFoodSetpointTemperatureInDegFx100;
   TemperatureDegFx100_t freezerSetpointTemperatureInDegFx100;
   uint8_t numberOfSecondaryOnlyDefrostsBeforeFullDefrost;
   uint8_t minTimeBetweenTemperatureAveragingInMinutes;
   uint8_t freshFoodStageTimeInMinutes;
   uint8_t freezerStageTimeInMinutes;
   uint8_t offStageTimeInMinutes;
   TemperatureDegFx100_t freshFoodLowSpeedHysteresisInDegFx100;
   TemperatureDegFx100_t freshFoodMediumSpeedHysteresisInDegFx100;
   TemperatureDegFx100_t freezerLowSpeedHysteresisInDegFx100;
   TemperatureDegFx100_t freezerMediumSpeedHysteresisInDegFx100;
   uint8_t lightsPwmDutyCyclePercentage;
} EnhancedSabbathData_t;

#endif
