/***************************************************************************//**
* \file Gea3_SPI_UART_PVT.h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component in SPI and UART modes.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_SPI_UART_PVT_Gea3_H)
#define CY_SCB_SPI_UART_PVT_Gea3_H

#include "Gea3_SPI_UART.h"


/***************************************
*     Internal Global Vars
***************************************/

#if (Gea3_INTERNAL_RX_SW_BUFFER_CONST)
    extern volatile uint32  Gea3_rxBufferHead;
    extern volatile uint32  Gea3_rxBufferTail;
    
    /**
    * \addtogroup group_globals
    * @{
    */
    
    /** Sets when internal software receive buffer overflow
     *  was occurred.
    */  
    extern volatile uint8   Gea3_rxBufferOverflow;
    /** @} globals */
#endif /* (Gea3_INTERNAL_RX_SW_BUFFER_CONST) */

#if (Gea3_INTERNAL_TX_SW_BUFFER_CONST)
    extern volatile uint32  Gea3_txBufferHead;
    extern volatile uint32  Gea3_txBufferTail;
#endif /* (Gea3_INTERNAL_TX_SW_BUFFER_CONST) */

#if (Gea3_INTERNAL_RX_SW_BUFFER)
    extern volatile uint8 Gea3_rxBufferInternal[Gea3_INTERNAL_RX_BUFFER_SIZE];
#endif /* (Gea3_INTERNAL_RX_SW_BUFFER) */

#if (Gea3_INTERNAL_TX_SW_BUFFER)
    extern volatile uint8 Gea3_txBufferInternal[Gea3_TX_BUFFER_SIZE];
#endif /* (Gea3_INTERNAL_TX_SW_BUFFER) */


/***************************************
*     Private Function Prototypes
***************************************/

void Gea3_SpiPostEnable(void);
void Gea3_SpiStop(void);

#if (Gea3_SCB_MODE_SPI_CONST_CFG)
    void Gea3_SpiInit(void);
#endif /* (Gea3_SCB_MODE_SPI_CONST_CFG) */

#if (Gea3_SPI_WAKE_ENABLE_CONST)
    void Gea3_SpiSaveConfig(void);
    void Gea3_SpiRestoreConfig(void);
#endif /* (Gea3_SPI_WAKE_ENABLE_CONST) */

void Gea3_UartPostEnable(void);
void Gea3_UartStop(void);

#if (Gea3_SCB_MODE_UART_CONST_CFG)
    void Gea3_UartInit(void);
#endif /* (Gea3_SCB_MODE_UART_CONST_CFG) */

#if (Gea3_UART_WAKE_ENABLE_CONST)
    void Gea3_UartSaveConfig(void);
    void Gea3_UartRestoreConfig(void);
#endif /* (Gea3_UART_WAKE_ENABLE_CONST) */


/***************************************
*         UART API Constants
***************************************/

/* UART RX and TX position to be used in Gea3_SetPins() */
#define Gea3_UART_RX_PIN_ENABLE    (Gea3_UART_RX)
#define Gea3_UART_TX_PIN_ENABLE    (Gea3_UART_TX)

/* UART RTS and CTS position to be used in  Gea3_SetPins() */
#define Gea3_UART_RTS_PIN_ENABLE    (0x10u)
#define Gea3_UART_CTS_PIN_ENABLE    (0x20u)


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Interrupt processing */
#define Gea3_SpiUartEnableIntRx(intSourceMask)  Gea3_SetRxInterruptMode(intSourceMask)
#define Gea3_SpiUartEnableIntTx(intSourceMask)  Gea3_SetTxInterruptMode(intSourceMask)
uint32  Gea3_SpiUartDisableIntRx(void);
uint32  Gea3_SpiUartDisableIntTx(void);


#endif /* (CY_SCB_SPI_UART_PVT_Gea3_H) */


/* [] END OF FILE */
