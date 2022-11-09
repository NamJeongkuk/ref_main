/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const SensorFilteringConfig_t freezerCabinetThermistorConfig = {
   .sensorAdcCountErd = Erd_FreezerThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_Freezer_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_Freezer_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t freshFoodCabinetThermistorConfig = {
   .sensorAdcCountErd = Erd_FreshFoodThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_FreshFood_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_FreshFood_ThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t freezerEvapThermistorConfig = {
   .sensorAdcCountErd = Erd_FreezerEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_FreezerEvap_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_FreezerEvap_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .timerModuleErd = Erd_TimerModule
};

void SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_Init(
   SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const SensorData_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData;

   SensorFiltering_Init(
      &instance->freezerCabinetThermistor,
      dataModel,
      &freezerCabinetThermistorConfig,
      sensorData->freezerCabinetThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance->freshFoodCabinetThermistor,
      dataModel,
      &freshFoodCabinetThermistorConfig,
      sensorData->freshFoodCabinetThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance->freezerEvapThermistor,
      dataModel,
      &freezerEvapThermistorConfig,
      sensorData->freezerEvapThermistor,
      sensorData->periodicUpdateRateInMs);

   DataModel_Write(
      dataModel,
      Erd_SensorsReadyToBeRead,
      set);
}
