/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "Pd4Heartbeat.h"
#include "stm8s.h"
#include "utils.h"

enum
{
   HalfPeriodInMsec = 250,
   Pin4 = (1 << 4)
};

static TinyTimer_t timer;

static void Blink(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   GPIOD->ODR ^= Pin4;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}

void Pd4Heartbeat_Init(TinyTimerModule_t *timerModule)
{
   GPIOD->CR1 |= Pin4;
   GPIOD->DDR |= Pin4;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
