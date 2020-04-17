/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "Heartbeat.h"
#include "utils.h"
#include "cypins.h"
#include "cydevice_trm.h"

enum
{
   HalfPeriodInMsec = 500,
   Pin5 = 5
};

static TinyTimer_t timer;

#define PC ((uint32_t *)CYREG_GPIO_PRT5_PC)
#define DR ((uint32_t *)CYREG_GPIO_PRT5_DR)

static void Blink(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);

   bool state = BIT_STATE(*DR, Pin5);
   BIT_WRITE(*DR, Pin5, !state);

   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}

void Heartbeat_Init(TinyTimerModule_t *timerModule)
{
   uint32_t value = *PC;
   BIT_SET(value, (3 * Pin5 + 1));
   BIT_SET(value, (3 * Pin5 + 2));
   *PC = value;
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
