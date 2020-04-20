/***************************************************************************//**
* \file Gea3_BOOT.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2014-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_Gea3_H)
#define CY_SCB_BOOT_Gea3_H

#include "Gea3_PVT.h"

#if (Gea3_SCB_MODE_I2C_INC)
    #include "Gea3_I2C.h"
#endif /* (Gea3_SCB_MODE_I2C_INC) */

#if (Gea3_SCB_MODE_EZI2C_INC)
    #include "Gea3_EZI2C.h"
#endif /* (Gea3_SCB_MODE_EZI2C_INC) */

#if (Gea3_SCB_MODE_SPI_INC || Gea3_SCB_MODE_UART_INC)
    #include "Gea3_SPI_UART.h"
#endif /* (Gea3_SCB_MODE_SPI_INC || Gea3_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define Gea3_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Gea3) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (Gea3_SCB_MODE_I2C_INC)
    #define Gea3_I2C_BTLDR_COMM_ENABLED     (Gea3_BTLDR_COMM_ENABLED && \
                                                            (Gea3_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             Gea3_I2C_SLAVE_CONST))
#else
     #define Gea3_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (Gea3_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (Gea3_SCB_MODE_EZI2C_INC)
    #define Gea3_EZI2C_BTLDR_COMM_ENABLED   (Gea3_BTLDR_COMM_ENABLED && \
                                                         Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define Gea3_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (Gea3_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (Gea3_SCB_MODE_SPI_INC)
    #define Gea3_SPI_BTLDR_COMM_ENABLED     (Gea3_BTLDR_COMM_ENABLED && \
                                                            (Gea3_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             Gea3_SPI_SLAVE_CONST))
#else
        #define Gea3_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (Gea3_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (Gea3_SCB_MODE_UART_INC)
       #define Gea3_UART_BTLDR_COMM_ENABLED    (Gea3_BTLDR_COMM_ENABLED && \
                                                            (Gea3_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (Gea3_UART_RX_DIRECTION && \
                                                              Gea3_UART_TX_DIRECTION)))
#else
     #define Gea3_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (Gea3_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define Gea3_BTLDR_COMM_MODE_ENABLED    (Gea3_I2C_BTLDR_COMM_ENABLED   || \
                                                     Gea3_SPI_BTLDR_COMM_ENABLED   || \
                                                     Gea3_EZI2C_BTLDR_COMM_ENABLED || \
                                                     Gea3_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void Gea3_I2CCyBtldrCommStart(void);
    void Gea3_I2CCyBtldrCommStop (void);
    void Gea3_I2CCyBtldrCommReset(void);
    cystatus Gea3_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Gea3_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (Gea3_SCB_MODE_I2C_CONST_CFG)
        #define Gea3_CyBtldrCommStart   Gea3_I2CCyBtldrCommStart
        #define Gea3_CyBtldrCommStop    Gea3_I2CCyBtldrCommStop
        #define Gea3_CyBtldrCommReset   Gea3_I2CCyBtldrCommReset
        #define Gea3_CyBtldrCommRead    Gea3_I2CCyBtldrCommRead
        #define Gea3_CyBtldrCommWrite   Gea3_I2CCyBtldrCommWrite
    #endif /* (Gea3_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void Gea3_EzI2CCyBtldrCommStart(void);
    void Gea3_EzI2CCyBtldrCommStop (void);
    void Gea3_EzI2CCyBtldrCommReset(void);
    cystatus Gea3_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Gea3_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (Gea3_SCB_MODE_EZI2C_CONST_CFG)
        #define Gea3_CyBtldrCommStart   Gea3_EzI2CCyBtldrCommStart
        #define Gea3_CyBtldrCommStop    Gea3_EzI2CCyBtldrCommStop
        #define Gea3_CyBtldrCommReset   Gea3_EzI2CCyBtldrCommReset
        #define Gea3_CyBtldrCommRead    Gea3_EzI2CCyBtldrCommRead
        #define Gea3_CyBtldrCommWrite   Gea3_EzI2CCyBtldrCommWrite
    #endif /* (Gea3_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void Gea3_SpiCyBtldrCommStart(void);
    void Gea3_SpiCyBtldrCommStop (void);
    void Gea3_SpiCyBtldrCommReset(void);
    cystatus Gea3_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Gea3_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (Gea3_SCB_MODE_SPI_CONST_CFG)
        #define Gea3_CyBtldrCommStart   Gea3_SpiCyBtldrCommStart
        #define Gea3_CyBtldrCommStop    Gea3_SpiCyBtldrCommStop
        #define Gea3_CyBtldrCommReset   Gea3_SpiCyBtldrCommReset
        #define Gea3_CyBtldrCommRead    Gea3_SpiCyBtldrCommRead
        #define Gea3_CyBtldrCommWrite   Gea3_SpiCyBtldrCommWrite
    #endif /* (Gea3_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void Gea3_UartCyBtldrCommStart(void);
    void Gea3_UartCyBtldrCommStop (void);
    void Gea3_UartCyBtldrCommReset(void);
    cystatus Gea3_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus Gea3_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (Gea3_SCB_MODE_UART_CONST_CFG)
        #define Gea3_CyBtldrCommStart   Gea3_UartCyBtldrCommStart
        #define Gea3_CyBtldrCommStop    Gea3_UartCyBtldrCommStop
        #define Gea3_CyBtldrCommReset   Gea3_UartCyBtldrCommReset
        #define Gea3_CyBtldrCommRead    Gea3_UartCyBtldrCommRead
        #define Gea3_CyBtldrCommWrite   Gea3_UartCyBtldrCommWrite
    #endif /* (Gea3_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_UART_BTLDR_COMM_ENABLED) */

/**
* \addtogroup group_bootloader
* @{
*/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_BTLDR_COMM_ENABLED)
    #if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void Gea3_CyBtldrCommStart(void);
        void Gea3_CyBtldrCommStop (void);
        void Gea3_CyBtldrCommReset(void);
        cystatus Gea3_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus Gea3_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Gea3)
        #define CyBtldrCommStart    Gea3_CyBtldrCommStart
        #define CyBtldrCommStop     Gea3_CyBtldrCommStop
        #define CyBtldrCommReset    Gea3_CyBtldrCommReset
        #define CyBtldrCommWrite    Gea3_CyBtldrCommWrite
        #define CyBtldrCommRead     Gea3_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Gea3) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (Gea3_BTLDR_COMM_ENABLED) */

/** @} group_bootloader */

/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define Gea3_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define Gea3_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define Gea3_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define Gea3_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef Gea3_SPI_BYTE_TO_BYTE
    #define Gea3_SPI_BYTE_TO_BYTE   (160u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef Gea3_UART_BYTE_TO_BYTE
    #define Gea3_UART_BYTE_TO_BYTE  (87u)
#endif /* Gea3_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_Gea3_H) */


/* [] END OF FILE */
