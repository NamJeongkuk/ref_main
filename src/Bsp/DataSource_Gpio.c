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
#include "ContextProtector_Rx2xx.h"
#include <string.h>
#include "Asymmetrical_Debouncer_bool.h"
#include "Constants_Binary.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspGpio_Start - 1)
#define ERD_FROM_CHANNEL(channel) (Erd_BspGpio_Start + 1 + channel)
#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End - 1))

#define GPIO_TABLE_EXPAND_AS_GPIO_COUNT(name, direction, offToOnDebounceCount, onToOffDebounceCount, pullUp, driveCapacity, port, pin, inverted) +1

#define GPIO_TABLE_EXPAND_AS_GPIO_INPUT_COUNT(name, direction, offToOnDebounceCount, onToOffDebounceCount, pullUp, driveCapacity, port, pin, inverted) \
   +((direction) == GpioDirection_Input)

enum
{
   InputPollPeriodInMsec = 10,
   BitsPerByte = 8,
   GpioCount = 0 GPIO_TABLE(GPIO_TABLE_EXPAND_AS_GPIO_COUNT)
};

enum
{
   GpioInputCount = 0 GPIO_TABLE(GPIO_TABLE_EXPAND_AS_GPIO_INPUT_COUNT)
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

#define EXPAND_PORTS_AND_PINS(channel, direction, offToOnDebounceCount, onToOffDebounceCount, pullUp, driveCapacity, port, pin, inverted) { direction, pullUp, driveCapacity, port, pin, inverted },

static const GpioDirectionPortsAndPins_t gpioPortsAndPins[] = {
   GPIO_TABLE(EXPAND_PORTS_AND_PINS)
};

static struct
{
   I_DataSource_t interface;
   I_GpioGroup_t gpioGroupInterface;
   Event_Synchronous_t *onChangeEvent;
   EventSubscription_t debouncePollSubscription;
   Asymmetrical_Debouncer_bool_t asymmetricalDebouncers[GpioInputCount];
   bool debounceInputDataDirtyFlags[GpioInputCount];
   Timer_t timer;
   uint8_t inputCache[((GpioCount - 1) / BitsPerByte) + 1];
   uint32_t debounceCallbackCount;
} instance;

#define EXPAND_AS_ASYMMETRICAL_DEBOUNCE_BOOL_CONFIGS(channel, direction, offToOnDebounceCount, onToOffDebounceCount, pullUp, driveCapacity, port, pin, inverted) \
   CONCAT(INCLUDE_, direction)                                                                                                                                   \
   ({ .debounceCountOffToOn = offToOnDebounceCount COMMA.debounceCountOnToOff = onToOffDebounceCount } COMMA)

static const Asymmetrical_Debouncer_bool_Config_t asymmetricalDebouncerConfigs[] = {
   GPIO_TABLE(EXPAND_AS_ASYMMETRICAL_DEBOUNCE_BOOL_CONFIGS)
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
   uassert(ERD_IS_IN_RANGE(erd) || (erd == Erd_GpioGroupInterface));

   if(erd == Erd_GpioGroupInterface)
   {
      *((I_GpioGroup_t **)data) = &instance.gpioGroupInterface;
   }
   else
   {
      bool value;
      uint16_t channel = CHANNEL_FROM_ERD(erd);
      if(channel < GpioInputCount)
      {
         value = Asymmetrical_Debouncer_bool_GetDebounced(&instance.asymmetricalDebouncers[channel]);
      }
      else
      {
         value = ReadGpio(channel);
      }
      memcpy(data, &value, sizeof(bool));
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   REINTERPRET(state, data, bool *);

   uassert(ERD_IS_IN_RANGE(erd) || (erd == Erd_GpioGroupInterface));

   if(erd != Erd_GpioGroupInterface)
   {
      GpioChannel_t channel = CHANNEL_FROM_ERD(erd);
      if(ReadGpio(channel) != *state)
      {
         WriteGpio(channel, *state);

         DataSourceOnDataChangeArgs_t args = { erd, data };
         Event_Synchronous_Publish(instance.onChangeEvent, &args);
      }
   }
}

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return (ERD_IS_IN_RANGE(erd) || (erd == Erd_GpioGroupInterface));
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd) || (erd == Erd_GpioGroupInterface));

   if(erd == Erd_GpioGroupInterface)
   {
      return sizeof(I_GpioGroup_t *);
   }
   else
   {
      return sizeof(bool);
   }
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

