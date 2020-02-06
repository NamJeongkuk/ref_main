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
#include "Event_Synchronous.h"
#include "XMacroUtils.h"
#include "utils.h"
#include "stm32f3xx.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspGpio_Start - 1)
#define ERD_FROM_CHANNEL(channel) (Erd_BspGpio_Start + 1 + channel)
#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End - 1))

#define GPIO_TABLE_EXPAND_AS_GPIO_COUNT(name, direction, pullUp, driveCapacity, port, pin) +1

enum
{
   BitsPerByte = 8,
   GpioCount = 0 GPIO_TABLE(GPIO_TABLE_EXPAND_AS_GPIO_COUNT)
};

static struct
{
   I_DataSource_t interface;
   Event_Synchronous_t *onChangeEvent;
   Timer_t timer;
   uint8_t inputCache[((GpioCount - 1) / BitsPerByte) + 1];
} instance;

enum
{
   InputPollPeriodInMsec = 10
};

#define EXPAND_AS_CLOCK_INIT(name, mode, pullUp, speed, port, pin) \
   {                                                               \
      __HAL_RCC_##port##_CLK_ENABLE();                             \
   }

#define EXPAND_AS_MODES_PORTS_AND_PINS(name, mode, pullUp, speed, port, pin) \
   {                                                                         \
      port, pin, mode, pullUp, speed                                         \
   },

typedef struct
{
   void *port;
   uint16_t pin;
   uint8_t mode;
   uint8_t pullUp;
   uint8_t speed;
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
   memcpy(data, &value, sizeof(value));
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   REINTERPRET(state, data, const bool *);

   uassert(ERD_IS_IN_RANGE(erd));

   GpioChannel_t channel = CHANNEL_FROM_ERD(erd);
   if(ReadGpio(channel) != *state)
   {
      WriteGpio(channel, *state);

      DataSourceOnDataChangeArgs_t args =
         { erd, data };
      Event_Synchronous_Publish(instance.onChangeEvent, &args);
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
   IGNORE(context);

   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioModesPortsAndPins); channel++)
   {
      if(gpioModesPortsAndPins[channel].mode == GPIO_MODE_INPUT)
      {
         bool state = ReadGpio(channel);
         if(BIT_STATE(instance.inputCache[channel / BitsPerByte], channel % BitsPerByte) != state)
         {
            BIT_WRITE(instance.inputCache[channel / BitsPerByte], channel % BitsPerByte, state);

            DataSourceOnDataChangeArgs_t args =
               { ERD_FROM_CHANNEL(channel), &state };
            Event_Synchronous_Publish(instance.onChangeEvent, &args);
         }
      }
   }
}

static void InitializeGpio(void)
{
   GPIO_TABLE(EXPAND_AS_CLOCK_INIT)

   GPIO_InitTypeDef gpioInitStruct;

   for(uint8_t i = 0; i < NUM_ELEMENTS(gpioModesPortsAndPins); i++)
   {
      if(gpioModesPortsAndPins[i].mode != GPIO_MODE_INPUT)
      {
         HAL_GPIO_WritePin(gpioModesPortsAndPins[i].port, gpioModesPortsAndPins[i].pin, GPIO_PIN_RESET);
      }

      gpioInitStruct.Pin = gpioModesPortsAndPins[i].pin;
      gpioInitStruct.Mode = gpioModesPortsAndPins[i].mode;
      gpioInitStruct.Pull = gpioModesPortsAndPins[i].pullUp;
      gpioInitStruct.Speed = gpioModesPortsAndPins[i].speed;
      HAL_GPIO_Init(gpioModesPortsAndPins[i].port, &gpioInitStruct);
   }
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

I_DataSource_t *DataSource_Gpio_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent)
{
   instance.interface.api = &api;
   instance.interface.OnDataChange = &onChangeEvent->interface;
   instance.onChangeEvent = onChangeEvent;

   InitializeGpio();

   if(AtLeastOneInputExists())
   {
      TimerModule_StartPeriodic(
         timerModule,
         &instance.timer,
         InputPollPeriodInMsec,
         PollInputs,
         &instance);
   }

   return &instance.interface;
}
