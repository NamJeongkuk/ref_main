/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const Erd_t freshFoodCabinetFilteredTemperatureOverrideRequestErdList[] = {
   Erd_FreshFood_AveragedTemperatureOverrideRequest,
   Erd_FreshFood_FilteredTemperatureOverrideRequest
};

static const Erd_t freshFoodCabinetFilteredTemperatureValueErdList[] = {
   Erd_FreshFood_FilteredTemperatureInDegFx100,
   Erd_FreshFood_EnhancedSabbath_AveragedTemperatureOverrideValueInDegFx100,
   Erd_FreshFood_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t freshFoodCabinetFilteredTemperatureArbiterConfiguration = {
   freshFoodCabinetFilteredTemperatureOverrideRequestErdList,
   freshFoodCabinetFilteredTemperatureValueErdList,
   Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(freshFoodCabinetFilteredTemperatureOverrideRequestErdList)
};

static const Erd_t freshFoodThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreshFoodThermistor_IsValidOverrideRequest
};

static const Erd_t freshFoodThermistorValidValueErdList[] = {
   Erd_FreshFood_ThermistorIsValid,
   Erd_FreshFoodThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freshFoodThermistorValidArbiterConfiguration = {
   freshFoodThermistorValidOverrideArbiterRequestErdList,
   freshFoodThermistorValidValueErdList,
   Erd_FreshFoodThermistor_IsValidResolved,
   NUM_ELEMENTS(freshFoodThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t freshFoodEvapThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_FreshFoodEvapThermistor_IsValidOverrideRequest
};

static const Erd_t freshFoodEvapThermistorValidValueErdList[] = {
   Erd_FreshFoodEvaporatorThermistorIsValid,
   Erd_FreshFoodEvapThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t freshFoodEvapThermistorValidArbiterConfiguration = {
   freshFoodEvapThermistorValidOverrideArbiterRequestErdList,
   freshFoodEvapThermistorValidValueErdList,
   Erd_FreshFoodEvapThermistor_IsValidResolved,
   NUM_ELEMENTS(freshFoodEvapThermistorValidOverrideArbiterRequestErdList)
};

static const Erd_t freshFoodEvaporatorFilteredTemperatureOverrideRequestErdList[] = {
   Erd_FreshFoodEvap_FilteredTemperatureOverrideRequest
};

static const Erd_t freshFoodEvaporatorFilteredTemperatureValueErdList[] = {
   Erd_FreshFoodEvap_FilteredTemperatureInDegFx100,
   Erd_FreshFoodEvap_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t freshFoodEvaporatorFilteredTemperatureArbiterConfiguration = {
   freshFoodEvaporatorFilteredTemperatureOverrideRequestErdList,
   freshFoodEvaporatorFilteredTemperatureValueErdList,
   Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(freshFoodEvaporatorFilteredTemperatureOverrideRequestErdList)
};

static const SensorFilteringConfig_t freshFoodCabinetThermistorConfig = {
   .sensorAdcCountErd = Erd_FreshFoodThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_FreshFood_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_FreshFood_ThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_FreshFoodThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_FreshFoodThermistorDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t freshFoodEvapThermistorConfig = {
   .sensorAdcCountErd = Erd_FreshFoodEvapThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_FreshFoodEvap_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_FreshFoodEvap_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_FreshFoodEvaporatorThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_FreshFoodEvaporatorThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_FreshFoodEvaporatorThermistorDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static void InitializeFilteredTemperatureOverrideArbiters(
   SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->freshFoodCabinetFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodCabinetFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freshFoodEvaporatorFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodEvaporatorFilteredTemperatureArbiterConfiguration);
}

static void InitializeThermistorIsValidArbiters(
   SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->freshFoodThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freshFoodEvapThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodEvapThermistorValidArbiterConfiguration);
}

static void InitializeSensorFiltering(
   SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   const SensorData_t *sensorData = PersonalityParametricData_Get(dataModel)->sensorData;

   SensorFiltering_Init(
      &instance->freshFoodCabinetThermistor,
      dataModel,
      &freshFoodCabinetThermistorConfig,
      sensorData->freshFoodCabinetThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance->freshFoodEvapThermistor,
      dataModel,
      &freshFoodEvapThermistorConfig,
      sensorData->freshFoodEvapThermistor,
      sensorData->periodicUpdateRateInMs);
}

void SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_Init(
   SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_t *instance,
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
