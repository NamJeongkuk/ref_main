/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SwitchedLedPlugin.h"
#include "TinySystemErds.h"
#include "utils.h"
#include "Constants_Binary.h"

static void DataSourceChanged(void *context, const void *_args)
{
   REINTERPRET(instance, context, SwitchedLedPlugin_t *);
   REINTERPRET(args, _args, const TinyDataSourceOnDataChangeArgs_t *);

   if(args->erd == instance->_private.configuration->buttonStateErd)
   {
      TinyDataSource_Write(instance->_private.dataSource, instance->_private.configuration->ledStateErd, args->data);
   }
}

void SwitchedLedPlugin_Init(
   SwitchedLedPlugin_t *instance,
   I_TinyDataSource_t *dataSource,
   const SwitchedLedPluginConfiguration_t *configuration)
{
   instance->_private.dataSource = dataSource;
   instance->_private.configuration = configuration;

   TinyEventSubscription_Init(&instance->_private.dataSourceOnChangeSubscription, instance, DataSourceChanged);
   I_TinyEvent_t *onChangeEvent = TinyDataSource_GetOnChangeEvent(dataSource);
   TinyEvent_Subscribe(onChangeEvent, &instance->_private.dataSourceOnChangeSubscription);
}
