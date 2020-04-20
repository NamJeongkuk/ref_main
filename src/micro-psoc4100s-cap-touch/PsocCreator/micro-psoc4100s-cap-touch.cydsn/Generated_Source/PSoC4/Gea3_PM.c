/***************************************************************************//**
* \file Gea3_PM.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Power Management support for
*  the SCB Component.
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

#include "Gea3.h"
#include "Gea3_PVT.h"

#if(Gea3_SCB_MODE_I2C_INC)
    #include "Gea3_I2C_PVT.h"
#endif /* (Gea3_SCB_MODE_I2C_INC) */

#if(Gea3_SCB_MODE_EZI2C_INC)
    #include "Gea3_EZI2C_PVT.h"
#endif /* (Gea3_SCB_MODE_EZI2C_INC) */

#if(Gea3_SCB_MODE_SPI_INC || Gea3_SCB_MODE_UART_INC)
    #include "Gea3_SPI_UART_PVT.h"
#endif /* (Gea3_SCB_MODE_SPI_INC || Gea3_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(Gea3_SCB_MODE_UNCONFIG_CONST_CFG || \
   (Gea3_SCB_MODE_I2C_CONST_CFG   && (!Gea3_I2C_WAKE_ENABLE_CONST))   || \
   (Gea3_SCB_MODE_EZI2C_CONST_CFG && (!Gea3_EZI2C_WAKE_ENABLE_CONST)) || \
   (Gea3_SCB_MODE_SPI_CONST_CFG   && (!Gea3_SPI_WAKE_ENABLE_CONST))   || \
   (Gea3_SCB_MODE_UART_CONST_CFG  && (!Gea3_UART_WAKE_ENABLE_CONST)))

    Gea3_BACKUP_STRUCT Gea3_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: Gea3_Sleep
****************************************************************************//**
*
*  Prepares the Gea3 component to enter Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has an influence on this 
*  function implementation:
*  - Checked: configures the component to be wakeup source from Deep Sleep.
*  - Unchecked: stores the current component state (enabled or disabled) and 
*    disables the component. See SCB_Stop() function for details about component 
*    disabling.
*
*  Call the Gea3_Sleep() function before calling the 
*  CyPmSysDeepSleep() function. 
*  Refer to the PSoC Creator System Reference Guide for more information about 
*  power management functions and Low power section of this document for the 
*  selected mode.
*
*  This function should not be called before entering Sleep.
*
*******************************************************************************/
void Gea3_Sleep(void)
{
#if(Gea3_SCB_MODE_UNCONFIG_CONST_CFG)

    if(Gea3_SCB_WAKE_ENABLE_CHECK)
    {
        if(Gea3_SCB_MODE_I2C_RUNTM_CFG)
        {
            Gea3_I2CSaveConfig();
        }
        else if(Gea3_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            Gea3_EzI2CSaveConfig();
        }
    #if(!Gea3_CY_SCBIP_V1)
        else if(Gea3_SCB_MODE_SPI_RUNTM_CFG)
        {
            Gea3_SpiSaveConfig();
        }
        else if(Gea3_SCB_MODE_UART_RUNTM_CFG)
        {
            Gea3_UartSaveConfig();
        }
    #endif /* (!Gea3_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        Gea3_backup.enableState = (uint8) Gea3_GET_CTRL_ENABLED;

        if(0u != Gea3_backup.enableState)
        {
            Gea3_Stop();
        }
    }

#else

    #if (Gea3_SCB_MODE_I2C_CONST_CFG && Gea3_I2C_WAKE_ENABLE_CONST)
        Gea3_I2CSaveConfig();

    #elif (Gea3_SCB_MODE_EZI2C_CONST_CFG && Gea3_EZI2C_WAKE_ENABLE_CONST)
        Gea3_EzI2CSaveConfig();

    #elif (Gea3_SCB_MODE_SPI_CONST_CFG && Gea3_SPI_WAKE_ENABLE_CONST)
        Gea3_SpiSaveConfig();

    #elif (Gea3_SCB_MODE_UART_CONST_CFG && Gea3_UART_WAKE_ENABLE_CONST)
        Gea3_UartSaveConfig();

    #else

        Gea3_backup.enableState = (uint8) Gea3_GET_CTRL_ENABLED;

        if(0u != Gea3_backup.enableState)
        {
            Gea3_Stop();
        }

    #endif /* defined (Gea3_SCB_MODE_I2C_CONST_CFG) && (Gea3_I2C_WAKE_ENABLE_CONST) */

#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Gea3_Wakeup
****************************************************************************//**
*
*  Prepares the Gea3 component for Active mode operation after 
*  Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has influence on this 
*  function implementation:
*  - Checked: restores the component Active mode configuration.
*  - Unchecked: enables the component if it was enabled before enter Deep Sleep.
*
*  This function should not be called after exiting Sleep.
*
*  \sideeffect
*   Calling the Gea3_Wakeup() function without first calling the 
*   Gea3_Sleep() function may produce unexpected behavior.
*
*******************************************************************************/
void Gea3_Wakeup(void)
{
#if(Gea3_SCB_MODE_UNCONFIG_CONST_CFG)

    if(Gea3_SCB_WAKE_ENABLE_CHECK)
    {
        if(Gea3_SCB_MODE_I2C_RUNTM_CFG)
        {
            Gea3_I2CRestoreConfig();
        }
        else if(Gea3_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            Gea3_EzI2CRestoreConfig();
        }
    #if(!Gea3_CY_SCBIP_V1)
        else if(Gea3_SCB_MODE_SPI_RUNTM_CFG)
        {
            Gea3_SpiRestoreConfig();
        }
        else if(Gea3_SCB_MODE_UART_RUNTM_CFG)
        {
            Gea3_UartRestoreConfig();
        }
    #endif /* (!Gea3_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        if(0u != Gea3_backup.enableState)
        {
            Gea3_Enable();
        }
    }

#else

    #if (Gea3_SCB_MODE_I2C_CONST_CFG  && Gea3_I2C_WAKE_ENABLE_CONST)
        Gea3_I2CRestoreConfig();

    #elif (Gea3_SCB_MODE_EZI2C_CONST_CFG && Gea3_EZI2C_WAKE_ENABLE_CONST)
        Gea3_EzI2CRestoreConfig();

    #elif (Gea3_SCB_MODE_SPI_CONST_CFG && Gea3_SPI_WAKE_ENABLE_CONST)
        Gea3_SpiRestoreConfig();

    #elif (Gea3_SCB_MODE_UART_CONST_CFG && Gea3_UART_WAKE_ENABLE_CONST)
        Gea3_UartRestoreConfig();

    #else

        if(0u != Gea3_backup.enableState)
        {
            Gea3_Enable();
        }

    #endif /* (Gea3_I2C_WAKE_ENABLE_CONST) */

#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */
