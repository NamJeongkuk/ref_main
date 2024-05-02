/*!
 * @file
 * @brief Calculates the gamma parameter used in ice cabinet temperature
 * control based on the freezer and ice cabinet temperatures relative to
 * their hysteresis ranges
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETGAMMACALCULATOR_H
#define ICECABINETGAMMACALCULATOR_H

#include "DeltaGridLines.h"
#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t freezerFilteredTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t iceCabinetTemperatureResolvedInDegFx100Erd; // TemperatureDegFx100_t
   Erd_t freezerAdjustedSetpointErd; // TemperatureDegFx100_t
   Erd_t iceCabinetAdjustedSetpointErd; // TemperatureDegFx100_t
   Erd_t gammaErd; // int8_t
} IceCabinetGammaCalculatorConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;
      const IceCabinetGammaCalculatorConfig_t *config;
      const DeltaAxisGridLines_t *freezerGridLineData;
      const DeltaAxisGridLines_t *iceCabinetGridLineData;
   } _private;
} IceCabinetGammaCalculator_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param freezerGridLineData
 * @param iceCabinetGridLineData
 */
void IceCabinetGammaCalculator_Init(
   IceCabinetGammaCalculator_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetGammaCalculatorConfig_t *config,
   const DeltaAxisGridLines_t *freezerGridLineData,
   const DeltaAxisGridLines_t *iceCabinetGridLineData);

#endif
