/***************************************************************************//**
* \file Gea3_PINS.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values for the pin components
*  buried into SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PINS_Gea3_H)
#define CY_SCB_PINS_Gea3_H

#include "cydevice_trm.h"
#include "cyfitter.h"
#include "cytypes.h"


/***************************************
*   Conditional Compilation Parameters
****************************************/

/* Unconfigured pins */
#define Gea3_REMOVE_RX_WAKE_SCL_MOSI_PIN  (1u)
#define Gea3_REMOVE_RX_SCL_MOSI_PIN      (1u)
#define Gea3_REMOVE_TX_SDA_MISO_PIN      (1u)
#define Gea3_REMOVE_CTS_SCLK_PIN      (1u)
#define Gea3_REMOVE_RTS_SS0_PIN      (1u)
#define Gea3_REMOVE_SS1_PIN                 (1u)
#define Gea3_REMOVE_SS2_PIN                 (1u)
#define Gea3_REMOVE_SS3_PIN                 (1u)

/* Mode defined pins */
#define Gea3_REMOVE_I2C_PINS                (1u)
#define Gea3_REMOVE_SPI_MASTER_PINS         (1u)
#define Gea3_REMOVE_SPI_MASTER_SCLK_PIN     (1u)
#define Gea3_REMOVE_SPI_MASTER_MOSI_PIN     (1u)
#define Gea3_REMOVE_SPI_MASTER_MISO_PIN     (1u)
#define Gea3_REMOVE_SPI_MASTER_SS0_PIN      (1u)
#define Gea3_REMOVE_SPI_MASTER_SS1_PIN      (1u)
#define Gea3_REMOVE_SPI_MASTER_SS2_PIN      (1u)
#define Gea3_REMOVE_SPI_MASTER_SS3_PIN      (1u)
#define Gea3_REMOVE_SPI_SLAVE_PINS          (1u)
#define Gea3_REMOVE_SPI_SLAVE_MOSI_PIN      (1u)
#define Gea3_REMOVE_SPI_SLAVE_MISO_PIN      (1u)
#define Gea3_REMOVE_UART_TX_PIN             (0u)
#define Gea3_REMOVE_UART_RX_TX_PIN          (1u)
#define Gea3_REMOVE_UART_RX_PIN             (0u)
#define Gea3_REMOVE_UART_RX_WAKE_PIN        (1u)
#define Gea3_REMOVE_UART_RTS_PIN            (1u)
#define Gea3_REMOVE_UART_CTS_PIN            (1u)

/* Unconfigured pins */
#define Gea3_RX_WAKE_SCL_MOSI_PIN (0u == Gea3_REMOVE_RX_WAKE_SCL_MOSI_PIN)
#define Gea3_RX_SCL_MOSI_PIN     (0u == Gea3_REMOVE_RX_SCL_MOSI_PIN)
#define Gea3_TX_SDA_MISO_PIN     (0u == Gea3_REMOVE_TX_SDA_MISO_PIN)
#define Gea3_CTS_SCLK_PIN     (0u == Gea3_REMOVE_CTS_SCLK_PIN)
#define Gea3_RTS_SS0_PIN     (0u == Gea3_REMOVE_RTS_SS0_PIN)
#define Gea3_SS1_PIN                (0u == Gea3_REMOVE_SS1_PIN)
#define Gea3_SS2_PIN                (0u == Gea3_REMOVE_SS2_PIN)
#define Gea3_SS3_PIN                (0u == Gea3_REMOVE_SS3_PIN)

/* Mode defined pins */
#define Gea3_I2C_PINS               (0u == Gea3_REMOVE_I2C_PINS)
#define Gea3_SPI_MASTER_PINS        (0u == Gea3_REMOVE_SPI_MASTER_PINS)
#define Gea3_SPI_MASTER_SCLK_PIN    (0u == Gea3_REMOVE_SPI_MASTER_SCLK_PIN)
#define Gea3_SPI_MASTER_MOSI_PIN    (0u == Gea3_REMOVE_SPI_MASTER_MOSI_PIN)
#define Gea3_SPI_MASTER_MISO_PIN    (0u == Gea3_REMOVE_SPI_MASTER_MISO_PIN)
#define Gea3_SPI_MASTER_SS0_PIN     (0u == Gea3_REMOVE_SPI_MASTER_SS0_PIN)
#define Gea3_SPI_MASTER_SS1_PIN     (0u == Gea3_REMOVE_SPI_MASTER_SS1_PIN)
#define Gea3_SPI_MASTER_SS2_PIN     (0u == Gea3_REMOVE_SPI_MASTER_SS2_PIN)
#define Gea3_SPI_MASTER_SS3_PIN     (0u == Gea3_REMOVE_SPI_MASTER_SS3_PIN)
#define Gea3_SPI_SLAVE_PINS         (0u == Gea3_REMOVE_SPI_SLAVE_PINS)
#define Gea3_SPI_SLAVE_MOSI_PIN     (0u == Gea3_REMOVE_SPI_SLAVE_MOSI_PIN)
#define Gea3_SPI_SLAVE_MISO_PIN     (0u == Gea3_REMOVE_SPI_SLAVE_MISO_PIN)
#define Gea3_UART_TX_PIN            (0u == Gea3_REMOVE_UART_TX_PIN)
#define Gea3_UART_RX_TX_PIN         (0u == Gea3_REMOVE_UART_RX_TX_PIN)
#define Gea3_UART_RX_PIN            (0u == Gea3_REMOVE_UART_RX_PIN)
#define Gea3_UART_RX_WAKE_PIN       (0u == Gea3_REMOVE_UART_RX_WAKE_PIN)
#define Gea3_UART_RTS_PIN           (0u == Gea3_REMOVE_UART_RTS_PIN)
#define Gea3_UART_CTS_PIN           (0u == Gea3_REMOVE_UART_CTS_PIN)


/***************************************
*             Includes
****************************************/

#if (Gea3_RX_WAKE_SCL_MOSI_PIN)
    #include "Gea3_uart_rx_wake_i2c_scl_spi_mosi.h"
#endif /* (Gea3_RX_SCL_MOSI) */

#if (Gea3_RX_SCL_MOSI_PIN)
    #include "Gea3_uart_rx_i2c_scl_spi_mosi.h"
#endif /* (Gea3_RX_SCL_MOSI) */

#if (Gea3_TX_SDA_MISO_PIN)
    #include "Gea3_uart_tx_i2c_sda_spi_miso.h"
#endif /* (Gea3_TX_SDA_MISO) */

#if (Gea3_CTS_SCLK_PIN)
    #include "Gea3_uart_cts_spi_sclk.h"
