/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SwitchedLedPlugin.h"
#include "SystemErds.h"
#include "utils.h"

static void ButtonStateChanged(void *context, const void *args)
{
   REINTERPRET(instance, context, SwitchedLedPlugin_t *);
   REINTERPRET(state, args, const bool *);

   bool newState = !(*state);
   DataModel_Write(instance->_private.dataModel, Erd_OtherLed, &newState);
}

void SwitchedLedPlugin_Init(
   SwitchedLedPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   instance->_private.dataModel = dataModel;

   EventSubscription_Init(&instance->_private.buttonPressSubscription, instance, ButtonStateChanged);

   Event_Subscribe(Input_GetOnChangeEvent(DataModel_GetInput(dataModel, Erd_PushButtonSwitch)), &instance->_private.buttonPressSubscription);
}
