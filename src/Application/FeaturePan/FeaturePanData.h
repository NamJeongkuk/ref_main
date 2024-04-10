/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANDATA_H
#define FEATUREPANDATA_H

#include "FanSpeed.h"
#include "TemperatureDegFx100.h"

typedef struct
{
   int16_t featurePanWarmupSlopeLowerLimitInDegFx100PerMinute;
   int16_t featurePanWarmupSlopeUpperLimitInDegFx100PerMinute;
   TemperatureDegFx100_t featurePanHeaterHighAmbientTemperatureLimit;
   FanSpeed_t freshFoodFanDeliCoolingSpeed;
} FeaturePanData_t;

#endif