#endif /* (Gea3_CTS_SCLK) */

#if (Gea3_RTS_SS0_PIN)
    #include "Gea3_uart_rts_spi_ss0.h"
#endif /* (Gea3_RTS_SS0_PIN) */

#if (Gea3_SS1_PIN)
    #include "Gea3_spi_ss1.h"
#endif /* (Gea3_SS1_PIN) */

#if (Gea3_SS2_PIN)
    #include "Gea3_spi_ss2.h"
#endif /* (Gea3_SS2_PIN) */

#if (Gea3_SS3_PIN)
    #include "Gea3_spi_ss3.h"
#endif /* (Gea3_SS3_PIN) */

#if (Gea3_I2C_PINS)
    #include "Gea3_scl.h"
    #include "Gea3_sda.h"
#endif /* (Gea3_I2C_PINS) */

#if (Gea3_SPI_MASTER_PINS)
#if (Gea3_SPI_MASTER_SCLK_PIN)
    #include "Gea3_sclk_m.h"
#endif /* (Gea3_SPI_MASTER_SCLK_PIN) */

#if (Gea3_SPI_MASTER_MOSI_PIN)
    #include "Gea3_mosi_m.h"
#endif /* (Gea3_SPI_MASTER_MOSI_PIN) */

#if (Gea3_SPI_MASTER_MISO_PIN)
    #include "Gea3_miso_m.h"
#endif /*(Gea3_SPI_MASTER_MISO_PIN) */
#endif /* (Gea3_SPI_MASTER_PINS) */

#if (Gea3_SPI_SLAVE_PINS)
    #include "Gea3_sclk_s.h"
    #include "Gea3_ss_s.h"

#if (Gea3_SPI_SLAVE_MOSI_PIN)
    #include "Gea3_mosi_s.h"
#endif /* (Gea3_SPI_SLAVE_MOSI_PIN) */

#if (Gea3_SPI_SLAVE_MISO_PIN)
    #include "Gea3_miso_s.h"
#endif /*(Gea3_SPI_SLAVE_MISO_PIN) */
#endif /* (Gea3_SPI_SLAVE_PINS) */

#if (Gea3_SPI_MASTER_SS0_PIN)
    #include "Gea3_ss0_m.h"
#endif /* (Gea3_SPI_MASTER_SS0_PIN) */

#if (Gea3_SPI_MASTER_SS1_PIN)
    #include "Gea3_ss1_m.h"
#endif /* (Gea3_SPI_MASTER_SS1_PIN) */

#if (Gea3_SPI_MASTER_SS2_PIN)
    #include "Gea3_ss2_m.h"
#endif /* (Gea3_SPI_MASTER_SS2_PIN) */

#if (Gea3_SPI_MASTER_SS3_PIN)
    #include "Gea3_ss3_m.h"
#endif /* (Gea3_SPI_MASTER_SS3_PIN) */

#if (Gea3_UART_TX_PIN)
    #include "Gea3_tx.h"
#endif /* (Gea3_UART_TX_PIN) */

#if (Gea3_UART_RX_TX_PIN)
    #include "Gea3_rx_tx.h"
#endif /* (Gea3_UART_RX_TX_PIN) */

#if (Gea3_UART_RX_PIN)
    #include "Gea3_rx.h"
#endif /* (Gea3_UART_RX_PIN) */

#if (Gea3_UART_RX_WAKE_PIN)
    #include "Gea3_rx_wake.h"
#endif /* (Gea3_UART_RX_WAKE_PIN) */

#if (Gea3_UART_RTS_PIN)
    #include "Gea3_rts.h"
#endif /* (Gea3_UART_RTS_PIN) */

#if (Gea3_UART_CTS_PIN)
    #include "Gea3_cts.h"
#endif /* (Gea3_UART_CTS_PIN) */


/***************************************
*              Registers
***************************************/

#if (Gea3_RX_SCL_MOSI_PIN)
    #define Gea3_RX_SCL_MOSI_HSIOM_REG   (*(reg32 *) Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM)
    #define Gea3_RX_SCL_MOSI_HSIOM_PTR   ( (reg32 *) Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM)
    
    #define Gea3_RX_SCL_MOSI_HSIOM_MASK      (Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM_MASK)
    #define Gea3_RX_SCL_MOSI_HSIOM_POS       (Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM_SHIFT)
    #define Gea3_RX_SCL_MOSI_HSIOM_SEL_GPIO  (Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM_GPIO)
    #define Gea3_RX_SCL_MOSI_HSIOM_SEL_I2C   (Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM_I2C)
    #define Gea3_RX_SCL_MOSI_HSIOM_SEL_SPI   (Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM_SPI)
    #define Gea3_RX_SCL_MOSI_HSIOM_SEL_UART  (Gea3_uart_rx_i2c_scl_spi_mosi__0__HSIOM_UART)
    
#elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG   (*(reg32 *) Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM)
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_PTR   ( (reg32 *) Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM)
    
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_MASK      (Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_MASK)
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_POS       (Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_SHIFT)
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_SEL_GPIO  (Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_GPIO)
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_SEL_I2C   (Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_I2C)
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_SEL_SPI   (Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_SPI)
    #define Gea3_RX_WAKE_SCL_MOSI_HSIOM_SEL_UART  (Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_UART)    
   
    #define Gea3_RX_WAKE_SCL_MOSI_INTCFG_REG (*(reg32 *) Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__INTCFG)
    #define Gea3_RX_WAKE_SCL_MOSI_INTCFG_PTR ( (reg32 *) Gea3_uart_rx_wake_i2c_scl_spi_mosi__0__INTCFG)
    #define Gea3_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS  (Gea3_uart_rx_wake_i2c_scl_spi_mosi__SHIFT)
    #define Gea3_RX_WAKE_SCL_MOSI_INTCFG_TYPE_MASK ((uint32) Gea3_INTCFG_TYPE_MASK << \
                                                                           Gea3_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS)
#else
    /* None of pins Gea3_RX_SCL_MOSI_PIN or Gea3_RX_WAKE_SCL_MOSI_PIN present.*/
#endif /* (Gea3_RX_SCL_MOSI_PIN) */

