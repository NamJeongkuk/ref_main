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

#define GRID_LINE_TEMP(_dimension, _line) \
   instance->_private.gridData->deltaGridLines->gridLines[_dimension].gridLineData[_line].gridLinesDegFx100

#define PARAMETRIC_GRID_LINE_BIT_MASK(_dimension, _line) \
   instance->_private.gridData->deltaGridLines->gridLines[_dimension].gridLineData[_line].bitMapping

#define GRID_LINE_ADJUSTMENTS(_dimension) \
   instance->_private.config->gridLineAdjustmentErds[_dimension]

static void CalculateAxisGridLines(
   GridLineCalculator_t *instance,
   CalculatedAxisGridLines_t *axisToCalculate,
   uint8_t axisDimension)
{
   for(uint8_t line = 0; line < NumberOfGridLinesPerAxis; line++)
   {
      axisToCalculate->gridLinesDegFx100[line] =
         GRID_LINE_TEMP(axisDimension, line);

      if(PARAMETRIC_GRID_LINE_BIT_MASK(axisDimension, line) & DeltaGridLines_BitMapping_SetpointBitMask)
      {
         SetpointVotedTemperature_t rawSetpoint;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(axisDimension).rawSetpointErd, &rawSetpoint);
         axisToCalculate->gridLinesDegFx100[line] += rawSetpoint.temperature;
      }
      if(PARAMETRIC_GRID_LINE_BIT_MASK(axisDimension, line) & DeltaGridLines_BitMapping_OffsetBitMask)
      {
         TemperatureDegFx100_t offset;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(axisDimension).offsetErd, &offset);
         axisToCalculate->gridLinesDegFx100[line] += offset;
      }
      if(PARAMETRIC_GRID_LINE_BIT_MASK(axisDimension, line) & DeltaGridLines_BitMapping_ShiftBitMask)
      {
         TemperatureDegFx100_t shift;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(axisDimension).shiftErd, &shift);
         axisToCalculate->gridLinesDegFx100[line] += shift;
      }
      if(PARAMETRIC_GRID_LINE_BIT_MASK(axisDimension, line) & DeltaGridLines_BitMapping_AdjSetpointBitMask)
      {
         SetpointVotedTemperature_t adjustedSetPoint;
         DataModel_Read(instance->_private.dataModel, GRID_LINE_ADJUSTMENTS(axisDimension).adjustedSetpointErd, &adjustedSetPoint);
         axisToCalculate->gridLinesDegFx100[line] += adjustedSetPoint.temperature;
      }
   }
}
static void ConfigureGridLines(
   GridLineCalculator_t *instance)
{
   CalculateAxisGridLines(instance, &instance->_private.freshFoodCalculatedAxis, FreshFoodGridLineDimension);
   CalculateAxisGridLines(instance, &instance->_private.freezerCalculatedAxis, FreezerGridLineDimension);

   instance->_private.calculatedGridLines[FreshFoodGridLineDimension].numberOfLines = NumberOfGridLinesPerAxis;
   instance->_private.calculatedGridLines[FreezerGridLineDimension].numberOfLines = NumberOfGridLinesPerAxis;

   instance->_private.calculatedGridLines[FreshFoodGridLineDimension].gridLinesDegFx100 =
      instance->_private.calculatedFreshFoodGridLines;

   instance->_private.calculatedGridLines[FreezerGridLineDimension].gridLinesDegFx100 =
      instance->_private.calculatedFreezerGridLines;

   instance->_private.calculatedGridLineOutput.gridLines =
      instance->_private.calculatedGridLines;

   instance->_private.calculatedGridLineOutput.dimensions = NumberGridDimensions;

   DataModel_Write(instance->_private.dataModel, instance->_private.config->calculatedGridLineErd, &instance->_private.calculatedGridLineOutput);
};

static void OnDataModelChanged(void *context, const void *args)
{
   GridLineCalculator_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeData = args;
   const Erd_t erd = onChangeData->erd;

   if((erd == instance->_private.config->gridLineAdjustmentErds[FreshFoodGridLineDimension].rawSetpointErd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[FreshFoodGridLineDimension].offsetErd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[FreshFoodGridLineDimension].adjustedSetpointErd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[FreezerGridLineDimension].shiftErd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[FreezerGridLineDimension].rawSetpointErd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[FreezerGridLineDimension].offsetErd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[FreezerGridLineDimension].adjustedSetpointErd) ||
      (erd == instance->_private.config->gridLineAdjustmentErds[FreezerGridLineDimension].shiftErd))
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

   instance->_private.freshFoodCalculatedAxis.gridLinesDegFx100 = instance->_private.calculatedFreshFoodGridLines;
   instance->_private.freshFoodCalculatedAxis.numberOfLines = NumberOfGridLinesPerAxis;

   instance->_private.freezerCalculatedAxis.gridLinesDegFx100 = instance->_private.calculatedFreezerGridLines;
   instance->_private.freezerCalculatedAxis.numberOfLines = NumberOfGridLinesPerAxis;

   ConfigureGridLines(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      OnDataModelChanged);

   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelSubscription);

   DataModel_Write(dataModel, Erd_GridLineCalculatorIsReady, set);
}
