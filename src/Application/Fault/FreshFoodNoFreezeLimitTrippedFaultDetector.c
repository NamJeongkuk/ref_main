/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodNoFreezeLimitTrippedFaultDetector.h"
#include "Constants_Binary.h"
#include "CalculatedGridLines.h"
#include "DeltaGridLines.h"
#include "utils.h"

static void SetAndClearFreshFoodNoFreezeLimitTrippedFault(FreshFoodNoFreezeLimitTrippedFaultDetector_t *instance)
{
   CalculatedGridLines_t calculatedGridLines;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculateGridLineErd,
      &calculatedGridLines);

   TemperatureDegFx100_t freshFoodTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodFilteredTemperatureErd,
      &freshFoodTemperature);

   if(freshFoodTemperature < calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_Nfl])
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freshFoodNoFreezeLimitTrippedFaultErd,
         set);
   }
   else if(freshFoodTemperature > calculatedGridLines.freshFoodGridLine.gridLinesDegFx100[GridLine_FreshFoodLowHyst])
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->freshFoodNoFreezeLimitTrippedFaultErd,
         clear);
   }
}

static void OnFreshFoodTemperatureChange(void *context, const void *args)
{
   FreshFoodNoFreezeLimitTrippedFaultDetector_t *instance = context;
   IGNORE(args);

   SetAndClearFreshFoodNoFreezeLimitTrippedFault(instance);
}

void FreshFoodNoFreezeLimitTrippedFaultDetector_Init(
   FreshFoodNoFreezeLimitTrippedFaultDetector_t *instance,
   I_DataModel_t *dataModel,
   const FreshFoodNoFreezeLimitTrippedFaultDetectorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   SetAndClearFreshFoodNoFreezeLimitTrippedFault(instance);

   EventSubscription_Init(
      &instance->_private.freshFoodTemperatureSubscription,
      instance,
      OnFreshFoodTemperatureChange);
   DataModel_Subscribe(
      dataModel,
      config->freshFoodFilteredTemperatureErd,
      &instance->_private.freshFoodTemperatureSubscription);
}
