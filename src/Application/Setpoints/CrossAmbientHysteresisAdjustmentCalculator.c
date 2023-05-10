/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdlib.h>
#include "CrossAmbientHysteresisAdjustmentCalculator.h"
#include "utils.h"

enum
{
   AdjustmentThresholdInDegFx100 = 9000,
   DegFx1000 = 1000,
};

#define GRID_LINE_TEMP(_dimension, _line) \
   instance->_private.gridData->deltaGridLines->gridLines[_dimension].gridLineData[_line].gridLinesDegFx100

static TemperatureDegFx100_t LowestHysteresis(CrossAmbientHysteresisAdjustmentCalculator_t *instance)
{
   return MIN(abs(GRID_LINE_TEMP(GridDelta_FreshFood, GridLine_FreshFoodLowHyst)), abs(GRID_LINE_TEMP(GridDelta_FreshFood, GridLine_FreshFoodHighHyst)));
}

static TemperatureDegFx100_t UncappedHysteresisAdjustment(CrossAmbientHysteresisAdjustmentCalculator_t *instance)
{
   TemperatureDegFx100_t crossAmbientWindowedTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->crossAmbientWindowAveragedTemperatureErd,
      &crossAmbientWindowedTemperature);

   return (instance->_private.gridData->freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF * (AdjustmentThresholdInDegFx100 - crossAmbientWindowedTemperature)) / DegFx1000;
}

static void CalculateCrossAmbientHysteresisAdjustment(CrossAmbientHysteresisAdjustmentCalculator_t *instance)
{
   TemperatureDegFx100_t ambientHysteresisAdjustment = 0;

   TemperatureDegFx100_t crossAmbientHysteresisAdjustment;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->crossAmbientHysteresisAdjustmentErd,
      &crossAmbientHysteresisAdjustment);

   if((LowestHysteresis(instance) >= instance->_private.gridData->freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100) &&
      (crossAmbientHysteresisAdjustment < AdjustmentThresholdInDegFx100))
   {
      ambientHysteresisAdjustment = LowestHysteresis(instance) - instance->_private.gridData->freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;

      if(UncappedHysteresisAdjustment(instance) <= ambientHysteresisAdjustment)
      {
         ambientHysteresisAdjustment = UncappedHysteresisAdjustment(instance);
      }
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->crossAmbientHysteresisAdjustmentErd,
      &ambientHysteresisAdjustment);
}

static void OnCrossAmbientWindowedAverageTemperatureChange(void *context, const void *_args)
{
   CrossAmbientHysteresisAdjustmentCalculator_t *instance = context;
   IGNORE(_args);

   CalculateCrossAmbientHysteresisAdjustment(instance);
}

void CrossAmbientHysteresisAdjustmentCalculator_Init(
   CrossAmbientHysteresisAdjustmentCalculator_t *instance,
   I_DataModel_t *dataModel,
   const GridData_t *gridData,
   const CrossAmbientHysteresisAdjustmentCalculatorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.gridData = gridData;

   CalculateCrossAmbientHysteresisAdjustment(instance);

   EventSubscription_Init(
      &instance->_private.onCrossAmbientWindowedAverageTemperatureSubscription,
      instance,
      OnCrossAmbientWindowedAverageTemperatureChange);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->crossAmbientWindowAveragedTemperatureErd,
      &instance->_private.onCrossAmbientWindowedAverageTemperatureSubscription);
}
