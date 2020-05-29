/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stddef.h>
#include <stdbool.h>
#include "stm32g0xx_ll_gpio.h"
#include "Pa5Heartbeat.h"
#include "utils.h"

enum
{
   HalfPeriodInMsec = 500
};

static TinyTimer_t timer;

static void Blink(void *context, TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}

void Pa5Heartbeat_Init(TinyTimerModule_t *timerModule)
{
   SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);

   LL_GPIO_InitTypeDef gpio;
   gpio.Pin = LL_GPIO_PIN_5;
   gpio.Mode = LL_GPIO_MODE_ALTERNATE;
   gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
   gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
   gpio.Pull = LL_GPIO_PULL_UP;
   gpio.Alternate = LL_GPIO_AF_1;
   LL_GPIO_Init(GPIOA, &gpio);

   TinyTimerModule_Start(timerModule, &timer, HalfPeriodInMsec, Blink, NULL);
}
