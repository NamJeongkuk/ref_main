/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "Gpio_Psoc6.h"
#include "DataSource_Gpio.h"
#include "SystemErds.h"
#include "uassert.h"
#include "I_GpioGroup.h"
#include "ContextProtector_Psoc6.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspGpio_Start - 1)
#define ERD_FROM_CHANNEL(channel) (Erd_BspGpio_Start + 1 + channel)
#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspGpio_Start + 1, erd, Erd_BspGpio_End - 1))

#define EXPAND_AS_GPIO_COUNT(name, direction, pullUp, driveCapacity, port, pin, inverted) +1

enum
{
   InputPollPeriodInMsec = 10,
   BitsPerByte = 8,
   GpioCount = 0 GPIO_TABLE(EXPAND_AS_GPIO_COUNT)
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
   GpioDirection_t direction;
   GpioPullUp_t pullUp;
   GpioDriveCapacity_t driveCapacity;
   en_hsiom_sel_t hsiomSelection;
   uint8_t port;
   uint8_t pin;
   bool inverted;
} GpioDirectionPortsAndPins_t;

#define EXPAND_PORTS_AND_PINS(channel, direction, pullUp, driveCapacity, port, pin, inverted) \
   { direction, pullUp, driveCapacity, CONCAT(P, CONCAT(port, CONCAT(_, CONCAT(pin, _GPIO)))), port, pin, inverted },

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

static bool ReadGpio(const GpioChannel_t channel)
{
   bool state = Gpio_Psoc6_Read(gpioPortsAndPins[channel].port, gpioPortsAndPins[channel].pin);

   return gpioPortsAndPins[channel].inverted ? !state : state;
}

static void WriteGpio(const GpioChannel_t channel, const bool state)
{
   Gpio_Psoc6_Write(gpioPortsAndPins[channel].port, gpioPortsAndPins[channel].pin, state);
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
   cy_stc_gpio_pin_config_t configuration = {
      .outVal = 1,
      .intEdge = CY_GPIO_INTR_DISABLE,
      .intMask = 0UL,
      .vtrip = CY_GPIO_VTRIP_CMOS,
      .slewRate = CY_GPIO_SLEW_FAST,
      .driveSel = CY_GPIO_DRIVE_1_2,
      .vregEn = 0UL,
      .ibufMode = 0UL,
      .vtripSel = 0UL,
      .vrefSel = 0UL,
      .vohSel = 0UL
   };

   for(GpioChannel_t i = 0; i < NUM_ELEMENTS(gpioPortsAndPins); i++)
   {
      GpioDirectionPortsAndPins_t channel = gpioPortsAndPins[i];
      configuration.hsiom = channel.hsiomSelection;

      if(channel.direction == GpioDirection_Output)
      {
         configuration.driveMode = CY_GPIO_DM_STRONG;
      }
      else if(channel.pullUp == GpioPullUp_Enable)
      {
         configuration.driveMode = CY_GPIO_DM_PULLUP;
      }
      else
      {
         configuration.driveMode = CY_GPIO_DM_HIGHZ;
      }

      Gpio_Psoc6_ConfigurePin(channel.port, channel.pin, &configuration);
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
