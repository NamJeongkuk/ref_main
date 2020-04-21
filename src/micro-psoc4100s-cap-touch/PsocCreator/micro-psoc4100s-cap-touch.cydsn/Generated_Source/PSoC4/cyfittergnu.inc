/*******************************************************************************
* File Name: cyfittergnu.inc
* 
* PSoC Creator  4.2
*
* Description:
* 
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2018 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

.ifndef INCLUDED_CYFITTERGNU_INC
.set INCLUDED_CYFITTERGNU_INC, 1
.include "cydevicegnu_trm.inc"

/* Gea3_rx */
.set Gea3_rx__0__DR, CYREG_GPIO_PRT7_DR
.set Gea3_rx__0__DR_CLR, CYREG_GPIO_PRT7_DR_CLR
.set Gea3_rx__0__DR_INV, CYREG_GPIO_PRT7_DR_INV
.set Gea3_rx__0__DR_SET, CYREG_GPIO_PRT7_DR_SET
.set Gea3_rx__0__HSIOM, CYREG_HSIOM_PORT_SEL7
.set Gea3_rx__0__HSIOM_GPIO, 0
.set Gea3_rx__0__HSIOM_I2C, 14
.set Gea3_rx__0__HSIOM_I2C_SCL, 14
.set Gea3_rx__0__HSIOM_MASK, 0x0000000F
.set Gea3_rx__0__HSIOM_SHIFT, 0
.set Gea3_rx__0__HSIOM_SPI, 15
.set Gea3_rx__0__HSIOM_SPI_MOSI, 15
.set Gea3_rx__0__HSIOM_UART, 9
.set Gea3_rx__0__HSIOM_UART_RX, 9
.set Gea3_rx__0__INTCFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_rx__0__INTR, CYREG_GPIO_PRT7_INTR
.set Gea3_rx__0__INTR_CFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_rx__0__INTSTAT, CYREG_GPIO_PRT7_INTR
.set Gea3_rx__0__MASK, 0x01
.set Gea3_rx__0__PC, CYREG_GPIO_PRT7_PC
.set Gea3_rx__0__PC2, CYREG_GPIO_PRT7_PC2
.set Gea3_rx__0__PORT, 7
.set Gea3_rx__0__PS, CYREG_GPIO_PRT7_PS
.set Gea3_rx__0__SHIFT, 0
.set Gea3_rx__DR, CYREG_GPIO_PRT7_DR
.set Gea3_rx__DR_CLR, CYREG_GPIO_PRT7_DR_CLR
.set Gea3_rx__DR_INV, CYREG_GPIO_PRT7_DR_INV
.set Gea3_rx__DR_SET, CYREG_GPIO_PRT7_DR_SET
.set Gea3_rx__INTCFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_rx__INTR, CYREG_GPIO_PRT7_INTR
.set Gea3_rx__INTR_CFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_rx__INTSTAT, CYREG_GPIO_PRT7_INTR
.set Gea3_rx__MASK, 0x01
.set Gea3_rx__PC, CYREG_GPIO_PRT7_PC
.set Gea3_rx__PC2, CYREG_GPIO_PRT7_PC2
.set Gea3_rx__PORT, 7
.set Gea3_rx__PS, CYREG_GPIO_PRT7_PS
.set Gea3_rx__SHIFT, 0

