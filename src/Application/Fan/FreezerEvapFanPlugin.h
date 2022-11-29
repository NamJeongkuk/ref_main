/*!
 * @file
 * @brief plugin for Freezer Evap Fan
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZEREVAPFANPLUGIN_H
#define FREEZEREVAPFANPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "FanSpeedResolver.h"
#include "FanController.h"

typedef struct
{
   struct
   {
      ErdResolver_t fanSpeedVoteResolver;
      FanSpeedResolver_t fanSpeedResolver;
      FanController_t fanController;
   } _private;
} FreezerEvapFanPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void FreezerEvapFanPlugin_Init(
   FreezerEvapFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif