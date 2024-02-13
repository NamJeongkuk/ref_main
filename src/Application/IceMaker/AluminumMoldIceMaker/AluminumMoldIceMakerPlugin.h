/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERPLUGIN_H
#define ALUMINUMMOLDICEMAKERPLUGIN_H

#include "I_DataModel.h"
#include "FillTubeHeaterVotingFrameworkPlugin.h"
#include "FreezerIceRateHandler.h"
#include "SoftPwm.h"
#include "AluminumMoldIceMaker.h"
#include "FeelerArmMonitor.h"
#include "HarvestCountCalculator.h"
#include "IceMakerEnableResolver.h"
#include "ErdResolver.h"
#include "RakeController.h"
#include "AluminumMoldIceMakerFullStatusUpdater.h"
#include "IceMakerMoldHeaterController.h"
#include "FlowMeterMonitor.h"
#include "IceMakerWaterFillMonitor.h"
#include "SensorFiltering.h"
#include "ResolvedVoteRelayConnector.h"
#include "OverrideArbiter.h"
#include "IceMakerLocation.h"
#include "FreezerIceRateData.h"
#include "NonHarvestFillTubeHeaterData.h"
#include "PercentageDutyCycleVote.h"
#include "PercentageDutyCycleVoteToPwmDutyCycleConverter.h"
#include "NonHarvestFillTubeHeaterControl.h"

typedef struct
{
   const FreezerIceRateHandlerConfig_t *freezerIceRateHandlerConfig;
   const AluminumMoldIceMakerConfig_t *aluminumMoldIceMakerConfig;
   const FeelerArmMonitorConfig_t *feelerArmMonitorConfig;
   const HarvestCountCalculatorConfiguration_t *harvestCountCalculatorConfig;
   const ErdLogicServiceConfiguration_t *iceMakerEnableResolverConfig;
   const ErdResolverConfiguration_t *heaterResolverConfig;
   const ErdResolverConfiguration_t *waterValveResolverConfig;
   const ErdResolverConfiguration_t *rakeMotorResolverConfig;
   const ErdResolverConfiguration_t *fillTubeHeaterResolverConfig;
   const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t *fillTubeHeaterDutyCycleToPercentageCalculatorConfig;
   const NonHarvestFillTubeHeaterControlConfig_t *nonHarvestFillTubeHeaterControlConfig;
   const RakeControllerConfig_t *rakeControllerConfig;
   const AluminumMoldIceMakerFullStatusUpdaterConfig_t *iceMakerFullStatusUpdaterConfig;
   const ResolvedVoteRelayConnectorConfiguration_t *rakeMotorDriverConfig;
   const IceMakerMoldHeaterControllerConfig_t *heaterControllerConfig;
   const SensorFilteringConfig_t *moldThermistorConfig;
   const IceMakerWaterFillMonitorConfig_t *fillMonitorConfig;
   const ResolvedVoteRelayConnectorConfiguration_t *waterValveRelayConnectorConfig;
   const OverrideArbiterConfiguration_t *filteredTemperatureArbiterConfig;
   const ResolvedVoteRelayConnectorConfiguration_t *heaterRelayConnectorConfig;
   const OverrideArbiterConfiguration_t *thermistorValidArbiterConfig;
} AluminumMoldIceMakerPluginConfig_t;

typedef struct
{
   struct
   {
      FillTubeHeaterVotingFrameworkPlugin_t fillTubeHeaterVotingFrameworkPlugin;
      FreezerIceRateHandler_t iceRateHandler;
      FeelerArmMonitor_t feelerArmMonitor;
      RakeController_t rakeController;
      AluminumMoldIceMakerFullStatusUpdater_t iceMakerFullStatusUpdater;
      ResolvedVoteRelayConnector_t rakeMotorRelayConnector;
      HarvestCountCalculator_t harvestCountCalculator;
      AluminumMoldIceMaker_t aluminumMoldIceMaker;
      IceMakerEnableResolver_t iceMakerEnableResolver;
      IceMakerMoldHeaterController_t iceMakerMoldHeaterController;
      IceMakerWaterFillMonitor_t iceMakerFillMonitor;

      ErdResolver_t iceMakerWaterValveVoteResolver;
      ErdResolver_t iceMakerHeaterVoteResolver;
      ErdResolver_t iceMakerMotorVoteResolver;
      SensorFiltering_t moldThermistor;
      ErdResolver_t iceMakerRakeMotorVoteResolver;
      ErdResolver_t fillTubeHeaterVoteResolver;
      ResolvedVoteRelayConnector_t iceMakerWaterValveRelayConnector;
      ResolvedVoteRelayConnector_t heaterRelayConnector;
      OverrideArbiter_t aluminumMoldIceMakerFilteredTemperatureArbiter;
      OverrideArbiter_t aluminumMoldIceMakerThermistorValidArbiter;
   } _private;
} AluminumMoldIceMakerPlugin_t;

/*!
 * @brief
 *
 * @param instance
 * @param dataModel
 * @param iceMakerLocation
 * @param aluminumMoldData
 * @param sensorData
 * @param freezerIceRateData
 * @param nonHarvestFillTubeHeaterData
 * @param config
 */
void AluminumMoldIceMakerPlugin_Init(
   AluminumMoldIceMakerPlugin_t *instance,
   I_DataModel_t *dataModel,
   IceMakerLocation_t iceMakerLocation,
   const AluminumMoldIceMakerData_t *aluminumMoldData,
   const SensorDataSensorType_t *sensorData,
   const uint16_t *sensorPeriodicUpdateRateInMs,
   const FreezerIceRateData_t *freezerIceRateData,
   const NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData,
   const AluminumMoldIceMakerPluginConfig_t *config);

#endif
