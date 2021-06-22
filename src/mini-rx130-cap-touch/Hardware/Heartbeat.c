/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "Heartbeat.h"
#include "iodefine.h"
#include "utils.h"

enum
{
   HalfPeriodInMsec = 500
};

static Timer_t timer;
static bool state;

static void Blink(void *context)
{
   IGNORE(context);
   state = !state;
   PORTD.PODR.BIT.B3 = state;
}

void Heartbeat_Init(TimerModule_t *timerModule)
{
   PORTD.PDR.BIT.B3 = 1;
   TimerModule_StartPeriodic(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
