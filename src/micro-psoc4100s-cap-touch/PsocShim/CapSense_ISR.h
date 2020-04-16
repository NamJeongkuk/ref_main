/*!
 * @file
 * @brief CapSense ISR Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPSENSEISR_H
#define CAPSENSEISR_H

#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void CapSense_ISR_Start(void);
void CapSense_ISR_StartEx(cyisraddress address);

void CapSenseIsrEnable(void);
void CapSenseIsrDisable(void);

void CapSense_ISR_SetPending(void);
void CapSense_ISR_ClearPending(void);

/* Interrupt Controller Constants */

/* Address of the CapSense_ISR ISR priority. */
#define CapSenseIsrInterruptPriority             ((reg32 *) CapSense_ISR__INTC_PRIOR_REG)

/* Priority of the CapSense_ISR interrupt. */
#define CapSenseIsrInterruptPriorityNumber      CapSense_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable CapSense_ISR interrupt. */
#define CapSenseIsrInterruptSetEnable            ((reg32 *) CapSense_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the CapSense_ISR interrupt. */
#define CapSenseIsrInterruptClearEnable            ((reg32 *) CapSense_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the CapSense_ISR interrupt state to pending. */
#define CapSenseIsrInterruptSetPending            ((reg32 *) CapSense_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the CapSense_ISR interrupt. */
#define CapSenseIsrInterruptClearPending            ((reg32 *) CapSense_ISR__INTC_CLR_PD_REG)

#endif