#if (Gea3_TX_SDA_MISO_PIN)
    #define Gea3_TX_SDA_MISO_HSIOM_REG   (*(reg32 *) Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM)
    #define Gea3_TX_SDA_MISO_HSIOM_PTR   ( (reg32 *) Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM)
    
    #define Gea3_TX_SDA_MISO_HSIOM_MASK      (Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM_MASK)
    #define Gea3_TX_SDA_MISO_HSIOM_POS       (Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM_SHIFT)
    #define Gea3_TX_SDA_MISO_HSIOM_SEL_GPIO  (Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM_GPIO)
    #define Gea3_TX_SDA_MISO_HSIOM_SEL_I2C   (Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM_I2C)
    #define Gea3_TX_SDA_MISO_HSIOM_SEL_SPI   (Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM_SPI)
    #define Gea3_TX_SDA_MISO_HSIOM_SEL_UART  (Gea3_uart_tx_i2c_sda_spi_miso__0__HSIOM_UART)
#endif /* (Gea3_TX_SDA_MISO_PIN) */

#if (Gea3_CTS_SCLK_PIN)
    #define Gea3_CTS_SCLK_HSIOM_REG   (*(reg32 *) Gea3_uart_cts_spi_sclk__0__HSIOM)
    #define Gea3_CTS_SCLK_HSIOM_PTR   ( (reg32 *) Gea3_uart_cts_spi_sclk__0__HSIOM)
    
    #define Gea3_CTS_SCLK_HSIOM_MASK      (Gea3_uart_cts_spi_sclk__0__HSIOM_MASK)
    #define Gea3_CTS_SCLK_HSIOM_POS       (Gea3_uart_cts_spi_sclk__0__HSIOM_SHIFT)
    #define Gea3_CTS_SCLK_HSIOM_SEL_GPIO  (Gea3_uart_cts_spi_sclk__0__HSIOM_GPIO)
    #define Gea3_CTS_SCLK_HSIOM_SEL_I2C   (Gea3_uart_cts_spi_sclk__0__HSIOM_I2C)
    #define Gea3_CTS_SCLK_HSIOM_SEL_SPI   (Gea3_uart_cts_spi_sclk__0__HSIOM_SPI)
    #define Gea3_CTS_SCLK_HSIOM_SEL_UART  (Gea3_uart_cts_spi_sclk__0__HSIOM_UART)
#endif /* (Gea3_CTS_SCLK_PIN) */

#if (Gea3_RTS_SS0_PIN)
    #define Gea3_RTS_SS0_HSIOM_REG   (*(reg32 *) Gea3_uart_rts_spi_ss0__0__HSIOM)
    #define Gea3_RTS_SS0_HSIOM_PTR   ( (reg32 *) Gea3_uart_rts_spi_ss0__0__HSIOM)
    
    #define Gea3_RTS_SS0_HSIOM_MASK      (Gea3_uart_rts_spi_ss0__0__HSIOM_MASK)
    #define Gea3_RTS_SS0_HSIOM_POS       (Gea3_uart_rts_spi_ss0__0__HSIOM_SHIFT)
    #define Gea3_RTS_SS0_HSIOM_SEL_GPIO  (Gea3_uart_rts_spi_ss0__0__HSIOM_GPIO)
    #define Gea3_RTS_SS0_HSIOM_SEL_I2C   (Gea3_uart_rts_spi_ss0__0__HSIOM_I2C)
    #define Gea3_RTS_SS0_HSIOM_SEL_SPI   (Gea3_uart_rts_spi_ss0__0__HSIOM_SPI)
#if !(Gea3_CY_SCBIP_V0 || Gea3_CY_SCBIP_V1)
    #define Gea3_RTS_SS0_HSIOM_SEL_UART  (Gea3_uart_rts_spi_ss0__0__HSIOM_UART)
#endif /* !(Gea3_CY_SCBIP_V0 || Gea3_CY_SCBIP_V1) */
#endif /* (Gea3_RTS_SS0_PIN) */

#if (Gea3_SS1_PIN)
    #define Gea3_SS1_HSIOM_REG  (*(reg32 *) Gea3_spi_ss1__0__HSIOM)
    #define Gea3_SS1_HSIOM_PTR  ( (reg32 *) Gea3_spi_ss1__0__HSIOM)
    
    #define Gea3_SS1_HSIOM_MASK     (Gea3_spi_ss1__0__HSIOM_MASK)
    #define Gea3_SS1_HSIOM_POS      (Gea3_spi_ss1__0__HSIOM_SHIFT)
    #define Gea3_SS1_HSIOM_SEL_GPIO (Gea3_spi_ss1__0__HSIOM_GPIO)
    #define Gea3_SS1_HSIOM_SEL_I2C  (Gea3_spi_ss1__0__HSIOM_I2C)
    #define Gea3_SS1_HSIOM_SEL_SPI  (Gea3_spi_ss1__0__HSIOM_SPI)
#endif /* (Gea3_SS1_PIN) */

#if (Gea3_SS2_PIN)
    #define Gea3_SS2_HSIOM_REG     (*(reg32 *) Gea3_spi_ss2__0__HSIOM)
    #define Gea3_SS2_HSIOM_PTR     ( (reg32 *) Gea3_spi_ss2__0__HSIOM)
    
    #define Gea3_SS2_HSIOM_MASK     (Gea3_spi_ss2__0__HSIOM_MASK)
    #define Gea3_SS2_HSIOM_POS      (Gea3_spi_ss2__0__HSIOM_SHIFT)
    #define Gea3_SS2_HSIOM_SEL_GPIO (Gea3_spi_ss2__0__HSIOM_GPIO)
    #define Gea3_SS2_HSIOM_SEL_I2C  (Gea3_spi_ss2__0__HSIOM_I2C)
    #define Gea3_SS2_HSIOM_SEL_SPI  (Gea3_spi_ss2__0__HSIOM_SPI)
#endif /* (Gea3_SS2_PIN) */

#if (Gea3_SS3_PIN)
    #define Gea3_SS3_HSIOM_REG     (*(reg32 *) Gea3_spi_ss3__0__HSIOM)
    #define Gea3_SS3_HSIOM_PTR     ( (reg32 *) Gea3_spi_ss3__0__HSIOM)
    
    #define Gea3_SS3_HSIOM_MASK     (Gea3_spi_ss3__0__HSIOM_MASK)
    #define Gea3_SS3_HSIOM_POS      (Gea3_spi_ss3__0__HSIOM_SHIFT)
    #define Gea3_SS3_HSIOM_SEL_GPIO (Gea3_spi_ss3__0__HSIOM_GPIO)
    #define Gea3_SS3_HSIOM_SEL_I2C  (Gea3_spi_ss3__0__HSIOM_I2C)
    #define Gea3_SS3_HSIOM_SEL_SPI  (Gea3_spi_ss3__0__HSIOM_SPI)
#endif /* (Gea3_SS3_PIN) */

