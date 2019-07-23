/*!
 * @file
 * @brief Gpio Group
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GpioGroup.h"
#include "iodefine.h"
#include "uassert.h"
#include "utils.h"
#include <stdint.h>

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
   (uint8_t *) &__port.PODR, (uint8_t *) &__port.PIDR, (uint8_t *) &__port.PDR, (uint8_t *) &__port.PMR, (uint8_t *) &__port.PCR, (uint8_t *) &__port.DSCR

#define GPIO_PORT_ADDRESS_ENTRY_WITHOUT_DSCR(__port) \
   (uint8_t *) &__port.PODR, (uint8_t *) &__port.PIDR, (uint8_t *) &__port.PDR, (uint8_t *) &__port.PMR, (uint8_t *) &__port.PCR, (uint8_t *) NULL

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

static void SetPullUp(I_GpioGroup_t *instance, const GpioChannel_t channel, const GpioPullUp_t pullUp)
{
   IGNORE_ARG(instance);

   if(gpioPortAddresses[gpioPortsAndPins[channel].port].pullUpControl)
   {
      uint8_t pullUpRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].pullUpControl;
      BIT_WRITE(pullUpRegister, gpioPortsAndPins[channel].pin, pullUp);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].pullUpControl = pullUpRegister;
   }
}

static void SetGpioMode(I_GpioGroup_t *instance, const GpioChannel_t channel)
{
   IGNORE_ARG(instance);

   if(gpioPortAddresses[gpioPortsAndPins[channel].port].mode)
   {
      uint8_t modeRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].mode;
      BIT_CLEAR(modeRegister, gpioPortsAndPins[channel].pin);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].mode = modeRegister;
   }
}

static void SetDriveCapacity(I_GpioGroup_t *instance, const GpioChannel_t channel, const GpioDriveCapacity_t driveCapacitySetting)
{
   IGNORE_ARG(instance);

   if(gpioPortAddresses[gpioPortsAndPins[channel].port].driveCapacity)
   {
      uint8_t driveCapacityRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].driveCapacity;
      BIT_WRITE(driveCapacityRegister, gpioPortsAndPins[channel].pin, driveCapacitySetting);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].driveCapacity = driveCapacityRegister;
   }
}

static bool Read(I_GpioGroup_t *instance, const GpioChannel_t channel)
{
   IGNORE_ARG(instance);

   if(gpioPortAddresses[gpioPortsAndPins[channel].port].inputData)
   {
      uint8_t inputRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].inputData;
      return BIT_STATE(inputRegister, gpioPortsAndPins[channel].pin);
   }

   return false;
}

static void Write(I_GpioGroup_t *instance, const GpioChannel_t channel, const bool state)
{
   IGNORE_ARG(instance);

   if(gpioPortAddresses[gpioPortsAndPins[channel].port].outputData)
   {
      uint8_t outputRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].outputData;
      BIT_WRITE(outputRegister, gpioPortsAndPins[channel].pin, state);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].outputData = outputRegister;
   }
}

static void SetDirection(I_GpioGroup_t *instance, const GpioChannel_t channel, const GpioDirection_t direction)
{
   IGNORE_ARG(instance);

   if(gpioPortAddresses[gpioPortsAndPins[channel].port].direction)
   {
      uint8_t directionRegister = *gpioPortAddresses[gpioPortsAndPins[channel].port].direction;
      BIT_WRITE(directionRegister, gpioPortsAndPins[channel].pin, direction);
      *gpioPortAddresses[gpioPortsAndPins[channel].port].direction = directionRegister;
   }
}

static const I_GpioGroup_Api_t api =
   { Read, Write, SetDirection };

I_GpioGroup_t *GpioGroup_Init(void)
{
   static I_GpioGroup_t gpio;
   gpio.api = &api;

   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioPortsAndPins); channel++)
   {
      SetPullUp(&gpio, channel, gpioPortsAndPins[channel].pullUp);
      SetDirection(&gpio, channel, gpioPortsAndPins[channel].direction);
      SetGpioMode(&gpio, channel);
      SetDriveCapacity(&gpio, channel, gpioPortsAndPins[channel].driveCapacity);
   }

   return &gpio;
}
