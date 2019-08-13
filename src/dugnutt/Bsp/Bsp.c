/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Bsp.h"
#include "SystemErds.h"
#include "uassert.h"
#include <string.h>

static void AddDataSourceToComposite(
   Bsp_t *instance,
   I_DataSource_t *dataSource,
   DataSource_CompositeComponent_t *component)
{
   DataSource_Composite_InitComponent(component, dataSource);
   DataSource_Composite_Add(&instance->_private.dataSource.composite, component);
}

void Bsp_Init(
   Bsp_t *instance,
   TimerModule_t *timerModule)
{
   DataSource_Composite_Init(
      &instance->_private.dataSource.composite);

   DataSource_Gpio_Init(
      &instance->_private.dataSource.gpio,
      timerModule);
   AddDataSourceToComposite(
      instance,
      &instance->_private.dataSource.gpio.interface,
      &instance->_private.dataSource.gpioComponent);
}

I_DataSource_t *Bsp_GetDataSource(Bsp_t *instance)
{
   return &instance->_private.dataSource.composite.interface;
}
