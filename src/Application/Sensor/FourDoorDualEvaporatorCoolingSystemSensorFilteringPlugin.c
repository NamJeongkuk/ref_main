/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin.h"
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

static const Erd_t convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdList[] = {
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideRequest
};

static const Erd_t convertibleCompartmentCabinetThermistorValidValueErdList[] = {
   Erd_ConvertibleCompartmentCabinetThermistorIsValid,
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidOverrideValue
};

static const OverrideArbiterConfiguration_t convertibleCompartmentCabinetThermistorValidArbiterConfiguration = {
   convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdList,
   convertibleCompartmentCabinetThermistorValidValueErdList,
   Erd_ConvertibleCompartmentCabinetThermistor_IsValidResolved,
   NUM_ELEMENTS(convertibleCompartmentCabinetThermistorValidOverrideArbiterRequestErdList)
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

static const Erd_t convertibleCompartmentCabinetFilteredTemperatureOverrideRequestErdList[] = {
   Erd_ConvertibleCompartmentState,
   Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideRequest
};

static const Erd_t convertibleCompartmentCabinetFilteredTemperatureValueErdList[] = {
   Erd_ConvertibleCompartmentAsFreshFood_FilteredTemperatureInDegFx100,
   Erd_ConvertibleCompartmentAsFreezer_FilteredTemperatureInDegFx100,
   Erd_ConvertibleCompartmentCabinet_FilteredTemperatureOverrideValueInDegFx100
};

static const OverrideArbiterConfiguration_t convertibleCompartmentCabinetFilteredTemperatureArbiterConfiguration = {
   convertibleCompartmentCabinetFilteredTemperatureOverrideRequestErdList,
   convertibleCompartmentCabinetFilteredTemperatureValueErdList,
   Erd_ConvertibleCompartmentCabinet_FilteredTemperatureResolvedInDegFx100,
   NUM_ELEMENTS(convertibleCompartmentCabinetFilteredTemperatureOverrideRequestErdList)
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

static const SensorFilteringConfig_t freshFoodCabinetThermistorConfig = {
   .sensorAdcCountErd = Erd_FreshFoodThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_FreshFood_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_FreshFood_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_FreshFood_ThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_FreshFoodThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_FreshFoodThermistorDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t convertibleCompartmentCabinetAsFreezerThermistorConfig = {
   .sensorAdcCountErd = Erd_ConvertibleCompartmentCabinetThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_ConvertibleCompartmentCabinet_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_ConvertibleCompartmentAsFreezer_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_ConvertibleCompartmentCabinetThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_ConvertibleCompartmentCabinetThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_ConvertibleCompartmentCabinetThermistorDiscovered,
   .timerModuleErd = Erd_TimerModule
};

static const SensorFilteringConfig_t convertibleCompartmentCabinetAsFreshFoodThermistorConfig = {
   .sensorAdcCountErd = Erd_ConvertibleCompartmentCabinetThermistor_AdcCount,
   .sensorUnfilteredTemperatureInDegFx100Erd = Erd_ConvertibleCompartmentCabinet_UnfilteredTemperatureInDegFx100,
   .sensorFilteredTemperatureInDegFx100Erd = Erd_ConvertibleCompartmentAsFreshFood_FilteredTemperatureInDegFx100,
   .sensorIsValidErd = Erd_ConvertibleCompartmentCabinetThermistorIsValid,
   .sensorIsInvalidFaultErd = Erd_ConvertibleCompartmentCabinetThermistorIsInvalidFault,
   .sensorDiscoveredErd = Erd_ConvertibleCompartmentCabinetThermistorDiscovered,
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
   FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t *instance,
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

   OverrideArbiter_Init(
      &instance->freshFoodCabinetFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodCabinetFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freshFoodEvaporatorFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodEvaporatorFilteredTemperatureArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->convertibleCompartmentCabinetFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentCabinetFilteredTemperatureArbiterConfiguration);
}

static void InitializeThermistorIsValidArbiters(
   FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   OverrideArbiter_Init(
      &instance->freshFoodThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freezerThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freshFoodEvapThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freshFoodEvapThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->freezerEvapThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &freezerEvapThermistorValidArbiterConfiguration);

   OverrideArbiter_Init(
      &instance->convertibleCompartmentCabinetThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      &convertibleCompartmentCabinetThermistorValidArbiterConfiguration);
}

static void InitializeSensorFiltering(
   FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t *instance,
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

   SensorFiltering_Init(
      &instance->freshFoodEvapThermistor,
      dataModel,
      &freshFoodEvapThermistorConfig,
      sensorData->freshFoodEvapThermistor,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance->convertibleCompartmentCabinetAsFreezer,
      dataModel,
      &convertibleCompartmentCabinetAsFreezerThermistorConfig,
      sensorData->convertibleCompartmentAsFreezer,
      sensorData->periodicUpdateRateInMs);

   SensorFiltering_Init(
      &instance->convertibleCompartmentCabinetAsFreshFood,
      dataModel,
      &convertibleCompartmentCabinetAsFreshFoodThermistorConfig,
      sensorData->convertibleCompartmentAsFreshFood,
      sensorData->periodicUpdateRateInMs);
}

void FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_Init(
   FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t *instance,
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
