/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HARVESTCOUNTCALCULATOR_H
#define HARVESTCOUNTCALCULATOR_H

#include "AluminumMoldIceMakerData.h"
#include "I_DataModel.h"
#include "Timer.h"

typedef struct
{
   Erd_t harvestCountIsReadyToHarvestErd; // bool
   Erd_t harvestCountCalculationRequestErd; // bool
   Erd_t iceMakerFilteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
} HarvestCountCalculatorConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const AluminumMoldIceMakerData_t *aluminumMoldIceMakerData;
      const HarvestCountCalculatorConfiguration_t *config;
      Timer_t minimumFreezeTimer;
      Timer_t harvestCountTimer;
      EventSubscription_t harvestCountCalculationRequestSubscription;
      uint32_t integrationTemperatureCount;
      bool minimumFreezeTimerIsExpired;
   } _private;
} HarvestCountCalculator_t;

void HarvestCountCalculator_Init(
   HarvestCountCalculator_t *instance,
   I_DataModel_t *dataModel,
   const HarvestCountCalculatorConfiguration_t *config,
   const AluminumMoldIceMakerData_t *aluminumMoldIceMakerData);

#endif
