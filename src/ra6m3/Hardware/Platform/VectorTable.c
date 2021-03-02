/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "bsp_api.h"

BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) = {
   [0] = gpt_counter_overflow_isr, /* GPT0 COUNTER OVERFLOW (Overflow) */
   [1] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
   [2] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
   [3] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
   [4] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
   [5] = sci_uart_rxi_isr, /* SCI7 RXI (Received data full) */
   [6] = sci_uart_txi_isr, /* SCI7 TXI (Transmit data empty) */
   [7] = sci_uart_tei_isr, /* SCI7 TEI (Transmit end) */
   [8] = sci_uart_eri_isr, /* SCI7 ERI (Receive error) */
   [9] = fcu_frdyi_isr, /* FCU FRDYI (Flash ready interrupt) */
   [10] = fcu_fiferr_isr, /* FCU FIFERR (Flash access error interrupt) */
};

const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] = {
   [0] = BSP_PRV_IELS_ENUM(EVENT_GPT0_COUNTER_OVERFLOW), /* GPT0 COUNTER OVERFLOW (Overflow) */
   [1] = BSP_PRV_IELS_ENUM(EVENT_SCI0_RXI), /* SCI0 RXI (Receive data full) */
   [2] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TXI), /* SCI0 TXI (Transmit data empty) */
   [3] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TEI), /* SCI0 TEI (Transmit end) */
   [4] = BSP_PRV_IELS_ENUM(EVENT_SCI0_ERI), /* SCI0 ERI (Receive error) */
   [5] = BSP_PRV_IELS_ENUM(EVENT_SCI7_RXI), /* SCI7 RXI (Received data full) */
   [6] = BSP_PRV_IELS_ENUM(EVENT_SCI7_TXI), /* SCI7 TXI (Transmit data empty) */
   [7] = BSP_PRV_IELS_ENUM(EVENT_SCI7_TEI), /* SCI7 TEI (Transmit end) */
   [8] = BSP_PRV_IELS_ENUM(EVENT_SCI7_ERI), /* SCI7 ERI (Receive error) */
   [9] = BSP_PRV_IELS_ENUM(EVENT_FCU_FRDYI), /* FCU FRDYI (Flash ready interrupt) */
   [10] = BSP_PRV_IELS_ENUM(EVENT_FCU_FIFERR), /* FCU FIFERR (Flash access error interrupt) */
};
