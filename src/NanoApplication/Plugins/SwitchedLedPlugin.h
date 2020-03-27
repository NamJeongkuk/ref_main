/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SWITCHEDLEDPLUGIN_H
#define SWITCHEDLEDPLUGIN_H

#include "I_TinyDataSource.h"
#include "TinyEventSubscription.h"

typedef struct
{
   Erd_t ledStateErd;
   Erd_t buttonStateErd;
} SwitchedLedPluginConfiguration_t;

typedef struct
{
   struct
   {
      I_TinyDataSource_t *dataSource;
      const SwitchedLedPluginConfiguration_t *configuration;
      TinyEventSubscription_t dataSourceOnChangeSubscription;
      Erd_t ledStateErd;
   } _private;
} SwitchedLedPlugin_t;

/*!
 * @param instance
 * @param dataSource
 * @param configuration
 */
void SwitchedLedPlugin_Init(
   SwitchedLedPlugin_t *instance,
   I_TinyDataSource_t *dataSource,
   const SwitchedLedPluginConfiguration_t *configuration);

#endif
