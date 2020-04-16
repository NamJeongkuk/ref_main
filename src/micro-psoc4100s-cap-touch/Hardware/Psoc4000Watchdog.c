/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Psoc4000Watchdog.h"
#include "cytypes.h"
#include "utils.h"
#include "CyLFClk.h"
#include "CyLib.h"

void Psoc4000Watchdog_InitWithDefaultConfiguration(void)
{
   CySysWdtSetIgnoreBits(1);
   CySysWdtEnable();
   CySysClkIloStartMeasurement();
}

void Psoc4000Watchdog_Kick(void)
{
   CySysWdtClearInterrupt();
}