#if (Gea3_I2C_PINS)
    #define Gea3_SCL_HSIOM_REG  (*(reg32 *) Gea3_scl__0__HSIOM)
    #define Gea3_SCL_HSIOM_PTR  ( (reg32 *) Gea3_scl__0__HSIOM)
    
    #define Gea3_SCL_HSIOM_MASK     (Gea3_scl__0__HSIOM_MASK)
    #define Gea3_SCL_HSIOM_POS      (Gea3_scl__0__HSIOM_SHIFT)
    #define Gea3_SCL_HSIOM_SEL_GPIO (Gea3_sda__0__HSIOM_GPIO)
    #define Gea3_SCL_HSIOM_SEL_I2C  (Gea3_sda__0__HSIOM_I2C)
    
    #define Gea3_SDA_HSIOM_REG  (*(reg32 *) Gea3_sda__0__HSIOM)
    #define Gea3_SDA_HSIOM_PTR  ( (reg32 *) Gea3_sda__0__HSIOM)
    
    #define Gea3_SDA_HSIOM_MASK     (Gea3_sda__0__HSIOM_MASK)
    #define Gea3_SDA_HSIOM_POS      (Gea3_sda__0__HSIOM_SHIFT)
    #define Gea3_SDA_HSIOM_SEL_GPIO (Gea3_sda__0__HSIOM_GPIO)
    #define Gea3_SDA_HSIOM_SEL_I2C  (Gea3_sda__0__HSIOM_I2C)
#endif /* (Gea3_I2C_PINS) */

#if (Gea3_SPI_SLAVE_PINS)
    #define Gea3_SCLK_S_HSIOM_REG   (*(reg32 *) Gea3_sclk_s__0__HSIOM)
    #define Gea3_SCLK_S_HSIOM_PTR   ( (reg32 *) Gea3_sclk_s__0__HSIOM)
    
    #define Gea3_SCLK_S_HSIOM_MASK      (Gea3_sclk_s__0__HSIOM_MASK)
    #define Gea3_SCLK_S_HSIOM_POS       (Gea3_sclk_s__0__HSIOM_SHIFT)
    #define Gea3_SCLK_S_HSIOM_SEL_GPIO  (Gea3_sclk_s__0__HSIOM_GPIO)
    #define Gea3_SCLK_S_HSIOM_SEL_SPI   (Gea3_sclk_s__0__HSIOM_SPI)
    
    #define Gea3_SS0_S_HSIOM_REG    (*(reg32 *) Gea3_ss0_s__0__HSIOM)
    #define Gea3_SS0_S_HSIOM_PTR    ( (reg32 *) Gea3_ss0_s__0__HSIOM)
    
    #define Gea3_SS0_S_HSIOM_MASK       (Gea3_ss0_s__0__HSIOM_MASK)
    #define Gea3_SS0_S_HSIOM_POS        (Gea3_ss0_s__0__HSIOM_SHIFT)
    #define Gea3_SS0_S_HSIOM_SEL_GPIO   (Gea3_ss0_s__0__HSIOM_GPIO)  
    #define Gea3_SS0_S_HSIOM_SEL_SPI    (Gea3_ss0_s__0__HSIOM_SPI)
#endif /* (Gea3_SPI_SLAVE_PINS) */

#if (Gea3_SPI_SLAVE_MOSI_PIN)
    #define Gea3_MOSI_S_HSIOM_REG   (*(reg32 *) Gea3_mosi_s__0__HSIOM)
    #define Gea3_MOSI_S_HSIOM_PTR   ( (reg32 *) Gea3_mosi_s__0__HSIOM)
    
    #define Gea3_MOSI_S_HSIOM_MASK      (Gea3_mosi_s__0__HSIOM_MASK)
    #define Gea3_MOSI_S_HSIOM_POS       (Gea3_mosi_s__0__HSIOM_SHIFT)
    #define Gea3_MOSI_S_HSIOM_SEL_GPIO  (Gea3_mosi_s__0__HSIOM_GPIO)
    #define Gea3_MOSI_S_HSIOM_SEL_SPI   (Gea3_mosi_s__0__HSIOM_SPI)
#endif /* (Gea3_SPI_SLAVE_MOSI_PIN) */

#if (Gea3_SPI_SLAVE_MISO_PIN)
    #define Gea3_MISO_S_HSIOM_REG   (*(reg32 *) Gea3_miso_s__0__HSIOM)
    #define Gea3_MISO_S_HSIOM_PTR   ( (reg32 *) Gea3_miso_s__0__HSIOM)
    
    #define Gea3_MISO_S_HSIOM_MASK      (Gea3_miso_s__0__HSIOM_MASK)
    #define Gea3_MISO_S_HSIOM_POS       (Gea3_miso_s__0__HSIOM_SHIFT)
    #define Gea3_MISO_S_HSIOM_SEL_GPIO  (Gea3_miso_s__0__HSIOM_GPIO)
    #define Gea3_MISO_S_HSIOM_SEL_SPI   (Gea3_miso_s__0__HSIOM_SPI)
#endif /* (Gea3_SPI_SLAVE_MISO_PIN) */

#if (Gea3_SPI_MASTER_MISO_PIN)
    #define Gea3_MISO_M_HSIOM_REG   (*(reg32 *) Gea3_miso_m__0__HSIOM)
    #define Gea3_MISO_M_HSIOM_PTR   ( (reg32 *) Gea3_miso_m__0__HSIOM)
    
    #define Gea3_MISO_M_HSIOM_MASK      (Gea3_miso_m__0__HSIOM_MASK)
    #define Gea3_MISO_M_HSIOM_POS       (Gea3_miso_m__0__HSIOM_SHIFT)
    #define Gea3_MISO_M_HSIOM_SEL_GPIO  (Gea3_miso_m__0__HSIOM_GPIO)
    #define Gea3_MISO_M_HSIOM_SEL_SPI   (Gea3_miso_m__0__HSIOM_SPI)
#endif /* (Gea3_SPI_MASTER_MISO_PIN) */

#if (Gea3_SPI_MASTER_MOSI_PIN)
    #define Gea3_MOSI_M_HSIOM_REG   (*(reg32 *) Gea3_mosi_m__0__HSIOM)
    #define Gea3_MOSI_M_HSIOM_PTR   ( (reg32 *) Gea3_mosi_m__0__HSIOM)
    
    #define Gea3_MOSI_M_HSIOM_MASK      (Gea3_mosi_m__0__HSIOM_MASK)
    #define Gea3_MOSI_M_HSIOM_POS       (Gea3_mosi_m__0__HSIOM_SHIFT)
    #define Gea3_MOSI_M_HSIOM_SEL_GPIO  (Gea3_mosi_m__0__HSIOM_GPIO)
    #define Gea3_MOSI_M_HSIOM_SEL_SPI   (Gea3_mosi_m__0__HSIOM_SPI)
