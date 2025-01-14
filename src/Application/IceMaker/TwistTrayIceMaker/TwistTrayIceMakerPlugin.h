/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERPLUGIN_H
#define TWISTTRAYICEMAKERPLUGIN_H

#include "TwistTrayIceMaker.h"
#include "TwistTrayIceMakerMotorController.h"
#include "TwistTrayIceMakerMotorControllerValueUpdater.h"
#include "TwistTrayIceMakerPlugin.h"
#include "I_DataModel.h"
#include "ErdResolver.h"
#include "TwistTrayIceMakerMotorRequestManager.h"
#include "TwistTrayIceMakerRunner.h"
#include "TwistTrayIceMakerMotorSwitchMonitor.h"
#include "FillTubeHeaterVotingFrameworkPlugin.h"
#include "SensorFiltering.h"
#include "ResolvedVoteRelayConnector.h"
#include "OverrideArbiter.h"
#include "IceMakerWaterFillMonitor.h"
#include "HarvestCountCalculator.h"
#include "IceMakerLocation.h"
#include "FreezerIceRateHandler.h"
#include "TwistTrayIceMakerData.h"
#include "SensorData.h"
#include "NonHarvestFillTubeHeaterData.h"
#include "IceMakerEnableResolver.h"
#include "IceCabinetFanHarvestFixVoting.h"

typedef struct
{
   const ErdLogicServiceConfiguration_t *iceMakerEnableResolverConfig;
   const TwistTrayIceMakerMotorRequestManagerConfig_t *motorRequestManagerConfig;
   const TwistTrayIceMakerMotorSwitchMonitorConfig_t *motorSwitchMonitorConfig;
   const FreezerIceRateHandlerConfig_t *freezerIceRateHandlerConfig;
   const SensorFilteringConfig_t *sensorFilterConfig;
   const OverrideArbiterConfiguration_t *thermistorValidArbiterConfig;
   const ErdResolverConfiguration_t *motorResolverConfiguration;
   const IceMakerWaterFillMonitorConfig_t *fillMonitorConfig;
   const ErdResolverConfiguration_t *fillTubeHeaterResolverConfig;
   const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t *fillTubeHeaterDutyCycleToPercentageCalculatorConfig;
   const NonHarvestFillTubeHeaterControlConfig_t *nonHarvestFillTubeHeaterControlConfig;
   const ErdResolverConfiguration_t *waterValveResolverConfig;
   const ResolvedVoteRelayConnectorConfiguration_t *waterValveRelayConnectorConfig;
   const TwistTrayIceMakerMotorControllerConfig_t *motorControllerConfig;
   const HarvestCountCalculatorConfiguration_t *harvestCountCalculatorConfig;
   const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t *motorControllerValueUpdaterConfig;
   const OverrideArbiterConfiguration_t *filteredTemperatureArbiterConfig;
   const IceCabinetFanHarvestFixVotingConfig_t *iceCabinetFanHarvestFixVotingConfig;
   const TwistTrayIceMakerConfiguration_t *twistTrayIceMakerConfig;
   Erd_t twistTrayMotorOutputErd;
} TwistTrayIceMakerPlugConfig_t;

typedef struct
{
   struct
   {
      IceMakerEnableResolver_t iceMakerEnableResolver;
      SensorFiltering_t sensorFilter;
      TwistTrayIceMaker_t twistTrayIceMaker;
      TwistTrayIceMakerMotorController_t twistTrayMotorController;
      TwistTrayIceMakerMotorControllerValueUpdater_t twistTrayMotorControllerValueUpdater;
      TwistTrayIceMakerRunner_t twistTrayIceMakerMotorControllerRunner;
      TwistTrayIceMakerMotorSwitchMonitor_t twistTrayIceMakerSwitchMonitor;
      TwistTrayIceMakerMotorRequestManager_t twistTrayMotorRequestManager;
      FillTubeHeaterVotingFrameworkPlugin_t fillTubeHeaterVotingFrameworkPlugin;
      ErdResolver_t twistTrayMotorVoteResolver;
      ErdResolver_t iceMakerWaterValveVoteResolver;
      ErdResolver_t fillTubeHeaterVoteResolver;
      ResolvedVoteRelayConnector_t iceMakerWaterValveRelayConnector;
      OverrideArbiter_t twistTrayIceMakerThermistorIsValidOverrideArbiter;
      OverrideArbiter_t twistTrayIceMakerFilteredTemperatureOverrideArbiter;
      IceMakerWaterFillMonitor_t twistTrayIceMakerFillMonitor;
      HarvestCountCalculator_t harvestCountCalculator;
      FreezerIceRateHandler_t iceRateHandler;
      IceCabinetFanHarvestFixVoting_t iceCabinetFanHarvestFixVoting;
   } _private;
} TwistTrayIceMakerPlugin_t;

void TwistTrayIceMakerPlugin_Init(
   TwistTrayIceMakerPlugin_t *instance,
   I_DataModel_t *dataModel,
   IceMakerLocation_t location,
   const TwistTrayIceMakerData_t *twistTrayData,
   const SensorDataSensorType_t *sensorData,
   const uint16_t *sensorPeriodicUpdateRateInMs,
   const FreezerIceRateData_t *freezerIceRateData,
   const NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData,
   const TwistTrayIceMakerPlugConfig_t *config);

#endif
