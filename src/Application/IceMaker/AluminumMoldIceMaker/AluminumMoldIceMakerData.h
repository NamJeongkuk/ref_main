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
   TemperatureDegFx100_t maximumHarvestTemperatureInDegFx100;
   TemperatureDegFx100_t startIntegrationTemperatureInDegFx100;
   uint32_t freezeIntegrationLimitInDegFx100TimesSeconds;
   uint8_t minimumFreezeTimeInMinutes;
   uint8_t minimumFeelerArmExtensionTimeInMinutes;
   uint8_t freezeThawHarvestFillTubeHeaterDutyCycle;
   uint16_t freezeThawHarvestFillTubeHeaterOnTimeInSeconds;
   uint8_t maximumHarvestTimeInMinutes;
   TemperatureDegFx100_t initialMotorOnTemperatureInDegFx100;
   uint8_t initialMinimumHeaterOnTimeInSeconds;
   TemperatureDegFx100_t heaterOffTemperatureInDegFx100;
   TemperatureDegFx100_t heaterOnTemperatureInDegFx100;
   uint8_t rakeNotHomeTestTimeInSeconds;
   uint8_t feelerArmTestTimeInSeconds;
} AluminumMoldIceMakerData_t;

#endif
