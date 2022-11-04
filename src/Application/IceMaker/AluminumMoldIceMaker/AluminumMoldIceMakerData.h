/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERDATA_H
#define ALUMINUMMOLDICEMAKERDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   TemperatureDegFx100_t maxHarvestTemperatureInDegFx100;
   TemperatureDegFx100_t startIntegrationTemperatureInDegFx100;
   uint32_t freezeIntegrationLimitInDegFx100TimesSeconds;
   uint8_t minimumFreezeTimeInMinutes;
   uint8_t minimumFeelerArmExtensionTimeInMinutes;
} AluminumMoldIceMakerData_t;

#endif
