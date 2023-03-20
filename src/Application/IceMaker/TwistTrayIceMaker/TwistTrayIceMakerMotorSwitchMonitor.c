/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorSwitchMonitor.h"
#include "utils.h"

enum
{
   DebounceCounts = 5,
   PollFrequencyInMsec = 4
};

static void OnInterrupt(void *context, const void *args)
{
   IGNORE(args);
   TwistTrayIceMakerMotorSwitchMonitor_t *instance = context;

   if(++instance->_private.ticks >= PollFrequencyInMsec)
   {
      Debouncer_bool_Process(
         &instance->_private.debouncer,
         GpioGroup_Read(
            instance->_private.gpioGroup,
            instance->_private.config->switchInputChannel));

      TwistTrayIceMakerMotorController_UpdateSwitchState(
         instance->_private.motorController,
         !Debouncer_bool_GetDebounced(&instance->_private.debouncer));

      instance->_private.ticks = 0;
   }
}

void TwistTrayIceMakerMotorSwitchMonitor_Init(
   TwistTrayIceMakerMotorSwitchMonitor_t *instance,
   TwistTrayIceMakerMotorController_t *motorController,
   I_GpioGroup_t *gpioGroup,
   I_Interrupt_t *interrupt,
   const TwistTrayIceMakerMotorSwitchMonitorConfig_t *config)
{
   instance->_private.gpioGroup = gpioGroup;
   instance->_private.motorController = motorController;
   instance->_private.ticks = 0;
   instance->_private.config = config;

   Debouncer_bool_Init(
      &instance->_private.debouncer,
      0,
      DebounceCounts);

   EventSubscription_Init(&instance->_private.on1MsInterruptSubscription, instance, OnInterrupt);
   Event_Subscribe(interrupt->OnInterrupt, &instance->_private.on1MsInterruptSubscription);
}
