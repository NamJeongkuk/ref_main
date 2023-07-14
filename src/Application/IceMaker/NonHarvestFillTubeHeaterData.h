/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NONHARVESTFILLTUBEHEATERDATA_H
#define NONHARVESTFILLTUBEHEATERDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t maximumCabinetTemperatureInDegFx100;
   uint8_t formationDutyCyclePercentage;
   uint8_t maintenanceDutyCyclePercentage;
   uint8_t offDutyCyclePercentage;
} NonHarvestFillTubeHeaterData_t;

#endif
