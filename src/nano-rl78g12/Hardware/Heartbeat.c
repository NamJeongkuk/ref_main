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
   P1_bit.no3 ^= 1;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}

void Heartbeat_Init(TinyTimerModule_t *timerModule)
{
   // Set P1.3 as an output
   PM1_bit.no3 = 0;

   // Use P1.3 as GPIO
   PMC1_bit.no3 = 0;

   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
