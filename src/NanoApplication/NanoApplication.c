/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "NanoApplication.h"
#include "NanoSystemErds.h"

static const SwitchedLedPluginConfiguration_t ledPluginConfiguration =
   {
      .ledStateErd = Erd_LedState,
      .buttonStateErd = Erd_ButtonState
   };

void NanoApplication_Init(NanoApplication_t *instance, I_TinyDataSource_t *dataSource)
{
   SwitchedLedPlugin_Init(&instance->_private.switchedLedPlugin, dataSource, &ledPluginConfiguration);
}
