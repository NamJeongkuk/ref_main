/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "iodefine.h"
#include "DataSource_Gpio.h"
#include "SystemErds.h"
#include "I_GpioGroup.h"
#include "uassert.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspGpio_Start - 1)
#define ERD_FROM_CHANNEL(channel) (Erd_BspGpio_Start + 1 + channel)
#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End - 1))

#define GPIO_TABLE_EXPAND_AS_GPIO_COUNT(name, direction, port, pin) +1

enum
{
   InputPollPeriodInMsec = 10,
   BitsPerByte = 8,
   GpioCount = 0 GPIO_TABLE(GPIO_TABLE_EXPAND_AS_GPIO_COUNT)
};

enum
{
   Port1,
   Port2,
   Port3,
   Port4,
   Port5,
   Port6,
   Port7,
   Port8,
   Port9,
   PortA,
   PortB,
   PortD,
   PortE,
   PortG
};

typedef struct
{
   uint8_t *ddr;
   uint8_t *dr;
   uint8_t *port;
} GpioPortAddresses_t;

#define GPIO_PORT_ADDRESS_ENTRY(__port) \
   (uint8_t *)&__port.DDR, (uint8_t *)&__port.DR, (uint8_t *)&__port.PORT

#define INPUT_ONLY_GPIO_PORT_ADDRESS_ENTRY(__port) \
   (uint8_t *)NULL, (uint8_t *)NULL, (uint8_t *)&__port.PORT

static const GpioPortAddresses_t gpioPortAddresses[] = {
   { GPIO_PORT_ADDRESS_ENTRY(PORT1) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT2) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT3) },
   { INPUT_ONLY_GPIO_PORT_ADDRESS_ENTRY(PORT4) },
   { INPUT_ONLY_GPIO_PORT_ADDRESS_ENTRY(PORT5) },
   { INPUT_ONLY_GPIO_PORT_ADDRESS_ENTRY(PORT6) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT7) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT8) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT9) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTA) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTB) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTD) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTE) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTG) }
};

typedef struct
{
   GpioDirection_t direction;
   uint8_t port;
   uint8_t pin;
} GpioDirectionPortsAndPins_t;

#define EXPAND_PORTS_AND_PINS(channel, direction, port, pin) \
   { direction, port, pin },

static const GpioDirectionPortsAndPins_t gpioPortsAndPins[] = {
   GPIO_TABLE(EXPAND_PORTS_AND_PINS)
};

static struct
{
   I_DataSource_t interface;
   Event_Synchronous_t *onChangeEvent;
   Timer_t timer;
   uint8_t inputCache[((GpioCount - 1) / BitsPerByte) + 1];
} instance;

static void SetDirection(const GpioChannel_t channel, const GpioDirection_t direction)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].ddr)
   {
      uint8_t directionRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].ddr;
      BIT_WRITE(directionRegister, gpioPortsAndPins[channel].pin, direction);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].ddr = directionRegister;
   }
}

static bool ReadGpio(const GpioChannel_t channel)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].port)
   {
      uint8_t inputRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].port;
      return BIT_STATE(inputRegister, gpioPortsAndPins[channel].pin);
   }

   return false;
}

static void WriteGpio(const GpioChannel_t channel, const bool state)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].dr)
   {
      uint8_t outputRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].dr;
      BIT_WRITE(outputRegister, gpioPortsAndPins[channel].pin, state);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].dr = outputRegister;
   }
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   bool value = ReadGpio(CHANNEL_FROM_ERD(erd));
   memcpy(data, &value, sizeof(bool));
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

      DataSourceOnDataChangeArgs_t args = { erd, data };
      Event_Synchronous_Publish(instance.onChangeEvent, &args);
   }
}

static bool Has(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   return sizeof(bool);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

static void PollInputs(void *context)
{
   IGNORE(context);

   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioPortsAndPins); channel++)
   {
      if(gpioPortsAndPins[channel].direction == GpioDirection_Input)
      {
         bool state = ReadGpio(channel);
         if(BIT_STATE(instance.inputCache[channel / BitsPerByte], channel % BitsPerByte) != state)
         {
            BIT_WRITE(instance.inputCache[channel / BitsPerByte], channel % BitsPerByte, state);

            DataSourceOnDataChangeArgs_t args = { ERD_FROM_CHANNEL(channel), &state };
            Event_Synchronous_Publish(instance.onChangeEvent, &args);
         }
      }
   }
}

static void InitializeGpio(void)
{
   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioPortsAndPins); channel++)
   {
      SetDirection(channel, gpioPortsAndPins[channel].direction);
   }
}

static bool AtLeastOneInputExists(void)
{
   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioPortsAndPins); channel++)
   {
      if(gpioPortsAndPins[channel].direction == GpioDirection_Input)
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
