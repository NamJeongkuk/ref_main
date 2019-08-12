/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Gpio.h"
#include "SystemErds.h"
#include "uassert.h"
#include "I_GpioGroup.h"
#include "iodefine.h"
#include <string.h>

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

static const GpioPortAddresses_t gpioPortAddresses[] =
   {
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
} GpioDirectionPortsAndPins_t;

#define EXPAND_PORTS_AND_PINS(channel, direction, pullUp, driveCapacity, port, pin) { direction, pullUp, driveCapacity, port, pin },

static const GpioDirectionPortsAndPins_t gpioPortsAndPins[] =
   {
      GPIO_TABLE(EXPAND_PORTS_AND_PINS)
   };

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
      return BIT_STATE(inputRegister, gpioPortsAndPins[channel].pin);
   }

   return false;
}

static void WriteGpio(const GpioChannel_t channel, const bool state)
{
   if(gpioPortAddresses[gpioPortsAndPins[channel].port].outputData)
   {
      uint8_t outputRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].outputData;
      BIT_WRITE(outputRegister, gpioPortsAndPins[channel].pin, state);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].outputData = outputRegister;
   }
}

static void Read(I_DataSource_t *instance, const Erd_t erd, void *data)
{
   IGNORE(instance);
   GpioChannel_t channel = Erd_BspGpio_Start + 1 - erd;

   if(IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End))
   {
      bool value = ReadGpio(channel);
      memcpy(data, &value, sizeof(bool));
   }
   else
   {
      uassert(!"ERD not supported");
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   REINTERPRET(instance, _instance, DataSource_Gpio_t *);

   if(IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End))
   {
      REINTERPRET(state, data, const bool *);

      GpioChannel_t channel = Erd_BspGpio_Start + 1 - erd;
      if(ReadGpio(channel) != *state)
      {
         WriteGpio(channel, *state);

         DataSourceOnDataChangeArgs_t args =
            { erd, data };
         Event_Synchronous_Publish(&instance->_private.OnDataChange, &args);
      }
   }
   else
   {
      uassert(!"ERD not supported");
   }
}

static bool Has(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End);
}

static uint8_t SizeOf(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);

   if(IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End))
   {
      return sizeof(bool);
   }
   else
   {
      uassert(!"ERD not supported");
   }

   return 0;
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

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

void DataSource_Gpio_Init(DataSource_Gpio_t *instance)
{
   instance->interface.api = &api;
   instance->interface.OnDataChange = &instance->_private.OnDataChange.interface;

   Event_Synchronous_Init(&instance->_private.OnDataChange);

   InitializeGpio();
}
