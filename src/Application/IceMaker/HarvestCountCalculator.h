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
   Erd_t moldFilteredTemperatureInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t moldFreezeIntegrationCountErd; // uint32_t
   Erd_t moldIceMakerMinimumFreezeTimeCounterInMinutesErd; // uint8_t
} HarvestCountCalculatorConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const HarvestCountCalculatorConfiguration_t *config;
      Timer_t minimumFreezeTimer;
      Timer_t harvestCountTimer;
      EventSubscription_t harvestCountCalculationRequestSubscription;
      bool minimumFreezeTimeIsSatisfied;
      const HarvestCountCalculatorData_t *harvestCountCalculatorData;
   } _private;
} HarvestCountCalculator_t;

void HarvestCountCalculator_Init(
   HarvestCountCalculator_t *instance,
   I_DataModel_t *dataModel,
   const HarvestCountCalculatorConfiguration_t *config,
   const HarvestCountCalculatorData_t *harvestCountCalculatorData);

#endif
