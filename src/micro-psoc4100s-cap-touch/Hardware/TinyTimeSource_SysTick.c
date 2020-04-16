/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TinyTimeSource_SysTick.h"
#include "CyLib.h"
#include "utils.h"

static I_TinyTimeSource_t timeSource;
static volatile TinyTimeSourceTickCount_t ticks;

CY_ISR(SysTickHandler)
{
   if(CySysTickGetCountFlag())
   {
      ticks++;
   }
}

static TinyTimeSourceTickCount_t GetTicks(I_TinyTimeSource_t *instance)
{
   IGNORE(instance);

   uint8_t savedInterruptStatus = CyEnterCriticalSection();
   TinyTimeSourceTickCount_t ticksCopy = ticks;
   CyExitCriticalSection(savedInterruptStatus);

   return ticksCopy;
}

static const I_TinyTimeSource_Api_t timeSourceApi = { GetTicks };

I_TinyTimeSource_t *TinyTimeSource_SysTick_Init(void)
{
   CySysTickSetClockSource(CY_SYS_SYST_CSR_CLK_SRC_SYSCLK);
   CySysTickSetReload(CYDEV_BCLK__HFCLK__KHZ);
   CySysTickClear();

   CySysTickEnable();

   timeSource.api = &timeSourceApi;

   return &timeSource;
}
