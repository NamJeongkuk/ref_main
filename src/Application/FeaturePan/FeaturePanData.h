/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANDATA_H
#define FEATUREPANDATA_H

#include "FanSpeed.h"
#include "FeaturePanMode.h"
#include "TemperatureDegFx100.h"

typedef struct
{
   int16_t featurePanWarmupSlopeLowerLimitInDegFx100PerMinute;
   int16_t featurePanWarmupSlopeUpperLimitInDegFx100PerMinute;
   TemperatureDegFx100_t featurePanHeaterHighAmbientTemperatureLimitInDegFx100;
   TemperatureDegFx100_t featurePanPulldownTemperatureLimitInDegFx100;
   FanSpeed_t freshFoodFanDeliCoolingSpeed;
   FeaturePanCurrentMode_t featurePanDefaultMode;
   FeaturePanCurrentMode_t featurePanMinAllowedMode;
   FeaturePanCurrentMode_t featurePanMaxAllowedMode;
} FeaturePanData_t;

#endif