#endif /* (Gea3_SPI_MASTER_MOSI_PIN) */

#if (Gea3_SPI_MASTER_SCLK_PIN)
    #define Gea3_SCLK_M_HSIOM_REG   (*(reg32 *) Gea3_sclk_m__0__HSIOM)
    #define Gea3_SCLK_M_HSIOM_PTR   ( (reg32 *) Gea3_sclk_m__0__HSIOM)
    
    #define Gea3_SCLK_M_HSIOM_MASK      (Gea3_sclk_m__0__HSIOM_MASK)
    #define Gea3_SCLK_M_HSIOM_POS       (Gea3_sclk_m__0__HSIOM_SHIFT)
    #define Gea3_SCLK_M_HSIOM_SEL_GPIO  (Gea3_sclk_m__0__HSIOM_GPIO)
    #define Gea3_SCLK_M_HSIOM_SEL_SPI   (Gea3_sclk_m__0__HSIOM_SPI)
#endif /* (Gea3_SPI_MASTER_SCLK_PIN) */

#if (Gea3_SPI_MASTER_SS0_PIN)
    #define Gea3_SS0_M_HSIOM_REG    (*(reg32 *) Gea3_ss0_m__0__HSIOM)
    #define Gea3_SS0_M_HSIOM_PTR    ( (reg32 *) Gea3_ss0_m__0__HSIOM)
    
    #define Gea3_SS0_M_HSIOM_MASK       (Gea3_ss0_m__0__HSIOM_MASK)
    #define Gea3_SS0_M_HSIOM_POS        (Gea3_ss0_m__0__HSIOM_SHIFT)
    #define Gea3_SS0_M_HSIOM_SEL_GPIO   (Gea3_ss0_m__0__HSIOM_GPIO)
    #define Gea3_SS0_M_HSIOM_SEL_SPI    (Gea3_ss0_m__0__HSIOM_SPI)
#endif /* (Gea3_SPI_MASTER_SS0_PIN) */

#if (Gea3_SPI_MASTER_SS1_PIN)
    #define Gea3_SS1_M_HSIOM_REG    (*(reg32 *) Gea3_ss1_m__0__HSIOM)
    #define Gea3_SS1_M_HSIOM_PTR    ( (reg32 *) Gea3_ss1_m__0__HSIOM)
    
    #define Gea3_SS1_M_HSIOM_MASK       (Gea3_ss1_m__0__HSIOM_MASK)
    #define Gea3_SS1_M_HSIOM_POS        (Gea3_ss1_m__0__HSIOM_SHIFT)
    #define Gea3_SS1_M_HSIOM_SEL_GPIO   (Gea3_ss1_m__0__HSIOM_GPIO)
    #define Gea3_SS1_M_HSIOM_SEL_SPI    (Gea3_ss1_m__0__HSIOM_SPI)
#endif /* (Gea3_SPI_MASTER_SS1_PIN) */

#if (Gea3_SPI_MASTER_SS2_PIN)
    #define Gea3_SS2_M_HSIOM_REG    (*(reg32 *) Gea3_ss2_m__0__HSIOM)
    #define Gea3_SS2_M_HSIOM_PTR    ( (reg32 *) Gea3_ss2_m__0__HSIOM)
    
    #define Gea3_SS2_M_HSIOM_MASK       (Gea3_ss2_m__0__HSIOM_MASK)
    #define Gea3_SS2_M_HSIOM_POS        (Gea3_ss2_m__0__HSIOM_SHIFT)
    #define Gea3_SS2_M_HSIOM_SEL_GPIO   (Gea3_ss2_m__0__HSIOM_GPIO)
    #define Gea3_SS2_M_HSIOM_SEL_SPI    (Gea3_ss2_m__0__HSIOM_SPI)
#endif /* (Gea3_SPI_MASTER_SS2_PIN) */

#if (Gea3_SPI_MASTER_SS3_PIN)
    #define Gea3_SS3_M_HSIOM_REG    (*(reg32 *) Gea3_ss3_m__0__HSIOM)
    #define Gea3_SS3_M_HSIOM_PTR    ( (reg32 *) Gea3_ss3_m__0__HSIOM)
    
    #define Gea3_SS3_M_HSIOM_MASK      (Gea3_ss3_m__0__HSIOM_MASK)
    #define Gea3_SS3_M_HSIOM_POS       (Gea3_ss3_m__0__HSIOM_SHIFT)
    #define Gea3_SS3_M_HSIOM_SEL_GPIO  (Gea3_ss3_m__0__HSIOM_GPIO)
    #define Gea3_SS3_M_HSIOM_SEL_SPI   (Gea3_ss3_m__0__HSIOM_SPI)
#endif /* (Gea3_SPI_MASTER_SS3_PIN) */

#if (Gea3_UART_RX_PIN)
    #define Gea3_RX_HSIOM_REG   (*(reg32 *) Gea3_rx__0__HSIOM)
    #define Gea3_RX_HSIOM_PTR   ( (reg32 *) Gea3_rx__0__HSIOM)
    
    #define Gea3_RX_HSIOM_MASK      (Gea3_rx__0__HSIOM_MASK)
    #define Gea3_RX_HSIOM_POS       (Gea3_rx__0__HSIOM_SHIFT)
    #define Gea3_RX_HSIOM_SEL_GPIO  (Gea3_rx__0__HSIOM_GPIO)
    #define Gea3_RX_HSIOM_SEL_UART  (Gea3_rx__0__HSIOM_UART)
#endif /* (Gea3_UART_RX_PIN) */

#if (Gea3_UART_RX_WAKE_PIN)
    #define Gea3_RX_WAKE_HSIOM_REG   (*(reg32 *) Gea3_rx_wake__0__HSIOM)
    #define Gea3_RX_WAKE_HSIOM_PTR   ( (reg32 *) Gea3_rx_wake__0__HSIOM)
    
    #define Gea3_RX_WAKE_HSIOM_MASK      (Gea3_rx_wake__0__HSIOM_MASK)
    #define Gea3_RX_WAKE_HSIOM_POS       (Gea3_rx_wake__0__HSIOM_SHIFT)
    #define Gea3_RX_WAKE_HSIOM_SEL_GPIO  (Gea3_rx_wake__0__HSIOM_GPIO)
    #define Gea3_RX_WAKE_HSIOM_SEL_UART  (Gea3_rx_wake__0__HSIOM_UART)
#endif /* (Gea3_UART_WAKE_RX_PIN) */