/* Gea3_SCB */
.set Gea3_SCB__CTRL, CYREG_SCB3_CTRL
.set Gea3_SCB__EZ_DATA0, CYREG_SCB3_EZ_DATA0
.set Gea3_SCB__EZ_DATA1, CYREG_SCB3_EZ_DATA1
.set Gea3_SCB__EZ_DATA10, CYREG_SCB3_EZ_DATA10
.set Gea3_SCB__EZ_DATA11, CYREG_SCB3_EZ_DATA11
.set Gea3_SCB__EZ_DATA12, CYREG_SCB3_EZ_DATA12
.set Gea3_SCB__EZ_DATA13, CYREG_SCB3_EZ_DATA13
.set Gea3_SCB__EZ_DATA14, CYREG_SCB3_EZ_DATA14
.set Gea3_SCB__EZ_DATA15, CYREG_SCB3_EZ_DATA15
.set Gea3_SCB__EZ_DATA16, CYREG_SCB3_EZ_DATA16
.set Gea3_SCB__EZ_DATA17, CYREG_SCB3_EZ_DATA17
.set Gea3_SCB__EZ_DATA18, CYREG_SCB3_EZ_DATA18
.set Gea3_SCB__EZ_DATA19, CYREG_SCB3_EZ_DATA19
.set Gea3_SCB__EZ_DATA2, CYREG_SCB3_EZ_DATA2
.set Gea3_SCB__EZ_DATA20, CYREG_SCB3_EZ_DATA20
.set Gea3_SCB__EZ_DATA21, CYREG_SCB3_EZ_DATA21
.set Gea3_SCB__EZ_DATA22, CYREG_SCB3_EZ_DATA22
.set Gea3_SCB__EZ_DATA23, CYREG_SCB3_EZ_DATA23
.set Gea3_SCB__EZ_DATA24, CYREG_SCB3_EZ_DATA24
.set Gea3_SCB__EZ_DATA25, CYREG_SCB3_EZ_DATA25
.set Gea3_SCB__EZ_DATA26, CYREG_SCB3_EZ_DATA26
.set Gea3_SCB__EZ_DATA27, CYREG_SCB3_EZ_DATA27
.set Gea3_SCB__EZ_DATA28, CYREG_SCB3_EZ_DATA28
.set Gea3_SCB__EZ_DATA29, CYREG_SCB3_EZ_DATA29
.set Gea3_SCB__EZ_DATA3, CYREG_SCB3_EZ_DATA3
.set Gea3_SCB__EZ_DATA30, CYREG_SCB3_EZ_DATA30
.set Gea3_SCB__EZ_DATA31, CYREG_SCB3_EZ_DATA31
.set Gea3_SCB__EZ_DATA4, CYREG_SCB3_EZ_DATA4
.set Gea3_SCB__EZ_DATA5, CYREG_SCB3_EZ_DATA5
.set Gea3_SCB__EZ_DATA6, CYREG_SCB3_EZ_DATA6
.set Gea3_SCB__EZ_DATA7, CYREG_SCB3_EZ_DATA7
.set Gea3_SCB__EZ_DATA8, CYREG_SCB3_EZ_DATA8
.set Gea3_SCB__EZ_DATA9, CYREG_SCB3_EZ_DATA9
.set Gea3_SCB__I2C_CFG, CYREG_SCB3_I2C_CFG
.set Gea3_SCB__I2C_CTRL, CYREG_SCB3_I2C_CTRL
.set Gea3_SCB__I2C_M_CMD, CYREG_SCB3_I2C_M_CMD
.set Gea3_SCB__I2C_S_CMD, CYREG_SCB3_I2C_S_CMD
.set Gea3_SCB__I2C_STATUS, CYREG_SCB3_I2C_STATUS
.set Gea3_SCB__INTR_CAUSE, CYREG_SCB3_INTR_CAUSE
.set Gea3_SCB__INTR_I2C_EC, CYREG_SCB3_INTR_I2C_EC
.set Gea3_SCB__INTR_I2C_EC_MASK, CYREG_SCB3_INTR_I2C_EC_MASK
.set Gea3_SCB__INTR_I2C_EC_MASKED, CYREG_SCB3_INTR_I2C_EC_MASKED
.set Gea3_SCB__INTR_M, CYREG_SCB3_INTR_M
.set Gea3_SCB__INTR_M_MASK, CYREG_SCB3_INTR_M_MASK
.set Gea3_SCB__INTR_M_MASKED, CYREG_SCB3_INTR_M_MASKED
.set Gea3_SCB__INTR_M_SET, CYREG_SCB3_INTR_M_SET
.set Gea3_SCB__INTR_RX, CYREG_SCB3_INTR_RX
.set Gea3_SCB__INTR_RX_MASK, CYREG_SCB3_INTR_RX_MASK
.set Gea3_SCB__INTR_RX_MASKED, CYREG_SCB3_INTR_RX_MASKED
.set Gea3_SCB__INTR_RX_SET, CYREG_SCB3_INTR_RX_SET
.set Gea3_SCB__INTR_S, CYREG_SCB3_INTR_S
.set Gea3_SCB__INTR_S_MASK, CYREG_SCB3_INTR_S_MASK
.set Gea3_SCB__INTR_S_MASKED, CYREG_SCB3_INTR_S_MASKED
.set Gea3_SCB__INTR_S_SET, CYREG_SCB3_INTR_S_SET
.set Gea3_SCB__INTR_SPI_EC, CYREG_SCB3_INTR_SPI_EC
.set Gea3_SCB__INTR_SPI_EC_MASK, CYREG_SCB3_INTR_SPI_EC_MASK
.set Gea3_SCB__INTR_SPI_EC_MASKED, CYREG_SCB3_INTR_SPI_EC_MASKED
.set Gea3_SCB__INTR_TX, CYREG_SCB3_INTR_TX
.set Gea3_SCB__INTR_TX_MASK, CYREG_SCB3_INTR_TX_MASK
.set Gea3_SCB__INTR_TX_MASKED, CYREG_SCB3_INTR_TX_MASKED
.set Gea3_SCB__INTR_TX_SET, CYREG_SCB3_INTR_TX_SET
.set Gea3_SCB__RX_CTRL, CYREG_SCB3_RX_CTRL
.set Gea3_SCB__RX_FIFO_CTRL, CYREG_SCB3_RX_FIFO_CTRL
.set Gea3_SCB__RX_FIFO_RD, CYREG_SCB3_RX_FIFO_RD
.set Gea3_SCB__RX_FIFO_RD_SILENT, CYREG_SCB3_RX_FIFO_RD_SILENT
.set Gea3_SCB__RX_FIFO_STATUS, CYREG_SCB3_RX_FIFO_STATUS
.set Gea3_SCB__RX_MATCH, CYREG_SCB3_RX_MATCH
.set Gea3_SCB__SPI_CTRL, CYREG_SCB3_SPI_CTRL
.set Gea3_SCB__SPI_STATUS, CYREG_SCB3_SPI_STATUS
.set Gea3_SCB__SS0_POSISTION, 0
.set Gea3_SCB__SS1_POSISTION, 1
.set Gea3_SCB__SS2_POSISTION, 2
.set Gea3_SCB__SS3_POSISTION, 3
.set Gea3_SCB__STATUS, CYREG_SCB3_STATUS
.set Gea3_SCB__TX_CTRL, CYREG_SCB3_TX_CTRL
.set Gea3_SCB__TX_FIFO_CTRL, CYREG_SCB3_TX_FIFO_CTRL
.set Gea3_SCB__TX_FIFO_STATUS, CYREG_SCB3_TX_FIFO_STATUS
.set Gea3_SCB__TX_FIFO_WR, CYREG_SCB3_TX_FIFO_WR
.set Gea3_SCB__UART_CTRL, CYREG_SCB3_UART_CTRL
.set Gea3_SCB__UART_FLOW_CTRL, CYREG_SCB3_UART_FLOW_CTRL
.set Gea3_SCB__UART_RX_CTRL, CYREG_SCB3_UART_RX_CTRL
.set Gea3_SCB__UART_RX_STATUS, CYREG_SCB3_UART_RX_STATUS
.set Gea3_SCB__UART_TX_CTRL, CYREG_SCB3_UART_TX_CTRL
.set Gea3_SCB_IRQ__INTC_CLR_EN_REG, CYREG_CM0P_ICER
.set Gea3_SCB_IRQ__INTC_CLR_PD_REG, CYREG_CM0P_ICPR
.set Gea3_SCB_IRQ__INTC_MASK, 0x400
.set Gea3_SCB_IRQ__INTC_NUMBER, 10
.set Gea3_SCB_IRQ__INTC_PRIOR_MASK, 0xC00000
.set Gea3_SCB_IRQ__INTC_PRIOR_NUM, 3
.set Gea3_SCB_IRQ__INTC_PRIOR_REG, CYREG_CM0P_IPR2
.set Gea3_SCB_IRQ__INTC_SET_EN_REG, CYREG_CM0P_ISER
.set Gea3_SCB_IRQ__INTC_SET_PD_REG, CYREG_CM0P_ISPR

/* Gea3_SCBCLK */
.set Gea3_SCBCLK__CTRL_REGISTER, CYREG_PERI_PCLK_CTL3
.set Gea3_SCBCLK__DIV_ID, 0x00000041
.set Gea3_SCBCLK__DIV_REGISTER, CYREG_PERI_DIV_16_CTL1
.set Gea3_SCBCLK__PA_DIV_ID, 0x000000FF

