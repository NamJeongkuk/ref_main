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

static void UpdateLongTermAverage(ShiftOffsetCalculator_t *instance)
{
   TemperatureDegFx100_t longTermAverage;
   Filter_Read(instance->_private.longTermAverageFilter, &longTermAverage);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->longTermAverageErd,
      &longTermAverage);
}

static void FilterFeed(ShiftOffsetCalculator_t *instance, TemperatureDegFx100_t feedValue)
{
   Filter_Feed(instance->_private.longTermAverageFilter, &feedValue);
   UpdateLongTermAverage(instance);
}

static void FilterSeed(ShiftOffsetCalculator_t *instance, TemperatureDegFx100_t seedValue)
{
   Filter_Seed(instance->_private.longTermAverageFilter, &seedValue);
   UpdateLongTermAverage(instance);
}

static void SetShiftOffsetTo(ShiftOffsetCalculator_t *instance, TemperatureDegFx100_t shiftOffset)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->shiftOffsetErd,
      &shiftOffset);
}

static TemperatureDegFx100_t AdjustedSetpoint(ShiftOffsetCalculator_t *instance)
{
   TemperatureDegFx100_t adjustedSetpoint;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->adjustedSetpointErd,
      &adjustedSetpoint);

   return adjustedSetpoint;
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
   ShiftOffsetCalculator_t *instance = context;
   TemperatureDegFx100_t filteredTemperature = FilteredTemperature(instance);
   FilterFeed(instance, filteredTemperature);
}

static void ShiftOffsetCalculateTimeExpired(void *context)
{
   ShiftOffsetCalculator_t *instance = context;
   TemperatureDegFx100_t adjustedSetpoint;
   TemperatureDegFx100_t average1;
   TemperatureDegFx100_t difference;
   TemperatureDegFx100_t shiftOffset;

   adjustedSetpoint = AdjustedSetpoint(instance);
   shiftOffset = ShiftOffset(instance);
   Filter_Read(instance->_private.longTermAverageFilter, &average1);

   TemperatureDegFx100_t adjustedSetpointWithoutShift = adjustedSetpoint - shiftOffset;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->adjustedSetpointWithoutShiftErd,
      &adjustedSetpointWithoutShift);

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

      if(shiftOffset > instance->_private.shiftOffsetData->maxShiftInDegFx100)
      {
         shiftOffset = instance->_private.shiftOffsetData->maxShiftInDegFx100;
      }
      else if(shiftOffset < instance->_private.shiftOffsetData->minShiftInDegFx100)
      {
         shiftOffset = instance->_private.shiftOffsetData->minShiftInDegFx100;
      }

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->shiftOffsetErd,
         &shiftOffset);
   }
}

static bool AdjustedSetpointPluginIsReady(
   I_DataModel_t *dataModel,
   Erd_t adjustedSetpointPluginReadyErd)
{
   bool adjustedSetpointPluginIsReady;

   DataModel_Read(
      dataModel,
      adjustedSetpointPluginReadyErd,
      &adjustedSetpointPluginIsReady);

   return adjustedSetpointPluginIsReady;
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
      FilterSeed(instance, AdjustedSetpoint(instance) - ShiftOffset(instance));
      SetShiftOffsetTo(instance, 0);
   }
}

void ShiftOffsetCalculator_Init(
   ShiftOffsetCalculator_t *instance,
   I_DataModel_t *dataModel,
   I_Filter_t *longTermAverageFilter,
   const ShiftOffsetCalculatorConfig_t *config,
   const ShiftOffsetData_t *shiftOffsetData)
{
   uassert(AdjustedSetpointPluginIsReady(dataModel, config->adjustedSetpointPluginReadyErd));

   instance->_private.dataModel = dataModel;
   instance->_private.longTermAverageFilter = longTermAverageFilter;
   instance->_private.shiftOffsetCalculatorData =
      PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->shiftOffsetCalculatorData;
   instance->_private.config = config;
   instance->_private.shiftOffsetData = shiftOffsetData;

   SetShiftOffsetTo(instance, 0);
   FilterSeed(instance, AdjustedSetpoint(instance));

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         dataModel,
         config->timerModuleErd),
      &instance->_private.longTermAverageUpdateTimer,
      instance->_private.shiftOffsetCalculatorData->longTermAverageUpdateTimeInMinutes * MSEC_PER_MIN,
      LongTermAverageUpdateTimeExpired,
      instance);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         dataModel,
         config->timerModuleErd),
      &instance->_private.shiftOffsetCalculateTimer,
      instance->_private.shiftOffsetCalculatorData->updateTimeInMinutes * MSEC_PER_MIN,
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
