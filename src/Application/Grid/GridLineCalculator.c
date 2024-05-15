/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GridLineCalculator.h"
#include "CalculatedGridLines.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
#include "Setpoint.h"
#include "uassert.h"

enum
{
   FirstDimension,
   SecondDimension,

   OneDimension = 1,
   TwoDimensions
};

#define GRID_LINE_TEMP(_dimension, _line) \
   instance->_private.deltaGridLines->gridLines[_dimension].gridLineData[_line].gridLinesDegFx100

#define PARAMETRIC_GRID_LINE_CORRECTION(_dimension, _line) \
   instance->_private.deltaGridLines->gridLines[_dimension].gridLineData[_line].correction

#define GRID_LINE_ADJUSTMENTS(_dimension) \
   instance->_private.config->gridLineAdjustmentErds[_dimension]

#define CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_MULTIPLIER(_dimension, _line) \
   instance->_private.deltaGridLines->gridLines[_dimension].gridLineData[_line].crossAmbientAdjustment->multiplier

#define CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_DIVIDER(_dimension, _line) \
   instance->_private.deltaGridLines->gridLines[_dimension].gridLineData[_line].crossAmbientAdjustment->divider

static bool AdjustedSetpointPluginIsReady(GridLineCalculator_t *instance)
{
   bool adjustedSetpointPluginIsReady;
   DataModel_Read(instance->_private.dataModel, Erd_AdjustedSetpointPluginReady, &adjustedSetpointPluginIsReady);

   return adjustedSetpointPluginIsReady;
}

static void CalculateAxisGridLines(
   GridLineCalculator_t *instance,
   CalculatedAxisGridLines_t *axisToCalculate,
   uint8_t dimension)
{
   uint8_t gridDeltaOffset = 0;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->gridDeltaOffsetErd,
      &gridDeltaOffset);

   // Have to do this because "Mode 1" starts at index 0 in parametric. Since "Mode 1" is enumerated as "1" in application,
   // we need to subtract 1 and truncate (in case it changes to "Off," or 0) so that we access the correct index.
   gridDeltaOffset = TRUNCATE_UNSIGNED_SUBTRACTION(gridDeltaOffset, 1);

   for(uint8_t line = 0; line < instance->_private.deltaGridLines->gridLines->numberOfLines; line++)
   {
      axisToCalculate->gridLinesDegFx100[line] =
         GRID_LINE_TEMP(dimension, ((CalculatedGridLines_MaxGridLinesCount * gridDeltaOffset) + line));

      if(DeltaGridLinesCorrection_AdjustedSetpoint == PARAMETRIC_GRID_LINE_CORRECTION(dimension, ((CalculatedGridLines_MaxGridLinesCount * gridDeltaOffset) + line)))
      {
         SetpointVotedTemperature_t adjustedSetPoint;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(dimension).adjustedSetpointInDegFx100Erd, &adjustedSetPoint);
         axisToCalculate->gridLinesDegFx100[line] += adjustedSetPoint.temperatureInDegFx100;
      }
      else if(DeltaGridLinesCorrection_Offset == PARAMETRIC_GRID_LINE_CORRECTION(dimension, ((CalculatedGridLines_MaxGridLinesCount * gridDeltaOffset) + line)))
      {
         TemperatureDegFx100_t offset = 0;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(dimension).offsetInDegFx100Erd, &offset);
         axisToCalculate->gridLinesDegFx100[line] += offset;
      }

      TemperatureDegFx100_t crossAmbientHysteresisAdjustment = 0;
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->crossAmbientHysteresisAdjustmentErd,
         &crossAmbientHysteresisAdjustment);

      crossAmbientHysteresisAdjustment =
         ((int32_t)crossAmbientHysteresisAdjustment *
            CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_MULTIPLIER(dimension, ((CalculatedGridLines_MaxGridLinesCount * gridDeltaOffset) + line))) /
         CROSS_AMBIENT_HYSTERESIS_ADJUSTMENT_DIVIDER(dimension, ((CalculatedGridLines_MaxGridLinesCount * gridDeltaOffset) + line));

      axisToCalculate->gridLinesDegFx100[line] += crossAmbientHysteresisAdjustment;
   }
}

static void ConfigureGridLines(GridLineCalculator_t *instance)
{
   if(OneDimension == instance->_private.deltaGridLines->dimensions)
   {
      CalculateAxisGridLines(
         instance,
         &instance->_private.oneDimensionalCalculatedGridLines.gridLines,
         FirstDimension);
      instance->_private.oneDimensionalCalculatedGridLines.numberOfGridLines = instance->_private.deltaGridLines->gridLines[FirstDimension].numberOfLines;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->calculatedGridLineErd,
         &instance->_private.oneDimensionalCalculatedGridLines);
   }
   else
   {
      CalculateAxisGridLines(
         instance,
         &instance->_private.twoDimensionalCalculatedGridLines.firstDimensionGridLines,
         FirstDimension);
      CalculateAxisGridLines(
         instance,
         &instance->_private.twoDimensionalCalculatedGridLines.secondDimensionGridLines,
         SecondDimension);
      instance->_private.twoDimensionalCalculatedGridLines.numberOfGridLinesPerDimension = instance->_private.deltaGridLines->gridLines[FirstDimension].numberOfLines;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->calculatedGridLineErd,
         &instance->_private.twoDimensionalCalculatedGridLines);
   }
};

static void OnDataModelChanged(void *context, const void *args)
{
   GridLineCalculator_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   if(erd == instance->_private.config->crossAmbientHysteresisAdjustmentErd ||
      erd == instance->_private.config->gridDeltaOffsetErd)
   {
      ConfigureGridLines(instance);
   }
   else
   {
      for(uint8_t i = 0; i < instance->_private.deltaGridLines->dimensions; i++)
      {
         if((erd == instance->_private.config->gridLineAdjustmentErds[i].offsetInDegFx100Erd) ||
            (erd == instance->_private.config->gridLineAdjustmentErds[i].adjustedSetpointInDegFx100Erd))
         {
            ConfigureGridLines(instance);
            break;
         }
      }
   }
}

void GridLineCalculator_Init(
   GridLineCalculator_t *instance,
   const GridLineCalculatorConfiguration_t *config,
   const DeltaGridLines_t *deltaGridLines,
   I_DataModel_t *dataModel)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.deltaGridLines = deltaGridLines;

   uassert(IN_RANGE(OneDimension, instance->_private.deltaGridLines->dimensions, TwoDimensions));

   if(OneDimension == instance->_private.deltaGridLines->dimensions)
   {
      memset(
         &instance->_private.oneDimensionalCalculatedGridLines,
         0,
         OneDimension * instance->_private.deltaGridLines->gridLines->numberOfLines * sizeof(TemperatureDegFx100_t));
   }
   else
   {
      memset(
         &instance->_private.twoDimensionalCalculatedGridLines,
         0,
         TwoDimensions * instance->_private.deltaGridLines->gridLines->numberOfLines * sizeof(TemperatureDegFx100_t));
   }

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
