/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Watchdog.h"
#include "cytypes.h"
#include "utils.h"
#include "CyLFClk.h"
#include "CyLib.h"

void Watchdog_Init(void)
{
   CySysWdtSetIgnoreBits(1);
   CySysWdtEnable();
   CySysClkIloStartMeasurement();
}

void Watchdog_Kick(void)
{
   CySysWdtClearInterrupt();
}
