/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "HeartbeatLed.h"
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
   PORT7.DR.BIT.B1 = (state = !state);
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}

void HeartbeatLed_Init(TinyTimerModule_t *timerModule)
{
   PORT7.DDR.BIT.B1 = 1;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
