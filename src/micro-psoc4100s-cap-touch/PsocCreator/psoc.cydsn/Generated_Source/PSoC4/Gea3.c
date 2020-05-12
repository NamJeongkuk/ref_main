/***************************************************************************//**
* \file Gea3.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component.
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

#if (Gea3_SCB_MODE_I2C_INC)
    #include "Gea3_I2C_PVT.h"
#endif /* (Gea3_SCB_MODE_I2C_INC) */

#if (Gea3_SCB_MODE_EZI2C_INC)
    #include "Gea3_EZI2C_PVT.h"
#endif /* (Gea3_SCB_MODE_EZI2C_INC) */

#if (Gea3_SCB_MODE_SPI_INC || Gea3_SCB_MODE_UART_INC)
    #include "Gea3_SPI_UART_PVT.h"
#endif /* (Gea3_SCB_MODE_SPI_INC || Gea3_SCB_MODE_UART_INC) */


/***************************************
*    Run Time Configuration Vars
***************************************/

/* Stores internal component configuration for Unconfigured mode */
#if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    uint8 Gea3_scbMode = Gea3_SCB_MODE_UNCONFIG;
    uint8 Gea3_scbEnableWake;
    uint8 Gea3_scbEnableIntr;

    /* I2C configuration variables */
    uint8 Gea3_mode;
    uint8 Gea3_acceptAddr;

    /* SPI/UART configuration variables */
    volatile uint8 * Gea3_rxBuffer;
    uint8  Gea3_rxDataBits;
    uint32 Gea3_rxBufferSize;

    volatile uint8 * Gea3_txBuffer;
    uint8  Gea3_txDataBits;
    uint32 Gea3_txBufferSize;

    /* EZI2C configuration variables */
    uint8 Gea3_numberOfAddr;
    uint8 Gea3_subAddrSize;
#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Common SCB Vars
***************************************/
/**
* \addtogroup group_general
* \{
*/

/** Gea3_initVar indicates whether the Gea3 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SCB_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the Gea3_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  Gea3_Init() function can be called before the 
*  Gea3_Start() or Gea3_Enable() function.
*/
uint8 Gea3_initVar = 0u;


#if (! (Gea3_SCB_MODE_I2C_CONST_CFG || \
        Gea3_SCB_MODE_EZI2C_CONST_CFG))
    /** This global variable stores TX interrupt sources after 
    * Gea3_Stop() is called. Only these TX interrupt sources 
    * will be restored on a subsequent Gea3_Enable() call.
    */
    uint16 Gea3_IntrTxMask = 0u;
#endif /* (! (Gea3_SCB_MODE_I2C_CONST_CFG || \
              Gea3_SCB_MODE_EZI2C_CONST_CFG)) */
/** \} globals */

#if (Gea3_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_Gea3_CUSTOM_INTR_HANDLER)
    void (*Gea3_customIntrHandler)(void) = NULL;
#endif /* !defined (CY_REMOVE_Gea3_CUSTOM_INTR_HANDLER) */
#endif /* (Gea3_SCB_IRQ_INTERNAL) */


/***************************************
*    Private Function Prototypes
***************************************/

static void Gea3_ScbEnableIntr(void);
static void Gea3_ScbModeStop(void);
static void Gea3_ScbModePostEnable(void);


/*******************************************************************************
* Function Name: Gea3_Init
****************************************************************************//**
*
*  Initializes the Gea3 component to operate in one of the selected
*  configurations: I2C, SPI, UART or EZI2C.
*  When the configuration is set to "Unconfigured SCB", this function does
*  not do any initialization. Use mode-specific initialization APIs instead:
*  Gea3_I2CInit, Gea3_SpiInit, 
*  Gea3_UartInit or Gea3_EzI2CInit.
*
*******************************************************************************/
void Gea3_Init(void)
{
#if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    if (Gea3_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        Gea3_initVar = 0u;
    }
    else
    {
        /* Initialization was done before this function call */
    }

#elif (Gea3_SCB_MODE_I2C_CONST_CFG)
    Gea3_I2CInit();

