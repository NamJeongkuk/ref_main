/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "iodefine.h"
#include "iodefine_ext.h"
#include "Heartbeat.h"
#include "utils.h"

enum
{
   HalfPeriodInMsec = 500
};

static TinyTimer_t timer;

static void Blink(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   P6_bit.no2 ^= 1;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}

void Heartbeat_Init(TinyTimerModule_t *timerModule)
{
   // Set P6.2 as an output
   PM6_bit.no2 = 0;

   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
