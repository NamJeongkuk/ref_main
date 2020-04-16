/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
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

#if !defined(CY_SCB_PVT_Gea3_H)
#define CY_SCB_PVT_Gea3_H

#include "Gea3.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define Gea3_SetI2CExtClkInterruptMode(interruptMask) Gea3_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define Gea3_ClearI2CExtClkInterruptSource(interruptMask) Gea3_CLEAR_INTR_I2C_EC(interruptMask)
#define Gea3_GetI2CExtClkInterruptSource()                (Gea3_INTR_I2C_EC_REG)
#define Gea3_GetI2CExtClkInterruptMode()                  (Gea3_INTR_I2C_EC_MASK_REG)
#define Gea3_GetI2CExtClkInterruptSourceMasked()          (Gea3_INTR_I2C_EC_MASKED_REG)

#if (!Gea3_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define Gea3_SetSpiExtClkInterruptMode(interruptMask) \
                                                                Gea3_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define Gea3_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                Gea3_CLEAR_INTR_SPI_EC(interruptMask)
    #define Gea3_GetExtSpiClkInterruptSource()                 (Gea3_INTR_SPI_EC_REG)
    #define Gea3_GetExtSpiClkInterruptMode()                   (Gea3_INTR_SPI_EC_MASK_REG)
    #define Gea3_GetExtSpiClkInterruptSourceMasked()           (Gea3_INTR_SPI_EC_MASKED_REG)
#endif /* (!Gea3_CY_SCBIP_V1) */

#if(Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void Gea3_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (Gea3_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_Gea3_CUSTOM_INTR_HANDLER)
    extern cyisraddress Gea3_customIntrHandler;
#endif /* !defined (CY_REMOVE_Gea3_CUSTOM_INTR_HANDLER) */
#endif /* (Gea3_SCB_IRQ_INTERNAL) */

extern Gea3_BACKUP_STRUCT Gea3_backup;

#if(Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 Gea3_scbMode;
    extern uint8 Gea3_scbEnableWake;
    extern uint8 Gea3_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 Gea3_mode;
    extern uint8 Gea3_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * Gea3_rxBuffer;
    extern uint8   Gea3_rxDataBits;
    extern uint32  Gea3_rxBufferSize;

    extern volatile uint8 * Gea3_txBuffer;
    extern uint8   Gea3_txDataBits;
    extern uint32  Gea3_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 Gea3_numberOfAddr;
    extern uint8 Gea3_subAddrSize;
#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (Gea3_SCB_MODE_I2C_CONST_CFG || \
        Gea3_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 Gea3_IntrTxMask;
#endif /* (! (Gea3_SCB_MODE_I2C_CONST_CFG || \
              Gea3_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define Gea3_SCB_MODE_I2C_RUNTM_CFG     (Gea3_SCB_MODE_I2C      == Gea3_scbMode)
    #define Gea3_SCB_MODE_SPI_RUNTM_CFG     (Gea3_SCB_MODE_SPI      == Gea3_scbMode)
    #define Gea3_SCB_MODE_UART_RUNTM_CFG    (Gea3_SCB_MODE_UART     == Gea3_scbMode)
    #define Gea3_SCB_MODE_EZI2C_RUNTM_CFG   (Gea3_SCB_MODE_EZI2C    == Gea3_scbMode)
    #define Gea3_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (Gea3_SCB_MODE_UNCONFIG == Gea3_scbMode)

    /* Defines wakeup enable */
    #define Gea3_SCB_WAKE_ENABLE_CHECK       (0u != Gea3_scbEnableWake)
#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!Gea3_CY_SCBIP_V1)
    #define Gea3_SCB_PINS_NUMBER    (7u)
#else
    #define Gea3_SCB_PINS_NUMBER    (2u)
#endif /* (!Gea3_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_Gea3_H) */


/* [] END OF FILE */
