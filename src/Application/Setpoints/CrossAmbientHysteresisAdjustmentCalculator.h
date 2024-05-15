/*!
 * @file
 * @brief Cross ambient hysteresis calculator that takes in a current windowed averaged temperature and calculates an adjustment value.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CROSSAMBIENTHYSTERESISADJUSTMENTCALCULATOR_H
#define CROSSAMBIENTHYSTERESISADJUSTMENTCALCULATOR_H

#include "I_DataModel.h"
#include "GridData.h"
#include "DeltaGridLines.h"

typedef struct
{
   Erd_t crossAmbientHysteresisAdjustmentErd; // TemperatureDegfx100
   Erd_t ambientWindowAveragedTemperatureInDegFx100Erd; // TemperatureDegfx100
} CrossAmbientHysteresisAdjustmentCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const CrossAmbientHysteresisAdjustmentCalculatorConfig_t *config;
      TemperatureDegFx100_t freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
      uint16_t freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF;
      const DeltaGridLines_t *deltaGridLines;
      EventSubscription_t onCrossAmbientWindowedAverageTemperatureSubscription;
      TemperatureDegFx100_t lowestHysteresis;
   } _private;
} CrossAmbientHysteresisAdjustmentCalculator_t;

void CrossAmbientHysteresisAdjustmentCalculator_Init(
   CrossAmbientHysteresisAdjustmentCalculator_t *instance,
   I_DataModel_t *dataModel,
   const TemperatureDegFx100_t freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100,
   const uint16_t freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF,
   const DeltaGridLines_t *deltaGridLines,
   const CrossAmbientHysteresisAdjustmentCalculatorConfig_t *config);

#endif
