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
#include "IceRateHandler.h"
#include "SoftPwm.h"

typedef struct
{
   struct
   {
      FillTubeHeaterVotingFrameworkPlugin_t fillTubeHeaterVotingFrameworkPlugin;
      IceRateHandler_t iceRatehandler;
      SoftPwm_t fillTubeHeaterSoftPwm;
   } _private;
} AluminumMoldIceMakerPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void AluminumMoldIceMakerPlugin_Init(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel);

#endif
