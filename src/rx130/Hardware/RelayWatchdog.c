/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "RelayWatchdog.h"
#include "DigitalOutput_OutputAdapter.h"
#include "RelayWatchdogKicker.h"
#include "DataModelErdPointerAccess.h"
#include "Timer.h"
#include "SystemErds.h"

enum
{
   RelayWatchdogKickTimeInMs = 20,
   MaxKicksWithoutBackgroundActivity = 1000
};

struct
{
   RelayWatchdogKicker_t relayWatchdogKicker;
   DigitalOutput_OutputAdapter_t digitalOutputAdapter;
   Timer_t relayWatchdogKickTimer;
} instance;

static void KickRelayWatchdog(void *context)
{
   RelayWatchdogKicker_t *kicker = context;
   RelayWatchdogKicker_KickFromBackground(kicker);
}

void RelayWatchdogPlugin_Init(I_DataModel_t *dataModel)
{
   DigitalOutput_OutputAdapter_Init(
      &instance.digitalOutputAdapter,
      DataModel_GetOutput(dataModel, Erd_RelayWatchdog));

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
