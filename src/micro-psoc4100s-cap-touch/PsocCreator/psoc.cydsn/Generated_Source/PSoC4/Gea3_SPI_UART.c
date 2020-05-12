/***************************************************************************//**
* \file Gea3_SPI_UART.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  SPI and UART modes.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Gea3_PVT.h"
#include "Gea3_SPI_UART_PVT.h"

/***************************************
*        SPI/UART Private Vars
***************************************/

#if(Gea3_INTERNAL_RX_SW_BUFFER_CONST)
    /* Start index to put data into the software receive buffer.*/
    volatile uint32 Gea3_rxBufferHead;
    /* Start index to get data from the software receive buffer.*/
    volatile uint32 Gea3_rxBufferTail;
    /**
    * \addtogroup group_globals
    * \{
    */
    /** Sets when internal software receive buffer overflow
    *  was occurred.
    */
    volatile uint8  Gea3_rxBufferOverflow;
    /** \} globals */
#endif /* (Gea3_INTERNAL_RX_SW_BUFFER_CONST) */

#if(Gea3_INTERNAL_TX_SW_BUFFER_CONST)
    /* Start index to put data into the software transmit buffer.*/
    volatile uint32 Gea3_txBufferHead;
    /* Start index to get data from the software transmit buffer.*/
    volatile uint32 Gea3_txBufferTail;
#endif /* (Gea3_INTERNAL_TX_SW_BUFFER_CONST) */

#if(Gea3_INTERNAL_RX_SW_BUFFER)
    /* Add one element to the buffer to receive full packet. One byte in receive buffer is always empty */
    volatile uint8 Gea3_rxBufferInternal[Gea3_INTERNAL_RX_BUFFER_SIZE];
#endif /* (Gea3_INTERNAL_RX_SW_BUFFER) */

#if(Gea3_INTERNAL_TX_SW_BUFFER)
    volatile uint8 Gea3_txBufferInternal[Gea3_TX_BUFFER_SIZE];
#endif /* (Gea3_INTERNAL_TX_SW_BUFFER) */


