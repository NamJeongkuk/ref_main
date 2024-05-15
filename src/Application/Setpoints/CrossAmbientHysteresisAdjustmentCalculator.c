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
   instance->_private.deltaGridLines->gridLines[_dimension].gridLineData[_line].gridLinesDegFx100

static TemperatureDegFx100_t LowestHysteresis(CrossAmbientHysteresisAdjustmentCalculator_t *instance)
{
   return MIN(abs(GRID_LINE_TEMP(GridDelta_FreshFood, GridLine_FreshFoodLowHyst)), abs(GRID_LINE_TEMP(GridDelta_FreshFood, GridLine_FreshFoodHighHyst)));
}

static TemperatureDegFx100_t UncappedHysteresisAdjustment(CrossAmbientHysteresisAdjustmentCalculator_t *instance)
{
   TemperatureDegFx100_t ambientWindowedTemperatureInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientWindowAveragedTemperatureInDegFx100Erd,
      &ambientWindowedTemperatureInDegFx100);

   return (instance->_private.freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF * (AdjustmentThresholdInDegFx100 - ambientWindowedTemperatureInDegFx100)) / DegFx1000;
}

static void CalculateCrossAmbientHysteresisAdjustment(CrossAmbientHysteresisAdjustmentCalculator_t *instance)
{
   TemperatureDegFx100_t ambientHysteresisAdjustment = 0;

   TemperatureDegFx100_t ambientWindowedTemperatureInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientWindowAveragedTemperatureInDegFx100Erd,
      &ambientWindowedTemperatureInDegFx100);

   TemperatureDegFx100_t lowestHysteresisInDegFx100 = LowestHysteresis(instance);

   if((lowestHysteresisInDegFx100 >= instance->_private.freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100) &&
      (ambientWindowedTemperatureInDegFx100 < AdjustmentThresholdInDegFx100))
   {
      ambientHysteresisAdjustment = lowestHysteresisInDegFx100 - instance->_private.freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;

      TemperatureDegFx100_t uncappedHysteresisAdjustment = UncappedHysteresisAdjustment(instance);
      if(uncappedHysteresisAdjustment <= ambientHysteresisAdjustment)
      {
         ambientHysteresisAdjustment = uncappedHysteresisAdjustment;
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
   const TemperatureDegFx100_t freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100,
   const uint16_t freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF,
   const DeltaGridLines_t *deltaGridLines,
   const CrossAmbientHysteresisAdjustmentCalculatorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100 = freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
   instance->_private.freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF = freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF;
   instance->_private.deltaGridLines = deltaGridLines;

   CalculateCrossAmbientHysteresisAdjustment(instance);

   EventSubscription_Init(
      &instance->_private.onCrossAmbientWindowedAverageTemperatureSubscription,
      instance,
      OnCrossAmbientWindowedAverageTemperatureChange);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->ambientWindowAveragedTemperatureInDegFx100Erd,
      &instance->_private.onCrossAmbientWindowedAverageTemperatureSubscription);
}