/* Miscellaneous */
.set Gea3_tx__0__DR, CYREG_GPIO_PRT7_DR
.set Gea3_tx__0__DR_CLR, CYREG_GPIO_PRT7_DR_CLR
.set Gea3_tx__0__DR_INV, CYREG_GPIO_PRT7_DR_INV
.set Gea3_tx__0__DR_SET, CYREG_GPIO_PRT7_DR_SET
.set Gea3_tx__0__HSIOM, CYREG_HSIOM_PORT_SEL7
.set Gea3_tx__0__HSIOM_GPIO, 0
.set Gea3_tx__0__HSIOM_I2C, 14
.set Gea3_tx__0__HSIOM_I2C_SDA, 14
.set Gea3_tx__0__HSIOM_MASK, 0x000000F0
.set Gea3_tx__0__HSIOM_SHIFT, 4
.set Gea3_tx__0__HSIOM_SPI, 15
.set Gea3_tx__0__HSIOM_SPI_MISO, 15
.set Gea3_tx__0__HSIOM_UART, 9
.set Gea3_tx__0__HSIOM_UART_TX, 9
.set Gea3_tx__0__INTCFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_tx__0__INTR, CYREG_GPIO_PRT7_INTR
.set Gea3_tx__0__INTR_CFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_tx__0__INTSTAT, CYREG_GPIO_PRT7_INTR
.set Gea3_tx__0__MASK, 0x02
.set Gea3_tx__0__PC, CYREG_GPIO_PRT7_PC
.set Gea3_tx__0__PC2, CYREG_GPIO_PRT7_PC2
.set Gea3_tx__0__PORT, 7
.set Gea3_tx__0__PS, CYREG_GPIO_PRT7_PS
.set Gea3_tx__0__SHIFT, 1
.set Gea3_tx__DR, CYREG_GPIO_PRT7_DR
.set Gea3_tx__DR_CLR, CYREG_GPIO_PRT7_DR_CLR
.set Gea3_tx__DR_INV, CYREG_GPIO_PRT7_DR_INV
.set Gea3_tx__DR_SET, CYREG_GPIO_PRT7_DR_SET
.set Gea3_tx__INTCFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_tx__INTR, CYREG_GPIO_PRT7_INTR
.set Gea3_tx__INTR_CFG, CYREG_GPIO_PRT7_INTR_CFG
.set Gea3_tx__INTSTAT, CYREG_GPIO_PRT7_INTR
.set Gea3_tx__MASK, 0x02
.set Gea3_tx__PC, CYREG_GPIO_PRT7_PC
.set Gea3_tx__PC2, CYREG_GPIO_PRT7_PC2
.set Gea3_tx__PORT, 7
.set Gea3_tx__PS, CYREG_GPIO_PRT7_PS
.set Gea3_tx__SHIFT, 1
.set CapTouch_CintA__0__DR, CYREG_GPIO_PRT4_DR
.set CapTouch_CintA__0__DR_CLR, CYREG_GPIO_PRT4_DR_CLR
.set CapTouch_CintA__0__DR_INV, CYREG_GPIO_PRT4_DR_INV
.set CapTouch_CintA__0__DR_SET, CYREG_GPIO_PRT4_DR_SET
.set CapTouch_CintA__0__HSIOM, CYREG_HSIOM_PORT_SEL4
.set CapTouch_CintA__0__HSIOM_MASK, 0x00000F00
.set CapTouch_CintA__0__HSIOM_SHIFT, 8
.set CapTouch_CintA__0__INTCFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintA__0__INTR, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintA__0__INTR_CFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintA__0__INTSTAT, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintA__0__MASK, 0x04
.set CapTouch_CintA__0__PC, CYREG_GPIO_PRT4_PC
.set CapTouch_CintA__0__PC2, CYREG_GPIO_PRT4_PC2
.set CapTouch_CintA__0__PORT, 4
.set CapTouch_CintA__0__PS, CYREG_GPIO_PRT4_PS
.set CapTouch_CintA__0__SHIFT, 2
.set CapTouch_CintA__DR, CYREG_GPIO_PRT4_DR
.set CapTouch_CintA__DR_CLR, CYREG_GPIO_PRT4_DR_CLR
.set CapTouch_CintA__DR_INV, CYREG_GPIO_PRT4_DR_INV
.set CapTouch_CintA__DR_SET, CYREG_GPIO_PRT4_DR_SET
.set CapTouch_CintA__INTCFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintA__INTR, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintA__INTR_CFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintA__INTSTAT, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintA__MASK, 0x04
.set CapTouch_CintA__PC, CYREG_GPIO_PRT4_PC
.set CapTouch_CintA__PC2, CYREG_GPIO_PRT4_PC2
.set CapTouch_CintA__PORT, 4
.set CapTouch_CintA__PS, CYREG_GPIO_PRT4_PS
.set CapTouch_CintA__SHIFT, 2
.set CapTouch_CintB__0__DR, CYREG_GPIO_PRT4_DR
.set CapTouch_CintB__0__DR_CLR, CYREG_GPIO_PRT4_DR_CLR
.set CapTouch_CintB__0__DR_INV, CYREG_GPIO_PRT4_DR_INV
.set CapTouch_CintB__0__DR_SET, CYREG_GPIO_PRT4_DR_SET
.set CapTouch_CintB__0__HSIOM, CYREG_HSIOM_PORT_SEL4
.set CapTouch_CintB__0__HSIOM_MASK, 0x0000F000
.set CapTouch_CintB__0__HSIOM_SHIFT, 12
.set CapTouch_CintB__0__INTCFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintB__0__INTR, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintB__0__INTR_CFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintB__0__INTSTAT, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintB__0__MASK, 0x08
.set CapTouch_CintB__0__PC, CYREG_GPIO_PRT4_PC
.set CapTouch_CintB__0__PC2, CYREG_GPIO_PRT4_PC2
.set CapTouch_CintB__0__PORT, 4
.set CapTouch_CintB__0__PS, CYREG_GPIO_PRT4_PS
.set CapTouch_CintB__0__SHIFT, 3
.set CapTouch_CintB__DR, CYREG_GPIO_PRT4_DR
.set CapTouch_CintB__DR_CLR, CYREG_GPIO_PRT4_DR_CLR
.set CapTouch_CintB__DR_INV, CYREG_GPIO_PRT4_DR_INV
.set CapTouch_CintB__DR_SET, CYREG_GPIO_PRT4_DR_SET
.set CapTouch_CintB__INTCFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintB__INTR, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintB__INTR_CFG, CYREG_GPIO_PRT4_INTR_CFG
.set CapTouch_CintB__INTSTAT, CYREG_GPIO_PRT4_INTR
.set CapTouch_CintB__MASK, 0x08
.set CapTouch_CintB__PC, CYREG_GPIO_PRT4_PC
.set CapTouch_CintB__PC2, CYREG_GPIO_PRT4_PC2
.set CapTouch_CintB__PORT, 4
.set CapTouch_CintB__PS, CYREG_GPIO_PRT4_PS
.set CapTouch_CintB__SHIFT, 3
.set CapTouch_CSD__ADC_CTL, CYREG_CSD_ADC_CTL
.set CapTouch_CSD__CMOD_PAD, 1
.set CapTouch_CSD__CSD_CONFIG, CYREG_CSD_CONFIG
.set CapTouch_CSD__CSD_INTR, CYREG_CSD_INTR
.set CapTouch_CSD__CSD_INTR_SET, CYREG_CSD_INTR_SET
.set CapTouch_CSD__CSD_NUMBER, 0
.set CapTouch_CSD__CSD_STATUS, CYREG_CSD_STATUS
.set CapTouch_CSD__CSDCMP, CYREG_CSD_CSDCMP
.set CapTouch_CSD__CSH_TANK_PAD, 2
.set CapTouch_CSD__CSHIELD_PAD, 4
.set CapTouch_CSD__HSCMP, CYREG_CSD_HSCMP
.set CapTouch_CSD__INTR_MASK, CYREG_CSD_INTR_MASK
.set CapTouch_CSD__REFGEN, CYREG_CSD_REFGEN
.set CapTouch_CSD__RESULT_VAL1, CYREG_CSD_RESULT_VAL1
.set CapTouch_CSD__RESULT_VAL2, CYREG_CSD_RESULT_VAL2
.set CapTouch_CSD__SENSE_DUTY, CYREG_CSD_SENSE_DUTY
.set CapTouch_CSD__SENSE_PERIOD, CYREG_CSD_SENSE_PERIOD
.set CapTouch_CSD__SEQ_INIT_CNT, CYREG_CSD_SEQ_INIT_CNT
.set CapTouch_CSD__SEQ_NORM_CNT, CYREG_CSD_SEQ_NORM_CNT
.set CapTouch_CSD__SEQ_START, CYREG_CSD_SEQ_START
.set CapTouch_CSD__SEQ_TIME, CYREG_CSD_SEQ_TIME
.set CapTouch_CSD__SW_AMUXBUF_SEL, CYREG_CSD_SW_AMUXBUF_SEL
.set CapTouch_CSD__SW_BYP_SEL, CYREG_CSD_SW_BYP_SEL
.set CapTouch_CSD__SW_CMP_N_SEL, CYREG_CSD_SW_CMP_N_SEL
.set CapTouch_CSD__SW_CMP_P_SEL, CYREG_CSD_SW_CMP_P_SEL
.set CapTouch_CSD__SW_DSI_SEL, CYREG_CSD_SW_DSI_SEL
.set CapTouch_CSD__SW_FW_MOD_SEL, CYREG_CSD_SW_FW_MOD_SEL
.set CapTouch_CSD__SW_FW_TANK_SEL, CYREG_CSD_SW_FW_TANK_SEL
.set CapTouch_CSD__SW_HS_N_SEL, CYREG_CSD_SW_HS_N_SEL
.set CapTouch_CSD__SW_HS_P_SEL, CYREG_CSD_SW_HS_P_SEL
.set CapTouch_CSD__SW_REFGEN_SEL, CYREG_CSD_SW_REFGEN_SEL
.set CapTouch_CSD__SW_RES, CYREG_CSD_SW_RES
.set CapTouch_CSD__SW_SHIELD_SEL, CYREG_CSD_SW_SHIELD_SEL
.set CapTouch_CSD__VREF_EXT_PAD, 8
.set CapTouch_IDACMod__CONFIG, CYREG_CSD_CONFIG
.set CapTouch_IDACMod__IDAC, CYREG_CSD_IDACA
.set CapTouch_IDACMod__POSITION, 0
.set CapTouch_ISR__INTC_CLR_EN_REG, CYREG_CM0P_ICER
.set CapTouch_ISR__INTC_CLR_PD_REG, CYREG_CM0P_ICPR
.set CapTouch_ISR__INTC_MASK, 0x10000
.set CapTouch_ISR__INTC_NUMBER, 16
.set CapTouch_ISR__INTC_PRIOR_MASK, 0xC0
.set CapTouch_ISR__INTC_PRIOR_NUM, 3
.set CapTouch_ISR__INTC_PRIOR_REG, CYREG_CM0P_IPR4
.set CapTouch_ISR__INTC_SET_EN_REG, CYREG_CM0P_ISER
.set CapTouch_ISR__INTC_SET_PD_REG, CYREG_CM0P_ISPR
.set CapTouch_ModClk__CTRL_REGISTER, CYREG_PERI_PCLK_CTL5
.set CapTouch_ModClk__DIV_ID, 0x00000040
.set CapTouch_ModClk__DIV_REGISTER, CYREG_PERI_DIV_16_CTL0
.set CapTouch_ModClk__PA_DIV_ID, 0x000000FF
.set CapTouch_Rx__0__DR, CYREG_GPIO_PRT1_DR
.set CapTouch_Rx__0__DR_CLR, CYREG_GPIO_PRT1_DR_CLR
.set CapTouch_Rx__0__DR_INV, CYREG_GPIO_PRT1_DR_INV
.set CapTouch_Rx__0__DR_SET, CYREG_GPIO_PRT1_DR_SET
.set CapTouch_Rx__0__HSIOM, CYREG_HSIOM_PORT_SEL1
.set CapTouch_Rx__0__HSIOM_MASK, 0xF0000000
.set CapTouch_Rx__0__HSIOM_SHIFT, 28
.set CapTouch_Rx__0__INTCFG, CYREG_GPIO_PRT1_INTR_CFG
.set CapTouch_Rx__0__INTR, CYREG_GPIO_PRT1_INTR
.set CapTouch_Rx__0__INTR_CFG, CYREG_GPIO_PRT1_INTR_CFG
.set CapTouch_Rx__0__INTSTAT, CYREG_GPIO_PRT1_INTR
.set CapTouch_Rx__0__MASK, 0x80
.set CapTouch_Rx__0__PC, CYREG_GPIO_PRT1_PC
.set CapTouch_Rx__0__PC2, CYREG_GPIO_PRT1_PC2
.set CapTouch_Rx__0__PORT, 1
.set CapTouch_Rx__0__PS, CYREG_GPIO_PRT1_PS
.set CapTouch_Rx__0__SHIFT, 7
.set CapTouch_Rx__1__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__1__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__1__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__1__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__1__HSIOM, CYREG_HSIOM_PORT_SEL2
.set CapTouch_Rx__1__HSIOM_MASK, 0x0000000F
.set CapTouch_Rx__1__HSIOM_SHIFT, 0
.set CapTouch_Rx__1__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__1__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__1__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__1__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__1__MASK, 0x01
.set CapTouch_Rx__1__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__1__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__1__PORT, 2
.set CapTouch_Rx__1__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__1__SHIFT, 0
.set CapTouch_Rx__2__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__2__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__2__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__2__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__2__HSIOM, CYREG_HSIOM_PORT_SEL2
.set CapTouch_Rx__2__HSIOM_MASK, 0x000000F0
.set CapTouch_Rx__2__HSIOM_SHIFT, 4
.set CapTouch_Rx__2__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__2__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__2__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__2__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__2__MASK, 0x02
.set CapTouch_Rx__2__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__2__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__2__PORT, 2
.set CapTouch_Rx__2__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__2__SHIFT, 1
.set CapTouch_Rx__3__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__3__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__3__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__3__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__3__HSIOM, CYREG_HSIOM_PORT_SEL2
.set CapTouch_Rx__3__HSIOM_MASK, 0x0F000000
.set CapTouch_Rx__3__HSIOM_SHIFT, 24
.set CapTouch_Rx__3__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__3__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__3__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__3__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__3__MASK, 0x40
.set CapTouch_Rx__3__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__3__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__3__PORT, 2
.set CapTouch_Rx__3__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__3__SHIFT, 6
.set CapTouch_Rx__4__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__4__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__4__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__4__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__4__HSIOM, CYREG_HSIOM_PORT_SEL2
.set CapTouch_Rx__4__HSIOM_MASK, 0xF0000000
.set CapTouch_Rx__4__HSIOM_SHIFT, 28
.set CapTouch_Rx__4__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__4__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__4__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__4__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__4__MASK, 0x80
.set CapTouch_Rx__4__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__4__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__4__PORT, 2
.set CapTouch_Rx__4__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__4__SHIFT, 7
.set CapTouch_Rx__U102_Rx0__DR, CYREG_GPIO_PRT1_DR
.set CapTouch_Rx__U102_Rx0__DR_CLR, CYREG_GPIO_PRT1_DR_CLR
.set CapTouch_Rx__U102_Rx0__DR_INV, CYREG_GPIO_PRT1_DR_INV
.set CapTouch_Rx__U102_Rx0__DR_SET, CYREG_GPIO_PRT1_DR_SET
.set CapTouch_Rx__U102_Rx0__INTCFG, CYREG_GPIO_PRT1_INTR_CFG
.set CapTouch_Rx__U102_Rx0__INTR, CYREG_GPIO_PRT1_INTR
.set CapTouch_Rx__U102_Rx0__INTR_CFG, CYREG_GPIO_PRT1_INTR_CFG
.set CapTouch_Rx__U102_Rx0__INTSTAT, CYREG_GPIO_PRT1_INTR
.set CapTouch_Rx__U102_Rx0__MASK, 0x80
.set CapTouch_Rx__U102_Rx0__PC, CYREG_GPIO_PRT1_PC
.set CapTouch_Rx__U102_Rx0__PC2, CYREG_GPIO_PRT1_PC2
.set CapTouch_Rx__U102_Rx0__PORT, 1
.set CapTouch_Rx__U102_Rx0__PS, CYREG_GPIO_PRT1_PS
.set CapTouch_Rx__U102_Rx0__SHIFT, 7
.set CapTouch_Rx__U105_Rx0__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__U105_Rx0__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__U105_Rx0__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__U105_Rx0__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__U105_Rx0__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U105_Rx0__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U105_Rx0__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U105_Rx0__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U105_Rx0__MASK, 0x01
.set CapTouch_Rx__U105_Rx0__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__U105_Rx0__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__U105_Rx0__PORT, 2
.set CapTouch_Rx__U105_Rx0__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__U105_Rx0__SHIFT, 0
.set CapTouch_Rx__U108_Rx0__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__U108_Rx0__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__U108_Rx0__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__U108_Rx0__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__U108_Rx0__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U108_Rx0__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U108_Rx0__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U108_Rx0__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U108_Rx0__MASK, 0x02
.set CapTouch_Rx__U108_Rx0__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__U108_Rx0__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__U108_Rx0__PORT, 2
.set CapTouch_Rx__U108_Rx0__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__U108_Rx0__SHIFT, 1
.set CapTouch_Rx__U111_Rx0__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__U111_Rx0__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__U111_Rx0__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__U111_Rx0__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__U111_Rx0__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U111_Rx0__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U111_Rx0__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U111_Rx0__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U111_Rx0__MASK, 0x40
.set CapTouch_Rx__U111_Rx0__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__U111_Rx0__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__U111_Rx0__PORT, 2
.set CapTouch_Rx__U111_Rx0__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__U111_Rx0__SHIFT, 6
.set CapTouch_Rx__U113_Rx0__DR, CYREG_GPIO_PRT2_DR
.set CapTouch_Rx__U113_Rx0__DR_CLR, CYREG_GPIO_PRT2_DR_CLR
.set CapTouch_Rx__U113_Rx0__DR_INV, CYREG_GPIO_PRT2_DR_INV
.set CapTouch_Rx__U113_Rx0__DR_SET, CYREG_GPIO_PRT2_DR_SET
.set CapTouch_Rx__U113_Rx0__INTCFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U113_Rx0__INTR, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U113_Rx0__INTR_CFG, CYREG_GPIO_PRT2_INTR_CFG
.set CapTouch_Rx__U113_Rx0__INTSTAT, CYREG_GPIO_PRT2_INTR
.set CapTouch_Rx__U113_Rx0__MASK, 0x80
.set CapTouch_Rx__U113_Rx0__PC, CYREG_GPIO_PRT2_PC
.set CapTouch_Rx__U113_Rx0__PC2, CYREG_GPIO_PRT2_PC2
.set CapTouch_Rx__U113_Rx0__PORT, 2
.set CapTouch_Rx__U113_Rx0__PS, CYREG_GPIO_PRT2_PS
.set CapTouch_Rx__U113_Rx0__SHIFT, 7
.set CapTouch_Tx__0__DR, CYREG_GPIO_PRT0_DR
.set CapTouch_Tx__0__DR_CLR, CYREG_GPIO_PRT0_DR_CLR
.set CapTouch_Tx__0__DR_INV, CYREG_GPIO_PRT0_DR_INV
.set CapTouch_Tx__0__DR_SET, CYREG_GPIO_PRT0_DR_SET
.set CapTouch_Tx__0__HSIOM, CYREG_HSIOM_PORT_SEL0
.set CapTouch_Tx__0__HSIOM_MASK, 0x0F000000
.set CapTouch_Tx__0__HSIOM_SHIFT, 24
.set CapTouch_Tx__0__INTCFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__0__INTR, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__0__INTR_CFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__0__INTSTAT, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__0__MASK, 0x40
.set CapTouch_Tx__0__PC, CYREG_GPIO_PRT0_PC
.set CapTouch_Tx__0__PC2, CYREG_GPIO_PRT0_PC2
.set CapTouch_Tx__0__PORT, 0
.set CapTouch_Tx__0__PS, CYREG_GPIO_PRT0_PS
.set CapTouch_Tx__0__SHIFT, 6
.set CapTouch_Tx__1__DR, CYREG_GPIO_PRT0_DR
.set CapTouch_Tx__1__DR_CLR, CYREG_GPIO_PRT0_DR_CLR
.set CapTouch_Tx__1__DR_INV, CYREG_GPIO_PRT0_DR_INV
.set CapTouch_Tx__1__DR_SET, CYREG_GPIO_PRT0_DR_SET
.set CapTouch_Tx__1__HSIOM, CYREG_HSIOM_PORT_SEL0
.set CapTouch_Tx__1__HSIOM_MASK, 0x000000F0
.set CapTouch_Tx__1__HSIOM_SHIFT, 4
.set CapTouch_Tx__1__INTCFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__1__INTR, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__1__INTR_CFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__1__INTSTAT, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__1__MASK, 0x02
.set CapTouch_Tx__1__PC, CYREG_GPIO_PRT0_PC
.set CapTouch_Tx__1__PC2, CYREG_GPIO_PRT0_PC2
.set CapTouch_Tx__1__PORT, 0
.set CapTouch_Tx__1__PS, CYREG_GPIO_PRT0_PS
.set CapTouch_Tx__1__SHIFT, 1
.set CapTouch_Tx__2__DR, CYREG_GPIO_PRT0_DR
.set CapTouch_Tx__2__DR_CLR, CYREG_GPIO_PRT0_DR_CLR
.set CapTouch_Tx__2__DR_INV, CYREG_GPIO_PRT0_DR_INV
.set CapTouch_Tx__2__DR_SET, CYREG_GPIO_PRT0_DR_SET
.set CapTouch_Tx__2__HSIOM, CYREG_HSIOM_PORT_SEL0
.set CapTouch_Tx__2__HSIOM_MASK, 0x0000000F
.set CapTouch_Tx__2__HSIOM_SHIFT, 0
.set CapTouch_Tx__2__INTCFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__2__INTR, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__2__INTR_CFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__2__INTSTAT, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__2__MASK, 0x01
.set CapTouch_Tx__2__PC, CYREG_GPIO_PRT0_PC
.set CapTouch_Tx__2__PC2, CYREG_GPIO_PRT0_PC2
.set CapTouch_Tx__2__PORT, 0
.set CapTouch_Tx__2__PS, CYREG_GPIO_PRT0_PS
.set CapTouch_Tx__2__SHIFT, 0
.set CapTouch_Tx__DR, CYREG_GPIO_PRT0_DR
.set CapTouch_Tx__DR_CLR, CYREG_GPIO_PRT0_DR_CLR
.set CapTouch_Tx__DR_INV, CYREG_GPIO_PRT0_DR_INV
.set CapTouch_Tx__DR_SET, CYREG_GPIO_PRT0_DR_SET
.set CapTouch_Tx__INTCFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__INTR, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__INTR_CFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__INTSTAT, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__PC, CYREG_GPIO_PRT0_PC
.set CapTouch_Tx__PC2, CYREG_GPIO_PRT0_PC2
.set CapTouch_Tx__PORT, 0
.set CapTouch_Tx__PS, CYREG_GPIO_PRT0_PS
.set CapTouch_Tx__U102_Tx__DR, CYREG_GPIO_PRT0_DR
.set CapTouch_Tx__U102_Tx__DR_CLR, CYREG_GPIO_PRT0_DR_CLR
.set CapTouch_Tx__U102_Tx__DR_INV, CYREG_GPIO_PRT0_DR_INV
.set CapTouch_Tx__U102_Tx__DR_SET, CYREG_GPIO_PRT0_DR_SET
.set CapTouch_Tx__U102_Tx__INTCFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__U102_Tx__INTR, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__U102_Tx__INTR_CFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__U102_Tx__INTSTAT, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__U102_Tx__MASK, 0x40
.set CapTouch_Tx__U102_Tx__PC, CYREG_GPIO_PRT0_PC
.set CapTouch_Tx__U102_Tx__PC2, CYREG_GPIO_PRT0_PC2
.set CapTouch_Tx__U102_Tx__PORT, 0
.set CapTouch_Tx__U102_Tx__PS, CYREG_GPIO_PRT0_PS
.set CapTouch_Tx__U102_Tx__SHIFT, 6
.set CapTouch_Tx__U103_Tx__DR, CYREG_GPIO_PRT0_DR
.set CapTouch_Tx__U103_Tx__DR_CLR, CYREG_GPIO_PRT0_DR_CLR
.set CapTouch_Tx__U103_Tx__DR_INV, CYREG_GPIO_PRT0_DR_INV
.set CapTouch_Tx__U103_Tx__DR_SET, CYREG_GPIO_PRT0_DR_SET
.set CapTouch_Tx__U103_Tx__INTCFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__U103_Tx__INTR, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__U103_Tx__INTR_CFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__U103_Tx__INTSTAT, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__U103_Tx__MASK, 0x02
.set CapTouch_Tx__U103_Tx__PC, CYREG_GPIO_PRT0_PC
.set CapTouch_Tx__U103_Tx__PC2, CYREG_GPIO_PRT0_PC2
.set CapTouch_Tx__U103_Tx__PORT, 0
.set CapTouch_Tx__U103_Tx__PS, CYREG_GPIO_PRT0_PS
.set CapTouch_Tx__U103_Tx__SHIFT, 1
.set CapTouch_Tx__U104_Tx__DR, CYREG_GPIO_PRT0_DR
.set CapTouch_Tx__U104_Tx__DR_CLR, CYREG_GPIO_PRT0_DR_CLR
.set CapTouch_Tx__U104_Tx__DR_INV, CYREG_GPIO_PRT0_DR_INV
.set CapTouch_Tx__U104_Tx__DR_SET, CYREG_GPIO_PRT0_DR_SET
.set CapTouch_Tx__U104_Tx__INTCFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__U104_Tx__INTR, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__U104_Tx__INTR_CFG, CYREG_GPIO_PRT0_INTR_CFG
.set CapTouch_Tx__U104_Tx__INTSTAT, CYREG_GPIO_PRT0_INTR
.set CapTouch_Tx__U104_Tx__MASK, 0x01
.set CapTouch_Tx__U104_Tx__PC, CYREG_GPIO_PRT0_PC
.set CapTouch_Tx__U104_Tx__PC2, CYREG_GPIO_PRT0_PC2
.set CapTouch_Tx__U104_Tx__PORT, 0
.set CapTouch_Tx__U104_Tx__PS, CYREG_GPIO_PRT0_PS
.set CapTouch_Tx__U104_Tx__SHIFT, 0
.set CYDEV_BCLK__HFCLK__HZ, 24000000
.set CYDEV_BCLK__HFCLK__KHZ, 24000
.set CYDEV_BCLK__HFCLK__MHZ, 24
.set CYDEV_BCLK__SYSCLK__HZ, 24000000
.set CYDEV_BCLK__SYSCLK__KHZ, 24000
.set CYDEV_BCLK__SYSCLK__MHZ, 24
.set CYDEV_CHIP_DIE_LEOPARD, 1
.set CYDEV_CHIP_DIE_PSOC4A, 18
.set CYDEV_CHIP_DIE_PSOC5LP, 2
.set CYDEV_CHIP_DIE_PSOC5TM, 3
.set CYDEV_CHIP_DIE_TMA4, 4
.set CYDEV_CHIP_DIE_UNKNOWN, 0
.set CYDEV_CHIP_FAMILY_FM0P, 5
.set CYDEV_CHIP_FAMILY_FM3, 6
.set CYDEV_CHIP_FAMILY_FM4, 7
.set CYDEV_CHIP_FAMILY_PSOC3, 1
.set CYDEV_CHIP_FAMILY_PSOC4, 2
.set CYDEV_CHIP_FAMILY_PSOC5, 3
.set CYDEV_CHIP_FAMILY_PSOC6, 4
.set CYDEV_CHIP_FAMILY_UNKNOWN, 0
.set CYDEV_CHIP_FAMILY_USED, CYDEV_CHIP_FAMILY_PSOC4
.set CYDEV_CHIP_JTAG_ID, 0x255011B5
.set CYDEV_CHIP_MEMBER_3A, 1
.set CYDEV_CHIP_MEMBER_4A, 18
.set CYDEV_CHIP_MEMBER_4D, 13
.set CYDEV_CHIP_MEMBER_4E, 6
.set CYDEV_CHIP_MEMBER_4F, 19
.set CYDEV_CHIP_MEMBER_4G, 4
.set CYDEV_CHIP_MEMBER_4H, 17
.set CYDEV_CHIP_MEMBER_4I, 23
.set CYDEV_CHIP_MEMBER_4J, 14
.set CYDEV_CHIP_MEMBER_4K, 15
.set CYDEV_CHIP_MEMBER_4L, 22
.set CYDEV_CHIP_MEMBER_4M, 21
.set CYDEV_CHIP_MEMBER_4N, 10
.set CYDEV_CHIP_MEMBER_4O, 7
.set CYDEV_CHIP_MEMBER_4P, 20
.set CYDEV_CHIP_MEMBER_4Q, 12
.set CYDEV_CHIP_MEMBER_4R, 8
.set CYDEV_CHIP_MEMBER_4S, 11
.set CYDEV_CHIP_MEMBER_4T, 9
.set CYDEV_CHIP_MEMBER_4U, 5
.set CYDEV_CHIP_MEMBER_4V, 16
.set CYDEV_CHIP_MEMBER_5A, 3
.set CYDEV_CHIP_MEMBER_5B, 2
.set CYDEV_CHIP_MEMBER_6A, 24
.set CYDEV_CHIP_MEMBER_FM3, 28
.set CYDEV_CHIP_MEMBER_FM4, 29
.set CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE1, 25
.set CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE2, 26
.set CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE3, 27
.set CYDEV_CHIP_MEMBER_UNKNOWN, 0
.set CYDEV_CHIP_MEMBER_USED, CYDEV_CHIP_MEMBER_4V
.set CYDEV_CHIP_DIE_EXPECT, CYDEV_CHIP_MEMBER_USED
.set CYDEV_CHIP_DIE_ACTUAL, CYDEV_CHIP_DIE_EXPECT
.set CYDEV_CHIP_REV_LEOPARD_ES1, 0
.set CYDEV_CHIP_REV_LEOPARD_ES2, 1
.set CYDEV_CHIP_REV_LEOPARD_ES3, 3
.set CYDEV_CHIP_REV_LEOPARD_PRODUCTION, 3
.set CYDEV_CHIP_REV_PSOC4A_ES0, 17
.set CYDEV_CHIP_REV_PSOC4A_PRODUCTION, 17
.set CYDEV_CHIP_REV_PSOC5LP_ES0, 0
.set CYDEV_CHIP_REV_PSOC5LP_PRODUCTION, 0
.set CYDEV_CHIP_REV_PSOC5TM_ES0, 0
.set CYDEV_CHIP_REV_PSOC5TM_ES1, 1
.set CYDEV_CHIP_REV_PSOC5TM_PRODUCTION, 1
.set CYDEV_CHIP_REV_TMA4_ES, 17
.set CYDEV_CHIP_REV_TMA4_ES2, 33
.set CYDEV_CHIP_REV_TMA4_PRODUCTION, 17
.set CYDEV_CHIP_REVISION_3A_ES1, 0
.set CYDEV_CHIP_REVISION_3A_ES2, 1
.set CYDEV_CHIP_REVISION_3A_ES3, 3
.set CYDEV_CHIP_REVISION_3A_PRODUCTION, 3
.set CYDEV_CHIP_REVISION_4A_ES0, 17
.set CYDEV_CHIP_REVISION_4A_PRODUCTION, 17
.set CYDEV_CHIP_REVISION_4D_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4E_CCG2_NO_USBPD, 0
.set CYDEV_CHIP_REVISION_4E_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4F_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4F_PRODUCTION_256DMA, 0
.set CYDEV_CHIP_REVISION_4F_PRODUCTION_256K, 0
.set CYDEV_CHIP_REVISION_4G_ES, 17
.set CYDEV_CHIP_REVISION_4G_ES2, 33
.set CYDEV_CHIP_REVISION_4G_PRODUCTION, 17
.set CYDEV_CHIP_REVISION_4H_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4I_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4J_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4K_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4L_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4M_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4N_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4O_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4P_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4Q_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4R_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4S_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4T_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4U_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_4V_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_5A_ES0, 0
.set CYDEV_CHIP_REVISION_5A_ES1, 1
.set CYDEV_CHIP_REVISION_5A_PRODUCTION, 1
.set CYDEV_CHIP_REVISION_5B_ES0, 0
.set CYDEV_CHIP_REVISION_5B_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_6A_ES, 17
.set CYDEV_CHIP_REVISION_6A_NO_UDB, 33
.set CYDEV_CHIP_REVISION_6A_PRODUCTION, 33
.set CYDEV_CHIP_REVISION_FM3_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_FM4_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_PDL_FM0P_TYPE1_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_PDL_FM0P_TYPE2_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_PDL_FM0P_TYPE3_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_USED, CYDEV_CHIP_REVISION_4V_PRODUCTION
.set CYDEV_CHIP_REV_EXPECT, CYDEV_CHIP_REVISION_USED
.set CYDEV_CONFIG_READ_ACCELERATOR, 1
.set CYDEV_CONFIG_UNUSED_IO_AllowButWarn, 0
.set CYDEV_CONFIG_UNUSED_IO_AllowWithInfo, 1
.set CYDEV_CONFIG_UNUSED_IO_Disallowed, 2
.set CYDEV_CONFIG_UNUSED_IO, CYDEV_CONFIG_UNUSED_IO_Disallowed
.set CYDEV_CONFIGURATION_COMPRESSED, 1
.set CYDEV_CONFIGURATION_MODE_COMPRESSED, 0
.set CYDEV_CONFIGURATION_MODE, CYDEV_CONFIGURATION_MODE_COMPRESSED
.set CYDEV_CONFIGURATION_MODE_DMA, 2
.set CYDEV_CONFIGURATION_MODE_UNCOMPRESSED, 1
.set CYDEV_DEBUG_PROTECT_KILL, 4
.set CYDEV_DEBUG_PROTECT_OPEN, 1
.set CYDEV_DEBUG_PROTECT, CYDEV_DEBUG_PROTECT_OPEN
.set CYDEV_DEBUG_PROTECT_PROTECTED, 2
.set CYDEV_DEBUGGING_DPS_Disable, 3
.set CYDEV_DEBUGGING_DPS_SWD, 2
.set CYDEV_DEBUGGING_DPS, CYDEV_DEBUGGING_DPS_SWD
.set CYDEV_DEBUGGING_ENABLE, 1
.set CYDEV_DFT_SELECT_CLK0, 8
.set CYDEV_DFT_SELECT_CLK1, 9
.set CYDEV_DMA_CHANNELS_AVAILABLE, 8
.set CYDEV_HEAP_SIZE, 0x0
.set CYDEV_IMO_TRIMMED_BY_USB, 0
.set CYDEV_IMO_TRIMMED_BY_WCO, 0
.set CYDEV_INTR_NUMBER_DMA, 14
.set CYDEV_IS_EXPORTING_CODE, 0
.set CYDEV_IS_IMPORTING_CODE, 0
.set CYDEV_PROJ_TYPE, 0
.set CYDEV_PROJ_TYPE_BOOTLOADER, 1
.set CYDEV_PROJ_TYPE_LAUNCHER, 5
.set CYDEV_PROJ_TYPE_LOADABLE, 2
.set CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER, 4
.set CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER, 3
.set CYDEV_PROJ_TYPE_STANDARD, 0
.set CYDEV_STACK_SIZE, 0x0500
.set CYDEV_USE_BUNDLED_CMSIS, 1
.set CYDEV_VARIABLE_VDDA, 1
.set CYDEV_VDDA_MV, 3300
.set CYDEV_VDDD_MV, 3300
.set CYDEV_WDT_GENERATE_ISR, 1
.set CYIPBLOCK_m0s8cpussv3_VERSION, 1
.set CYIPBLOCK_m0s8crypto_VERSION, 2
.set CYIPBLOCK_m0s8csdv2_VERSION, 2
.set CYIPBLOCK_m0s8exco_VERSION, 1
.set CYIPBLOCK_m0s8ioss_VERSION, 1
.set CYIPBLOCK_m0s8lcd_VERSION, 2
.set CYIPBLOCK_m0s8lpcomp_VERSION, 2
.set CYIPBLOCK_m0s8pass4a_VERSION, 1
.set CYIPBLOCK_m0s8peri_VERSION, 1
.set CYIPBLOCK_m0s8scb_VERSION, 2
.set CYIPBLOCK_m0s8tcpwm_VERSION, 2
.set CYIPBLOCK_m0s8wco_VERSION, 1
.set CYIPBLOCK_s8srsslt_VERSION, 1
.set DMA_CHANNELS_USED__MASK, 0
.set CYDEV_BOOTLOADER_ENABLE, 0
.endif