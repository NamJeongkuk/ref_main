/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CrossAmbientOffsetCalculator.h"
#include "PersonalityParametricData.h"
#include "TemperatureDegFx100.h"
#include "SetpointZone.h"
#include "utils.h"

static void CalculateCrossAmbientOffset(void *context, const void *args)
{
   IGNORE(args);
   CrossAmbientCalculator_t *instance = context;

   TemperatureDegFx100_t averageTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->crossAmbientWindowAveragedTemperatureInDegFx100Erd,
      &averageTemperature);

   TemperatureDegFx100_t offset = (instance->_private.crossAmbientOffsetData->slopeCoefficient * (averageTemperature - 9000)) / 10000;
   offset = CLAMP(offset, instance->_private.crossAmbientOffsetData->minimumAmbientOffset, instance->_private.crossAmbientOffsetData->maximumAmbientOffset);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->crossAmbientOffsetErd,
      &offset);
}

void CrossAmbientOffsetCalculator_Init(
   CrossAmbientCalculator_t *instance,
   I_DataModel_t *dataModel,
   const CrossAmbientOffsetData_t *data,
   const CrossAmbientCalculatorConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.crossAmbientOffsetData = data;

   EventSubscription_Init(
      &instance->_private.crossAmbientWindowAveragedTemperatureSubscription,
      instance,
      CalculateCrossAmbientOffset);

   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->crossAmbientWindowAveragedTemperatureInDegFx100Erd,
      &instance->_private.crossAmbientWindowAveragedTemperatureSubscription);
}
