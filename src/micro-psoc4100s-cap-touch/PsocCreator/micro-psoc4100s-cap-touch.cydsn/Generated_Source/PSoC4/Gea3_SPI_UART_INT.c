/***************************************************************************//**
* \file Gea3_SPI_UART_INT.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Interrupt Service Routine for
*  the SCB Component in SPI and UART modes.
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

#include "Gea3_PVT.h"
#include "Gea3_SPI_UART_PVT.h"
#include "cyapicallbacks.h"

#if (Gea3_SCB_IRQ_INTERNAL)
/*******************************************************************************
* Function Name: Gea3_SPI_UART_ISR
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the SCB SPI or UART modes.
*
*******************************************************************************/
CY_ISR(Gea3_SPI_UART_ISR)
{
#if (Gea3_INTERNAL_RX_SW_BUFFER_CONST)
    uint32 locHead;
#endif /* (Gea3_INTERNAL_RX_SW_BUFFER_CONST) */

#if (Gea3_INTERNAL_TX_SW_BUFFER_CONST)
    uint32 locTail;
#endif /* (Gea3_INTERNAL_TX_SW_BUFFER_CONST) */

#ifdef Gea3_SPI_UART_ISR_ENTRY_CALLBACK
    Gea3_SPI_UART_ISR_EntryCallback();
#endif /* Gea3_SPI_UART_ISR_ENTRY_CALLBACK */

    if (NULL != Gea3_customIntrHandler)
    {
        Gea3_customIntrHandler();
    }

    #if(Gea3_CHECK_SPI_WAKE_ENABLE)
    {
        /* Clear SPI wakeup source */
        Gea3_ClearSpiExtClkInterruptSource(Gea3_INTR_SPI_EC_WAKE_UP);
    }
    #endif

    #if (Gea3_CHECK_RX_SW_BUFFER)
    {
        if (Gea3_CHECK_INTR_RX_MASKED(Gea3_INTR_RX_NOT_EMPTY))
        {
            do
            {
                /* Move local head index */
                locHead = (Gea3_rxBufferHead + 1u);

                /* Adjust local head index */
                if (Gea3_INTERNAL_RX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                if (locHead == Gea3_rxBufferTail)
                {
                    #if (Gea3_CHECK_UART_RTS_CONTROL_FLOW)
                    {
                        /* There is no space in the software buffer - disable the
                        * RX Not Empty interrupt source. The data elements are
                        * still being received into the RX FIFO until the RTS signal
                        * stops the transmitter. After the data element is read from the
                        * buffer, the RX Not Empty interrupt source is enabled to
                        * move the next data element in the software buffer.
                        */
                        Gea3_INTR_RX_MASK_REG &= ~Gea3_INTR_RX_NOT_EMPTY;
                        break;
                    }
                    #else
                    {
                        /* Overflow: through away received data element */
                        (void) Gea3_RX_FIFO_RD_REG;
                        Gea3_rxBufferOverflow = (uint8) Gea3_INTR_RX_OVERFLOW;
                    }
                    #endif
                }
                else
                {
                    /* Store received data */
                    Gea3_PutWordInRxBuffer(locHead, Gea3_RX_FIFO_RD_REG);

                    /* Move head index */
                    Gea3_rxBufferHead = locHead;
                }
            }
            while(0u != Gea3_GET_RX_FIFO_ENTRIES);

            Gea3_ClearRxInterruptSource(Gea3_INTR_RX_NOT_EMPTY);
        }
    }
    #endif


    #if (Gea3_CHECK_TX_SW_BUFFER)
    {
        if (Gea3_CHECK_INTR_TX_MASKED(Gea3_INTR_TX_NOT_FULL))
        {
            do
            {
                /* Check for room in TX software buffer */
                if (Gea3_txBufferHead != Gea3_txBufferTail)
                {
                    /* Move local tail index */
                    locTail = (Gea3_txBufferTail + 1u);

                    /* Adjust local tail index */
                    if (Gea3_TX_BUFFER_SIZE == locTail)
                    {
                        locTail = 0u;
                    }

                    /* Put data into TX FIFO */
                    Gea3_TX_FIFO_WR_REG = Gea3_GetWordFromTxBuffer(locTail);

                    /* Move tail index */
                    Gea3_txBufferTail = locTail;
                }
                else
                {
                    /* TX software buffer is empty: complete transfer */
                    Gea3_DISABLE_INTR_TX(Gea3_INTR_TX_NOT_FULL);
                    break;
                }
            }
            while (Gea3_SPI_UART_FIFO_SIZE != Gea3_GET_TX_FIFO_ENTRIES);

            Gea3_ClearTxInterruptSource(Gea3_INTR_TX_NOT_FULL);
        }
    }
    #endif

#ifdef Gea3_SPI_UART_ISR_EXIT_CALLBACK
    Gea3_SPI_UART_ISR_ExitCallback();
#endif /* Gea3_SPI_UART_ISR_EXIT_CALLBACK */

}

#endif /* (Gea3_SCB_IRQ_INTERNAL) */


/* [] END OF FILE */
