/*!
 * @file
 * @brief This is a temporary module to monitor the switch input. It will be replaced once the interrupt driven debouncer is merged
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORSWITCHMONITOR_H
#define TWISTTRAYICEMAKERMOTORSWITCHMONITOR_H

#include "EventSubscription.h"
#include "I_Interrupt.h"
#include "I_GpioGroup.h"
#include "Erd.h"
#include "Debouncer_bool.h"
#include "TwistTrayIceMakerMotorController.h"

typedef struct
{
   Erd_t switchInputChannel;
} TwistTrayIceMakerMotorSwitchMonitorConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t on1MsInterruptSubscription;
      I_GpioGroup_t *gpioGroup;
      Debouncer_bool_t debouncer;
      uint8_t ticks;
      const TwistTrayIceMakerMotorSwitchMonitorConfig_t *config;
      TwistTrayIceMakerMotorController_t *motorController;
   } _private;
} TwistTrayIceMakerMotorSwitchMonitor_t;

void TwistTrayIceMakerMotorSwitchMonitor_Init(
   TwistTrayIceMakerMotorSwitchMonitor_t *instance,
   TwistTrayIceMakerMotorController_t *motorController,
   I_GpioGroup_t *gpioGroup,
   I_Interrupt_t *interrupt,
   const TwistTrayIceMakerMotorSwitchMonitorConfig_t *config);

#endif
