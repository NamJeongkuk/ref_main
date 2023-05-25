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
#include "HarvestCountCalculator.h"

typedef struct
{
   struct
   {
      SensorFiltering_t sensorFilter;
      TwistTrayIceMaker_t twistTrayIceMaker;
      Output_TwistTrayIceMakerMotorState_t motorStateOutput;
      TwistTrayIceMakerMotorController_t twistTrayMotorController;
      TwistTrayIceMakerRunner_t twistTrayIceMakerMotorControllerRunner;
      TwistTrayIceMakerMotorSwitchMonitor_t twistTrayIceMakerSwitchMonitor;
      TwistTrayIceMakerMotorRequestManager_t twistTrayMotorRequestManager;
      FillTubeHeaterVotingFrameworkPlugin_t fillTubeHeaterVotingFrameworkPlugin;
      ErdResolver_t twistTrayMotorVoteResolver;
      ErdResolver_t iceMakerWaterValveVoteResolver;
      ResolvedVoteRelayConnector_t iceMakerWaterValveRelayConnector;
      OverrideArbiter_t twistTrayIceMakerThermistorIsValidOverrideArbiter;
      OverrideArbiter_t twistTrayIceMakerFilteredTemperatureOverrideArbiter;
      HarvestCountCalculator_t harvestCountCalculator;
   } _private;
} TwistTrayIceMakerPlugin_t;

void TwistTrayIceMakerPlugin_Init(TwistTrayIceMakerPlugin_t *instance, I_DataModel_t *dataModel);

#endif
