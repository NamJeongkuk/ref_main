/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RelayWatchdog.h"
#include "DigitalOutput_GpioGroupAdapter.h"
#include "RelayWatchdogKicker.h"
#include "DataModelErdPointerAccess.h"
#include "Timer.h"
#include "SystemErds.h"
#include "DataSource_Gpio.h"

#define CHANNEL_FROM_ERD(erd) (erd - Erd_BspGpio_Start - 1)

enum
{
   RelayWatchdogKickTimeInMs = 20,
   MaxKicksWithoutBackgroundActivity = 1000
};

struct
{
   RelayWatchdogKicker_t relayWatchdogKicker;
   DigitalOutput_GpioGroupAdapter_t digitalOutputAdapter;
   Timer_t relayWatchdogKickTimer;
} instance;

static void KickRelayWatchdog(void *context)
{
   RelayWatchdogKicker_t *kicker = context;
   RelayWatchdogKicker_KickFromBackground(kicker);
}

void RelayWatchdogPlugin_Init(I_DataModel_t *dataModel)
{
   DigitalOutput_GpioGroupAdapter_Init(
      &instance.digitalOutputAdapter,
      DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroupInterface),
      Erd_BspGpio_RLY_09);

   RelayWatchdogKicker_Init(
      &instance.relayWatchdogKicker,
      DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt),
      &instance.digitalOutputAdapter.interface,
      MaxKicksWithoutBackgroundActivity);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      &instance.relayWatchdogKickTimer,
      RelayWatchdogKickTimeInMs,
      KickRelayWatchdog,
      &instance.relayWatchdogKicker);
}
