/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerPlugin.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "DataSource_Gpio.h"

void TwistTrayIceMakerPlugin_Init(
   TwistTrayIceMakerPlugin_t *instance,
   I_DataModel_t *dataModel,
   IceMakerLocation_t location,
   const TwistTrayIceMakerData_t *twistTrayData,
   const SensorDataSensorType_t *sensorData,
   const uint16_t *sensorPeriodicUpdateRateInMs,
   const FreezerIceRateData_t *freezerIceRateData,
   const NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData,
   const TwistTrayIceMakerPlugConfig_t *config)
{
   SensorFiltering_Init(
      &instance->_private.sensorFilter,
      dataModel,
      config->sensorFilterConfig,
      sensorData,
      *sensorPeriodicUpdateRateInMs);

   OverrideArbiter_Init(
      &instance->_private.twistTrayIceMakerThermistorIsValidOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      config->thermistorValidArbiterConfig);

   OverrideArbiter_Init(
      &instance->_private.twistTrayIceMakerFilteredTemperatureOverrideArbiter,
      DataModel_AsDataSource(dataModel),
      config->filteredTemperatureArbiterConfig);

   ErdResolver_Init(
      &instance->_private.twistTrayMotorVoteResolver,
      DataModel_AsDataSource(dataModel),
      config->motorResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.iceMakerWaterValveVoteResolver,
      DataModel_AsDataSource(dataModel),
      config->waterValveResolverConfig);

   ErdResolver_Init(
      &instance->_private.fillTubeHeaterVoteResolver,
      DataModel_AsDataSource(dataModel),
      config->fillTubeHeaterResolverConfig);

   ResolvedVoteRelayConnector_Init(
      &instance->_private.iceMakerWaterValveRelayConnector,
      dataModel,
      config->waterValveRelayConnectorConfig);

   HarvestCountCalculator_Init(
      &instance->_private.harvestCountCalculator,
      dataModel,
      config->harvestCountCalculatorConfig,
      twistTrayData->freezeData.harvestCountCalculatorData);

   IceMakerEnableResolver_Init(
      &instance->_private.iceMakerEnableResolver,
      dataModel,
      config->iceMakerEnableResolverConfig);

   TwistTrayIceMakerRunner_Init(
      &instance->_private.twistTrayIceMakerMotorControllerRunner,
      &instance->_private.twistTrayMotorController,
      DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt));

   TwistTrayIceMakerMotorRequestManager_Init(
      &instance->_private.twistTrayMotorRequestManager,
      dataModel,
      config->motorRequestManagerConfig);

   TwistTrayIceMakerMotorController_Init(
      &instance->_private.twistTrayMotorController,
      twistTrayData,
      DataModelErdPointerAccess_GetOutput(dataModel, config->twistTrayMotorOutputErd),
      dataModel,
      config->motorControllerConfig);

   TwistTrayIceMakerMotorControllerValueUpdater_Init(
      &instance->_private.twistTrayMotorControllerValueUpdater,
      dataModel,
      &instance->_private.twistTrayMotorController,
      config->motorControllerValueUpdaterConfig);

   TwistTrayIceMakerMotorSwitchMonitor_Init(
      &instance->_private.twistTrayIceMakerSwitchMonitor,
      dataModel,
      &instance->_private.twistTrayMotorController,
      config->motorSwitchMonitorConfig);

   FillTubeHeaterVotingFrameworkPlugin_Init(
      &instance->_private.fillTubeHeaterVotingFrameworkPlugin,
      dataModel,
      nonHarvestFillTubeHeaterData,
      config->fillTubeHeaterDutyCycleToPercentageCalculatorConfig,
      config->nonHarvestFillTubeHeaterControlConfig);

   if(location == IceMakerLocation_Freezer)
   {
      FreezerIceRateHandler_Init(
         &instance->_private.iceRateHandler,
         dataModel,
         config->freezerIceRateHandlerConfig,
         freezerIceRateData);
   }

   IceMakerWaterFillMonitor_Init(
      &instance->_private.twistTrayIceMakerFillMonitor,
      dataModel,
      config->fillMonitorConfig,
      twistTrayData->fillData.iceMakerFillMonitorData);

   TwistTrayIceMaker_Init(
      &instance->_private.twistTrayIceMaker,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      DataModel_AsDataSource(dataModel),
      config->twistTrayIceMakerConfig,
      twistTrayData);
}
