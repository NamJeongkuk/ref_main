/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "stm32g0xx.h"
#include "Reset.h"

void Reset(void)
{
   NVIC_SystemReset();
}
