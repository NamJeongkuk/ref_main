/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "AluminumMoldIceMakerPlugin.h"
#include "PersonalityParametricData.h"
#include "IceMakerEnableResolver.h"
#include "Vote.h"

static void InitializeIceMakerOverrideArbiters(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel, const AluminumMoldIceMakerPluginConfig_t *config)
{
   OverrideArbiter_Init(
      &instance->_private.aluminumMoldIceMakerFilteredTemperatureArbiter,
      DataModel_AsDataSource(dataModel),
      config->filteredTemperatureArbiterConfig);

   OverrideArbiter_Init(
      &instance->_private.aluminumMoldIceMakerThermistorValidArbiter,
      DataModel_AsDataSource(dataModel),
      config->thermistorValidArbiterConfig);
}

static void InitializeErdResolvers(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel, const AluminumMoldIceMakerPluginConfig_t *config)
{
   ErdResolver_Init(
      &instance->_private.iceMakerHeaterVoteResolver,
      DataModel_AsDataSource(dataModel),
      config->heaterResolverConfig);

   ErdResolver_Init(
      &instance->_private.iceMakerWaterValveVoteResolver,
      DataModel_AsDataSource(dataModel),
      config->waterValveResolverConfig);

   ErdResolver_Init(
      &instance->_private.iceMakerRakeMotorVoteResolver,
      DataModel_AsDataSource(dataModel),
      config->rakeMotorResolverConfig);

   ErdResolver_Init(
      &instance->_private.fillTubeHeaterVoteResolver,
      DataModel_AsDataSource(dataModel),
      config->fillTubeHeaterResolverConfig);
}

void AluminumMoldIceMakerPlugin_Init(
   AluminumMoldIceMakerPlugin_t *instance,
   I_DataModel_t *dataModel,
   IceMakerLocation_t iceMakerLocation,
   const AluminumMoldIceMakerData_t *aluminumMoldData,
   const SensorDataSensorType_t *sensorData,
   const uint16_t *sensorPeriodicUpdateRateInMs,
   const FreezerIceRateData_t *freezerIceRateData,
   const NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData,
   const AluminumMoldIceMakerPluginConfig_t *config)
{
   InitializeIceMakerOverrideArbiters(instance, dataModel, config);

   SensorFiltering_Init(
      &instance->_private.moldThermistor,
      dataModel,
      config->moldThermistorConfig,
      sensorData,
      *sensorPeriodicUpdateRateInMs);

   InitializeErdResolvers(
      instance,
      dataModel,
      config);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.iceMakerWaterValveRelayConnector,
      dataModel,
      config->waterValveRelayConnectorConfig);

   FillTubeHeaterVotingFrameworkPlugin_Init(
      &instance->_private.fillTubeHeaterVotingFrameworkPlugin,
      dataModel,
      nonHarvestFillTubeHeaterData,
      config->fillTubeHeaterDutyCycleToPercentageCalculatorConfig,
      config->nonHarvestFillTubeHeaterControlConfig);

   if(iceMakerLocation == IceMakerLocation_Freezer)
   {
      FreezerIceRateHandler_Init(
         &instance->_private.iceRateHandler,
         dataModel,
         config->freezerIceRateHandlerConfig,
         freezerIceRateData);
   }

   FeelerArmMonitor_Init(
      &instance->_private.feelerArmMonitor,
      dataModel,
      config->feelerArmMonitorConfig,
      &aluminumMoldData->freezeData.minimumFeelerArmExtensionTimeInMinutes);

   RakeController_Init(
      &instance->_private.rakeController,
      dataModel,
      config->rakeControllerConfig,
      &aluminumMoldData->harvestData);

   AluminumMoldIceMakerFullStatusUpdater_Init(
      &instance->_private.iceMakerFullStatusUpdater,
      dataModel,
      config->iceMakerFullStatusUpdaterConfig);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.rakeMotorRelayConnector,
      dataModel,
      config->rakeMotorDriverConfig);

   HarvestCountCalculator_Init(
      &instance->_private.harvestCountCalculator,
      dataModel,
      config->harvestCountCalculatorConfig,
      aluminumMoldData->freezeData.harvestCountCalculatorData);

   IceMakerEnableResolver_Init(
      &instance->_private.iceMakerEnableResolver,
      dataModel,
      config->iceMakerEnableResolverConfig);

   IceMakerMoldHeaterController_Init(
      &instance->_private.iceMakerMoldHeaterController,
      dataModel,
      config->heaterControllerConfig,
      &aluminumMoldData->harvestData);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.heaterRelayConnector,
      dataModel,
      config->heaterRelayConnectorConfig);

   IceMakerWaterFillMonitor_Init(
      &instance->_private.iceMakerFillMonitor,
      dataModel,
      config->fillMonitorConfig,
      aluminumMoldData->fillData.iceMakerFillMonitorData);

   AluminumMoldIceMaker_Init(
      &instance->_private.aluminumMoldIceMaker,
      dataModel,
      config->aluminumMoldIceMakerConfig,
      aluminumMoldData);
}
