/*!
 * @file
 * @brief CapTouch ISR Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPTOUCHISR_H
#define CAPTOUCHISR_H

#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void CapTouch_ISR_Start(void);
void CapTouch_ISR_StartEx(cyisraddress address);

void CapTouchIsrEnable(void);
void CapTouchIsrDisable(void);

void CapTouch_ISR_SetPending(void);
void CapTouch_ISR_ClearPending(void);

/* Interrupt Controller Constants */

/* Address of the CapTouch_ISR ISR priority. */
#define CapTouchIsrInterruptPriority ((reg32 *)CapTouch_ISR__INTC_PRIOR_REG)

/* Priority of the CapTouch_ISR interrupt. */
#define CapTouchIsrInterruptPriorityNumber CapTouch_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable CapTouch_ISR interrupt. */
#define CapTouchIsrInterruptSetEnable ((reg32 *)CapTouch_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the CapTouch_ISR interrupt. */
#define CapTouchIsrInterruptClearEnable ((reg32 *)CapTouch_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the CapTouch_ISR interrupt state to pending. */
#define CapTouchIsrInterruptSetPending ((reg32 *)CapTouch_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the CapTouch_ISR interrupt. */
#define CapTouchIsrInterruptClearPending ((reg32 *)CapTouch_ISR__INTC_CLR_PD_REG)

#endif
