/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETFANPLUGIN_H
#define ICECABINETFANPLUGIN_H

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
} IceCabinetFanPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void IceCabinetFanPlugin_Init(
   IceCabinetFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
