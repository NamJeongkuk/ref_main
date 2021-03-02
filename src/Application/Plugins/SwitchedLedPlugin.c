/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SwitchedLedPlugin.h"
#include "SystemErds.h"
#include "utils.h"
#include "Constants_Binary.h"

static void ButtonStateChanged(void *context, const void *args)
{
   REINTERPRET(instance, context, SwitchedLedPlugin_t *);
   REINTERPRET(state, args, const bool *);

   bool newState = !(*state);
   DataModel_Write(instance->_private.dataModel, Erd_OtherLed, &newState);
}

void SwitchedLedPlugin_Init(
   SwitchedLedPlugin_t *instance,
   I_DataModel_t *dataModel,
   Erd_t erd)
{
   instance->_private.dataModel = dataModel;

   EventSubscription_Init(&instance->_private.buttonPressSubscription, instance, ButtonStateChanged);
   DataModel_Subscribe(dataModel, erd, &instance->_private.buttonPressSubscription);

   bool initialState = ON;
   DataModel_Write(instance->_private.dataModel, Erd_OtherLed, &initialState);
}
