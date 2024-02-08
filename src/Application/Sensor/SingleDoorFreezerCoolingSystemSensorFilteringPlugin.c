/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreezerCoolingSystemSensorFilteringPlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const Erd_t freezerEvaporatorFilteredTemperatureOverrideRequestErdList[] = {
   Erd_FreezerEvap_FilteredTemperatureOverrideRequest
};

static const Erd_t freezerEvaporatorFilteredTemperatureValueErdList[] = {
   Erd_FreezerEvap_FilteredTemperatureInDegFx100,
   Erd_FreezerEvap_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t freezerEvaporatorFilteredTemperatureArbiterConfiguration = {
   freezerEvaporatorFilteredTemperatureOverrideRequestErdList,
   freezerEvaporatorFilteredTemperatureValueErdList,
   Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(freezerEvaporatorFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t freezerCabinetFilteredTemperatureOverrideRequestErdList[] = {
   Erd_Freezer_AveragedTemperatureOverrideRequest,
   Erd_Freezer_FilteredTemperatureOverrideRequest
};

static const Erd_t freezerCabinetFilteredTemperatureValueErdList[] = {
   Erd_Freezer_FilteredTemperatureInDegFx100,
   Erd_Freezer_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,
   Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t freezerCabinetFilteredTemperatureArbiterConfiguration = {
   freezerCabinetFilteredTemperatureOverrideRequestErdList,
   freezerCabinetFilteredTemperatureValueErdList,
   Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(freezerCabinetFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t freezerThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreezerThermistor_IsValidOverrideRequest
};

static const Erd_t freezerThermistorValidValueErdList[] = {
   Erd_Freezer_ThermistorIsValid,
   Erd_FreezerThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freezerThermistorValidArbiterConfiguration = {
   freezerThermistorValidOverrideArbiterRequestErdList,
   freezerThermistorValidValueErdList,
   Erd_FreezerThermistor_IsValidResolved,
   NUM_ELEMENTS(freezerThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t freezerEvapThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreezerEvapThermistor_IsValidOverrideRequest
};

static const Erd_t freezerEvapThermistorValidValueErdList[] = {
   Erd_FreezerEvaporatorThermistorIsValid,
   Erd_FreezerEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freezerEvapThermistorValidArbiterConfiguration = {
   freezerEvapThermistorValidOverrideArbiterRequestErdList,
   freezerEvapThermistorValidValueErdList,
   Erd_FreezerEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(freezerEvapThermistorValidOverrideArbiterRequestErdList)
};

static const SensorFilteringConfig_t freezerCabinetThermistorConfig = {
   .sensorAdcCountErd = Erd_FreezerThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_Freezer_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_Freezer_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_Freezer_ThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_FreezerThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_FreezerThermistorDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t freezerEvapThermistorConfig = {
   .sensorAdcCountErd = Erd_FreezerEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_FreezerEvap_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_FreezerEvap_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_FreezerEvaporatorThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_FreezerEvaporatorThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_FreezerEvaporatorThermistorDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static void InitializeFilteredTemperatureOverrideArbiters(
   SingleDoorFreezerCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->freezerEvaporatorFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerEvaporatorFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freezerCabinetFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerCabinetFilteredTemperatureArbiterConfiguration);
}

static void InitializeThermistorIsValidArbiters(
   SingleDoorFreezerCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->freezerThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freezerEvapThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerEvapThermistorValidArbiterConfiguration);
}

static void InitializeSensorFiltering(
   SingleDoorFreezerCoolingSystemSensorFilteringPlugin_t *instance,
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
      &instance->freezerEvapThermistor,
      dataModel,
      &freezerEvapThermistorConfig,
      sensorData->freezerEvapThermistor,
      sensorData->periodicUpdateRateInMs);
}

void SingleDoorFreezerCoolingSystemSensorFilteringPlugin_Init(
   SingleDoorFreezerCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   InitializeFilteredTemperatureOverrideArbiters(instance, dataModel);
   InitializeThermistorIsValidArbiters(instance, dataModel);
   InitializeSensorFiltering(instance, dataModel);

   DataModel_Write(
      dataModel,
      Erd_SensorsReadyToBeRead,
      set);
}
