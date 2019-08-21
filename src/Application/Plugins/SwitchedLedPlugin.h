/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SWITCHEDLEDPLUGIN_H
#define SWITCHEDLEDPLUGIN_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t buttonPressSubscription;
   } _private;
} SwitchedLedPlugin_t;

void SwitchedLedPlugin_Init(
   SwitchedLedPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
