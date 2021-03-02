/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VECTORTABLE_H
#define VECTORTABLE_H

void gpt_counter_overflow_isr(void);
void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void sci_uart_tei_isr(void);
void sci_uart_eri_isr(void);
void fcu_frdyi_isr(void);
void fcu_fiferr_isr(void);
void dmac_int_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW ((IRQn_Type)0) /* GPT0 COUNTER OVERFLOW (Overflow) */
#define VECTOR_NUMBER_SCI0_RXI ((IRQn_Type)1) /* SCI0 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type)2) /* SCI0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type)3) /* SCI0 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI0_ERI ((IRQn_Type)4) /* SCI0 ERI (Receive error) */
#define VECTOR_NUMBER_SCI7_RXI ((IRQn_Type)5) /* SCI7 RXI (Received data full) */
#define VECTOR_NUMBER_SCI7_TXI ((IRQn_Type)6) /* SCI7 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI7_TEI ((IRQn_Type)7) /* SCI7 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI7_ERI ((IRQn_Type)8) /* SCI7 ERI (Receive error) */
#define VECTOR_NUMBER_FCU_FRDYI ((IRQn_Type)9) /* FCU FRDYI (Flash ready interrupt) */
#define VECTOR_NUMBER_FCU_FIFERR ((IRQn_Type)10) /* FCU FIFERR (Flash access error interrupt) */

typedef enum IRQn
{
   Reset_IRQn = -15,
   NonMaskableInt_IRQn = -14,
   HardFault_IRQn = -13,
   MemoryManagement_IRQn = -12,
   BusFault_IRQn = -11,
   UsageFault_IRQn = -10,
   SecureFault_IRQn = -9,
   SVCall_IRQn = -5,
   DebugMonitor_IRQn = -4,
   PendSV_IRQn = -2,
   SysTick_IRQn = -1,
   GPT0_COUNTER_OVERFLOW_IRQn = 0, /* GPT0 COUNTER OVERFLOW (Overflow) */
   SCI0_RXI_IRQn = 1, /* SCI0 RXI (Receive data full) */
   SCI0_TXI_IRQn = 2, /* SCI0 TXI (Transmit data empty) */
   SCI0_TEI_IRQn = 3, /* SCI0 TEI (Transmit end) */
   SCI0_ERI_IRQn = 4, /* SCI0 ERI (Receive error) */
   SCI7_RXI_IRQn = 5, /* SCI7 RXI (Received data full) */
   SCI7_TXI_IRQn = 6, /* SCI7 TXI (Transmit data empty) */
   SCI7_TEI_IRQn = 7, /* SCI7 TEI (Transmit end) */
   SCI7_ERI_IRQn = 8, /* SCI7 ERI (Receive error) */
   FCU_FRDYI_IRQn = 9, /* FCU FRDYI (Flash ready interrupt) */
   FCU_FIFERR_IRQn = 10, /* FCU FIFERR (Flash access error interrupt) */
} IRQn_Type;

#endif
