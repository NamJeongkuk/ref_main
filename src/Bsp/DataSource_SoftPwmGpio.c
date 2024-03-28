/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_SoftPwmGpio.h"
#include "iodefine.h"
#include "I_GpioGroup.h"
#include "utils.h"
#include "uassert.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspSoftPwmGpio_Start - 1)
#define ERD_FROM_CHANNEL(channel) (Erd_BspSoftPwmGpio_Start + 1 + channel)
#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspSoftPwmGpio_Start + 1, erd, Erd_BspSoftPwmGpio_End - 1))

#define SOFTPWMGPIO_TABLE_EXPAND_AS_SOFTPWMGPIO_COUNT(name, pullUp, driveCapacity, port, pin, inverted) +1

enum
{
   SoftPwmGpioCount = 0 SOFTPWMGPIO_TABLE(SOFTPWMGPIO_TABLE_EXPAND_AS_SOFTPWMGPIO_COUNT)
};

enum
{
   Port0,
   Port1,
   Port2,
   Port3,
   Port4,
   Port5,
   PortA,
   PortB,
   PortC,
   PortD,
   PortE,
   PortH,
   PortJ
};

typedef enum
{
   GpioPullUp_Disable = 0,
   GpioPullUp_Enable,
   GpioPullUp_Max
} GpioPullUp_t;

typedef enum
{
   GpioDriveCapacity_Normal = 0,
   GpioDriveCapacity_High,
   GpioDriveCapacity_Max
} GpioDriveCapacity_t;

typedef struct
{
   uint8_t *outputData;
   uint8_t *inputData;
   uint8_t *direction;
   uint8_t *mode;
   uint8_t *pullUpControl;
   uint8_t *driveCapacity;
} GpioPortAddresses_t;

#define GPIO_PORT_ADDRESS_ENTRY(__port) \
   (uint8_t *)&__port.PODR, (uint8_t *)&__port.PIDR, (uint8_t *)&__port.PDR, (uint8_t *)&__port.PMR, (uint8_t *)&__port.PCR, (uint8_t *)&__port.DSCR

#define GPIO_PORT_ADDRESS_ENTRY_WITHOUT_DSCR(__port) \
   (uint8_t *)&__port.PODR, (uint8_t *)&__port.PIDR, (uint8_t *)&__port.PDR, (uint8_t *)&__port.PMR, (uint8_t *)&__port.PCR, (uint8_t *)NULL

static const GpioPortAddresses_t gpioPortAddresses[] = {
   { GPIO_PORT_ADDRESS_ENTRY_WITHOUT_DSCR(PORT0) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT1) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT2) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT3) },
   { GPIO_PORT_ADDRESS_ENTRY_WITHOUT_DSCR(PORT4) },
   { GPIO_PORT_ADDRESS_ENTRY(PORT5) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTA) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTB) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTC) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTD) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTE) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTH) },
   { GPIO_PORT_ADDRESS_ENTRY(PORTJ) }
};

typedef struct
{
   GpioDirection_t direction;
   GpioPullUp_t pullUp;
   GpioDriveCapacity_t driveCapacity;
   uint8_t port;
   uint8_t pin;
   bool inverted;
} GpioDirectionPortsAndPins_t;

#define EXPAND_PORTS_AND_PINS(channel, pullUp, driveCapacity, port, pin, inverted) { GPIO_DIRECTION_OUTPUT, pullUp, driveCapacity, port, pin, inverted },

static const GpioDirectionPortsAndPins_t gpioPortsAndPins[] = {
   SOFTPWMGPIO_TABLE(EXPAND_PORTS_AND_PINS)
};

static struct
{
   I_DataSource_t interface;
   Event_Synchronous_t *onChangeEvent;
} instance;

static void SetPullUp(const GpioChannel_t channel, const GpioPullUp_t pullUp)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].pullUpControl)
   {
      uint8_t pullUpRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].pullUpControl;
      BIT_WRITE(pullUpRegister, gpioPortsAndPins[channel].pin, pullUp);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].pullUpControl = pullUpRegister;
   }
}

static void SetGpioMode(const GpioChannel_t channel)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].mode)
   {
      uint8_t modeRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].mode;
      BIT_CLEAR(modeRegister, gpioPortsAndPins[channel].pin);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].mode = modeRegister;
   }
}

static void SetDriveCapacity(const GpioChannel_t channel, const GpioDriveCapacity_t driveCapacitySetting)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].driveCapacity)
   {
      uint8_t driveCapacityRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].driveCapacity;
      BIT_WRITE(driveCapacityRegister, gpioPortsAndPins[channel].pin, driveCapacitySetting);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].driveCapacity = driveCapacityRegister;
   }
}

static void SetDirection(const GpioChannel_t channel, const GpioDirection_t direction)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].direction)
   {
      uint8_t directionRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].direction;
      BIT_WRITE(directionRegister, gpioPortsAndPins[channel].pin, direction);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].direction = directionRegister;
   }
}

static bool ReadGpio(const GpioChannel_t channel)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].inputData)
   {
      uint8_t inputRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].inputData;
      bool pinValue = BIT_STATE(inputRegister, gpioPortsAndPins[channel].pin);
      if(gpioPortsAndPins[channel].inverted)
      {
         pinValue = !pinValue;
      }
      return pinValue;
   }
   return false;
}

static void WriteGpio(const GpioChannel_t channel, bool state)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].outputData)
   {
      uint8_t outputRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].outputData;
      if(gpioPortsAndPins[channel].inverted)
      {
         state = !state;
      }
      BIT_WRITE(outputRegister, gpioPortsAndPins[channel].pin, state);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].outputData = outputRegister;
   }
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   bool value;
   GpioChannel_t channel = CHANNEL_FROM_ERD(erd);
   value = ReadGpio(channel);
   memcpy(data, &value, sizeof(bool));
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   const bool *state = data;

   uassert(ERD_IS_IN_RANGE(erd));

   GpioChannel_t channel = CHANNEL_FROM_ERD(erd);
   if(ReadGpio(channel) != *state)
   {
      WriteGpio(channel, *state);

      DataSourceOnDataChangeArgs_t args = { erd, data };
      Event_Synchronous_Publish(instance.onChangeEvent, &args);
   }
}

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return (ERD_IS_IN_RANGE(erd));
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   return sizeof(bool);
}

static void InitializeGpio(void)
{
   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioPortsAndPins); channel++)
   {
      SetPullUp(channel, gpioPortsAndPins[channel].pullUp);
      SetDirection(channel, gpioPortsAndPins[channel].direction);
      SetGpioMode(channel);
      SetDriveCapacity(channel, gpioPortsAndPins[channel].driveCapacity);
   }
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_SoftPwmGpio_Init(Event_Synchronous_t *onChangeEvent)
{
   instance.interface.api = &api;
   instance.interface.OnDataChange = &onChangeEvent->interface;
   instance.onChangeEvent = onChangeEvent;

   InitializeGpio();

   return &instance.interface;
}
