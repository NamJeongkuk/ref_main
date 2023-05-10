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

typedef struct
{
   Erd_t crossAmbientHysteresisAdjustmentErd; // TemperatureDegfx100
   Erd_t crossAmbientWindowAveragedTemperatureErd; // TemperatureDegfx100
} CrossAmbientHysteresisAdjustmentCalculatorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const CrossAmbientHysteresisAdjustmentCalculatorConfig_t *config;
      const GridData_t *gridData;
      EventSubscription_t onCrossAmbientWindowedAverageTemperatureSubscription;
      TemperatureDegFx100_t lowestHysteresis;
   } _private;
} CrossAmbientHysteresisAdjustmentCalculator_t;

void CrossAmbientHysteresisAdjustmentCalculator_Init(
   CrossAmbientHysteresisAdjustmentCalculator_t *instance,
   I_DataModel_t *dataModel,
   const GridData_t *gridData,
   const CrossAmbientHysteresisAdjustmentCalculatorConfig_t *config);

#endif
