/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "stm8s.h"
#include "stm8s_wwdg.h"

void Reset(void)
{
   WWDG->CR = 0x80;
}
