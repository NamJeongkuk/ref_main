/*!
 * @file
 * @brief CMT0 configuration
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CMT0_H
#define CMT0_H

#include "I_Interrupt.h"

/*!
 * Initialize the CMT0 timer
 */
void Cmt0_Init(void);

/*!
 * Get the interrupt hook for the CMT0 timer
 */
I_Interrupt_t * Cmt0_Init(void);

#endif