bool ReadGpioGroup(I_GpioGroup_t *_instance, const GpioChannel_t channel)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(channel));

   return Asymmetrical_Debouncer_bool_GetDebounced(&instance.asymmetricalDebouncers[channel]);
}

void WriteGpioGroup(I_GpioGroup_t *instance, const GpioChannel_t channel, const bool state)
{
   IGNORE(instance);
   uassert(ERD_IS_IN_RANGE(channel));

   WriteGpio(CHANNEL_FROM_ERD(channel), state);
}

void SetDirectionGpioGroup(I_GpioGroup_t *instance, const GpioChannel_t channel, const GpioDirection_t direction)
{
   IGNORE(instance);
   uassert(ERD_IS_IN_RANGE(channel));

   SetDirection(CHANNEL_FROM_ERD(channel), direction);
}

static const I_GpioGroup_Api_t apiGpioGroup = { ReadGpioGroup, WriteGpioGroup, SetDirectionGpioGroup };

static void InitializeBoolDebouncer(uint8_t channel)
{
   instance.debounceCallbackCount = 0;

   if(gpioPortsAndPins[channel].direction == GpioDirection_Input)
   {
      instance.debounceInputDataDirtyFlags[channel] = false;

      bool currentValue = ReadGpio(channel);
      Asymmetrical_Debouncer_bool_Init(
         &instance.asymmetricalDebouncers[channel],
         &asymmetricalDebouncerConfigs[channel],
         currentValue);
   }
}

static void InitializeGpio(void)
{
   for(GpioChannel_t channel = 0; channel < NUM_ELEMENTS(gpioPortsAndPins); channel++)
   {
      SetPullUp(channel, gpioPortsAndPins[channel].pullUp);
      SetDirection(channel, gpioPortsAndPins[channel].direction);
      SetGpioMode(channel);
      SetDriveCapacity(channel, gpioPortsAndPins[channel].driveCapacity);
      InitializeBoolDebouncer(channel);
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

void DataSource_Gpio_Run(void)
{
   for(GpioChannel_t channel = 0; channel < GpioInputCount; channel++)
   {
      if(instance.debounceInputDataDirtyFlags[channel])
      {
         bool debouncedInput = Asymmetrical_Debouncer_bool_GetDebounced(&instance.asymmetricalDebouncers[channel]);
         DataSourceOnDataChangeArgs_t arguments = { ERD_FROM_CHANNEL(channel), &debouncedInput };
         Event_Synchronous_Publish(instance.onChangeEvent, &arguments);
         instance.debounceInputDataDirtyFlags[channel] = false;
      }
   }
}

static void DebounceCallback(void *context, const void *args)
{
   IGNORE(context);
   IGNORE(args);

   instance.debounceCallbackCount++;

   if(instance.debounceCallbackCount == InputPollPeriodInMsec)
   {
      instance.debounceCallbackCount = 0;
      for(GpioChannel_t channel = 0; channel < GpioInputCount; channel++)
      {
         bool rawInputValue = ReadGpio(channel);

         if(gpioPortsAndPins[channel].inverted)
         {
            rawInputValue = !rawInputValue;
         }
         if(Asymmetrical_Debouncer_bool_Process(&instance.asymmetricalDebouncers[channel], rawInputValue))
         {
            instance.debounceInputDataDirtyFlags[channel] = true;
         }
      }
   }
}

I_DataSource_t *DataSource_Gpio_Init(
   Event_Synchronous_t *onChangeEvent,
   I_Interrupt_t *debounceInterrupt)
{
   instance.interface.api = &api;
   instance.gpioGroupInterface.api = &apiGpioGroup;
   instance.interface.OnDataChange = &onChangeEvent->interface;
   instance.onChangeEvent = onChangeEvent;

   InitializeGpio();
   EventSubscription_Init(&instance.debouncePollSubscription, NULL, DebounceCallback);

   if(AtLeastOneInputExists())
   {
      Event_Subscribe(debounceInterrupt->OnInterrupt, &instance.debouncePollSubscription);
   }

   return &instance.interface;
}
