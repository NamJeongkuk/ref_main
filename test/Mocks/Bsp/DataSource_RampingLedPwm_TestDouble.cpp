/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_RampingLedPwm.h"
}

#include "DataSource_NonMockTestDouble.h"

#define EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS(name, pwm, initialValue, portNumber, port, pin, timerNumber) \
   {                                                                                                             \
      .erd = name, .sizeOfEntity = sizeof(RampingPwmDutyCycle_t)                                                 \
   },

static struct
{
   DataSource_NonMockTestDouble_t dataSourceNonMockTestDouble;
} instance;

static const DataSource_NonMockTestDoubleItem_t rampingLedItems[] = {
   RAMPING_LED_PWM_TABLE(EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS)
};

extern "C" I_DataSource_t *DataSource_RampingLedPwm_Init(
   Event_Synchronous_t *onChangeEvent,
   I_Interrupt_t *interrupt)
{
   (void)interrupt;

   DataSource_NonMockTestDouble_Init(
      &instance.dataSourceNonMockTestDouble,
      rampingLedItems,
      NUM_ELEMENTS(rampingLedItems));

   static EventSubscription_t subscription;
   EventSubscription_Init(
      &subscription, onChangeEvent, +[](void *context, const void *args) {
         Event_Synchronous_Publish(reinterpret_cast<Event_Synchronous_t *>(context), args);
      });
   DataSource_SubscribeAll(&instance.dataSourceNonMockTestDouble.interface, &subscription);

   return &instance.dataSourceNonMockTestDouble.interface;
}
