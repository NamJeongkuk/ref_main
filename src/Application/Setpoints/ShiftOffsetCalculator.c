/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ShiftOffsetCalculator.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "PersonalityParametricData.h"
#include "TemperatureDegFx100.h"
#include "uassert.h"
#include <stdlib.h>

static bool FilterIsReady(ShiftOffsetCalculator_t *instance)
{
   bool ready;
   Input_Read(Filter_GetReadyInput(instance->_private.longTermAverageFilter), &ready);
   return ready;
}

static void UpdateLongTermAverage(ShiftOffsetCalculator_t *instance)
{
   TemperatureDegFx100_t longTermAverage;
   Filter_Read(instance->_private.longTermAverageFilter, &longTermAverage);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->longTermAverageErd,
      &longTermAverage);
}

static void FilterSeed(ShiftOffsetCalculator_t *instance, TemperatureDegFx100_t seedValue)
{
   Filter_Seed(instance->_private.longTermAverageFilter, &seedValue);
   UpdateLongTermAverage(instance);
}

static void FilterFeed(ShiftOffsetCalculator_t *instance, TemperatureDegFx100_t feedValue)
{
   if(FilterIsReady(instance))
   {
      Filter_Feed(instance->_private.longTermAverageFilter, &feedValue);
      UpdateLongTermAverage(instance);
   }
   else
   {
      FilterSeed(instance, feedValue);
   }
}

static void SetShiftOffsetTo(ShiftOffsetCalculator_t *instance, TemperatureDegFx100_t shiftOffset)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->shiftOffsetErd,
      &shiftOffset);
}

static TemperatureDegFx100_t ShiftOffset(ShiftOffsetCalculator_t *instance)
{
   TemperatureDegFx100_t shiftOffset;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->shiftOffsetErd,
      &shiftOffset);

   return shiftOffset;
}

static TemperatureDegFx100_t FilteredTemperature(ShiftOffsetCalculator_t *instance)
{
   TemperatureDegFx100_t filteredTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->filteredTemperatureInDegFx100Erd,
      &filteredTemperature);

   return filteredTemperature;
}

static void LongTermAverageUpdateTimeExpired(void *context)
{
   FilterFeed(context, FilteredTemperature(context));
}

static TemperatureDegFx100_t AdjustedSetpointWithoutShift(ShiftOffsetCalculator_t *instance)
{
   TemperatureDegFx100_t adjustedSetpointWithoutShift;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->adjustedSetpointWithoutShiftErd,
      &adjustedSetpointWithoutShift);

   return adjustedSetpointWithoutShift;
}

static void ShiftOffsetCalculateTimeExpired(void *context)
{
   ShiftOffsetCalculator_t *instance = context;
   TemperatureDegFx100_t average1;
   TemperatureDegFx100_t difference;
   TemperatureDegFx100_t shiftOffset = ShiftOffset(instance);

   Filter_Read(instance->_private.longTermAverageFilter, &average1);

   TemperatureDegFx100_t adjustedSetpointWithoutShift = AdjustedSetpointWithoutShift(instance);

   if(average1 > (adjustedSetpointWithoutShift - instance->_private.shiftOffsetData->lowerAdjustmentLimitInDegFx100) &&
      average1 < (adjustedSetpointWithoutShift + instance->_private.shiftOffsetData->upperAdjustmentLimitInDegFx100))
   {
      difference = average1 - adjustedSetpointWithoutShift;
      if(difference > 100)
      {
         shiftOffset -= 10;
      }
      else if(difference > 20)
      {
         shiftOffset -= 2;
      }
      else if(difference < -100)
      {
         shiftOffset += 10;
      }
      else if(difference < -20)
      {
         shiftOffset += 2;
      }

      shiftOffset = CLAMP(shiftOffset,
         instance->_private.shiftOffsetData->minShiftInDegFx100,
         instance->_private.shiftOffsetData->maxShiftInDegFx100);

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->shiftOffsetErd,
         &shiftOffset);
   }
}

static void DataModelUpdated(void *context, const void *args)
{
   ShiftOffsetCalculator_t *instance = context;
   const DataModelOnDataChangeArgs_t *onDataChangeArgs = args;

   if(onDataChangeArgs->erd == instance->_private.config->postDwellCompletionSignalErd)
   {
      SetShiftOffsetTo(instance, instance->_private.shiftOffsetData->postDefrostShiftInDegFx100);
   }
   else if(onDataChangeArgs->erd == instance->_private.config->resetThermalShiftOffsetSignalErd)
   {
      FilterSeed(instance, AdjustedSetpointWithoutShift(instance));
      SetShiftOffsetTo(instance, 0);
   }
}

void ShiftOffsetCalculator_Init(
   ShiftOffsetCalculator_t *instance,
   I_DataModel_t *dataModel,
   I_Filter_t *longTermAverageFilter,
   TimerModule_t *timerModule,
   const ShiftOffsetCalculatorConfig_t *config,
   const ShiftOffsetData_t *shiftOffsetData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.longTermAverageFilter = longTermAverageFilter;
   instance->_private.config = config;
   instance->_private.shiftOffsetData = shiftOffsetData;

   const ShiftOffsetCalculatorData_t *shiftOffsetCalculatorData =
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->shiftOffsetCalculatorData;

   SetShiftOffsetTo(instance, 0);
   FilterSeed(instance, AdjustedSetpointWithoutShift(instance));

   TimerModule_StartPeriodic(
      timerModule,
      &instance->_private.longTermAverageUpdateTimer,
      shiftOffsetCalculatorData->longTermAverageUpdateTimeInMinutes * MSEC_PER_MIN,
      LongTermAverageUpdateTimeExpired,
      instance);

   TimerModule_StartPeriodic(
      timerModule,
      &instance->_private.shiftOffsetCalculateTimer,
      shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN,
      ShiftOffsetCalculateTimeExpired,
      instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelUpdated);

   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelSubscription);
}
