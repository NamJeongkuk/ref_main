/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SystemErds.h"
#include "AluminumMoldIceMakerPlugin.h"

void AluminumMoldIceMakerPlugin_Init(AluminumMoldIceMakerPlugin_t *instance, I_DataModel_t *dataModel)
{
   FillTubeHeaterVotingFrameworkPlugin_Init(&instance->_private.fillTubeHeaterVotingFrameworkPlugin, dataModel);
}
