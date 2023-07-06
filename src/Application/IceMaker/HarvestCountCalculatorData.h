/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HARVESTCOUNTCALCULATORDATA_H
#define HARVESTCOUNTCALCULATORDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t startIntegrationTemperatureInDegFx100;
   TemperatureDegFx100_t minimumFreezeTimeInitiationTemperatureInDegFx100;
   uint32_t targetIntegrationLimitInDegFx100TimesSeconds;
   uint8_t minimumFreezeTimeInMinutes;
} HarvestCountCalculatorData_t;

#endif
