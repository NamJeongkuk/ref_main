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
#include "Output_TwistTrayIceMakerMotorState.h"
#include "TwistTrayIceMakerRunner.h"
#include "TwistTrayIceMakerMotorSwitchMonitor.h"
#include "FillTubeHeaterVotingFrameworkPlugin.h"
#include "SensorFiltering.h"
#include "ResolvedVoteRelayConnector.h"
#include "OverrideArbiter.h"
#include "IceMakerWaterFillMonitor.h"
#include "HarvestCountCalculator.h"
#include "IceMakerLocation.h"
#include "IceMakerMotorAction.h"
#include "FreezerIceRateHandler.h"
#include "TwistTrayIceMakerData.h"
#include "SensorData.h"
#include "NonHarvestFillTubeHeaterData.h"

typedef struct
{
   const Output_TwistTrayIceMakerMotorStateConfig_t *outputMotorStateConfig;
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
   const TwistTrayIceMakerConfiguration_t *twistTrayIceMakerConfig;
} TwistTrayIceMakerPlugConfig_t;

typedef struct
{
   struct
   {
      SensorFiltering_t sensorFilter;
      TwistTrayIceMaker_t twistTrayIceMaker;
      Output_TwistTrayIceMakerMotorState_t motorStateOutput;
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
