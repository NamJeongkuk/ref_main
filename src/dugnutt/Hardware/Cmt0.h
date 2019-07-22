/*!
 * @file
 * @brief Timer module CMT0 for RX130
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef CMT0_H
#define CMT0_H

#include "I_Interrupt.h"

/*!
 * Initialize CMT0 to be the system tick
 * @return Pointer to interrupt handle
 */
I_Interrupt_t * CMT0_Init(void);

#endif
