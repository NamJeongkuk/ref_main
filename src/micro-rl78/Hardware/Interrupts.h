/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "iodefine.h"

/*!
 */
static inline void Interrupts_Disable(void)
{
   DI();
}

/*!
 */
static inline void Interrupts_Enable(void)
{
   EI();
}

#endif
