/*!
 * @file
 * @brief plugin for Condenser Fan
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONDENSERFANPLUGIN_H
#define CONDENSERFANPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "FanSpeedResolver.h"
#include "FanController.h"
#include "FanFaultHandler.h"
#include "CondenserFanAntiSweatBehavior.h"

typedef struct
{
   struct
   {
      ErdResolver_t fanSpeedVoteResolver;
      FanSpeedResolver_t fanSpeedResolver;
      FanController_t fanController;
      FanFaultHandler_t fanFaultHandler;
      CondenserFanAntiSweatBehavior_t condenserFanAntiSweatBehavior;
   } _private;
} CondenserFanPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void CondenserFanPlugin_Init(
   CondenserFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
