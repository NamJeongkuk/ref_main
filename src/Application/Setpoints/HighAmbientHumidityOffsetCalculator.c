/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "HighAmbientHumidityOffsetCalculator.h"
#include "PersonalityParametricData.h"
#include "TemperatureDegFx100.h"
#include "SetpointZone.h"
#include "utils.h"

static void CalculateHighAmbientHumidityOffset(void *context, const void *args)
{
   HighAmbientOffsetCalculator_t *instance = context;
   IGNORE(args);

   Setpoint_t userSetpointStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->setpointStatusErd,
      &userSetpointStatus);

   RelativeHumidityPercentx100_t relativeHumidityPercentx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredHumidityResolvedPercentx100Erd,
      &relativeHumidityPercentx100);

   bool ambientHumiditySensorIsValid;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->ambientHumiditySensorIsValidResolvedErd,
      &ambientHumiditySensorIsValid);

   if((ambientHumiditySensorIsValid) &&
      (userSetpointStatus < instance->_private.highAmbientOffsetData->userSetpointTriggerForHighAmbientOffsetInDegF) &&
      ((relativeHumidityPercentx100 / 100) > instance->_private.highAmbientOffsetData->relativeHumidityTriggerForHighAmbientOffsetInPercent))
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->highAmbientOffsetErd,
         &(instance->_private.highAmbientOffsetData->offsetForHighAmbientHumidityInDegFx100));
   }
   else
   {
      TemperatureDegFx100_t zeroOffset = 0;
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->highAmbientOffsetErd,
         &zeroOffset);
   }
}

void HighAmbientHumidityOffsetCalculator_Init(
   HighAmbientOffsetCalculator_t *instance,
   I_DataModel_t *dataModel,
   const HighAmbientOffsetData_t *data,
   const HighAmbientOffsetCalculatorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.highAmbientOffsetData = data;

   CalculateHighAmbientHumidityOffset(instance, NULL);

   EventSubscription_Init(
      &instance->_private.setpointStatusSubscription,
      instance,
      CalculateHighAmbientHumidityOffset);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->setpointStatusErd,
      &instance->_private.setpointStatusSubscription);

   EventSubscription_Init(
      &instance->_private.filteredHumidityResolvedPercentx100Subscription,
      instance,
      CalculateHighAmbientHumidityOffset);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->ambientFilteredHumidityResolvedPercentx100Erd,
      &instance->_private.filteredHumidityResolvedPercentx100Subscription);
}
