/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
*/

#include "cy_pdl.h"
#include "Clock.h"

enum
{
   kHz = 1000,
   MHz = kHz * kHz,

   ImoFreqHz = 8 * MHz,
   HighFreqClockMHz = 100,

   HighFreqClockHz = HighFreqClockMHz * MHz,
   NoTimeout = 0,
   LowPowerMode = false,
};

void Clock_Init(void)
{
   Cy_SysLib_SetWaitStates(LowPowerMode, HighFreqClockMHz);

   Cy_SysClk_FllConfigure(ImoFreqHz, HighFreqClockHz, CY_SYSCLK_FLLPLL_OUTPUT_AUTO);
   Cy_SysClk_FllEnable(NoTimeout);
   while(!Cy_SysClk_FllLocked())
   {
      // Wait
   }

   // Low speed clock setup
   Cy_SysClk_ClkLfSetSource(CY_SYSCLK_CLKLF_IN_ILO);
   Cy_SysClk_IloEnable();
   Cy_SysClk_IloHibernateOn(true);

   // Maintain clock freq variables
   SystemCoreClockUpdate();
}
