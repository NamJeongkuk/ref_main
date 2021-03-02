/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "HeartbeatLed.h"
#include "Gpio_Ra.h"
#include "utils.h"

#define LED_PIN (BSP_IO_PORT_04_PIN_00)

enum
{
   HalfPeriodInMsec = 500,
   Pin5 = (1 << 5)
};

static Timer_t timer;
static bool state;

static void Blink(void *context)
{
   IGNORE(context);
   state = !state;

   Gpio_Ra_Write(LED_PIN, state);
}

void HeartbeatLed_Init(TimerModule_t *timerModule)
{
   Gpio_Ra_ConfigurePin(LED_PIN, (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_LOW));
   TimerModule_StartPeriodic(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
