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
#include "IceMakerMoldHeaterController.h"
#include "SensorFiltering.h"

typedef struct
{
   struct
   {
      FillTubeHeaterVotingFrameworkPlugin_t fillTubeHeaterVotingFrameworkPlugin;
      FreezerIceRateHandler_t iceRateHandler;
      FeelerArmMonitor_t feelerArmMonitor;
      RakeController_t rakeController;
      HarvestCountCalculator_t harvestCountCalculator;
      AluminumMoldIceMaker_t aluminumMoldIceMaker;
      IceMakerEnableResolver_t iceMakerEnableResolver;
      IceMakerMoldHeaterController_t iceMakerMoldHeaterController;

      ErdResolver_t iceMakerWaterValveVoteResolver;
      ErdResolver_t iceMakerHeaterVoteResolver;
      ErdResolver_t iceMakerMotorVoteResolver;
      SensorFiltering_t moldThermistor;
      ErdResolver_t iceMakerRakeMotorVoteResolver;
   } _private;
} AluminumMoldIceMakerPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void AluminumMoldIceMakerPlugin_Init(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel);

#endif
