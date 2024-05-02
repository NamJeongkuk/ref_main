/*!
 * @file
 * @brief plugin for Fresh Food Evap Fan
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODEVAPFANPLUGIN_H
#define FRESHFOODEVAPFANPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "FanSpeedResolver.h"
#include "FanController.h"
#include "FanFaultHandler.h"
#include "SabbathDelayHandler.h"

typedef struct
{
   struct
   {
      ErdResolver_t fanSpeedVoteResolver;
      FanSpeedResolver_t fanSpeedResolver;
      FanController_t fanController;
      FanFaultHandler_t fanFaultHandler;
      SabbathDelayHandler_t sabbathDelayHandler;
   } _private;
} FreshFoodEvapFanPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void FreshFoodEvapFanPlugin_Init(
   FreshFoodEvapFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
