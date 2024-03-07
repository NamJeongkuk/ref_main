/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_SoftPwmGpio.h"
}

#include "DataSource_NonMockTestDouble.h"

#define EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS(name, pullUp, driveCapacity, port, pin, inverted) \
   {                                                                                                  \
      .erd = name, .sizeOfEntity = sizeof(bool)                                                       \
   },

static struct
{
   DataSource_NonMockTestDouble_t dataSourceNonMockTestDouble;
} instance;

static const DataSource_NonMockTestDoubleItem_t softPwmGpioItems[] = {
   SOFTPWMGPIO_TABLE(EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS)
};

extern "C" I_DataSource_t *DataSource_SoftPwmGpio_Init(Event_Synchronous_t *onChangeEvent)
{
   (void)onChangeEvent;

   DataSource_NonMockTestDouble_Init(
      &instance.dataSourceNonMockTestDouble,
      softPwmGpioItems,
      NUM_ELEMENTS(softPwmGpioItems));

   return &instance.dataSourceNonMockTestDouble.interface;
}
