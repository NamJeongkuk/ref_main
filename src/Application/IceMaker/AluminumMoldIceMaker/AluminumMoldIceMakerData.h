/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERDATA_H
#define ALUMINUMMOLDICEMAKERDATA_H

#include "TemperatureDegFx100.h"
#include "FillTubeHeaterData.h"
#include "IceMakerTypeInformation.h"

typedef struct
{
   TemperatureDegFx100_t maximumHarvestTemperatureInDegFx100;
   TemperatureDegFx100_t startIntegrationTemperatureInDegFx100;
   uint32_t freezeIntegrationLimitInDegFx100TimesSeconds;
   uint8_t minimumFreezeTimeInMinutes;
   uint8_t minimumFeelerArmExtensionTimeInMinutes;
} AluminumMoldIceMakerFreezeData_t;

typedef struct
{
   uint8_t maximumHarvestTimeInMinutes;
   TemperatureDegFx100_t initialMotorOnTemperatureInDegFx100;
   uint8_t initialMinimumHeaterOnTimeInSeconds;
   TemperatureDegFx100_t heaterOffTemperatureInDegFx100;
   TemperatureDegFx100_t heaterOnTemperatureInDegFx100;
   uint8_t rakeNotHomeTestTimeInSeconds;
   uint8_t feelerArmTestTimeInSeconds;
} AluminumMoldIceMakerHarvestData_t;

typedef struct
{
   uint8_t rakeMotorControlTimeInSeconds;
   uint16_t harvestFaultMaxTimeInMinutes;
} AluminumMoldIceMakerHarvestFaultData_t;

typedef struct
{
   TemperatureDegFx100_t heaterOffTemperatureInDegFx100;
   TemperatureDegFx100_t heaterOnTemperatureInDegFx100;
   uint8_t motorOffTimeInSeconds;
   uint8_t motorOnTimeInSeconds;
   uint8_t maximumHarvestFixTimeInMinutes;
} AluminumMoldIceMakerHarvestFixData_t;

typedef struct
{
   IceMakerTypeInformation_t typeInformation;
   AluminumMoldIceMakerFreezeData_t freezeData;
   AluminumMoldIceMakerHarvestData_t harvestData;
   AluminumMoldIceMakerHarvestFaultData_t harvestFaultData;
   AluminumMoldIceMakerHarvestFixData_t harvestFixData;
   FillTubeHeaterData_t fillTubeHeaterData;
} AluminumMoldIceMakerData_t;

#endif