#elif (Gea3_SCB_MODE_SPI_CONST_CFG)
    Gea3_SpiInit();

#elif (Gea3_SCB_MODE_UART_CONST_CFG)
    Gea3_UartInit();

#elif (Gea3_SCB_MODE_EZI2C_CONST_CFG)
    Gea3_EzI2CInit();

#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Gea3_Enable
****************************************************************************//**
*
*  Enables Gea3 component operation: activates the hardware and 
*  internal interrupt. It also restores TX interrupt sources disabled after the 
*  Gea3_Stop() function was called (note that level-triggered TX 
*  interrupt sources remain disabled to not cause code lock-up).
*  For I2C and EZI2C modes the interrupt is internal and mandatory for 
*  operation. For SPI and UART modes the interrupt can be configured as none, 
*  internal or external.
*  The Gea3 configuration should be not changed when the component
*  is enabled. Any configuration changes should be made after disabling the 
*  component.
*  When configuration is set to “Unconfigured Gea3”, the component 
*  must first be initialized to operate in one of the following configurations: 
*  I2C, SPI, UART or EZ I2C, using the mode-specific initialization API. 
*  Otherwise this function does not enable the component.
*
*******************************************************************************/
void Gea3_Enable(void)
{
#if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Enable SCB block, only if it is already configured */
    if (!Gea3_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        Gea3_CTRL_REG |= Gea3_CTRL_ENABLED;

        Gea3_ScbEnableIntr();

        /* Call PostEnable function specific to current operation mode */
        Gea3_ScbModePostEnable();
    }
#else
    Gea3_CTRL_REG |= Gea3_CTRL_ENABLED;

    Gea3_ScbEnableIntr();

    /* Call PostEnable function specific to current operation mode */
    Gea3_ScbModePostEnable();
#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Gea3_Start
****************************************************************************//**
*
*  Invokes Gea3_Init() and Gea3_Enable().
*  After this function call, the component is enabled and ready for operation.
*  When configuration is set to "Unconfigured SCB", the component must first be
*  initialized to operate in one of the following configurations: I2C, SPI, UART
*  or EZI2C. Otherwise this function does not enable the component.
*
* \globalvars
*  Gea3_initVar - used to check initial configuration, modified
*  on first function call.
*
*******************************************************************************/
void Gea3_Start(void)
{
    if (0u == Gea3_initVar)
    {
        Gea3_Init();
        Gea3_initVar = 1u; /* Component was initialized */
    }

    Gea3_Enable();
}


/*******************************************************************************
* Function Name: Gea3_Stop
****************************************************************************//**
*
*  Disables the Gea3 component: disable the hardware and internal 
*  interrupt. It also disables all TX interrupt sources so as not to cause an 
*  unexpected interrupt trigger because after the component is enabled, the 
*  TX FIFO is empty.
*  Refer to the function Gea3_Enable() for the interrupt 
*  configuration details.
*  This function disables the SCB component without checking to see if 
*  communication is in progress. Before calling this function it may be 
*  necessary to check the status of communication to make sure communication 
*  is complete. If this is not done then communication could be stopped mid 
*  byte and corrupted data could result.
*
*******************************************************************************/
void Gea3_Stop(void)
{
#if (Gea3_SCB_IRQ_INTERNAL)
    Gea3_DisableInt();
#endif /* (Gea3_SCB_IRQ_INTERNAL) */

    /* Call Stop function specific to current operation mode */
    Gea3_ScbModeStop();

    /* Disable SCB IP */
    Gea3_CTRL_REG &= (uint32) ~Gea3_CTRL_ENABLED;

    /* Disable all TX interrupt sources so as not to cause an unexpected
    * interrupt trigger after the component will be enabled because the 
    * TX FIFO is empty.
    * For SCB IP v0, it is critical as it does not mask-out interrupt
    * sources when it is disabled. This can cause a code lock-up in the
    * interrupt handler because TX FIFO cannot be loaded after the block
    * is disabled.
    */
    Gea3_SetTxInterruptMode(Gea3_NO_INTR_SOURCES);

#if (Gea3_SCB_IRQ_INTERNAL)
    Gea3_ClearPendingInt();
#endif /* (Gea3_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: Gea3_SetRxFifoLevel
****************************************************************************//**
*
*  Sets level in the RX FIFO to generate a RX level interrupt.
*  When the RX FIFO has more entries than the RX FIFO level an RX level
*  interrupt request is generated.
*
*  \param level: Level in the RX FIFO to generate RX level interrupt.
*   The range of valid level values is between 0 and RX FIFO depth - 1.
*
*******************************************************************************/
void Gea3_SetRxFifoLevel(uint32 level)
{
    uint32 rxFifoCtrl;

    rxFifoCtrl = Gea3_RX_FIFO_CTRL_REG;

    rxFifoCtrl &= ((uint32) ~Gea3_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    rxFifoCtrl |= ((uint32) (Gea3_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    Gea3_RX_FIFO_CTRL_REG = rxFifoCtrl;
}


/*******************************************************************************
* Function Name: Gea3_SetTxFifoLevel
****************************************************************************//**
*
*  Sets level in the TX FIFO to generate a TX level interrupt.
*  When the TX FIFO has less entries than the TX FIFO level an TX level
*  interrupt request is generated.
*
*  \param level: Level in the TX FIFO to generate TX level interrupt.
*   The range of valid level values is between 0 and TX FIFO depth - 1.
*
*******************************************************************************/
void Gea3_SetTxFifoLevel(uint32 level)
{
    uint32 txFifoCtrl;

    txFifoCtrl = Gea3_TX_FIFO_CTRL_REG;

    txFifoCtrl &= ((uint32) ~Gea3_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    txFifoCtrl |= ((uint32) (Gea3_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    Gea3_TX_FIFO_CTRL_REG = txFifoCtrl;
}


#if (Gea3_SCB_IRQ_INTERNAL)
    /*******************************************************************************
    * Function Name: Gea3_SetCustomInterruptHandler
    ****************************************************************************//**
    *
    *  Registers a function to be called by the internal interrupt handler.
    *  First the function that is registered is called, then the internal interrupt
    *  handler performs any operation such as software buffer management functions
    *  before the interrupt returns.  It is the user's responsibility not to break
    *  the software buffer operations. Only one custom handler is supported, which
    *  is the function provided by the most recent call.
    *  At the initialization time no custom handler is registered.
    *
    *  \param func: Pointer to the function to register.
    *        The value NULL indicates to remove the current custom interrupt
    *        handler.
    *
    *******************************************************************************/
    void Gea3_SetCustomInterruptHandler(void (*func)(void))
    {
    #if !defined (CY_REMOVE_Gea3_CUSTOM_INTR_HANDLER)
        Gea3_customIntrHandler = func; /* Register interrupt handler */
    #else
        if (NULL != func)
        {
            /* Suppress compiler warning */
        }
    #endif /* !defined (CY_REMOVE_Gea3_CUSTOM_INTR_HANDLER) */
    }
#endif /* (Gea3_SCB_IRQ_INTERNAL) */


/*******************************************************************************
* Function Name: Gea3_ScbModeEnableIntr
****************************************************************************//**
*
*  Enables an interrupt for a specific mode.
*
*******************************************************************************/
static void Gea3_ScbEnableIntr(void)
{
#if (Gea3_SCB_IRQ_INTERNAL)
    /* Enable interrupt in NVIC */
    #if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
        if (0u != Gea3_scbEnableIntr)
        {
            Gea3_EnableInt();
        }

    #else
        Gea3_EnableInt();

    #endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */
#endif /* (Gea3_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: Gea3_ScbModePostEnable
****************************************************************************//**
*
*  Calls the PostEnable function for a specific operation mode.
*
*******************************************************************************/
static void Gea3_ScbModePostEnable(void)
{
#if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
#if (!Gea3_CY_SCBIP_V1)
    if (Gea3_SCB_MODE_SPI_RUNTM_CFG)
    {
        Gea3_SpiPostEnable();
    }
    else if (Gea3_SCB_MODE_UART_RUNTM_CFG)
    {
        Gea3_UartPostEnable();
    }
    else
    {
        /* Unknown mode: do nothing */
    }
#endif /* (!Gea3_CY_SCBIP_V1) */

#elif (Gea3_SCB_MODE_SPI_CONST_CFG)
    Gea3_SpiPostEnable();

#elif (Gea3_SCB_MODE_UART_CONST_CFG)
    Gea3_UartPostEnable();

#else
    /* Unknown mode: do nothing */
#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: Gea3_ScbModeStop
****************************************************************************//**
*
*  Calls the Stop function for a specific operation mode.
*
*******************************************************************************/
static void Gea3_ScbModeStop(void)
{
#if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    if (Gea3_SCB_MODE_I2C_RUNTM_CFG)
    {
        Gea3_I2CStop();
    }
    else if (Gea3_SCB_MODE_EZI2C_RUNTM_CFG)
    {
        Gea3_EzI2CStop();
    }
#if (!Gea3_CY_SCBIP_V1)
    else if (Gea3_SCB_MODE_SPI_RUNTM_CFG)
    {
        Gea3_SpiStop();
    }
    else if (Gea3_SCB_MODE_UART_RUNTM_CFG)
    {
        Gea3_UartStop();
    }
#endif /* (!Gea3_CY_SCBIP_V1) */
    else
    {
        /* Unknown mode: do nothing */
    }
#elif (Gea3_SCB_MODE_I2C_CONST_CFG)
    Gea3_I2CStop();

#elif (Gea3_SCB_MODE_EZI2C_CONST_CFG)
    Gea3_EzI2CStop();

#elif (Gea3_SCB_MODE_SPI_CONST_CFG)
    Gea3_SpiStop();

#elif (Gea3_SCB_MODE_UART_CONST_CFG)
    Gea3_UartStop();

#else
    /* Unknown mode: do nothing */
#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (Gea3_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: Gea3_SetPins
    ****************************************************************************//**
    *
    *  Sets the pins settings accordingly to the selected operation mode.
    *  Only available in the Unconfigured operation mode. The mode specific
    *  initialization function calls it.
    *  Pins configuration is set by PSoC Creator when a specific mode of operation
    *  is selected in design time.
    *
    *  \param mode:      Mode of SCB operation.
    *  \param subMode:   Sub-mode of SCB operation. It is only required for SPI and UART
    *             modes.
    *  \param uartEnableMask: enables TX or RX direction and RTS and CTS signals.
    *
    *******************************************************************************/
    void Gea3_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask)
    {
        uint32 pinsDm[Gea3_SCB_PINS_NUMBER];
        uint32 i;
        
    #if (!Gea3_CY_SCBIP_V1)
        uint32 pinsInBuf = 0u;
    #endif /* (!Gea3_CY_SCBIP_V1) */
        
        uint32 hsiomSel[Gea3_SCB_PINS_NUMBER] = 
        {
            Gea3_RX_SCL_MOSI_HSIOM_SEL_GPIO,
            Gea3_TX_SDA_MISO_HSIOM_SEL_GPIO,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

    #if (Gea3_CY_SCBIP_V1)
        /* Supress compiler warning. */
        if ((0u == subMode) || (0u == uartEnableMask))
        {
        }
    #endif /* (Gea3_CY_SCBIP_V1) */

        /* Set default HSIOM to GPIO and Drive Mode to Analog Hi-Z */
        for (i = 0u; i < Gea3_SCB_PINS_NUMBER; i++)
        {
            pinsDm[i] = Gea3_PIN_DM_ALG_HIZ;
        }

        if ((Gea3_SCB_MODE_I2C   == mode) ||
            (Gea3_SCB_MODE_EZI2C == mode))
        {
        #if (Gea3_RX_SCL_MOSI_PIN)
            hsiomSel[Gea3_RX_SCL_MOSI_PIN_INDEX] = Gea3_RX_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [Gea3_RX_SCL_MOSI_PIN_INDEX] = Gea3_PIN_DM_OD_LO;
        #elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX] = Gea3_RX_WAKE_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX] = Gea3_PIN_DM_OD_LO;
        #else
        #endif /* (Gea3_RX_SCL_MOSI_PIN) */
        
        #if (Gea3_TX_SDA_MISO_PIN)
            hsiomSel[Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_TX_SDA_MISO_HSIOM_SEL_I2C;
            pinsDm  [Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_PIN_DM_OD_LO;
        #endif /* (Gea3_TX_SDA_MISO_PIN) */
        }
    #if (!Gea3_CY_SCBIP_V1)
        else if (Gea3_SCB_MODE_SPI == mode)
        {
        #if (Gea3_RX_SCL_MOSI_PIN)
            hsiomSel[Gea3_RX_SCL_MOSI_PIN_INDEX] = Gea3_RX_SCL_MOSI_HSIOM_SEL_SPI;
        #elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX] = Gea3_RX_WAKE_SCL_MOSI_HSIOM_SEL_SPI;
        #else
        #endif /* (Gea3_RX_SCL_MOSI_PIN) */
        
        #if (Gea3_TX_SDA_MISO_PIN)
            hsiomSel[Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_TX_SDA_MISO_HSIOM_SEL_SPI;
        #endif /* (Gea3_TX_SDA_MISO_PIN) */
        
        #if (Gea3_CTS_SCLK_PIN)
            hsiomSel[Gea3_CTS_SCLK_PIN_INDEX] = Gea3_CTS_SCLK_HSIOM_SEL_SPI;
        #endif /* (Gea3_CTS_SCLK_PIN) */

            if (Gea3_SPI_SLAVE == subMode)
            {
                /* Slave */
                pinsDm[Gea3_RX_SCL_MOSI_PIN_INDEX] = Gea3_PIN_DM_DIG_HIZ;
                pinsDm[Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                pinsDm[Gea3_CTS_SCLK_PIN_INDEX] = Gea3_PIN_DM_DIG_HIZ;

            #if (Gea3_RTS_SS0_PIN)
                /* Only SS0 is valid choice for Slave */
                hsiomSel[Gea3_RTS_SS0_PIN_INDEX] = Gea3_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm  [Gea3_RTS_SS0_PIN_INDEX] = Gea3_PIN_DM_DIG_HIZ;
            #endif /* (Gea3_RTS_SS0_PIN) */

            #if (Gea3_TX_SDA_MISO_PIN)
                /* Disable input buffer */
                 pinsInBuf |= Gea3_TX_SDA_MISO_PIN_MASK;
            #endif /* (Gea3_TX_SDA_MISO_PIN) */
            }
            else 
            {
                /* (Master) */
                pinsDm[Gea3_RX_SCL_MOSI_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                pinsDm[Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_PIN_DM_DIG_HIZ;
                pinsDm[Gea3_CTS_SCLK_PIN_INDEX] = Gea3_PIN_DM_STRONG;

            #if (Gea3_RTS_SS0_PIN)
                hsiomSel [Gea3_RTS_SS0_PIN_INDEX] = Gea3_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm   [Gea3_RTS_SS0_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                pinsInBuf |= Gea3_RTS_SS0_PIN_MASK;
            #endif /* (Gea3_RTS_SS0_PIN) */

            #if (Gea3_SS1_PIN)
                hsiomSel [Gea3_SS1_PIN_INDEX] = Gea3_SS1_HSIOM_SEL_SPI;
                pinsDm   [Gea3_SS1_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                pinsInBuf |= Gea3_SS1_PIN_MASK;
            #endif /* (Gea3_SS1_PIN) */

            #if (Gea3_SS2_PIN)
                hsiomSel [Gea3_SS2_PIN_INDEX] = Gea3_SS2_HSIOM_SEL_SPI;
                pinsDm   [Gea3_SS2_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                pinsInBuf |= Gea3_SS2_PIN_MASK;
            #endif /* (Gea3_SS2_PIN) */

            #if (Gea3_SS3_PIN)
                hsiomSel [Gea3_SS3_PIN_INDEX] = Gea3_SS3_HSIOM_SEL_SPI;
                pinsDm   [Gea3_SS3_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                pinsInBuf |= Gea3_SS3_PIN_MASK;
            #endif /* (Gea3_SS3_PIN) */

                /* Disable input buffers */
            #if (Gea3_RX_SCL_MOSI_PIN)
                pinsInBuf |= Gea3_RX_SCL_MOSI_PIN_MASK;
            #elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
                pinsInBuf |= Gea3_RX_WAKE_SCL_MOSI_PIN_MASK;
            #else
            #endif /* (Gea3_RX_SCL_MOSI_PIN) */

            #if (Gea3_CTS_SCLK_PIN)
                pinsInBuf |= Gea3_CTS_SCLK_PIN_MASK;
            #endif /* (Gea3_CTS_SCLK_PIN) */
            }
        }
        else /* UART */
        {
            if (Gea3_UART_MODE_SMARTCARD == subMode)
            {
                /* SmartCard */
            #if (Gea3_TX_SDA_MISO_PIN)
                hsiomSel[Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_TX_SDA_MISO_HSIOM_SEL_UART;
                pinsDm  [Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_PIN_DM_OD_LO;
            #endif /* (Gea3_TX_SDA_MISO_PIN) */
            }
            else /* Standard or IrDA */
            {
                if (0u != (Gea3_UART_RX_PIN_ENABLE & uartEnableMask))
                {
                #if (Gea3_RX_SCL_MOSI_PIN)
                    hsiomSel[Gea3_RX_SCL_MOSI_PIN_INDEX] = Gea3_RX_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [Gea3_RX_SCL_MOSI_PIN_INDEX] = Gea3_PIN_DM_DIG_HIZ;
                #elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
                    hsiomSel[Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX] = Gea3_RX_WAKE_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX] = Gea3_PIN_DM_DIG_HIZ;
                #else
                #endif /* (Gea3_RX_SCL_MOSI_PIN) */
                }

                if (0u != (Gea3_UART_TX_PIN_ENABLE & uartEnableMask))
                {
                #if (Gea3_TX_SDA_MISO_PIN)
                    hsiomSel[Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_TX_SDA_MISO_HSIOM_SEL_UART;
                    pinsDm  [Gea3_TX_SDA_MISO_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                    
                    /* Disable input buffer */
                    pinsInBuf |= Gea3_TX_SDA_MISO_PIN_MASK;
                #endif /* (Gea3_TX_SDA_MISO_PIN) */
                }

            #if !(Gea3_CY_SCBIP_V0 || Gea3_CY_SCBIP_V1)
                if (Gea3_UART_MODE_STD == subMode)
                {
                    if (0u != (Gea3_UART_CTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* CTS input is multiplexed with SCLK */
                    #if (Gea3_CTS_SCLK_PIN)
                        hsiomSel[Gea3_CTS_SCLK_PIN_INDEX] = Gea3_CTS_SCLK_HSIOM_SEL_UART;
                        pinsDm  [Gea3_CTS_SCLK_PIN_INDEX] = Gea3_PIN_DM_DIG_HIZ;
                    #endif /* (Gea3_CTS_SCLK_PIN) */
                    }

                    if (0u != (Gea3_UART_RTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* RTS output is multiplexed with SS0 */
                    #if (Gea3_RTS_SS0_PIN)
                        hsiomSel[Gea3_RTS_SS0_PIN_INDEX] = Gea3_RTS_SS0_HSIOM_SEL_UART;
                        pinsDm  [Gea3_RTS_SS0_PIN_INDEX] = Gea3_PIN_DM_STRONG;
                        
                        /* Disable input buffer */
                        pinsInBuf |= Gea3_RTS_SS0_PIN_MASK;
                    #endif /* (Gea3_RTS_SS0_PIN) */
                    }
                }
            #endif /* !(Gea3_CY_SCBIP_V0 || Gea3_CY_SCBIP_V1) */
            }
        }
    #endif /* (!Gea3_CY_SCBIP_V1) */

    /* Configure pins: set HSIOM, DM and InputBufEnable */
    /* Note: the DR register settings do not effect the pin output if HSIOM is other than GPIO */

    #if (Gea3_RX_SCL_MOSI_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_RX_SCL_MOSI_HSIOM_REG,
                                       Gea3_RX_SCL_MOSI_HSIOM_MASK,
                                       Gea3_RX_SCL_MOSI_HSIOM_POS,
                                        hsiomSel[Gea3_RX_SCL_MOSI_PIN_INDEX]);

        Gea3_uart_rx_i2c_scl_spi_mosi_SetDriveMode((uint8) pinsDm[Gea3_RX_SCL_MOSI_PIN_INDEX]);

        #if (!Gea3_CY_SCBIP_V1)
            Gea3_SET_INP_DIS(Gea3_uart_rx_i2c_scl_spi_mosi_INP_DIS,
                                         Gea3_uart_rx_i2c_scl_spi_mosi_MASK,
                                         (0u != (pinsInBuf & Gea3_RX_SCL_MOSI_PIN_MASK)));
        #endif /* (!Gea3_CY_SCBIP_V1) */
    
    #elif (Gea3_RX_WAKE_SCL_MOSI_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_RX_WAKE_SCL_MOSI_HSIOM_REG,
                                       Gea3_RX_WAKE_SCL_MOSI_HSIOM_MASK,
                                       Gea3_RX_WAKE_SCL_MOSI_HSIOM_POS,
                                       hsiomSel[Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        Gea3_uart_rx_wake_i2c_scl_spi_mosi_SetDriveMode((uint8)
                                                               pinsDm[Gea3_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        Gea3_SET_INP_DIS(Gea3_uart_rx_wake_i2c_scl_spi_mosi_INP_DIS,
                                     Gea3_uart_rx_wake_i2c_scl_spi_mosi_MASK,
                                     (0u != (pinsInBuf & Gea3_RX_WAKE_SCL_MOSI_PIN_MASK)));

         /* Set interrupt on falling edge */
        Gea3_SET_INCFG_TYPE(Gea3_RX_WAKE_SCL_MOSI_INTCFG_REG,
                                        Gea3_RX_WAKE_SCL_MOSI_INTCFG_TYPE_MASK,
                                        Gea3_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS,
                                        Gea3_INTCFG_TYPE_FALLING_EDGE);
    #else
    #endif /* (Gea3_RX_WAKE_SCL_MOSI_PIN) */

    #if (Gea3_TX_SDA_MISO_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_TX_SDA_MISO_HSIOM_REG,
                                       Gea3_TX_SDA_MISO_HSIOM_MASK,
                                       Gea3_TX_SDA_MISO_HSIOM_POS,
                                        hsiomSel[Gea3_TX_SDA_MISO_PIN_INDEX]);

        Gea3_uart_tx_i2c_sda_spi_miso_SetDriveMode((uint8) pinsDm[Gea3_TX_SDA_MISO_PIN_INDEX]);

    #if (!Gea3_CY_SCBIP_V1)
        Gea3_SET_INP_DIS(Gea3_uart_tx_i2c_sda_spi_miso_INP_DIS,
                                     Gea3_uart_tx_i2c_sda_spi_miso_MASK,
                                    (0u != (pinsInBuf & Gea3_TX_SDA_MISO_PIN_MASK)));
    #endif /* (!Gea3_CY_SCBIP_V1) */
    #endif /* (Gea3_RX_SCL_MOSI_PIN) */

    #if (Gea3_CTS_SCLK_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_CTS_SCLK_HSIOM_REG,
                                       Gea3_CTS_SCLK_HSIOM_MASK,
                                       Gea3_CTS_SCLK_HSIOM_POS,
                                       hsiomSel[Gea3_CTS_SCLK_PIN_INDEX]);

        Gea3_uart_cts_spi_sclk_SetDriveMode((uint8) pinsDm[Gea3_CTS_SCLK_PIN_INDEX]);

        Gea3_SET_INP_DIS(Gea3_uart_cts_spi_sclk_INP_DIS,
                                     Gea3_uart_cts_spi_sclk_MASK,
                                     (0u != (pinsInBuf & Gea3_CTS_SCLK_PIN_MASK)));
    #endif /* (Gea3_CTS_SCLK_PIN) */

    #if (Gea3_RTS_SS0_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_RTS_SS0_HSIOM_REG,
                                       Gea3_RTS_SS0_HSIOM_MASK,
                                       Gea3_RTS_SS0_HSIOM_POS,
                                       hsiomSel[Gea3_RTS_SS0_PIN_INDEX]);

        Gea3_uart_rts_spi_ss0_SetDriveMode((uint8) pinsDm[Gea3_RTS_SS0_PIN_INDEX]);

        Gea3_SET_INP_DIS(Gea3_uart_rts_spi_ss0_INP_DIS,
                                     Gea3_uart_rts_spi_ss0_MASK,
                                     (0u != (pinsInBuf & Gea3_RTS_SS0_PIN_MASK)));
    #endif /* (Gea3_RTS_SS0_PIN) */

    #if (Gea3_SS1_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_SS1_HSIOM_REG,
                                       Gea3_SS1_HSIOM_MASK,
                                       Gea3_SS1_HSIOM_POS,
                                       hsiomSel[Gea3_SS1_PIN_INDEX]);

        Gea3_spi_ss1_SetDriveMode((uint8) pinsDm[Gea3_SS1_PIN_INDEX]);

        Gea3_SET_INP_DIS(Gea3_spi_ss1_INP_DIS,
                                     Gea3_spi_ss1_MASK,
                                     (0u != (pinsInBuf & Gea3_SS1_PIN_MASK)));
    #endif /* (Gea3_SS1_PIN) */

    #if (Gea3_SS2_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_SS2_HSIOM_REG,
                                       Gea3_SS2_HSIOM_MASK,
                                       Gea3_SS2_HSIOM_POS,
                                       hsiomSel[Gea3_SS2_PIN_INDEX]);

        Gea3_spi_ss2_SetDriveMode((uint8) pinsDm[Gea3_SS2_PIN_INDEX]);

        Gea3_SET_INP_DIS(Gea3_spi_ss2_INP_DIS,
                                     Gea3_spi_ss2_MASK,
                                     (0u != (pinsInBuf & Gea3_SS2_PIN_MASK)));
    #endif /* (Gea3_SS2_PIN) */

    #if (Gea3_SS3_PIN)
        Gea3_SET_HSIOM_SEL(Gea3_SS3_HSIOM_REG,
                                       Gea3_SS3_HSIOM_MASK,
                                       Gea3_SS3_HSIOM_POS,
                                       hsiomSel[Gea3_SS3_PIN_INDEX]);

        Gea3_spi_ss3_SetDriveMode((uint8) pinsDm[Gea3_SS3_PIN_INDEX]);

        Gea3_SET_INP_DIS(Gea3_spi_ss3_INP_DIS,
                                     Gea3_spi_ss3_MASK,
                                     (0u != (pinsInBuf & Gea3_SS3_PIN_MASK)));
    #endif /* (Gea3_SS3_PIN) */
    }

#endif /* (Gea3_SCB_MODE_UNCONFIG_CONST_CFG) */


#if (Gea3_CY_SCBIP_V0 || Gea3_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: Gea3_I2CSlaveNackGeneration
    ****************************************************************************//**
    *
    *  Sets command to generate NACK to the address or data.
    *
    *******************************************************************************/
    void Gea3_I2CSlaveNackGeneration(void)
    {
        /* Check for EC_AM toggle condition: EC_AM and clock stretching for address are enabled */
        if ((0u != (Gea3_CTRL_REG & Gea3_CTRL_EC_AM_MODE)) &&
            (0u == (Gea3_I2C_CTRL_REG & Gea3_I2C_CTRL_S_NOT_READY_ADDR_NACK)))
        {
            /* Toggle EC_AM before NACK generation */
            Gea3_CTRL_REG &= ~Gea3_CTRL_EC_AM_MODE;
            Gea3_CTRL_REG |=  Gea3_CTRL_EC_AM_MODE;
        }

        Gea3_I2C_SLAVE_CMD_REG = Gea3_I2C_SLAVE_CMD_S_NACK;
    }
#endif /* (Gea3_CY_SCBIP_V0 || Gea3_CY_SCBIP_V1) */


/* [] END OF FILE */
