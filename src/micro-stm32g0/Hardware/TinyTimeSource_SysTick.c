/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "stm32g0xx.h"
#include "TinyTimeSource_SysTick.h"
#include "utils.h"

static struct
{
   I_TinyTimeSource_t interface;
   volatile TinyTimeSourceTickCount_t ticks;
} instance;

void SysTick_Handler(void)
{
   instance.ticks++;
}

static TinyTimeSourceTickCount_t GetTicks(I_TinyTimeSource_t *_instance)
{
   IGNORE(_instance);

   TinyTimeSourceTickCount_t ticks1;
   TinyTimeSourceTickCount_t ticks2;

   do
   {
      ticks1 = instance.ticks;
      ticks2 = instance.ticks;
   } while(ticks1 != ticks2);

   return ticks1;
}

static const I_TinyTimeSource_Api_t api = { GetTicks };

I_TinyTimeSource_t *TinyTimeSource_SysTick_Init(void)
{
   if(SysTick_Config(SystemCoreClock / 1000))
   {
      NVIC_SystemReset();
   }

   NVIC_SetPriority(SysTick_IRQn, 7);

   instance.interface.api = &api;

   return &instance.interface;
}