#if(Gea3_RX_DIRECTION)
    /*******************************************************************************
    * Function Name: Gea3_SpiUartReadRxData
    ****************************************************************************//**
    *
    *  Retrieves the next data element from the receive buffer.
    *   - RX software buffer is disabled: Returns data element retrieved from
    *     RX FIFO. Undefined data will be returned if the RX FIFO is empty.
    *   - RX software buffer is enabled: Returns data element from the software
    *     receive buffer. Zero value is returned if the software receive buffer
    *     is empty.
    *
    * \return
    *  Next data element from the receive buffer. 
    *  The amount of data bits to be received depends on RX data bits selection 
    *  (the data bit counting starts from LSB of return value).
    *
    * \globalvars
    *  Gea3_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  Gea3_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    uint32 Gea3_SpiUartReadRxData(void)
    {
        uint32 rxData = 0u;

    #if (Gea3_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (Gea3_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (Gea3_CHECK_RX_SW_BUFFER)
        {
            if (Gea3_rxBufferHead != Gea3_rxBufferTail)
            {
                /* There is data in RX software buffer */

                /* Calculate index to read from */
                locTail = (Gea3_rxBufferTail + 1u);

                if (Gea3_INTERNAL_RX_BUFFER_SIZE == locTail)
                {
                    locTail = 0u;
                }

                /* Get data from RX software buffer */
                rxData = Gea3_GetWordFromRxBuffer(locTail);

                /* Change index in the buffer */
                Gea3_rxBufferTail = locTail;

                #if (Gea3_CHECK_UART_RTS_CONTROL_FLOW)
                {
                    /* Check if RX Not Empty is disabled in the interrupt */
                    if (0u == (Gea3_INTR_RX_MASK_REG & Gea3_INTR_RX_NOT_EMPTY))
                    {
                        /* Enable RX Not Empty interrupt source to continue
                        * receiving data into software buffer.
                        */
                        Gea3_INTR_RX_MASK_REG |= Gea3_INTR_RX_NOT_EMPTY;
                    }
                }
                #endif

            }
        }
        #else
        {
            /* Read data from RX FIFO */
            rxData = Gea3_RX_FIFO_RD_REG;
        }
        #endif

        return (rxData);
    }


    /*******************************************************************************
    * Function Name: Gea3_SpiUartGetRxBufferSize
    ****************************************************************************//**
    *
    *  Returns the number of received data elements in the receive buffer.
    *   - RX software buffer disabled: returns the number of used entries in
    *     RX FIFO.
    *   - RX software buffer enabled: returns the number of elements which were
    *     placed in the receive buffer. This does not include the hardware RX FIFO.
    *
    * \return
    *  Number of received data elements.
    *
    * \globalvars
    *  Gea3_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  Gea3_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    uint32 Gea3_SpiUartGetRxBufferSize(void)
    {
        uint32 size;
    #if (Gea3_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (Gea3_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (Gea3_CHECK_RX_SW_BUFFER)
        {
            locHead = Gea3_rxBufferHead;

            if(locHead >= Gea3_rxBufferTail)
            {
                size = (locHead - Gea3_rxBufferTail);
            }
            else
            {
                size = (locHead + (Gea3_INTERNAL_RX_BUFFER_SIZE - Gea3_rxBufferTail));
            }
        }
        #else
        {
            size = Gea3_GET_RX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: Gea3_SpiUartClearRxBuffer
    ****************************************************************************//**
    *
    *  Clears the receive buffer and RX FIFO.
    *
    * \globalvars
    *  Gea3_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  Gea3_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    void Gea3_SpiUartClearRxBuffer(void)
    {
        #if (Gea3_CHECK_RX_SW_BUFFER)
        {
            /* Lock from component interruption */
            Gea3_DisableInt();

            /* Flush RX software buffer */
            Gea3_rxBufferHead = Gea3_rxBufferTail;
            Gea3_rxBufferOverflow = 0u;

            Gea3_CLEAR_RX_FIFO;
            Gea3_ClearRxInterruptSource(Gea3_INTR_RX_ALL);

            #if (Gea3_CHECK_UART_RTS_CONTROL_FLOW)
            {
                /* Enable RX Not Empty interrupt source to continue receiving
                * data into software buffer.
                */
                Gea3_INTR_RX_MASK_REG |= Gea3_INTR_RX_NOT_EMPTY;
            }
            #endif
            
            /* Release lock */
            Gea3_EnableInt();
        }
        #else
        {
            Gea3_CLEAR_RX_FIFO;
        }
        #endif
    }

#endif /* (Gea3_RX_DIRECTION) */


