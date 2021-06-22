/*!
 * @file
 * @brief Interrupt control macros.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/*!
 * Enable interrupts.
 */
#define Interrupts_Enable() __builtin_rx_setpsw('I')

/*!
 * Disable interrupts.
 */
#define Interrupts_Disable() __builtin_rx_clrpsw('I')

#endif
