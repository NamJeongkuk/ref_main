/*!
 * @file
 * @brief Timer module CMT0 for RX130
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef INTERRUPT_CMT0_H
#define INTERRUPT_CMT0_H

#include "I_Interrupt.h"

/*!
 * Initialize CMT0 to be the system tick
 * @return Interrupt interface
 */
I_Interrupt_t *Interrupt_Cmt0_Init(void);

#endif
