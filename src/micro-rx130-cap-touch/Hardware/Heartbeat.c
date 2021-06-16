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

static TinyTimer_t timer;
static bool state;

static void Blink(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   state = !state;
   PORTD.PODR.BIT.B3 = state;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}

void Heartbeat_Init(TinyTimerModule_t *timerModule)
{
   PORTD.PDR.BIT.B3 = 1;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
