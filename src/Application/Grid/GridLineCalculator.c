/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridLineCalculator.h"
#include "CalculatedGridLines.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
#include "Setpoint.h"
#include "uassert.h"

#define GRID_LINE_TEMP(_dimension, _line) \
   instance->_private.gridData->deltaGridLines->gridLines[_dimension].gridLineData[_line].gridLinesDegFx100

#define PARAMETRIC_GRID_LINE_CORRECTION(_dimension, _line) \
   instance->_private.gridData->deltaGridLines->gridLines[_dimension].gridLineData[_line].correction

#define GRID_LINE_ADJUSTMENTS(_dimension) \
   instance->_private.config->gridLineAdjustmentErds[_dimension]

#define CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_MULTIPLIER(_dimension, _line) \
   instance->_private.gridData->deltaGridLines->gridLines[_dimension].gridLineData[_line].crossAmbientAdjustment->multiplier

#define CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_DIVIDER(_dimension, _line) \
   instance->_private.gridData->deltaGridLines->gridLines[_dimension].gridLineData[_line].crossAmbientAdjustment->divider

static bool AdjustedSetpointPluginIsReady(GridLineCalculator_t *instance)
{
   bool adjustedSetpointPluginIsReady;
   DataModel_Read(instance->_private.dataModel, Erd_AdjustedSetpointPluginReady, &adjustedSetpointPluginIsReady);

   return adjustedSetpointPluginIsReady;
}

static void CalculateAxisGridLines(
   GridLineCalculator_t *instance,
   CalculatedAxisGridLines_t *axisToCalculate,
   uint8_t axisDimension)
{
   for(uint8_t line = 0; line < NumberOfGridLinesPerAxis; line++)
   {
      axisToCalculate->gridLinesDegFx100[line] =
         GRID_LINE_TEMP(axisDimension, line);

      if(PARAMETRIC_GRID_LINE_CORRECTION(axisDimension, line) == DeltaGridLinesCorrection_AdjustedSetpoint)
      {
         SetpointVotedTemperature_t adjustedSetPoint;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(axisDimension).adjustedSetpointInDegFx100Erd, &adjustedSetPoint);
         axisToCalculate->gridLinesDegFx100[line] += adjustedSetPoint.temperatureInDegFx100;
      }
      else if(PARAMETRIC_GRID_LINE_CORRECTION(axisDimension, line) == DeltaGridLinesCorrection_Offset)
      {
         TemperatureDegFx100_t offset;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(axisDimension).offsetInDegFx100Erd, &offset);
         axisToCalculate->gridLinesDegFx100[line] += offset;
      }

      TemperatureDegFx100_t crossAmbientHysteresisAdjustment;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->crossAmbientHysteresisAdjustmentErd,
         &crossAmbientHysteresisAdjustment);

      crossAmbientHysteresisAdjustment =
         ((int32_t)crossAmbientHysteresisAdjustment *
         CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_MULTIPLIER(axisDimension, line)) /
         CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_DIVIDER(axisDimension, line);

      axisToCalculate->gridLinesDegFx100[line] += crossAmbientHysteresisAdjustment;
   }
}
static void ConfigureGridLines(
   GridLineCalculator_t *instance)
{
   CalculateAxisGridLines(instance, &instance->_private.calculatedGridLineOutput.freshFoodGridLine, GridDelta_FreshFood);
   CalculateAxisGridLines(instance, &instance->_private.calculatedGridLineOutput.freezerGridLine, GridDelta_Freezer);

   DataModel_Write(instance->_private.dataModel, instance->_private.config->calculatedGridLineErd, &instance->_private.calculatedGridLineOutput);
};

static void OnDataModelChanged(void *context, const void *args)
{
   GridLineCalculator_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   if((erd == instance->_private.config->gridLineAdjustmentErds[GridDelta_FreshFood].offsetInDegFx100Erd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[GridDelta_FreshFood].adjustedSetpointInDegFx100Erd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[GridDelta_Freezer].offsetInDegFx100Erd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[GridDelta_Freezer].adjustedSetpointInDegFx100Erd) ||
      (erd == instance->_private.config->crossAmbientHysteresisAdjustmentErd))
   {
      ConfigureGridLines(instance);
   }
}

void GridLineCalculator_Init(GridLineCalculator_t *instance,
   const GridLineCalculatorConfiguration_t *config,
   I_DataModel_t *dataModel)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.gridData = PersonalityParametricData_Get(dataModel)->gridData;
   memset(&instance->_private.calculatedGridLineOutput, 0, sizeof(CalculatedGridLines_t));

   uassert(AdjustedSetpointPluginIsReady(instance));

   ConfigureGridLines(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);

   DataModel_Write(dataModel, Erd_GridLineCalculatorIsReady, set);
}
