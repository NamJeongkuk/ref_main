/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "iodefine.h"
#include "Reset.h"

void Reset(void)
{
   SYSTEM.PRCR.WORD = 0xA502;
   SYSTEM.SWRR = 0xA501;
   SYSTEM.PRCR.WORD = 0xA500;
}
