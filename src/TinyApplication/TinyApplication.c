/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TinyApplication.h"
#include "TinySystemErds.h"

static const SwitchedLedPluginConfiguration_t ledPluginConfiguration =
   {
      .ledStateErd = Erd_LedState,
      .buttonStateErd = Erd_ButtonState
   };

void TinyApplication_Init(TinyApplication_t *instance, I_TinyDataSource_t *dataSource)
{
   SwitchedLedPlugin_Init(&instance->_private.switchedLedPlugin, dataSource, &ledPluginConfiguration);
}