#if (Gea3_UART_CTS_PIN)
    #define Gea3_CTS_HSIOM_REG   (*(reg32 *) Gea3_cts__0__HSIOM)
    #define Gea3_CTS_HSIOM_PTR   ( (reg32 *) Gea3_cts__0__HSIOM)
    
    #define Gea3_CTS_HSIOM_MASK      (Gea3_cts__0__HSIOM_MASK)
    #define Gea3_CTS_HSIOM_POS       (Gea3_cts__0__HSIOM_SHIFT)
    #define Gea3_CTS_HSIOM_SEL_GPIO  (Gea3_cts__0__HSIOM_GPIO)
    #define Gea3_CTS_HSIOM_SEL_UART  (Gea3_cts__0__HSIOM_UART)
#endif /* (Gea3_UART_CTS_PIN) */

#if (Gea3_UART_TX_PIN)
    #define Gea3_TX_HSIOM_REG   (*(reg32 *) Gea3_tx__0__HSIOM)
    #define Gea3_TX_HSIOM_PTR   ( (reg32 *) Gea3_tx__0__HSIOM)
    
    #define Gea3_TX_HSIOM_MASK      (Gea3_tx__0__HSIOM_MASK)
    #define Gea3_TX_HSIOM_POS       (Gea3_tx__0__HSIOM_SHIFT)
    #define Gea3_TX_HSIOM_SEL_GPIO  (Gea3_tx__0__HSIOM_GPIO)
    #define Gea3_TX_HSIOM_SEL_UART  (Gea3_tx__0__HSIOM_UART)
#endif /* (Gea3_UART_TX_PIN) */

#if (Gea3_UART_RX_TX_PIN)
    #define Gea3_RX_TX_HSIOM_REG   (*(reg32 *) Gea3_rx_tx__0__HSIOM)
    #define Gea3_RX_TX_HSIOM_PTR   ( (reg32 *) Gea3_rx_tx__0__HSIOM)
    
    #define Gea3_RX_TX_HSIOM_MASK      (Gea3_rx_tx__0__HSIOM_MASK)
    #define Gea3_RX_TX_HSIOM_POS       (Gea3_rx_tx__0__HSIOM_SHIFT)
    #define Gea3_RX_TX_HSIOM_SEL_GPIO  (Gea3_rx_tx__0__HSIOM_GPIO)
    #define Gea3_RX_TX_HSIOM_SEL_UART  (Gea3_rx_tx__0__HSIOM_UART)
#endif /* (Gea3_UART_RX_TX_PIN) */

#if (Gea3_UART_RTS_PIN)
    #define Gea3_RTS_HSIOM_REG      (*(reg32 *) Gea3_rts__0__HSIOM)
    #define Gea3_RTS_HSIOM_PTR      ( (reg32 *) Gea3_rts__0__HSIOM)
    
    #define Gea3_RTS_HSIOM_MASK     (Gea3_rts__0__HSIOM_MASK)
    #define Gea3_RTS_HSIOM_POS      (Gea3_rts__0__HSIOM_SHIFT)    
    #define Gea3_RTS_HSIOM_SEL_GPIO (Gea3_rts__0__HSIOM_GPIO)
    #define Gea3_RTS_HSIOM_SEL_UART (Gea3_rts__0__HSIOM_UART)    
#endif /* (Gea3_UART_RTS_PIN) */


/***************************************
*        Registers Constants
***************************************/

/* HSIOM switch values. */ 
#define Gea3_HSIOM_DEF_SEL      (0x00u)
#define Gea3_HSIOM_GPIO_SEL     (0x00u)
/* The HSIOM values provided below are valid only for Gea3_CY_SCBIP_V0 
* and Gea3_CY_SCBIP_V1. It is not recommended to use them for 
* Gea3_CY_SCBIP_V2. Use pin name specific HSIOM constants provided 
* above instead for any SCB IP block version.
*/
#define Gea3_HSIOM_UART_SEL     (0x09u)
#define Gea3_HSIOM_I2C_SEL      (0x0Eu)
#define Gea3_HSIOM_SPI_SEL      (0x0Fu)

/* Pins settings index. */
#define Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX   (0u)
#define Gea3_RX_SCL_MOSI_PIN_INDEX       (0u)
#define Gea3_TX_SDA_MISO_PIN_INDEX       (1u)
#define Gea3_CTS_SCLK_PIN_INDEX       (2u)
#define Gea3_RTS_SS0_PIN_INDEX       (3u)
#define Gea3_SS1_PIN_INDEX                  (4u)
#define Gea3_SS2_PIN_INDEX                  (5u)
#define Gea3_SS3_PIN_INDEX                  (6u)

/* Pins settings mask. */
#define Gea3_RX_WAKE_SCL_MOSI_PIN_MASK ((uint32) 0x01u << Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX)
#define Gea3_RX_SCL_MOSI_PIN_MASK     ((uint32) 0x01u << Gea3_RX_SCL_MOSI_PIN_INDEX)
#define Gea3_TX_SDA_MISO_PIN_MASK     ((uint32) 0x01u << Gea3_TX_SDA_MISO_PIN_INDEX)
#define Gea3_CTS_SCLK_PIN_MASK     ((uint32) 0x01u << Gea3_CTS_SCLK_PIN_INDEX)
#define Gea3_RTS_SS0_PIN_MASK     ((uint32) 0x01u << Gea3_RTS_SS0_PIN_INDEX)
#define Gea3_SS1_PIN_MASK                ((uint32) 0x01u << Gea3_SS1_PIN_INDEX)
#define Gea3_SS2_PIN_MASK                ((uint32) 0x01u << Gea3_SS2_PIN_INDEX)
#define Gea3_SS3_PIN_MASK                ((uint32) 0x01u << Gea3_SS3_PIN_INDEX)

/* Pin interrupt constants. */
#define Gea3_INTCFG_TYPE_MASK           (0x03u)
#define Gea3_INTCFG_TYPE_FALLING_EDGE   (0x02u)

/* Pin Drive Mode constants. */
#define Gea3_PIN_DM_ALG_HIZ  (0u)
#define Gea3_PIN_DM_DIG_HIZ  (1u)
#define Gea3_PIN_DM_OD_LO    (4u)
#define Gea3_PIN_DM_STRONG   (6u)


/***************************************
*          Macro Definitions
***************************************/

/* Return drive mode of the pin */
#define Gea3_DM_MASK    (0x7u)
#define Gea3_DM_SIZE    (3u)
#define Gea3_GET_P4_PIN_DM(reg, pos) \
    ( ((reg) & (uint32) ((uint32) Gea3_DM_MASK << (Gea3_DM_SIZE * (pos)))) >> \
                                                              (Gea3_DM_SIZE * (pos)) )

