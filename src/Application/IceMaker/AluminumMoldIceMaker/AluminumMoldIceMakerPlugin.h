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

typedef struct
{
   struct
   {
      FillTubeHeaterVotingFrameworkPlugin_t fillTubeHeaterVotingFrameworkPlugin;
      FreezerIceRateHandler_t iceRatehandler;
      SoftPwm_t fillTubeHeaterSoftPwm;
      FeelerArmMonitor_t feelerArmMonitor;
      AluminumMoldIceMaker_t aluminumMoldIceMaker;
   } _private;
} AluminumMoldIceMakerPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void AluminumMoldIceMakerPlugin_Init(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel);

#endif
