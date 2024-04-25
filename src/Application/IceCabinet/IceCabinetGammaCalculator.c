/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetGammaCalculator.h"
#include "utils.h"
#include "IceCabinetData.h"

static void CalculateGamma(IceCabinetGammaCalculator_t *instance)
{
   TemperatureDegFx100_t freezerTemperatureInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerFilteredTemperatureResolvedInDegFx100Erd,
      &freezerTemperatureInDegFx100);

   TemperatureDegFx100_t iceCabinetTemperatureInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetTemperatureResolvedInDegFx100Erd,
      &iceCabinetTemperatureInDegFx100);

   TemperatureDegFx100_t freezerAdjustedSetpointInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freezerAdjustedSetpointErd,
      &freezerAdjustedSetpointInDegFx100);

   TemperatureDegFx100_t iceCabinetAdjustedSetpointInDegFx100;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceCabinetAdjustedSetpointErd,
      &iceCabinetAdjustedSetpointInDegFx100);

   uint8_t freezerHysteresisRatio = CLAMP((100 * (int16_t)(freezerTemperatureInDegFx100 - (freezerAdjustedSetpointInDegFx100 + instance->_private.freezerGridLineData->gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)) /
                                             (instance->_private.freezerGridLineData->gridLineData[GridLine_FreezerHighHyst].gridLinesDegFx100 -
                                                instance->_private.freezerGridLineData->gridLineData[GridLine_FreezerLowHyst].gridLinesDegFx100)),
      0,
      100);

   uint8_t iceCabinetHysteresisRatio = CLAMP((100 * (int16_t)(iceCabinetTemperatureInDegFx100 - (iceCabinetAdjustedSetpointInDegFx100 + instance->_private.iceCabinetGridLineData->gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)) /
                                                (instance->_private.iceCabinetGridLineData->gridLineData[GridLine_IceCabinetHighHyst].gridLinesDegFx100 -
                                                   instance->_private.iceCabinetGridLineData->gridLineData[GridLine_IceCabinetLowHyst].gridLinesDegFx100)),
      0,
      100);

   Gamma_t gamma = (100 - (freezerHysteresisRatio - iceCabinetHysteresisRatio)) / 2;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->gammaErd,
      &gamma);
}

static void DataModelChanged(void *context, const void *args)
{
   IceCabinetGammaCalculator_t *instance = context;
   const DataModelOnDataChangeArgs_t *onChangeArgs = args;
   Erd_t erd = onChangeArgs->erd;

   if(erd == instance->_private.config->freezerFilteredTemperatureResolvedInDegFx100Erd ||
      erd == instance->_private.config->iceCabinetTemperatureResolvedInDegFx100Erd ||
      erd == instance->_private.config->freezerAdjustedSetpointErd ||
      erd == instance->_private.config->iceCabinetAdjustedSetpointErd)
   {
      CalculateGamma(instance);
   }
}

void IceCabinetGammaCalculator_Init(
   IceCabinetGammaCalculator_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetGammaCalculatorConfig_t *config,
   const DeltaAxisGridLines_t *freezerGridLineData,
   const DeltaAxisGridLines_t *iceCabinetGridLineData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.freezerGridLineData = freezerGridLineData;
   instance->_private.iceCabinetGridLineData = iceCabinetGridLineData;

   CalculateGamma(instance);

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelChanged);

   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.dataModelSubscription);
}