#if (Gea3_TX_SDA_MISO_PIN)
    #define Gea3_CHECK_TX_SDA_MISO_PIN_USED \
                (Gea3_PIN_DM_ALG_HIZ != \
                    Gea3_GET_P4_PIN_DM(Gea3_uart_tx_i2c_sda_spi_miso_PC, \
                                                   Gea3_uart_tx_i2c_sda_spi_miso_SHIFT))
#endif /* (Gea3_TX_SDA_MISO_PIN) */

#if (Gea3_RTS_SS0_PIN)
    #define Gea3_CHECK_RTS_SS0_PIN_USED \
                (Gea3_PIN_DM_ALG_HIZ != \
                    Gea3_GET_P4_PIN_DM(Gea3_uart_rts_spi_ss0_PC, \
                                                   Gea3_uart_rts_spi_ss0_SHIFT))
#endif /* (Gea3_RTS_SS0_PIN) */

/* Set bits-mask in register */
#define Gea3_SET_REGISTER_BITS(reg, mask, pos, mode) \
                    do                                           \
                    {                                            \
                        (reg) = (((reg) & ((uint32) ~(uint32) (mask))) | ((uint32) ((uint32) (mode) << (pos)))); \
                    }while(0)

/* Set bit in the register */
#define Gea3_SET_REGISTER_BIT(reg, mask, val) \
                    ((val) ? ((reg) |= (mask)) : ((reg) &= ((uint32) ~((uint32) (mask)))))

#define Gea3_SET_HSIOM_SEL(reg, mask, pos, sel) Gea3_SET_REGISTER_BITS(reg, mask, pos, sel)
#define Gea3_SET_INCFG_TYPE(reg, mask, pos, intType) \
                                                        Gea3_SET_REGISTER_BITS(reg, mask, pos, intType)
#define Gea3_SET_INP_DIS(reg, mask, val) Gea3_SET_REGISTER_BIT(reg, mask, val)

/* Gea3_SET_I2C_SCL_DR(val) - Sets I2C SCL DR register.
*  Gea3_SET_I2C_SCL_HSIOM_SEL(sel) - Sets I2C SCL HSIOM settings.
*/
/* SCB I2C: scl signal */
#if (Gea3_CY_SCBIP_V0)
#if (Gea3_I2C_PINS)
    #define Gea3_SET_I2C_SCL_DR(val) Gea3_scl_Write(val)

    #define Gea3_SET_I2C_SCL_HSIOM_SEL(sel) \
                          Gea3_SET_HSIOM_SEL(Gea3_SCL_HSIOM_REG,  \
                                                         Gea3_SCL_HSIOM_MASK, \
                                                         Gea3_SCL_HSIOM_POS,  \
                                                         (sel))
    #define Gea3_WAIT_SCL_SET_HIGH  (0u == Gea3_scl_Read())

/* Unconfigured SCB: scl signal */
#elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
    #define Gea3_SET_I2C_SCL_DR(val) \
                            Gea3_uart_rx_wake_i2c_scl_spi_mosi_Write(val)

    #define Gea3_SET_I2C_SCL_HSIOM_SEL(sel) \
                    Gea3_SET_HSIOM_SEL(Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG,  \
                                                   Gea3_RX_WAKE_SCL_MOSI_HSIOM_MASK, \
                                                   Gea3_RX_WAKE_SCL_MOSI_HSIOM_POS,  \
                                                   (sel))

    #define Gea3_WAIT_SCL_SET_HIGH  (0u == Gea3_uart_rx_wake_i2c_scl_spi_mosi_Read())

#elif (Gea3_RX_SCL_MOSI_PIN)
    #define Gea3_SET_I2C_SCL_DR(val) \
                            Gea3_uart_rx_i2c_scl_spi_mosi_Write(val)


    #define Gea3_SET_I2C_SCL_HSIOM_SEL(sel) \
                            Gea3_SET_HSIOM_SEL(Gea3_RX_SCL_MOSI_HSIOM_REG,  \
                                                           Gea3_RX_SCL_MOSI_HSIOM_MASK, \
                                                           Gea3_RX_SCL_MOSI_HSIOM_POS,  \
                                                           (sel))

    #define Gea3_WAIT_SCL_SET_HIGH  (0u == Gea3_uart_rx_i2c_scl_spi_mosi_Read())

#else
    #define Gea3_SET_I2C_SCL_DR(val)        do{ /* Does nothing */ }while(0)
    #define Gea3_SET_I2C_SCL_HSIOM_SEL(sel) do{ /* Does nothing */ }while(0)

    #define Gea3_WAIT_SCL_SET_HIGH  (0u)
#endif /* (Gea3_I2C_PINS) */

/* SCB I2C: sda signal */
#if (Gea3_I2C_PINS)
    #define Gea3_WAIT_SDA_SET_HIGH  (0u == Gea3_sda_Read())
/* Unconfigured SCB: sda signal */
#elif (Gea3_TX_SDA_MISO_PIN)
    #define Gea3_WAIT_SDA_SET_HIGH  (0u == Gea3_uart_tx_i2c_sda_spi_miso_Read())
#else
    #define Gea3_WAIT_SDA_SET_HIGH  (0u)
#endif /* (Gea3_MOSI_SCL_RX_PIN) */
#endif /* (Gea3_CY_SCBIP_V0) */

/* Clear UART wakeup source */
#if (Gea3_RX_SCL_MOSI_PIN)
    #define Gea3_CLEAR_UART_RX_WAKE_INTR        do{ /* Does nothing */ }while(0)
    
#elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
    #define Gea3_CLEAR_UART_RX_WAKE_INTR \
            do{                                      \
                (void) Gea3_uart_rx_wake_i2c_scl_spi_mosi_ClearInterrupt(); \
            }while(0)

#elif(Gea3_UART_RX_WAKE_PIN)
    #define Gea3_CLEAR_UART_RX_WAKE_INTR \
            do{                                      \
                (void) Gea3_rx_wake_ClearInterrupt(); \
            }while(0)
