/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_Gpio.h"
}

#include "DataSource_NonMockTestDouble.h"
#include "GpioGroup_TestDouble.h"

#define EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS(name, direction, offToOnDebounceCount, onToOffDebounceCount, pullUp, driveCapacity, port, pin, inverted) \
   {                                                                                                                                                         \
      .erd = name, .sizeOfEntity = sizeof(bool)                                                                                                              \
   },

static struct
{
   DataSource_NonMockTestDouble_t dataSourceNonMockTestDouble;
   GpioGroup_TestDouble_t gpioGroup;
   bool inputStates[UINT16_MAX];
   bool outputStates[UINT16_MAX];
} instance;

static const DataSource_NonMockTestDoubleItem_t gpioItems[] = {
   GPIO_TABLE(EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS)
   //
   { .erd = Erd_GpioGroupInterface, .sizeOfEntity = sizeof(I_GpioGroup_t *) },
   { .erd = Erd_Gpio_SABBATH, .sizeOfEntity = sizeof(bool) }
};

extern "C" I_DataSource_t *DataSource_Gpio_Init(
   Event_Synchronous_t *onChangeEvent,
   I_Interrupt_t *debounceInterrupt)
{
   (void)debounceInterrupt;

   DataSource_NonMockTestDouble_Init(
      &instance.dataSourceNonMockTestDouble,
      gpioItems,
      NUM_ELEMENTS(gpioItems));

   static EventSubscription_t subscription;
   EventSubscription_Init(
      &subscription, onChangeEvent, +[](void *context, const void *args) {
         Event_Synchronous_Publish(reinterpret_cast<Event_Synchronous_t *>(context), args);
      });
   DataSource_SubscribeAll(&instance.dataSourceNonMockTestDouble.interface, &subscription);

   GpioGroup_TestDouble_Init(
      &instance.gpioGroup,
      instance.inputStates,
      instance.outputStates);

   DataSource_WritePointer(
      &instance.dataSourceNonMockTestDouble.interface,
      Erd_GpioGroupInterface,
      &instance.gpioGroup.interface);

   return &instance.dataSourceNonMockTestDouble.interface;
}
