/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_Gpio.h"
#include "SystemErds.h"
#include "uassert.h"
#include "stm32f3xx.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspGpio_Start - 1)
#define ERD_FROM_CHANNEL(channel) (Erd_BspGpio_Start + 1 + channel)
#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End))

enum
{
   InputPollPeriodInMsec = 10
};

#define EXPAND_AS_INIT(name, mode, pullUp, speed, port, pin)   \
   {                                                           \
      GPIO_InitTypeDef gpioInitStruct;                         \
      if(mode != GPIO_MODE_INPUT)                              \
      {                                                        \
         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); \
      }                                                        \
      gpioInitStruct.Pin = pin;                                \
      gpioInitStruct.Mode = mode;                              \
      gpioInitStruct.Pull = pullUp;                            \
      gpioInitStruct.Speed = speed;                            \
      HAL_GPIO_Init(port, &gpioInitStruct);                    \
   }

#define EXPAND_AS_MODES_PORTS_AND_PINS(name, mode, pullUp, speed, port, pin) \
   {                                                                         \
      mode, port, pin                                                        \
   },

typedef struct
{
   uint8_t mode;
   void *port;
   uint16_t pin;
} GpioModePortAndPin_t;

static const GpioModePortAndPin_t gpioModesPortsAndPins[] =
   {
      GPIO_TABLE(EXPAND_AS_MODES_PORTS_AND_PINS)
   };

static bool ReadGpio(const GpioChannel_t channel)
{
   return HAL_GPIO_ReadPin(gpioModesPortsAndPins[channel].port, gpioModesPortsAndPins[channel].pin);
}

static void WriteGpio(const GpioChannel_t channel, const bool state)
{
   if(gpioModesPortsAndPins[channel].mode != GPIO_MODE_INPUT)
   {
      HAL_GPIO_WritePin(gpioModesPortsAndPins[channel].port, gpioModesPortsAndPins[channel].pin, state);
   }
}

static void Read(I_DataSource_t *instance, const Erd_t erd, void *data)
{
   IGNORE(instance);
   uassert(ERD_IS_IN_RANGE(erd));

   bool value = ReadGpio(CHANNEL_FROM_ERD(erd));
   memcpy(data, &value, sizeof(bool));
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   REINTERPRET(instance, _instance, DataSource_Gpio_t *);
   REINTERPRET(state, data, const bool *);

   uassert(ERD_IS_IN_RANGE(erd));

   GpioChannel_t channel = CHANNEL_FROM_ERD(erd);
   if(ReadGpio(channel) != *state)
   {
      WriteGpio(channel, *state);

      DataSourceOnDataChangeArgs_t args =
         { erd, data };
      Event_Synchronous_Publish(instance->_private.onChangeEvent, &args);
   }
}

static bool Has(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   uassert(ERD_IS_IN_RANGE(erd));

   return sizeof(bool);
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

static void PollInputs(void *context)
{
   REINTERPRET(instance, context, DataSource_Gpio_t *);

   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioModesPortsAndPins); channel++)
   {
      if(gpioModesPortsAndPins[channel].mode == GPIO_MODE_INPUT)
      {
         bool state = ReadGpio(channel);
         if(BIT_STATE(instance->_private.inputCache[channel / BitsPerByte], channel % BitsPerByte) != state)
         {
            BIT_WRITE(instance->_private.inputCache[channel / BitsPerByte], channel % BitsPerByte, state);

            DataSourceOnDataChangeArgs_t args =
               { ERD_FROM_CHANNEL(channel), &state };
            Event_Synchronous_Publish(instance->_private.onChangeEvent, &args);
         }
      }
   }
}

static void InitializeGpio(void)
{
   GPIO_TABLE(EXPAND_AS_INIT)
}

static bool AtLeastOneInputExists(void)
{
   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioModesPortsAndPins); channel++)
   {
      if(gpioModesPortsAndPins[channel].mode == GPIO_MODE_INPUT)
      {
         return true;
      }
   }
   return false;
}

void DataSource_Gpio_Init(
   DataSource_Gpio_t *instance,
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent)
{
   instance->interface.api = &api;
   instance->interface.OnDataChange = &onChangeEvent->interface;
   instance->_private.onChangeEvent = onChangeEvent;

   InitializeGpio();

   if(AtLeastOneInputExists())
   {
      TimerModule_StartPeriodic(
         timerModule,
         &instance->_private.timer,
         InputPollPeriodInMsec,
         PollInputs,
         instance);
   }
}
