/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DigitalOutput_HeartbeatLed.h"
#include "stm32f3xx.h"
#include "utils.h"

static void Write(I_DigitalOutput_t *instance, bool state)
{
   IGNORE(instance);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, state);
}

static void InitializeHardware(void)
{
   GPIO_InitTypeDef gpioInitStruct = { 0 };

   __HAL_RCC_GPIOB_CLK_ENABLE();

   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

   gpioInitStruct.Pin = GPIO_PIN_9;
   gpioInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   gpioInitStruct.Pull = GPIO_NOPULL;
   gpioInitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOB, &gpioInitStruct);
}

static const I_DigitalOutput_Api_t api =
   { Write };

static I_DigitalOutput_t instance;

I_DigitalOutput_t *DigitalOutput_HeartbeatLed_Init(void)
{
   InitializeHardware();
   instance.api = &api;
   return &instance;
}