#if(Gea3_TX_DIRECTION)
    /*******************************************************************************
    * Function Name: Gea3_SpiUartWriteTxData
    ****************************************************************************//**
    *
    *  Places a data entry into the transmit buffer to be sent at the next available
    *  bus time.
    *  This function is blocking and waits until there is space available to put the
    *  requested data in the transmit buffer.
    *
    *  \param txDataByte: the data to be transmitted.
    *   The amount of data bits to be transmitted depends on TX data bits selection 
    *   (the data bit counting starts from LSB of txDataByte).
    *
    * \globalvars
    *  Gea3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Gea3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void Gea3_SpiUartWriteTxData(uint32 txData)
    {
    #if (Gea3_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (Gea3_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (Gea3_CHECK_TX_SW_BUFFER)
        {
            /* Put data directly into the TX FIFO */
            if ((Gea3_txBufferHead == Gea3_txBufferTail) &&
                (Gea3_SPI_UART_FIFO_SIZE != Gea3_GET_TX_FIFO_ENTRIES))
            {
                /* TX software buffer is empty: put data directly in TX FIFO */
                Gea3_TX_FIFO_WR_REG = txData;
            }
            /* Put data into TX software buffer */
            else
            {
                /* Head index to put data */
                locHead = (Gea3_txBufferHead + 1u);

                /* Adjust TX software buffer index */
                if (Gea3_TX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                /* Wait for space in TX software buffer */
                while (locHead == Gea3_txBufferTail)
                {
                }

                /* TX software buffer has at least one room */

                /* Clear old status of INTR_TX_NOT_FULL. It sets at the end of transfer when TX FIFO is empty. */
                Gea3_ClearTxInterruptSource(Gea3_INTR_TX_NOT_FULL);

                Gea3_PutWordInTxBuffer(locHead, txData);

                Gea3_txBufferHead = locHead;

                /* Check if TX Not Full is disabled in interrupt */
                if (0u == (Gea3_INTR_TX_MASK_REG & Gea3_INTR_TX_NOT_FULL))
                {
                    /* Enable TX Not Full interrupt source to transmit from software buffer */
                    Gea3_INTR_TX_MASK_REG |= (uint32) Gea3_INTR_TX_NOT_FULL;
                }
            }
        }
        #else
        {
            /* Wait until TX FIFO has space to put data element */
            while (Gea3_SPI_UART_FIFO_SIZE == Gea3_GET_TX_FIFO_ENTRIES)
            {
            }

            Gea3_TX_FIFO_WR_REG = txData;
        }
        #endif
    }


    /*******************************************************************************
    * Function Name: Gea3_SpiUartPutArray
    ****************************************************************************//**
    *
    *  Places an array of data into the transmit buffer to be sent.
    *  This function is blocking and waits until there is a space available to put
    *  all the requested data in the transmit buffer. The array size can be greater
    *  than transmit buffer size.
    *
    * \param wrBuf: pointer to an array of data to be placed in transmit buffer. 
    *  The width of the data to be transmitted depends on TX data width selection 
    *  (the data bit counting starts from LSB for each array element).
    * \param count: number of data elements to be placed in the transmit buffer.
    *
    * \globalvars
    *  Gea3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Gea3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void Gea3_SpiUartPutArray(const uint8 wrBuf[], uint32 count)
    {
        uint32 i;

        for (i=0u; i < count; i++)
        {
            Gea3_SpiUartWriteTxData((uint32) wrBuf[i]);
        }
    }


    /*******************************************************************************
    * Function Name: Gea3_SpiUartGetTxBufferSize
    ****************************************************************************//**
    *
    *  Returns the number of elements currently in the transmit buffer.
    *   - TX software buffer is disabled: returns the number of used entries in
    *     TX FIFO.
    *   - TX software buffer is enabled: returns the number of elements currently
    *     used in the transmit buffer. This number does not include used entries in
    *     the TX FIFO. The transmit buffer size is zero until the TX FIFO is
    *     not full.
    *
    * \return
    *  Number of data elements ready to transmit.
    *
    * \globalvars
    *  Gea3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Gea3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    uint32 Gea3_SpiUartGetTxBufferSize(void)
    {
        uint32 size;
    #if (Gea3_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (Gea3_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (Gea3_CHECK_TX_SW_BUFFER)
        {
            /* Get current Tail index */
            locTail = Gea3_txBufferTail;

            if (Gea3_txBufferHead >= locTail)
            {
                size = (Gea3_txBufferHead - locTail);
            }
            else
            {
                size = (Gea3_txBufferHead + (Gea3_TX_BUFFER_SIZE - locTail));
            }
        }
        #else
        {
            size = Gea3_GET_TX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: Gea3_SpiUartClearTxBuffer
    ****************************************************************************//**
    *
    *  Clears the transmit buffer and TX FIFO.
    *
    * \globalvars
    *  Gea3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  Gea3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void Gea3_SpiUartClearTxBuffer(void)
    {
        #if (Gea3_CHECK_TX_SW_BUFFER)
        {
            /* Lock from component interruption */
            Gea3_DisableInt();

            /* Flush TX software buffer */
            Gea3_txBufferHead = Gea3_txBufferTail;

            Gea3_INTR_TX_MASK_REG &= (uint32) ~Gea3_INTR_TX_NOT_FULL;
            Gea3_CLEAR_TX_FIFO;
            Gea3_ClearTxInterruptSource(Gea3_INTR_TX_ALL);

            /* Release lock */
            Gea3_EnableInt();
        }
        #else
        {
            Gea3_CLEAR_TX_FIFO;
        }
        #endif
    }

#endif /* (Gea3_TX_DIRECTION) */


/*******************************************************************************
* Function Name: Gea3_SpiUartDisableIntRx
****************************************************************************//**
*
*  Disables the RX interrupt sources.
*
*  \return
*   Returns the RX interrupt sources enabled before the function call.
*
*******************************************************************************/
uint32 Gea3_SpiUartDisableIntRx(void)
{
    uint32 intSource;

    intSource = Gea3_GetRxInterruptMode();

    Gea3_SetRxInterruptMode(Gea3_NO_INTR_SOURCES);

    return (intSource);
}


/*******************************************************************************
* Function Name: Gea3_SpiUartDisableIntTx
****************************************************************************//**
*
*  Disables TX interrupt sources.
*
*  \return
*   Returns TX interrupt sources enabled before function call.
*
*******************************************************************************/
uint32 Gea3_SpiUartDisableIntTx(void)
{
    uint32 intSourceMask;

    intSourceMask = Gea3_GetTxInterruptMode();

    Gea3_SetTxInterruptMode(Gea3_NO_INTR_SOURCES);

    return (intSourceMask);
}


#if(Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: Gea3_PutWordInRxBuffer
    ****************************************************************************//**
    *
    *  Stores a byte/word into the RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param index:      index to store data byte/word in the RX buffer.
    *  \param rxDataByte: byte/word to store.
    *
    *******************************************************************************/
    void Gea3_PutWordInRxBuffer(uint32 idx, uint32 rxDataByte)
    {
        /* Put data in buffer */
        if (Gea3_ONE_BYTE_WIDTH == Gea3_rxDataBits)
        {
            Gea3_rxBuffer[idx] = ((uint8) rxDataByte);
        }
        else
        {
            Gea3_rxBuffer[(uint32)(idx << 1u)]      = LO8(LO16(rxDataByte));
            Gea3_rxBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(rxDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: Gea3_GetWordFromRxBuffer
    ****************************************************************************//**
    *
    *  Reads byte/word from RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \return
    *   Returns byte/word read from RX buffer.
    *
    *******************************************************************************/
    uint32 Gea3_GetWordFromRxBuffer(uint32 idx)
    {
        uint32 value;

        if (Gea3_ONE_BYTE_WIDTH == Gea3_rxDataBits)
        {
            value = Gea3_rxBuffer[idx];
        }
        else
        {
            value  = (uint32) Gea3_rxBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32)Gea3_rxBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }


    /*******************************************************************************
    * Function Name: Gea3_PutWordInTxBuffer
    ****************************************************************************//**
    *
    *  Stores byte/word into the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx:        index to store data byte/word in the TX buffer.
    *  \param txDataByte: byte/word to store.
    *
    *******************************************************************************/
    void Gea3_PutWordInTxBuffer(uint32 idx, uint32 txDataByte)
    {
        /* Put data in buffer */
        if (Gea3_ONE_BYTE_WIDTH == Gea3_txDataBits)
        {
            Gea3_txBuffer[idx] = ((uint8) txDataByte);
        }
        else
        {
            Gea3_txBuffer[(uint32)(idx << 1u)]      = LO8(LO16(txDataByte));
            Gea3_txBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(txDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: Gea3_GetWordFromTxBuffer
    ****************************************************************************//**
    *
    *  Reads byte/word from the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx: index to get data byte/word from the TX buffer.
    *
    *  \return
    *   Returns byte/word read from the TX buffer.
    *
    *******************************************************************************/
    uint32 Gea3_GetWordFromTxBuffer(uint32 idx)
    {
        uint32 value;

        if (Gea3_ONE_BYTE_WIDTH == Gea3_txDataBits)
        {
            value = (uint32) Gea3_txBuffer[idx];
        }
        else
        {
            value  = (uint32) Gea3_txBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32) Gea3_txBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }

#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */


/* [] END OF FILE */