#else
#endif /* (Gea3_RX_SCL_MOSI_PIN) */


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Unconfigured pins */
#define Gea3_REMOVE_MOSI_SCL_RX_WAKE_PIN    Gea3_REMOVE_RX_WAKE_SCL_MOSI_PIN
#define Gea3_REMOVE_MOSI_SCL_RX_PIN         Gea3_REMOVE_RX_SCL_MOSI_PIN
#define Gea3_REMOVE_MISO_SDA_TX_PIN         Gea3_REMOVE_TX_SDA_MISO_PIN
#ifndef Gea3_REMOVE_SCLK_PIN
#define Gea3_REMOVE_SCLK_PIN                Gea3_REMOVE_CTS_SCLK_PIN
#endif /* Gea3_REMOVE_SCLK_PIN */
#ifndef Gea3_REMOVE_SS0_PIN
#define Gea3_REMOVE_SS0_PIN                 Gea3_REMOVE_RTS_SS0_PIN
#endif /* Gea3_REMOVE_SS0_PIN */

/* Unconfigured pins */
#define Gea3_MOSI_SCL_RX_WAKE_PIN   Gea3_RX_WAKE_SCL_MOSI_PIN
#define Gea3_MOSI_SCL_RX_PIN        Gea3_RX_SCL_MOSI_PIN
#define Gea3_MISO_SDA_TX_PIN        Gea3_TX_SDA_MISO_PIN
#ifndef Gea3_SCLK_PIN
#define Gea3_SCLK_PIN               Gea3_CTS_SCLK_PIN
#endif /* Gea3_SCLK_PIN */
#ifndef Gea3_SS0_PIN
#define Gea3_SS0_PIN                Gea3_RTS_SS0_PIN
#endif /* Gea3_SS0_PIN */

#if (Gea3_MOSI_SCL_RX_WAKE_PIN)
    #define Gea3_MOSI_SCL_RX_WAKE_HSIOM_REG     Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define Gea3_MOSI_SCL_RX_WAKE_HSIOM_PTR     Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define Gea3_MOSI_SCL_RX_WAKE_HSIOM_MASK    Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define Gea3_MOSI_SCL_RX_WAKE_HSIOM_POS     Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG

    #define Gea3_MOSI_SCL_RX_WAKE_INTCFG_REG    Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define Gea3_MOSI_SCL_RX_WAKE_INTCFG_PTR    Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG

    #define Gea3_MOSI_SCL_RX_WAKE_INTCFG_TYPE_POS   Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define Gea3_MOSI_SCL_RX_WAKE_INTCFG_TYPE_MASK  Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG
#endif /* (Gea3_RX_WAKE_SCL_MOSI_PIN) */

#if (Gea3_MOSI_SCL_RX_PIN)
    #define Gea3_MOSI_SCL_RX_HSIOM_REG      Gea3_RX_SCL_MOSI_HSIOM_REG
    #define Gea3_MOSI_SCL_RX_HSIOM_PTR      Gea3_RX_SCL_MOSI_HSIOM_PTR
    #define Gea3_MOSI_SCL_RX_HSIOM_MASK     Gea3_RX_SCL_MOSI_HSIOM_MASK
    #define Gea3_MOSI_SCL_RX_HSIOM_POS      Gea3_RX_SCL_MOSI_HSIOM_POS
#endif /* (Gea3_MOSI_SCL_RX_PIN) */

#if (Gea3_MISO_SDA_TX_PIN)
    #define Gea3_MISO_SDA_TX_HSIOM_REG      Gea3_TX_SDA_MISO_HSIOM_REG
    #define Gea3_MISO_SDA_TX_HSIOM_PTR      Gea3_TX_SDA_MISO_HSIOM_REG
    #define Gea3_MISO_SDA_TX_HSIOM_MASK     Gea3_TX_SDA_MISO_HSIOM_REG
    #define Gea3_MISO_SDA_TX_HSIOM_POS      Gea3_TX_SDA_MISO_HSIOM_REG
#endif /* (Gea3_MISO_SDA_TX_PIN_PIN) */

#if (Gea3_SCLK_PIN)
    #ifndef Gea3_SCLK_HSIOM_REG
    #define Gea3_SCLK_HSIOM_REG     Gea3_CTS_SCLK_HSIOM_REG
    #define Gea3_SCLK_HSIOM_PTR     Gea3_CTS_SCLK_HSIOM_PTR
    #define Gea3_SCLK_HSIOM_MASK    Gea3_CTS_SCLK_HSIOM_MASK
    #define Gea3_SCLK_HSIOM_POS     Gea3_CTS_SCLK_HSIOM_POS
    #endif /* Gea3_SCLK_HSIOM_REG */
#endif /* (Gea3_SCLK_PIN) */

#if (Gea3_SS0_PIN)
    #ifndef Gea3_SS0_HSIOM_REG
    #define Gea3_SS0_HSIOM_REG      Gea3_RTS_SS0_HSIOM_REG
    #define Gea3_SS0_HSIOM_PTR      Gea3_RTS_SS0_HSIOM_PTR
    #define Gea3_SS0_HSIOM_MASK     Gea3_RTS_SS0_HSIOM_MASK
    #define Gea3_SS0_HSIOM_POS      Gea3_RTS_SS0_HSIOM_POS
    #endif /* Gea3_SS0_HSIOM_REG */
#endif /* (Gea3_SS0_PIN) */

#define Gea3_MOSI_SCL_RX_WAKE_PIN_INDEX Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX
#define Gea3_MOSI_SCL_RX_PIN_INDEX      Gea3_RX_SCL_MOSI_PIN_INDEX
#define Gea3_MISO_SDA_TX_PIN_INDEX      Gea3_TX_SDA_MISO_PIN_INDEX
#ifndef Gea3_SCLK_PIN_INDEX
#define Gea3_SCLK_PIN_INDEX             Gea3_CTS_SCLK_PIN_INDEX
#endif /* Gea3_SCLK_PIN_INDEX */
#ifndef Gea3_SS0_PIN_INDEX
#define Gea3_SS0_PIN_INDEX              Gea3_RTS_SS0_PIN_INDEX
#endif /* Gea3_SS0_PIN_INDEX */

#define Gea3_MOSI_SCL_RX_WAKE_PIN_MASK Gea3_RX_WAKE_SCL_MOSI_PIN_MASK
#define Gea3_MOSI_SCL_RX_PIN_MASK      Gea3_RX_SCL_MOSI_PIN_MASK
#define Gea3_MISO_SDA_TX_PIN_MASK      Gea3_TX_SDA_MISO_PIN_MASK
#ifndef Gea3_SCLK_PIN_MASK
#define Gea3_SCLK_PIN_MASK             Gea3_CTS_SCLK_PIN_MASK
#endif /* Gea3_SCLK_PIN_MASK */
#ifndef Gea3_SS0_PIN_MASK
#define Gea3_SS0_PIN_MASK              Gea3_RTS_SS0_PIN_MASK
#endif /* Gea3_SS0_PIN_MASK */

#endif /* (CY_SCB_PINS_Gea3_H) */


/* [] END OF FILE */
