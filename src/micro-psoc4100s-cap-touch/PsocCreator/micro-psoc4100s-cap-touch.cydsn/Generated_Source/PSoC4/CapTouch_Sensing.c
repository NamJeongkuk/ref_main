/***************************************************************************//**
* \file CapTouch_Sensing.c
* \version 6.0
*
* \brief
*   This file contains the source of functions common for
*   different sensing methods.
*
* \see CapTouch v6.0 Datasheet
*
*//*****************************************************************************
* Copyright (2016-2018), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#include "cyfitter.h"
#include "CapTouch_Configuration.h"
#include "CapTouch_Structure.h"
#include "CapTouch_Sensing.h"
#if (CapTouch_ENABLE == CapTouch_CSX_EN)
    #include "CapTouch_SensingCSX_LL.h"
#endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) */
#if (CapTouch_ENABLE == CapTouch_ISX_EN)
    #include "CapTouch_SensingISX_LL.h"
#endif /* (CapTouch_ENABLE == CapTouch_ISX_EN) */
#if (CapTouch_ENABLE == CapTouch_CSD_EN)
    #include "CapTouch_SensingCSD_LL.h"
#endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */
#if (CapTouch_CSD_SS_DIS != CapTouch_CSD_AUTOTUNE)
    #include "CapTouch_SmartSense_LL.h"
#endif  /* (CapTouch_CSD_SS_DIS != CapTouch_CSD_AUTOTUNE) */
#if (CapTouch_ENABLE == CapTouch_ADC_EN)
    #include "CapTouch_Adc.h"
#endif /* (CapTouch_ENABLE == CapTouch_ADC_EN) */
#if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)
    #include "CapTouch_SelfTest.h"
#endif

#if (0u != CapTouch_ISX_EN)
    #include "CapTouch_DigPrt2.h"
    #if (CapTouch_USES_PORT_3)
        #include "CapTouch_DigPrt3.h"
    #endif
#endif /* (0u != CapTouch_ISX_EN) */
/***************************************
* API Constants
***************************************/

#define CapTouch_IMO_FREQUENCY_OFFSET_MIN               (0u)
#define CapTouch_IMO_FREQUENCY_OFFSET_MAX               (255u)
#define CapTouch_CALIBRATION_RESOLUTION                 (12u)
#define CapTouch_CALIBRATION_FREQ_KHZ                   (1500u)
#define CapTouch_CSD_AUTOTUNE_CAL_LEVEL                 (CapTouch_CSD_RAWCOUNT_CAL_LEVEL)
#define CapTouch_CSD_AUTOTUNE_CAL_UNITS                 (1000u)
#define CapTouch_CP_MIN                                 (0u)
#define CapTouch_CP_MAX                                 (65000Lu)
#define CapTouch_CP_ERROR                               (4000Lu)
#define CapTouch_CLK_SOURCE_LFSR_SCALE_OFFSET           (4u)

#if (CapTouch_ENABLE == CapTouch_CSDV2)
    #define CapTouch_CSD_SNS_FREQ_KHZ_MAX               (6000u)
#else
    #define CapTouch_CSD_SNS_FREQ_KHZ_MAX               (12600u)
#endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

#define CapTouch_FLIP_FLOP_DIV                          (1u)

#define CapTouch_MOD_CSD_CLK_12000KHZ                   (12000uL)
#define CapTouch_MOD_CSD_CLK_24000KHZ                   (24000uL)
#define CapTouch_MOD_CSD_CLK_48000KHZ                   (48000uL)

/*****************************************************************************/
/* Enumeration types definition                                              */
/*****************************************************************************/

typedef enum
{
    CapTouch_RES_PULLUP_E   = 0x02u,
    CapTouch_RES_PULLDOWN_E = 0x03u
} CapTouch_PORT_TEST_DM;

typedef enum
{
    CapTouch_STS_RESET      = 0x01u,
    CapTouch_STS_NO_RESET   = 0x02u
} CapTouch_TEST_TYPE;


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
/**
* \cond SECTION_C_INTERNAL
* \addtogroup group_c_internal
* \{
*/

#if (CapTouch_DISABLE == CapTouch_CSDV2)
    static void CapTouch_SsTrimIdacs(void);
    #if ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
         (CapTouch_ENABLE == CapTouch_ISX_EN) || \
         (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG))
        static void CapTouch_SsTrimIdacsSinking(void);
    #endif /* ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
               (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG)) */
    #if ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
         (CapTouch_ENABLE == CapTouch_ISX_EN) || \
         (CapTouch_IDAC_SOURCING == CapTouch_CSD_IDAC_CONFIG))
        static void CapTouch_SsTrimIdacsSourcing(void);
    #endif /* ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
               (CapTouch_IDAC_SOURCING == CapTouch_CSD_IDAC_CONFIG)) */
#endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

#if (CapTouch_ENABLE == CapTouch_CSD_EN)
    static void CapTouch_SsCSDDisableMode(void);
#endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */
#if (CapTouch_ENABLE == CapTouch_CSX_EN)
    static void CapTouch_SsDisableCSXMode(void);
#endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) */
#if (CapTouch_ENABLE == CapTouch_ISX_EN)
    static void CapTouch_SsDisableISXMode(void);
#endif /* (CapTouch_ENABLE == CapTouch_ISX_EN) */

#if(((CapTouch_ENABLE == CapTouch_CSX_EN) && \
     (CapTouch_ENABLE == CapTouch_CSDV2) && \
     (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSX_TX_CLK_SOURCE) && \
     (CapTouch_DISABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES)) ||\
    ((CapTouch_ENABLE == CapTouch_CSD_EN) && \
     (CapTouch_ENABLE == CapTouch_CSDV2) && \
     (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE)))
    static uint8 CapTouch_SsCalcLfsrSize(uint32 snsClkDivider, uint32 conversionsNum);
    static uint8 CapTouch_SsCalcLfsrScale(uint32 snsClkDivider, uint8 lfsrSize);
#endif

#if (CapTouch_ENABLE == CapTouch_CSD_EN)
    static void CapTouch_SsSetWidgetSenseClkSrc(uint32 wdgtIndex, CapTouch_RAM_WD_BASE_STRUCT * ptrWdgt);
#endif

#if ((CapTouch_ENABLE == CapTouch_CSX_EN) && (CapTouch_ENABLE == CapTouch_CSDV2))
    static void CapTouch_SsSetWidgetTxClkSrc(uint32 wdgtIndex, CapTouch_RAM_WD_BASE_STRUCT * ptrWdgt);
#endif

/** \}
* \endcond */

/*******************************************************************************
* Defines module variables
*******************************************************************************/

CapTouch_SENSE_METHOD_ENUM CapTouch_currentSenseMethod = CapTouch_UNDEFINED_E;

#if(CapTouch_ENABLE == CapTouch_MULTI_FREQ_SCAN_EN)
    /* Module variable keep track of multi-frequency scan channel index */
    uint8 CapTouch_scanFreqIndex = 0u;
    /* Variable keep frequency offsets */
    uint8 CapTouch_immunity[CapTouch_NUM_SCAN_FREQS] = {0u, 0u, 0u};
#else
    /* const allows C-compiler to do optimization */
    const uint8 CapTouch_scanFreqIndex = 0u;
#endif

/* Global software variables */
volatile uint8 CapTouch_widgetIndex = 0u;    /* Index of the scanning widget */
volatile uint8 CapTouch_sensorIndex = 0u;    /* Index of the scanning sensor */
uint8 CapTouch_requestScanAllWidget = 0u;

/* Pointer to RAM_SNS_STRUCT structure */
CapTouch_RAM_SNS_STRUCT *CapTouch_curRamSnsPtr;

#if ((CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN) || \
     (CapTouch_ENABLE == CapTouch_CSX_EN) || \
     (CapTouch_ENABLE == CapTouch_ISX_EN))
    /* Pointer to Flash structure holding configuration of widget to be scanned */
    CapTouch_FLASH_WD_STRUCT const *CapTouch_curFlashWdgtPtr = 0u;
#endif

#if (CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN)
    /* Pointer to Flash structure holding info of sensor to be scanned */
    CapTouch_FLASH_SNS_STRUCT const *CapTouch_curFlashSnsPtr = 0u;
#endif

/* Pointer to Flash structure to hold Sns electrode that was connected previously */
CapTouch_FLASH_IO_STRUCT const *CapTouch_curSnsIOPtr;


/*******************************************************************************
* Function Name: CapTouch_IsBusy
****************************************************************************//**
*
* \brief
*  Returns the current status of the Component (Scan is completed or Scan is in
*  progress).
*
* \details
*  This function returns a status of the hardware block whether a scan is
*  currently in progress or not. If the Component is busy, no new scan or setup
*  widgets is made. The critical section (i.e. disable global interrupt)
*  is recommended for the application when the device transitions from
*  the active mode to sleep or deep sleep modes.
*
* \return
*  Returns the current status of the Component:
*    - CapTouch_NOT_BUSY - No scan is in progress and a next scan
*      can be initiated.
*    - CapTouch_SW_STS_BUSY - The previous scanning is not completed
*      and the hardware block is busy.
*
*******************************************************************************/
uint32 CapTouch_IsBusy(void)
{
    return (CapTouch_dsRam.status & CapTouch_SW_STS_BUSY);
}

/*******************************************************************************
* Function Name: CapTouch_SetupWidget
****************************************************************************//**
*
* \brief
*  Performs the initialization required to scan the specified widget.
*
* \details
*  This function prepares the Component to scan all the sensors in the specified
*  widget by executing the following tasks:
*    1. Re-initialize the hardware if it is not configured to perform the
*       sensing method used by the specified widget, this happens only if multiple
*       sensing methods are used in the Component.
*    2. Initialize the hardware with specific sensing configuration (e.g.
*       sensor clock, scan resolution) used by the widget.
*    3. Disconnect all previously connected electrodes, if the electrodes
*       connected by the
*       \if SECTION_C_HIGH_LEVEL
*       CapTouch_CSDSetupWidgetExt(), CapTouch_CSXSetupWidgetExt(),
*       CapTouch_CSDConnectSns() functions and not disconnected.
*       \endif
*       \if SECTION_I_REP
*       CapTouch_ISXSetupWidgetExt() function and not disconnected.
*       \endif
*
*  This function does not start sensor scanning, the CapTouch_Scan()
*  function must be called to start the scan sensors in the widget. If this
*  function is called more than once, it does not break the Component operation,
*  but only the last initialized widget is in effect.
*
* \param widgetId
*  Specifies the ID number of the widget to be initialized for scanning.
*  A macro for the widget ID can be found in the
*  CapTouch Configuration header file defined as
*  CapTouch_<WidgetName>_WDGT_ID.
*
* \return
*  Returns the status of the widget setting up operation:
*    - CYRET_SUCCESS - The operation is successfully completed.
*    - CYRET_BAD_PARAM - The widget is invalid or if the specified widget is
*      disabled
*    - CYRET_INVALID_STATE - The previous scanning is not completed and the
*      hardware block is busy.
*    - CYRET_UNKNOWN - An unknown sensing method is used by the widget or any
*      other spurious error occurred.
*
**********************************************************************************/
cystatus CapTouch_SetupWidget(uint32 widgetId)
{
    cystatus widgetStatus;

    if (CapTouch_WDGT_SW_STS_BUSY == (CapTouch_dsRam.status & CapTouch_WDGT_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        widgetStatus = CYRET_INVALID_STATE;
    }
    /*
     *  Check if widget id is valid, specified widget is enabled and widget did not
     *  detect any fault conditions if BIST is enabled. If all conditions are met,
     *  set widgetStatus as good, if not, set widgetStatus as bad.
     */
    else if ((CapTouch_TOTAL_WIDGETS > widgetId) &&
        (0uL != CapTouch_GET_WIDGET_EN_STATUS(widgetId)))

    {
        widgetStatus = CYRET_SUCCESS;
    }
    else
    {
        widgetStatus = CYRET_BAD_PARAM;
    }

    /*
     * Check widgetStatus flag that is set earlier, if flag is good, then set up only
     * widget
     */
    if (CYRET_SUCCESS == widgetStatus)
    {
        switch (CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[widgetId]))
        {
        #if (CapTouch_ENABLE == CapTouch_CSX_EN)
            case CapTouch_SENSE_METHOD_CSX_E:
                /* Set up widget for CSX scan */
                CapTouch_CSXSetupWidget(widgetId);
                break;
        #endif
        #if (CapTouch_ENABLE == CapTouch_CSD_EN)
            case CapTouch_SENSE_METHOD_CSD_E:
                /* Set up widget for CSD scan */
                CapTouch_CSDSetupWidget(widgetId);
                break;
        #endif
        #if (CapTouch_ENABLE == CapTouch_ISX_EN)
            case CapTouch_SENSE_METHOD_ISX_E:
                /* Set up widget for ISX scan */
                CapTouch_ISXSetupWidget(widgetId);
                break;
        #endif
        default:
            /* Sensing method is invalid, return error to caller */
            widgetStatus = CYRET_UNKNOWN;
            break;
        }
    }
    return (widgetStatus);
}


/*******************************************************************************
* Function Name: CapTouch_Scan
****************************************************************************//**
*
* \brief
*  Initiates scanning of all the sensors in the widget initialized by
*  CapTouch_SetupWidget(), if no scan is in progress.
*
* \details
*  This function is called only after the CapTouch_SetupWidget()
*  function is called to start the scanning of the sensors in the widget. The
*  status of a sensor scan must be checked using the CapTouch_IsBusy()
*  API prior to starting a next scan or setting up another widget.
*
* \return
*  Returns the status of the scan initiation operation:
*  - CYRET_SUCCESS - Scanning is successfully started.
*  - CYRET_INVALID_STATE - The previous scanning is not completed and the
*    hardware block is busy.
*  - CYRET_UNKNOWN - An unknown sensing method is used by the widget.
*
********************************************************************************/
cystatus CapTouch_Scan(void)
{
    cystatus scanStatus = CYRET_SUCCESS;

    if (CapTouch_WDGT_SW_STS_BUSY == (CapTouch_dsRam.status & CapTouch_WDGT_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        scanStatus = CYRET_INVALID_STATE;
    }
    else
    {
        switch (CapTouch_currentSenseMethod)
        {
        #if (CapTouch_ENABLE == CapTouch_CSX_EN)
            case CapTouch_SENSE_METHOD_CSX_E:
                CapTouch_CSXScan();
                break;
        #endif
        #if (CapTouch_ENABLE == CapTouch_CSD_EN)
            case CapTouch_SENSE_METHOD_CSD_E:
                CapTouch_CSDScan();
                break;
        #endif
        #if (CapTouch_ENABLE == CapTouch_ISX_EN)
            case CapTouch_SENSE_METHOD_ISX_E:
                CapTouch_ISXScan();
                break;
        #endif
        default:
            scanStatus = CYRET_UNKNOWN;
            break;
        }
    }
    return (scanStatus);
}


/*******************************************************************************
* Function Name: CapTouch_ScanAllWidgets
****************************************************************************//**
*
* \brief
*  Initializes the first enabled widget and scanning of all the sensors in the
*  widget, then the same process is repeated for all the widgets in the Component,
*  i.e. scanning of all the widgets in the Component.
*
* \details
*  This function initializes a widget and scans all the sensors in the widget,
*  and then repeats the same for all the widgets in the Component. The tasks of
*  the CapTouch_SetupWidget() and CapTouch_Scan() functions are
*  executed by these functions. The status of a sensor scan must be checked
*  using the CapTouch_IsBusy() API prior to starting a next scan
*  or setting up another widget.
*
* \return
*  Returns the status of the operation:
*  - CYRET_SUCCESS - Scanning is successfully started.
*  - CYRET_BAD_PARAM - All the widgets are disabled.
*  - CYRET_INVALID_STATE - The previous scanning is not completed and the HW block is busy.
*  - CYRET_UNKNOWN - There are unknown errors.
*
*******************************************************************************/
cystatus CapTouch_ScanAllWidgets(void)
{
    cystatus scanStatus = CYRET_UNKNOWN;

    uint32 wdgtIndex;

    if (CapTouch_SW_STS_BUSY == (CapTouch_dsRam.status & CapTouch_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        scanStatus = CYRET_INVALID_STATE;
    }
    else
    {
        /*
         *  Set up widget first widget.
         *  If widget returned error, set up next, continue same until widget does not return error.
         */
        for (wdgtIndex = 0u;
             wdgtIndex < CapTouch_TOTAL_WIDGETS;
             wdgtIndex++)
        {

            scanStatus = CapTouch_SetupWidget(wdgtIndex);

            if (CYRET_SUCCESS == scanStatus)
            {
                #if (0u != (CapTouch_TOTAL_WIDGETS - 1u))
                    /* If there are more than one widget to be scanned, request callback to scan next widget */
                    if ((CapTouch_TOTAL_WIDGETS - 1u) > wdgtIndex)
                    {
                         /* Request callback to scan next widget in ISR */
                        CapTouch_requestScanAllWidget = CapTouch_ENABLE;
                    }
                    else
                    {
                        /* Request to exit in ISR (Do not scan the next widgets) */
                        CapTouch_requestScanAllWidget = CapTouch_DISABLE;
                    }
                #else
                    {
                        /* Request to exit in ISR (We have only one widget) */
                        CapTouch_requestScanAllWidget = CapTouch_DISABLE;
                    }
                #endif  /* (0u != (CapTouch_TOTAL_WIDGETS - 1u)) */

                /* Initiate scan and quit loop */
                scanStatus = CapTouch_Scan();

                break;
            }
        }
    }

    return (scanStatus);
}


/*******************************************************************************
* Function Name: CapTouch_SsInitialize
****************************************************************************//**
*
* \brief
*  Performs hardware and firmware initialization required for proper operation
*  of the CapTouch Component. This function is called from
*  the CapTouch_Start() API prior to calling any other APIs of the Component.
*
* \details
*  Performs hardware and firmware initialization required for proper operation
*  of the CapTouch Component. This function is called from
*  the CapTouch_Start() API prior to calling any other APIs of the Component.
*  1. Depending on the configuration, the function initializes the CSD block
*     for the corresponding sensing mode.
*  2. The function updates the dsRam.wdgtWorking variable with 1 when Self-test
*     is enabled.
*
*  Calling the CapTouch_Start API is the recommended method to initialize
*  the CapTouch Component at power-up. The CapTouch_SsInitialize()
*  API should not be used for initialization, resume, or wake-up operations.
*  The dsRam.wdgtWorking variable is updated.
*
* \return status
*  Returns status of operation:
*  - Zero        - Indicates successful initialization.
*  - Non-zero    - One or more errors occurred in the initialization process.
*
*******************************************************************************/
cystatus CapTouch_SsInitialize(void)
{
    cystatus initStatus;

    #if ((CapTouch_ENABLE == CapTouch_MULTI_FREQ_SCAN_EN) &&\
         (CapTouch_MFS_IMO == CapTouch_MFS_METHOD))
        uint8 centerFreqTrim;

        centerFreqTrim = (uint8)CY_GET_REG32(CY_SYS_CLK_IMO_TRIM1_PTR);
        CapTouch_immunity[CapTouch_FREQ_CHANNEL_0] = centerFreqTrim;

        if (centerFreqTrim < CapTouch_IMO_FREQUENCY_OFFSET_F1)
        {
            CapTouch_immunity[CapTouch_FREQ_CHANNEL_1] = centerFreqTrim + CapTouch_IMO_FREQUENCY_OFFSET_F1;
            CapTouch_immunity[CapTouch_FREQ_CHANNEL_2] = centerFreqTrim + (CapTouch_IMO_FREQUENCY_OFFSET_F1 +
                                                                                           CapTouch_IMO_FREQUENCY_OFFSET_F2);
        }
        else if (centerFreqTrim > (CapTouch_IMO_FREQUENCY_OFFSET_MAX - CapTouch_IMO_FREQUENCY_OFFSET_F2))
        {
            CapTouch_immunity[CapTouch_FREQ_CHANNEL_1] = centerFreqTrim - CapTouch_IMO_FREQUENCY_OFFSET_F1;
            CapTouch_immunity[CapTouch_FREQ_CHANNEL_2] = centerFreqTrim - (CapTouch_IMO_FREQUENCY_OFFSET_F1 +
                                                                                           CapTouch_IMO_FREQUENCY_OFFSET_F2);
        }
        else
        {
            CapTouch_immunity[CapTouch_FREQ_CHANNEL_1] = centerFreqTrim - CapTouch_IMO_FREQUENCY_OFFSET_F1;
            CapTouch_immunity[CapTouch_FREQ_CHANNEL_2] = centerFreqTrim + CapTouch_IMO_FREQUENCY_OFFSET_F2;
        }

    #endif /* ((CapTouch_ENABLE == CapTouch_MULTI_FREQ_SCAN_EN) &&\
               (CapTouch_MFS_IMO == CapTouch_MFS_METHOD)) */

    initStatus = CYRET_SUCCESS;

    #if (CapTouch_DISABLE == CapTouch_CSDV2)
        CapTouch_SsTrimIdacs();
    #endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

    #if((CapTouch_ENABLE == CapTouch_CSD_EN) ||\
    ((CapTouch_ENABLE == CapTouch_CSX_EN) && (CapTouch_ENABLE == CapTouch_CSDV2)))
        CapTouch_SsInitializeSourceSenseClk();
    #endif /* ((CapTouch_ENABLE == CapTouch_CSD_EN) ||\
              ((CapTouch_ENABLE == CapTouch_CSX_EN) && (CapTouch_ENABLE == CapTouch_CSDV2))) */

    /* Set all IO states to the default state */
    CapTouch_SsSetIOsInDefaultState();

    #if (CapTouch_ENABLE == CapTouch_MANY_SENSE_MODES_EN)
        /* Initialize CSD block for ADC scanning */
        CapTouch_SsSwitchSensingMode(CapTouch_UNDEFINED_E);
    #elif (CapTouch_ENABLE == CapTouch_CSD_EN)
        CapTouch_SsSwitchSensingMode(CapTouch_SENSE_METHOD_CSD_E);
    #elif (CapTouch_ENABLE == CapTouch_CSX_EN)
        CapTouch_SsSwitchSensingMode(CapTouch_SENSE_METHOD_CSX_E);
    #elif (CapTouch_ENABLE == CapTouch_ISX_EN)
        CapTouch_SsSwitchSensingMode(CapTouch_SENSE_METHOD_ISX_E);
    #else
        #error "No sensing method enabled, Component cannot work in this mode"
        initStatus = CYRET_UNKNOWN;
    #endif

    return (initStatus);
}


/*******************************************************************************
* Function Name: CapTouch_SetPinState
****************************************************************************//**
*
* \brief
*  Sets the state (drive mode and output state) of the port pin used by a sensor.
*  The possible states are GND, Shield, High-Z, Tx or Rx, Sensor. If the sensor
*  specified in the input parameter is a ganged sensor, then the state of all pins
*  associated with the ganged sensor is updated.
*
* \details
*  This function sets a specified state for a specified sensor element. For the
*  CSD widgets, sensor element is a sensor ID, for the CSX widgets, it is either
*  an Rx or Tx electrode ID. If the specified sensor is a ganged sensor, then
*  the specified state is set for all the electrodes belong to the sensor.
*  This function must not be called while the Component is in the busy state.
*
*  This function accepts the CapTouch_SHIELD and
*  CapTouch_SENSOR states as an input only if there is at least
*  one CSD widget. Similarly, this function accepts the CapTouch_TX_PIN
*  and CapTouch_RX_PIN states as an input only if there is at least
*  one CSX widget in the project.
*
*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the custom-specific
*  use cases. Functions that perform a setup and scan of a sensor/widget
*  automatically set the required pin states. They ignore changes
*  in the design made by the CapTouch_SetPinState() function.
*  This function neither check wdgtIndex nor sensorElement for the correctness.
*
* \param widgetId
*  Specifies the ID of the widget to change the pin state of the specified
*  sensor.
*  A macro for the widget ID can be found in the CapTouch Configuration
*  header file defined as CapTouch_<WidgetName>_WDGT_ID.
*
* \param sensorElement
*  Specifies the ID of the sensor element within the widget to change
*  its pin state.
*  \if SECTION_C_LOW_LEVEL
*  For the CSD widgets, sensorElement is the sensor ID and can be found in the
*  CapTouch Configuration header file defined as
*  - CapTouch_<WidgetName>_SNS<SensorNumber>_ID.
*  For the CSX widgets, sensorElement is defined either as Rx ID or Tx ID.
*  The first Rx in a widget corresponds to sensorElement = 0, the second
*  Rx in a widget corresponds to sensorElement = 1, and so on.
*  The last Tx in a widget corresponds to sensorElement = (RxNum + TxNum).
*  \endif
*  Macros for Rx and Tx IDs can be found in the
*  CapTouch Configuration header file defined as:
*  - CapTouch_<WidgetName>_RX<RXNumber>_ID
*  - CapTouch_<WidgetName>_TX<TXNumber>_ID.
*
* \param state
*  Specifies the state of the sensor to be set:
*     1. CapTouch_GROUND - The pin is connected to the ground.
*     2. CapTouch_HIGHZ - The drive mode of the pin is set to High-Z
*        Analog.
*     3. CapTouch_SHIELD - The shield signal is routed to the pin
*        (available only if CSD sensing method with shield electrode is enabled).
*     4. CapTouch_SENSOR - The pin is connected to the scanning bus
*        (available only if CSD sensing method is enabled).
*     5. CapTouch_TX_PIN - The Tx signal is routed to the sensor
*        (available only if CSX sensing method is enabled).
*     6. CapTouch_RX_PIN - The pin is connected to the scanning bus
*        (available only if CSX sensing method is enabled).
*
*******************************************************************************/
void CapTouch_SetPinState(uint32 widgetId, uint32 sensorElement, uint32 state)
{
    uint32 eltdNum;
    uint32 eltdIndex;
    uint8  interruptState;
    CapTouch_FLASH_IO_STRUCT const *ioPtr;
    #if (CapTouch_ENABLE == CapTouch_GANGED_SNS_EN)
        CapTouch_FLASH_SNS_STRUCT const *curFlashSnsPtr;
    #endif

    /* Getting sensor element pointer and number of electrodes */
    #if (CapTouch_ENABLE == CapTouch_GANGED_SNS_EN)
        /* Check the ganged sns flag */
        if (CapTouch_GANGED_SNS_MASK == (CapTouch_dsFlash.wdgtArray[widgetId].staticConfig & CapTouch_GANGED_SNS_MASK))
        {
            curFlashSnsPtr = CapTouch_dsFlash.wdgtArray[widgetId].ptr2SnsFlash;
            curFlashSnsPtr += sensorElement;
            ioPtr = &CapTouch_ioList[curFlashSnsPtr->firstPinId];
            eltdNum = curFlashSnsPtr->numPins;
        }
        else
        {
            ioPtr = (CapTouch_FLASH_IO_STRUCT const *)CapTouch_dsFlash.wdgtArray[widgetId].ptr2SnsFlash + sensorElement;
            eltdNum = 1u;
        }
    #else
        ioPtr = (CapTouch_FLASH_IO_STRUCT const *)CapTouch_dsFlash.wdgtArray[widgetId].ptr2SnsFlash + sensorElement;
        eltdNum = 1u;
    #endif

    /* Loop through all electrodes of the specified sensor element */
    for (eltdIndex = 0u; eltdIndex < eltdNum; eltdIndex++)
    {
        /* Reset HSIOM and PC registers */
        interruptState = CyEnterCriticalSection();
        CY_SET_REG32(ioPtr->hsiomPtr, CY_GET_REG32(ioPtr->hsiomPtr) & ~(CapTouch_HSIOM_SEL_MASK << ioPtr->hsiomShift));
        CY_SET_REG32(ioPtr->pcPtr, CY_GET_REG32(ioPtr->pcPtr) & ~(CapTouch_GPIO_PC_MASK << ioPtr->shift));
        CyExitCriticalSection(interruptState);

        switch (state)
        {
        case CapTouch_GROUND:

            interruptState = CyEnterCriticalSection();
            CY_SET_REG32(ioPtr->pcPtr, CY_GET_REG32(ioPtr->pcPtr) | (CapTouch_SNS_GROUND_CONNECT << ioPtr->shift));
            CY_SET_REG32(ioPtr->drPtr, CY_GET_REG32(ioPtr->drPtr) & (uint32)(~(uint32)((uint32)1u << ioPtr->drShift)));
            CyExitCriticalSection(interruptState);
            break;

        case CapTouch_HIGHZ:
            interruptState = CyEnterCriticalSection();
            CY_SET_REG32(ioPtr->drPtr, CY_GET_REG32(ioPtr->drPtr) & (uint32)~(uint32)((uint32)1u << ioPtr->drShift));
            CyExitCriticalSection(interruptState);
            break;

        #if (CapTouch_ENABLE == CapTouch_CSD_EN)
            case CapTouch_SENSOR:
                /* Enable sensor */
                CapTouch_CSDConnectSns(ioPtr);
                break;

            #if (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN)
                case CapTouch_SHIELD:
                    /* Set appropriate bit in HSIOM register to configure pin to shield mode */
                    interruptState = CyEnterCriticalSection();
                    CY_SET_REG32(ioPtr->hsiomPtr, CY_GET_REG32(ioPtr->hsiomPtr) | (CapTouch_HSIOM_SEL_CSD_SHIELD << ioPtr->hsiomShift));
                    CyExitCriticalSection(interruptState);
                    break;
            #endif /* (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) */
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */

        #if ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
             (CapTouch_ENABLE == CapTouch_ISX_EN))
            case CapTouch_TX_PIN:
                interruptState = CyEnterCriticalSection();
                CY_SET_REG32(ioPtr->hsiomPtr, CY_GET_REG32(ioPtr->hsiomPtr) | (CapTouch_HSIOM_SEL_CSD_SENSE << ioPtr->hsiomShift));
                CY_SET_REG32(ioPtr->pcPtr, CY_GET_REG32(ioPtr->pcPtr) | (CapTouch_GPIO_STRGDRV << ioPtr->shift));
                CyExitCriticalSection(interruptState);
                break;

            case CapTouch_RX_PIN:
                interruptState = CyEnterCriticalSection();
                CY_SET_REG32(ioPtr->hsiomPtr, CY_GET_REG32(ioPtr->hsiomPtr) | (CapTouch_HSIOM_SEL_AMUXA << ioPtr->hsiomShift));
                CY_SET_REG32(ioPtr->pcPtr, CY_GET_REG32(ioPtr->pcPtr) & ~(CapTouch_GPIO_PC_MASK << ioPtr->shift));
                CyExitCriticalSection(interruptState);
                break;
        #endif

        default:
            /* Wrong state */
            break;
        }

        ioPtr++;
    }
}

#if (CapTouch_ENABLE == CapTouch_CSD_EN)
    /*******************************************************************************
    * Function Name: CapTouch_SsCSDDisableMode
    ****************************************************************************//**
    *
    * \brief
    *  This function disables CSD mode.
    *
    * \details
    *  To disable CSD mode the following tasks are performed:
    *  1. Disconnect Cmod from AMUXBUS-A.
    *  2. Disconnect previous CSX electrode if it has been connected.
    *  3. Disconnect Csh from AMUXBUS-B.
    *  4. Disable Shield Electrodes.
    *
    *******************************************************************************/
    static void CapTouch_SsCSDDisableMode(void)
    {
        uint32 newRegValue;

        /* Disconnect Cmod from AMUXBUS-A using HSIOM registers */
        newRegValue = CY_GET_REG32(CapTouch_CMOD_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapTouch_CMOD_HSIOM_MASK);
        CY_SET_REG32(CapTouch_CMOD_HSIOM_PTR, newRegValue);

        #if ((CapTouch_ENABLE == CapTouch_CSDV2) && \
             ((CapTouch_ENABLE == CapTouch_CSD_IDAC_COMP_EN) && \
             (CapTouch_ENABLE == CapTouch_CSD_DEDICATED_IDAC_COMP_EN)))
            /* Disconnect IDACA and IDACB */
            newRegValue = CY_GET_REG32(CapTouch_SW_REFGEN_SEL_PTR);
            newRegValue &= (uint32)(~CapTouch_SW_REFGEN_SEL_SW_IAIB_MASK);
            CY_SET_REG32(CapTouch_SW_REFGEN_SEL_PTR, newRegValue);
        #endif /* ((CapTouch_ENABLE == CapTouch_CSDV2) && \
                   ((CapTouch_ENABLE == CapTouch_CSD_IDAC_COMP_EN) && \
                   (CapTouch_ENABLE == CapTouch_CSD_DEDICATED_IDAC_COMP_EN))) */

        /* Disconnect previous CSD electrode if it has been connected */
        CapTouch_SsCSDElectrodeCheck();

        /* Disconnect Csh from AMUXBUS-B */
        #if ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
             (CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN))
            newRegValue = CY_GET_REG32(CapTouch_CSH_HSIOM_PTR);
            newRegValue &= (uint32)(~(uint32)(CapTouch_CSH_TO_AMUXBUS_B_MASK << CapTouch_CSH_HSIOM_SHIFT));
            CY_SET_REG32(CapTouch_CSH_HSIOM_PTR, newRegValue);
        #endif /* ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
                   (CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN)) */

        #if ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
             (0u != CapTouch_CSD_TOTAL_SHIELD_COUNT))
            CapTouch_SsCSDDisableShieldElectrodes();
        #endif /* ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
                   (0u != CapTouch_CSD_TOTAL_SHIELD_COUNT)) */
    }
#endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */


#if (CapTouch_ENABLE == CapTouch_CSX_EN)
    /*******************************************************************************
    * Function Name: CapTouch_SsDisableCSXMode
    ****************************************************************************//**
    *
    * \brief
    *  This function disables CSX mode.
    *
    * \details
    *  To disable CSX mode the following tasks are performed:
    *  1. Disconnect CintA and CintB from AMUXBUS-A.
    *  2. Disconnect previous CSX electrode if it has been connected.
    *
    *******************************************************************************/
    static void CapTouch_SsDisableCSXMode(void)
    {
        uint32 newRegValue;

        /* Disconnect CintA from AMUXBUS-A using HSIOM registers */
        newRegValue = CY_GET_REG32(CapTouch_CintA_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapTouch_CintA_HSIOM_MASK);
        CY_SET_REG32(CapTouch_CintA_HSIOM_PTR, newRegValue);

        /* Disconnect CintB from AMUXBUS-A using HSIOM registers */
        newRegValue = CY_GET_REG32(CapTouch_CintB_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapTouch_CintB_HSIOM_MASK);
        CY_SET_REG32(CapTouch_CintB_HSIOM_PTR, newRegValue);

        /* Disconnect previous CSX electrode if it has been connected */
        CapTouch_CSXElectrodeCheck();
    }
#endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) */

#if (CapTouch_ENABLE == CapTouch_ISX_EN)
    /*******************************************************************************
    * Function Name: CapTouch_SsDisableISXMode
    ****************************************************************************//**
    *
    * \brief
    *  This function disables ISX mode.
    *
    * \details
    *  To disable ISX mode the following tasks are performed:
    *  1. Disconnect CintA and CintB from AMUXBUS-A
    *  2. Disconnect previous ISX electrode if it has been connected
    *
    *******************************************************************************/
    static void CapTouch_SsDisableISXMode(void)
    {
        uint32 newRegValue;

        /* Disconnect CintA from AMUXBUS-A using HSIOM registers.   */
        newRegValue = CY_GET_REG32(CapTouch_CintA_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapTouch_CintA_HSIOM_MASK);
        CY_SET_REG32(CapTouch_CintA_HSIOM_PTR, newRegValue);

        /* Disconnect CintB from AMUXBUS-A using HSIOM registers.   */
        newRegValue = CY_GET_REG32(CapTouch_CintB_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapTouch_CintB_HSIOM_MASK);
        CY_SET_REG32(CapTouch_CintB_HSIOM_PTR, newRegValue);

        /* Disconnect previous ISX electrode if it has been connected */
        CapTouch_ISXElectrodeCheck();
        CapTouch_DigPrt2_Stop();
        #if (CapTouch_USES_PORT_3)
            CapTouch_DigPrt3_Stop();
        #endif /* CapTouch_USES_PORT_3 */
    }
#endif /* (CapTouch_ENABLE == CapTouch_ISX_EN) */

/*******************************************************************************
* Function Name: CapTouch_SsSwitchSensingMode
****************************************************************************//**
*
* \brief
*  This function changes the mode for case when both CSD and CSX widgets are
*  scanned.
*
* \details
*  To switch to the CSD mode the following tasks are performed:
*  1. Disconnect CintA and CintB from AMUXBUS-A.
*  2. Disconnect previous CSD electrode if it has been connected.
*  3. Initialize CSD mode.
*
*  To switch to the CSX mode the following tasks are performed:
*  1. Disconnect Cmod from AMUXBUS-A.
*  2. Disconnect previous CSX electrode if it has been connected.
*  3. Initialize CSX mode.
*
* \param mode
*  Specifies the scan mode:
*  - CapTouch_SENSE_METHOD_CSD_E
*  - CapTouch_SENSE_METHOD_CSX_E
*  - CapTouch_SENSE_METHOD_ISX_E
*  - CapTouch_SENSE_METHOD_BIST_E
*  - CapTouch_UNDEFINED_E
*
*******************************************************************************/
void CapTouch_SsSwitchSensingMode(CapTouch_SENSE_METHOD_ENUM mode)
{
    if (CapTouch_currentSenseMethod != mode)
    {
        /* The requested mode differes to the current one. Disable the current mode */
        if (CapTouch_SENSE_METHOD_CSD_E == CapTouch_currentSenseMethod)
        {
            #if (CapTouch_ENABLE == CapTouch_CSD_EN)
                CapTouch_SsCSDDisableMode();
            #endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */
        }
        else if (CapTouch_SENSE_METHOD_CSX_E == CapTouch_currentSenseMethod)
        {
            #if (CapTouch_ENABLE == CapTouch_CSX_EN)
                CapTouch_SsDisableCSXMode();
            #endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) */
        }
        else if (CapTouch_SENSE_METHOD_ISX_E == CapTouch_currentSenseMethod)
        {
            #if (CapTouch_ENABLE == CapTouch_ISX_EN)
                CapTouch_SsDisableISXMode();
            #endif /* (CapTouch_ENABLE == CapTouch_ISX_EN) */
        }
        else if (CapTouch_SENSE_METHOD_BIST_E == CapTouch_currentSenseMethod)
        {
            #if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)
                CapTouch_BistDisableMode();
            #endif /* (CapTouch_ENABLE == CapTouch_SELF_TEST_EN) */
        }
        else
        {
            #if (CapTouch_ENABLE == CapTouch_ADC_EN)
                /* Release ADC resources */
                (void)CapTouch_AdcReleaseResources();
            #endif /* (CapTouch_ENABLE == CapTouch_ADC_EN) */
        }

        /* Enable the specified mode */
        if (CapTouch_SENSE_METHOD_CSD_E == mode)
        {
            #if (CapTouch_ENABLE == CapTouch_CSD_EN)
                /* Initialize CSD mode to guarantee configured CSD mode */
                CapTouch_SsCSDInitialize();
                CapTouch_currentSenseMethod = CapTouch_SENSE_METHOD_CSD_E;
            #endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */
        }
        else if (CapTouch_SENSE_METHOD_CSX_E == mode)
        {
            #if (CapTouch_ENABLE == CapTouch_CSX_EN)
                /* Initialize CSX mode to guarantee configured CSX mode */
                CapTouch_CSXInitialize();
                CapTouch_currentSenseMethod = CapTouch_SENSE_METHOD_CSX_E;
            #endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) */
        }
        else if (CapTouch_SENSE_METHOD_ISX_E == mode)
        {
            #if (CapTouch_ENABLE == CapTouch_ISX_EN)
                /* Initialize ISX mode to guarantee configured ISX mode */
                CapTouch_ISXInitialize();
                CapTouch_currentSenseMethod = CapTouch_SENSE_METHOD_ISX_E;
            #endif /* (CapTouch_ENABLE == CapTouch_ISX_EN) */
        }
        else if (CapTouch_SENSE_METHOD_BIST_E == mode)
        {
            #if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)
                CapTouch_BistInitialize();
                CapTouch_currentSenseMethod = CapTouch_SENSE_METHOD_BIST_E;
            #endif /* (CapTouch_ENABLE == CapTouch_SELF_TEST_EN) */
        }
        else
        {
            CapTouch_currentSenseMethod = CapTouch_UNDEFINED_E;
        }
    }
}


/*******************************************************************************
* Function Name: CapTouch_SsSetIOsInDefaultState
****************************************************************************//**
*
* \brief
*  Sets all electrodes into a default state.
*
* \details
*  Sets all the CSD/CSX IOs into a default state:
*  - HSIOM   - Disconnected, the GPIO mode.
*  - DM      - Strong drive.
*  - State   - Zero.
*
*  Sets all the ADC channels into a default state:
*  - HSIOM   - Disconnected, the GPIO mode.
*  - DM      - HiZ-Analog.
*  - State   - Zero.
*
*  It is not recommended to call this function directly from the application
*  layer.
*
*******************************************************************************/
void CapTouch_SsSetIOsInDefaultState(void)
{
    CapTouch_FLASH_IO_STRUCT const *ioPtr = &CapTouch_ioList[0u];
    uint32 loopIndex;
    uint32 regValue;

    /* Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < CapTouch_TOTAL_ELECTRODES; loopIndex++)
    {
        /*
        * 1. Disconnect HSIOM.
        * 2. Set strong DM.
        * 3. Set pin state to logic 0.
        */
        regValue = CY_GET_REG32 (ioPtr->hsiomPtr);
        regValue &= ~(ioPtr->hsiomMask);
        CY_SET_REG32 (ioPtr->hsiomPtr, regValue);

        regValue = CY_GET_REG32 (ioPtr->pcPtr);
        regValue &= ~(CapTouch_GPIO_PC_MASK << ioPtr->shift);
        regValue |=  (CapTouch_GPIO_STRGDRV << ioPtr->shift);
        CY_SET_REG32 (ioPtr->pcPtr, regValue);

        regValue = CY_GET_REG32 (ioPtr->drPtr);
        regValue &= ~(ioPtr->mask);
        CY_SET_REG32 (ioPtr->drPtr, regValue);

        /* Get next electrode */
        ioPtr++;
    }

    #if(CapTouch_ENABLE == CapTouch_ADC_EN)
        CapTouch_ClearAdcChannels();
    #endif /* (CapTouch_ENABLE == CapTouch_ADC_EN) */
    
    #if(CapTouch_ENABLE == CapTouch_ISX_EN)
        CapTouch_ISXSetDefaultDigital();
    #endif /* (CapTouch_ENABLE == CapTouch_ISX_EN) */
}

#if (CapTouch_ENABLE == CapTouch_ADC_EN)
/*******************************************************************************
* Function Name: CapTouch_SsReleaseResources()
****************************************************************************//**
*
* \brief
*  This function sets the resources that do not belong to the sensing HW block to
*  default state.
*
* \details
*  The function performs following tasks:
*  1. Checks if CSD block busy and returns error if it is busy
*  2. Disconnects integration capacitors (CintA, CintB for CSX mode and
*     Cmod for CSD mode)
*  3. Disconnect electroded if they have been connected.
*
* \return
*  Returns the status of the operation:
*  - Zero        - Resources released successfully.
*  - Non-zero    - One or more errors occurred in releasing process.
*
*******************************************************************************/
cystatus CapTouch_SsReleaseResources(void)
{
    cystatus busyStatus = CYRET_SUCCESS;

    if (CapTouch_SW_STS_BUSY == (CapTouch_dsRam.status & CapTouch_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        busyStatus = CYRET_INVALID_STATE;
    }
    else
    {
        #if (CapTouch_ENABLE == CapTouch_CSX_EN)
            CapTouch_SsDisableCSXMode();
        #endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) */

        #if (CapTouch_ENABLE == CapTouch_ISX_EN)
            CapTouch_SsDisableISXMode();
        #endif /* (CapTouch_ENABLE == CapTouch_ISX_EN) */

        #if (CapTouch_ENABLE == CapTouch_CSD_EN)
            CapTouch_SsCSDDisableMode();
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */

        #if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)
            CapTouch_BistDisableMode();
        #endif /* (CapTouch_ENABLE == CapTouch_SELF_TEST_EN) */

        #if ((CapTouch_ENABLE == CapTouch_CSD_EN) && \
             (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) &&  \
             (CapTouch_SNS_CONNECTION_SHIELD == CapTouch_CSD_INACTIVE_SNS_CONNECTION))
            CapTouch_SsSetIOsInDefaultState();
        #endif /* ((CapTouch_ENABLE == CapTouch_CSD_EN) && \
             (CapTouch_DISABLE != CapTouch_CSD_SHIELD_EN) &&  \
             (CapTouch_SNS_CONNECTION_SHIELD == CapTouch_CSD_INACTIVE_SNS_CONNECTION)) */

        /*
         * Reset of the currentSenseMethod variable to make sure that the next
         * call of SetupWidget() API setups the correct widget mode
         */
        CapTouch_currentSenseMethod = CapTouch_UNDEFINED_E;
    }

    return busyStatus;
}
#endif /* (CapTouch_ENABLE == CapTouch_ADC_EN) */


/*******************************************************************************
* Function Name: CapTouch_SsPostAllWidgetsScan
****************************************************************************//**
*
* \brief
*  The ISR function for multiple widget scanning implementation.
*
* \details
*  This is the function used by the CapTouch ISR to implement multiple widget
*  scanning.
*  Should not be used by the application layer.
*
*******************************************************************************/
void CapTouch_SsPostAllWidgetsScan(void)
{
    /*
    *  1. Increment widget index
    *  2. Check if all the widgets are scanned
    *  3. If all the widgets are not scanned, set up and scan next widget
    */
    #if (1u != CapTouch_TOTAL_WIDGETS)
        cystatus postScanStatus;

        do
        {
            CapTouch_widgetIndex++;

            postScanStatus = CapTouch_SetupWidget((uint32)CapTouch_widgetIndex);

            if (CYRET_SUCCESS == postScanStatus)
            {
                if((CapTouch_TOTAL_WIDGETS - 1u) == CapTouch_widgetIndex)
                {
                    /* The last widget will be scanned. Reset flag to skip configuring the next widget setup in ISR. */
                    CapTouch_requestScanAllWidget = CapTouch_DISABLE;
                }
                (void)CapTouch_Scan();
            }
            else if((CapTouch_TOTAL_WIDGETS - 1u) == CapTouch_widgetIndex)
            {
                #if ((CapTouch_ENABLE == CapTouch_BLOCK_OFF_AFTER_SCAN_EN) && \
                     (CapTouch_ENABLE == CapTouch_CSD_EN))
                    if (CapTouch_SENSE_METHOD_CSD_E ==
                        CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[CapTouch_widgetIndex]))
                    {
                        /* Disable the CSD block */
                        CY_SET_REG32(CapTouch_CONFIG_PTR, CapTouch_configCsd);
                    }
                #endif /* ((CapTouch_ENABLE == CapTouch_BLOCK_OFF_AFTER_SCAN_EN) && \
                           (CapTouch_ENABLE == CapTouch_CSD_EN)) */

                /* Update scan Counter */
                CapTouch_dsRam.scanCounter++;
                /* All widgets are totally processed. Reset BUSY flag */
                CapTouch_dsRam.status &= ~CapTouch_SW_STS_BUSY;

                /* Update status with with the failure */
                CapTouch_dsRam.status &= ~CapTouch_STATUS_ERR_MASK;
                CapTouch_dsRam.status |= ((postScanStatus & CapTouch_STATUS_ERR_SIZE) << CapTouch_STATUS_ERR_SHIFT);

                /* Set postScanStatus to exit the while loop */
                postScanStatus = CYRET_SUCCESS;
            }
            else
            {
                /* Update status with with the failure. Configure the next widget in while() loop */
                CapTouch_dsRam.status &= ~CapTouch_STATUS_ERR_MASK;
                CapTouch_dsRam.status |= ((postScanStatus & CapTouch_STATUS_ERR_SIZE) << CapTouch_STATUS_ERR_SHIFT);
            }
        } while (CYRET_SUCCESS != postScanStatus);
    #endif /* (1u != CapTouch_TOTAL_WIDGETS) */
}


/*******************************************************************************
* Function Name: CapTouch_SsIsrInitialize
****************************************************************************//**
*
* \brief
*  Enables and initializes for the function pointer for a callback for the ISR.
*
* \details
*  The "address" is a special type cyisraddress defined by Cylib. This function
*  is used by Component APIs and should not be used by an application program for
*  proper working of the Component.
*
* \param address
*  The address of the function to be called when interrupt is fired.
*
*******************************************************************************/
void CapTouch_SsIsrInitialize(cyisraddress address)
{
    CapTouch_ISR_StartEx(address);
}


/*******************************************************************************
* Function Name: CapTouch_SsSetModClkClockDivider
****************************************************************************//**
*
* \brief
*  Sets the divider values for the modulator clock and then starts
*  the modulator clock.
*
* \details
*  It is not recommended to call this function directly by the application layer.
*  It is used by initialization, widget APIs or wakeup functions to
*  enable the clocks.
*
* \param modClk
*  The divider value for the modulator clock.
*
*******************************************************************************/
void CapTouch_SsSetModClkClockDivider(uint32 modClk)
{
#if (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK)
    /* Stop modulator clock */
    CY_SET_REG32(CapTouch_MODCLK_CMD_PTR,
                 ((uint32)CapTouch_ModClk__DIV_ID <<
                 CapTouch_MODCLK_CMD_DIV_SHIFT)|
                 ((uint32)CapTouch_MODCLK_CMD_DISABLE_MASK));

    /*
     * Set divider value for sense modClk.
     * 1u is subtracted from modClk because Divider register value 0 corresponds
     * to dividing by 1.
     */
    CY_SET_REG32(CapTouch_MODCLK_DIV_PTR, ((modClk - 1u) << 8u));

    /* Check whether previous modulator clock start command has finished. */
    while(0u != (CY_GET_REG32(CapTouch_MODCLK_CMD_PTR) & CapTouch_MODCLK_CMD_ENABLE_MASK))
    {
        /* Wait until previous modulator clock start command has finished */
    }

    /* Start modulator clock, aligned to HFCLK */
    CY_SET_REG32(CapTouch_MODCLK_CMD_PTR,
                 (uint32)(((uint32)CapTouch_ModClk__DIV_ID << CapTouch_MODCLK_CMD_DIV_SHIFT) |
                  ((uint32)CapTouch_ModClk__PA_DIV_ID << CapTouch_MODCLK_CMD_PA_DIV_SHIFT) |
                  CapTouch_MODCLK_CMD_ENABLE_MASK));
#else
    uint32 newRegValue;

    /* Clear bit to disable ModClk clock. */
    CY_SET_REG32(CapTouch_MODCLK_CMD_PTR,
                 CY_GET_REG32(CapTouch_MODCLK_CMD_PTR) &
                 (uint32)(~CapTouch_ModClk__ENABLE_MASK));

    /*
     * Update ModClk clock divider.
     * 1u is subtracted from modClk because Divider register value has range (0-65535).
     */
    newRegValue = CY_GET_REG32(CapTouch_MODCLK_DIV_PTR) & (uint32)(~(uint32)CapTouch_ModClk__DIVIDER_MASK);
    newRegValue |= (modClk - 1u);
    CY_SET_REG32(CapTouch_MODCLK_DIV_PTR, newRegValue);

    /* Set bit to enable ModClk clock. */
    CY_SET_REG32(CapTouch_MODCLK_CMD_PTR,
                 CY_GET_REG32(CapTouch_MODCLK_CMD_PTR) |
                CapTouch_ModClk__ENABLE_MASK);
#endif /* (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK) */
}


/*******************************************************************************
* Function Name: CapTouch_SsSetSnsClockDivider
****************************************************************************//**
*
* \brief
*  Sets the divider values for the sense clock and then starts
*  the sense clock.
*
* \details
*  It is not recommended to call this function directly by the application layer.
*  It is used by initialization, widget APIs or wakeup functions to
*  enable the clocks.
*
* \param
*  snsClk The divider value for the sense clock.
*
*******************************************************************************/
void CapTouch_SsSetSnsClockDivider(uint32 snsClk)
{
#if (CapTouch_ENABLE == CapTouch_CSDV2)
    uint32 newRegValue;

    /*
     * Set divider value for sense clock.
     * 1u is subtracted from snsClk because SENSE_DIV value 0 corresponds
     * to dividing by 1.
     */
    newRegValue = CY_GET_REG32(CapTouch_SENSE_PERIOD_PTR);
    newRegValue &= (uint32)(~CapTouch_SENSE_PERIOD_SENSE_DIV_MASK);
    newRegValue |= snsClk - 1u;
    CY_SET_REG32(CapTouch_SENSE_PERIOD_PTR, newRegValue);
#else
    #if (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK)
        /* Stop sense clock clock   */
        CY_SET_REG32(CapTouch_SNSCLK_CMD_PTR,
                     ((uint32)CapTouch_SnsClk__DIV_ID <<
                     CapTouch_SNSCLK_CMD_DIV_SHIFT)|
                     ((uint32)CapTouch_SNSCLK_CMD_DISABLE_MASK));

        /*
         * Set divider value for sense clock.
         * 1u is subtracted from snsClk because Divider register value 0 corresponds
         * to dividing by 1.
         */
        CY_SET_REG32(CapTouch_SNSCLK_DIV_PTR, ((snsClk - 1u) << 8u));

        /* Check whether previous sense clock start command has finished. */
        while(0u != (CY_GET_REG32(CapTouch_SNSCLK_CMD_PTR) & CapTouch_SNSCLK_CMD_ENABLE_MASK))
        {
            /* Wait until previous sense clock start command has finished. */
        }

        /* Start sense clock aligned to previously started modulator clock. */
        CY_SET_REG32(CapTouch_SNSCLK_CMD_PTR,
                     (uint32)(((uint32)CapTouch_SnsClk__DIV_ID << CapTouch_SNSCLK_CMD_DIV_SHIFT) |
                      ((uint32)CapTouch_ModClk__PA_DIV_ID << CapTouch_SNSCLK_CMD_PA_DIV_SHIFT) |
                      CapTouch_SNSCLK_CMD_ENABLE_MASK));
    #else
        uint32 newRegValue;

        /* Clear bit to disable SnsClk clock. */
        CY_SET_REG32(CapTouch_SNSCLK_CMD_PTR,
                     CY_GET_REG32(CapTouch_SNSCLK_CMD_PTR) &
                     (uint32)(~(uint32)CapTouch_SnsClk__ENABLE_MASK));

        /*
         * Update snsClk clock divider.
         * 1u is subtracted from snsClk because Divider register value has range (0-65535).
         */
        newRegValue = CY_GET_REG32(CapTouch_SNSCLK_DIV_PTR) & (uint32)(~(uint32)CapTouch_SnsClk__DIVIDER_MASK);
        newRegValue |= (snsClk - 1u);
        CY_SET_REG32(CapTouch_SNSCLK_DIV_PTR, newRegValue);

        /* Set bit to enable SnsClk clock. */
        CY_SET_REG32(CapTouch_SNSCLK_CMD_PTR,
                     CY_GET_REG32(CapTouch_SNSCLK_CMD_PTR) |
                    CapTouch_SnsClk__ENABLE_MASK);
    #endif /* (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK) */
#endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */
}


/*******************************************************************************
* Function Name: CapTouch_SsSetClockDividers
****************************************************************************//**
*
* \brief
*  Sets the divider values for sense and modulator clocks and then starts
*  a modulator clock-phase aligned to HFCLK and sense clock-phase aligned to
*  the modulator clock.
*
* \details
*  It is not recommended to call this function directly by the application layer.
*  It is used by initialization, widget APIs or wakeup functions to
*  enable the clocks.
*
* \param
*  snsClk The divider value for the sense clock.
*  modClk The divider value for the modulator clock.
*
*******************************************************************************/
void CapTouch_SsSetClockDividers(uint32 snsClk, uint32 modClk)
{
    /* Configure Mod clock */
    CapTouch_SsSetModClkClockDivider(modClk);

    /* Configure Sns clock */
    CapTouch_SsSetSnsClockDivider(snsClk);
}


#if (CapTouch_ANYMODE_AUTOCAL)
    /*******************************************************************************
    * Function Name: CapTouch_CalibrateWidget
    ****************************************************************************//**
    *
    * \brief
    *  Calibrates the IDACs for all the sensors in the specified widget to the default
    *  target, this function detects the sensing method used by the
    *  widget prior to calibration.
    *
    * \details
    *  This function performs exactly the same tasks as
    *  CapTouch_CalibrateAllWidgets, but only for a specified widget.
    *  This function detects the sensing method used by the widgets and uses
    *  the Enable compensation IDAC parameter. 
    *  \if SECTION_I_REP
    *  For ISX mode, the frequency is also calibrated.
    *  \endif
    *
    *  \if SECTION_C_LOW_LEVEL
    *  This function is available when the CSD and/or CSX Enable IDAC
    *  auto-calibration parameter is enabled.
    *  \endif
    *  \if SECTION_I_REP
    *  This function is available when the ISX Enable auto-calibration parameter
    *  is enabled.
    *  \endif
    *
    * \param widgetId
    *  Specifies the ID number of the widget to calibrate its raw count.
    *  A macro for the widget ID can be found in the
    *  CapTouch Configuration header file defined as
    *  CapTouch_<WidgetName>_WDGT_ID.
    *
    * \return
    *  Returns the status of the specified widget calibration:
    *  - CYRET_SUCCESS - The operation is successfully completed.
    *  - CYRET_BAD_PARAM - The input parameter is invalid.
    *  - CYRET_BAD_DATA - The calibration failed and the Component may not
    *    operate as expected.
    *
    *******************************************************************************/
    cystatus CapTouch_CalibrateWidget(uint32 widgetId)
    {
        cystatus calibrateStatus = CYRET_SUCCESS;

        do
        {
            if (CapTouch_TOTAL_WIDGETS <= widgetId)
            {
                calibrateStatus = CYRET_BAD_PARAM;
            }

            /*
             *  Check if widget id is valid, specified widget did not
             *  detect any faults conditions if BIST is enabled.
             */
            #if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)
                if (0u != CapTouch_GET_WIDGET_EN_STATUS(widgetId))
                {
                    calibrateStatus = CYRET_SUCCESS;
                }
                else
                {
                    calibrateStatus = CYRET_INVALID_STATE;
                }
            #endif  /* (CapTouch_ENABLE == CapTouch_SELF_TEST_EN) */

            if (CYRET_SUCCESS != calibrateStatus)
            {
                /* Exit from the loop because of a fail */
                break;
            }

            /* If both CSD and CSX are enabled, calibrate widget using sensing method */
            #if (CapTouch_ENABLE == CapTouch_CSD_CSX_EN)
                /* Check widget sensing method and call appropriate APIs */
                #if (CapTouch_ENABLE == CapTouch_CSX_IDAC_AUTOCAL_EN)
                    if (CapTouch_SENSE_METHOD_CSX_E ==
                        CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[widgetId]))
                    {
                        /* Calibrate CSX widget */
                       CapTouch_CSXCalibrateWidget(widgetId, CapTouch_CSX_RAWCOUNT_CAL_LEVEL);
                    }
                #endif  /* (CapTouch_ENABLE == CapTouch_CSX_IDAC_AUTOCAL_EN) */

                #if (CapTouch_ENABLE == CapTouch_CSD_IDAC_AUTOCAL_EN)
                    if (CapTouch_SENSE_METHOD_CSD_E ==
                        CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[widgetId]))
                    {
                        /* Calibrate CSD widget */
                        calibrateStatus = CapTouch_CSDCalibrateWidget(widgetId, CapTouch_CSD_RAWCOUNT_CAL_LEVEL);
                    }
                #endif  /* (CapTouch_ENABLE == CapTouch_CSD_IDAC_AUTOCAL_EN) */

            /* If only CSD is enabled, calibrate CSD sensor */
            #elif (CapTouch_ENABLE == CapTouch_CSD_EN)
                calibrateStatus = CapTouch_CSDCalibrateWidget(widgetId, CapTouch_CSD_RAWCOUNT_CAL_LEVEL);

            /* If only CSX is enabled, call CSX scan */
            #elif (CapTouch_ENABLE == CapTouch_CSX_EN)
                CapTouch_CSXCalibrateWidget(widgetId, CapTouch_CSX_RAWCOUNT_CAL_LEVEL);

            /* If only ISX is enabled, call ISX scan */
            #elif (CapTouch_ENABLE == CapTouch_ISX_EN)
                CapTouch_ISXCalibrateWidget(widgetId, CapTouch_ISX_RAWCOUNT_CAL_LEVEL);

            #else
                calibrateStatus = CYRET_UNKNOWN;
            #endif

             /* Update CRC */
            #if (CapTouch_ENABLE ==CapTouch_TST_WDGT_CRC_EN)
                CapTouch_DsUpdateWidgetCrc(widgetId);
            #endif /* (CapTouch_ENABLE ==CapTouch_TST_WDGT_CRC_EN) */

        } while (0u);

        return calibrateStatus;
    }


    /*******************************************************************************
    * Function Name: CapTouch_CalibrateAllWidgets
    ****************************************************************************//**
    *
    * \brief
    *  Calibrates the IDACs for all the widgets in the Component to the default
    *  target, this function detects the sensing method used by the widgets
    *  prior to calibration.
    *
    * \details
    *  Calibrates the IDACs for all the widgets in the Component to the default
    *  target value. This function detects the sensing method used by the widgets
    *  and regards the Enable compensation IDAC parameter.
    *  \if SECTION_I_REP
    *  For ISX mode, the frequency is also calibrated.
    *  \endif
    *
    *  \cond SECTION_C_LOW_LEVEL
    *  This function is available when the CSD and/or CSX Enable IDAC
    *  auto-calibration parameter is enabled.
    *  \endcond
    *  \if SECTION_I_REP
    *  This function is available when the ISX Enable Auto-calibration parameter
    *  is enabled.
    *  \endif
    *
    * \return
    *  Returns the status of the calibration process:
    *  - CYRET_SUCCESS - The operation is successfully completed.
    *  - CYRET_BAD_DATA - The calibration failed and the Component may not
    *    operate as expected.
    *
    *******************************************************************************/
    cystatus CapTouch_CalibrateAllWidgets(void)
    {
        cystatus calibrateStatus = CYRET_SUCCESS;
        uint32 wdgtIndex;

        for (wdgtIndex = 0u; wdgtIndex < CapTouch_TOTAL_WIDGETS; wdgtIndex++)
        {
            calibrateStatus |= CapTouch_CalibrateWidget(wdgtIndex);
        }

        return calibrateStatus;
    }
#endif /* (CapTouch_ANYMODE_AUTOCAL) */


#if (CapTouch_CSD_SS_DIS != CapTouch_CSD_AUTOTUNE)
    /*******************************************************************************
    * Function Name: CapTouch_SsAutoTune
    ****************************************************************************//**
    *
    * \brief
    *  This API performs the parameters auto-tuning for the optimal CapTouch operation.
    *
    * \details
    *  This API performs the following:
    *  - Calibrates Modulator and Compensation IDACs.
    *  - Tunes the Sense Clock optimal value to get a Sense Clock period greater than
    *    2*5*R*Cp.
    *  - Calculates the resolution for the optimal finger capacitance.
    *
    * \return
    *  Returns the status of the operation:
    *  - Zero     - All the widgets are auto-tuned successfully.
    *  - Non-zero - Auto-tuning failed for any widget.
    *
    *******************************************************************************/
    cystatus CapTouch_SsAutoTune(void)
    {
        cystatus autoTuneStatus = CYRET_SUCCESS;
        uint32 wdgtIndex;

        uint32 cp = 0uL;
        #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
            uint32 cpRow = 0uL;
        #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */

        uint32 cpWidget[CapTouch_TOTAL_WIDGETS];
        CapTouch_RAM_WD_BASE_STRUCT *ptrWdgt;
        AUTO_TUNE_CONFIG_TYPE autoTuneConfig;

        /* Configure common config variables */
        autoTuneConfig.snsClkConstantR = CapTouch_CSD_SNSCLK_R_CONST;
        autoTuneConfig.vRef = CapTouch_CSD_VREF_MV;
        autoTuneConfig.iDacGain = CapTouch_CSD_IDAC_GAIN_VALUE_NA;

        /* Calculate snsClk Input Clock in KHz */
        #if (CapTouch_ENABLE == CapTouch_CSDV2)
            /* Dividers are chained (Fourth-generation HW block). Flip flop is not available */
            autoTuneConfig.snsClkInputClock = (CYDEV_BCLK__HFCLK__KHZ / CapTouch_dsRam.modCsdClk);
        #elif ((CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK) && (CapTouch_DISABLE == CY_PSOC4_4000))
            /* Dividers are not chained */
            autoTuneConfig.snsClkInputClock = CYDEV_BCLK__HFCLK__KHZ >> CapTouch_FLIP_FLOP_DIV;
        #elif (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK)
            /* Dividers are not chained (PSoC 4000) */
            autoTuneConfig.snsClkInputClock = CYDEV_BCLK__HFCLK__KHZ >> CapTouch_FLIP_FLOP_DIV;
        #else
            /* Dividers are chained (PSoC 4100, PSoC 4200) */
            autoTuneConfig.snsClkInputClock = (CYDEV_BCLK__HFCLK__KHZ / CapTouch_dsRam.modCsdClk) >> CapTouch_FLIP_FLOP_DIV;
        #endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

        /* If both CSD and CSX are enabled, calibrate widget using sensing method */
        #if (CapTouch_ENABLE == CapTouch_CSD_CSX_EN)
            /* Initialize CSD mode */
            CapTouch_SsCSDInitialize();
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_CSX_EN) */

        /*
        * Autotune phase #1:
        * - performing the first calibration at fixed settings
        * - getting sensor Cp
        * - getting sense clock frequency based on Cp
        */

        /* Tune sense clock for all the widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapTouch_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapTouch_SENSE_METHOD_CSD_E ==
                CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)
                          CapTouch_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                /* Set calibration resolution to 12 bits */
                ptrWdgt->resolution = CapTouch_CALIBRATION_RESOLUTION;

                /* Set clock source direct and sense clock frequency to 1.5 MHz */
                ptrWdgt->snsClkSource = (uint8)CapTouch_CLK_SOURCE_DIRECT;
                ptrWdgt->snsClk = (uint16)((uint32)autoTuneConfig.snsClkInputClock / CapTouch_CALIBRATION_FREQ_KHZ);
                #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapTouch_WD_TOUCHPAD_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapTouch_WD_MATRIX_BUTTON_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType))
                    {
                        ptrWdgt->rowSnsClkSource = (uint8)CapTouch_CLK_SOURCE_DIRECT;
                        ptrWdgt->rowSnsClk = (uint16)((uint32)autoTuneConfig.snsClkInputClock / CapTouch_CALIBRATION_FREQ_KHZ);
                    }
                #endif

                /* Calibrate CSD widget to the default calibration target */
                (void)CapTouch_CSDCalibrateWidget(wdgtIndex, CapTouch_CSD_AUTOTUNE_CAL_LEVEL);

                #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapTouch_WD_TOUCHPAD_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapTouch_WD_MATRIX_BUTTON_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType))
                    {
                        /* Get pointer to Sense Clock Divider for columns */
                        autoTuneConfig.ptrSenseClk = &ptrWdgt->rowSnsClk;

                        /* Get IDAC */
                        autoTuneConfig.iDac = CapTouch_calibratedIdacRow;

                        /* Calculate achived calibration level */
                        autoTuneConfig.calTarget = (uint16)(((uint32)CapTouch_calibratedRawcountRow * CapTouch_CSD_AUTOTUNE_CAL_UNITS) /
                                ((uint32)(0x01uL << CapTouch_CALIBRATION_RESOLUTION) - 1u));

                        /* Find correct sense clock value */
                        cpRow = SmartSense_TunePrescalers(&autoTuneConfig);

                        if ((CapTouch_CP_MAX + CapTouch_CP_ERROR) <= cpRow)
                        {
                            autoTuneStatus = CYRET_BAD_DATA;
                        }

                        /*
                        * Multiply the sense Clock Divider by 2 while the desired Sense Clock Frequency is greater
                        * than maximum supported Sense Clock Frequency.
                        */
                        while((((uint32)autoTuneConfig.snsClkInputClock) > ((uint32)ptrWdgt->snsClk * CapTouch_CSD_SNS_FREQ_KHZ_MAX)) ||
                                (CapTouch_MIN_SNS_CLK_DIVIDER > ptrWdgt->snsClk))
                        {
                            ptrWdgt->snsClk <<= 1u;
                        }

                        #if (CapTouch_ENABLE != CapTouch_CSDV2)
                            #if (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK)
                                /* Make sure that ModClk >= 2 * rowSnsClk */
                                if (CapTouch_dsRam.modCsdClk >= ((uint8)(ptrWdgt->rowSnsClk << CapTouch_FLIP_FLOP_DIV)))
                                {
                                     ptrWdgt->rowSnsClk = CapTouch_dsRam.modCsdClk;
                                }
                            #else
                                /* Sense clock never equals to Modulator clock for chained dividers because of Flip Flop */
                            #endif
                        #endif /* (CapTouch_ENABLE != CapTouch_CSDV2) */
                    }
                #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */

                /* Get pointer to Sense Clock Divider for columns */
                autoTuneConfig.ptrSenseClk = &ptrWdgt->snsClk;

                /* Get IDAC */
                autoTuneConfig.iDac = CapTouch_calibratedIdac;

                /* Calculate achived calibration level */
                autoTuneConfig.calTarget = (uint16)(((uint32)CapTouch_calibratedRawcount * CapTouch_CSD_AUTOTUNE_CAL_UNITS) /
                        ((uint32)(0x01uL << CapTouch_CALIBRATION_RESOLUTION) - 1u));

                /* Find correct sense clock value */
                cp = SmartSense_TunePrescalers(&autoTuneConfig);

                if ((CapTouch_CP_MAX + CapTouch_CP_ERROR) <= cp)
                {
                    autoTuneStatus = CYRET_BAD_DATA;
                }

                /*
                * Multiply the sense Clock Divider by 2 while the desired Sense Clock Frequency is greater
                * than MAX supported Sense Clock Frequency.
                */
                while((((uint32)autoTuneConfig.snsClkInputClock) > ((uint32)ptrWdgt->snsClk * CapTouch_CSD_SNS_FREQ_KHZ_MAX)) ||
                        (CapTouch_MIN_SNS_CLK_DIVIDER > ptrWdgt->snsClk))
                {
                    ptrWdgt->snsClk <<= 1u;
                }

                #if (CapTouch_ENABLE != CapTouch_CSDV2)
                    #if (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK)
                        /* Make sure that ModClk >= 2 * snsClk */
                        if (CapTouch_dsRam.modCsdClk >= ((uint8)(ptrWdgt->snsClk << CapTouch_FLIP_FLOP_DIV)))
                        {
                             ptrWdgt->snsClk = CapTouch_dsRam.modCsdClk;
                        }
                    #else
                        /* Sense clock never equals to Modulator clock for chained dividers because of Flip Flop */
                    #endif
                #endif /* (CapTouch_ENABLE != CapTouch_CSDV2) */

                cpWidget[wdgtIndex] = cp;

                #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapTouch_WD_TOUCHPAD_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapTouch_WD_MATRIX_BUTTON_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType))
                    {
                        if (cpRow > cp)
                        {
                            cpWidget[wdgtIndex] = cpRow;
                        }
                    }
                #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */
            }
            else
            {
                #if (CapTouch_ENABLE == CapTouch_CSX_EN)
                    /* Non-CSD widget */
                    cpWidget[wdgtIndex] = 1u;
                #endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) */
            }
        }

        /*
        * Autotune phase #2:
        * - repeating calibration with new sense clock frequency
        * - getting resolution
        */

        /* Tune resolution for all the widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapTouch_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapTouch_SENSE_METHOD_CSD_E ==
                CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)
                          CapTouch_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                /* Calibrate CSD widget to the default calibration target */
                autoTuneStatus |= CapTouch_CSDCalibrateWidget(wdgtIndex, CapTouch_CSD_AUTOTUNE_CAL_LEVEL);

                /* Get pointer to Sense Clock Divider (column or row) */
                autoTuneConfig.ptrSenseClk = &ptrWdgt->snsClk;

                /* Set parasitic capacitance for columns */
                autoTuneConfig.sensorCap = cpWidget[wdgtIndex];

                /* Get IDAC */
                autoTuneConfig.iDac = CapTouch_calibratedIdac;

                #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapTouch_WD_TOUCHPAD_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapTouch_WD_MATRIX_BUTTON_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType))

                    {
                        /* Set the minimum sense clock frequency to calculate the resolution */
                        if (ptrWdgt->snsClk < ptrWdgt->rowSnsClk)
                        {
                            /* Rewrite pointer to Sense Clock Divider for rows */
                            autoTuneConfig.ptrSenseClk = &ptrWdgt->rowSnsClk;

                            /* Set parasitic capacitance for rows */
                            autoTuneConfig.sensorCap = cpWidget[wdgtIndex];

                            /* Get IDAC */
                            autoTuneConfig.iDac = CapTouch_calibratedIdacRow;
                        }
                    }
                #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */

                /* Get finger capacitance */
                autoTuneConfig.fingerCap = ptrWdgt->fingerCap;

                /* Init pointer to sigPFC */
                autoTuneConfig.sigPFC = &ptrWdgt->sigPFC;

                /* Find resolution */
                ptrWdgt->resolution = SmartSense_TuneSensitivity(&autoTuneConfig);
            }
        }

        /*
        * Autotune phase #3:
        * - selecting a widget clock source if AUTO
        * - repeating calibration with found clock frequency, resolution and clock source
        * - updating sensitivity
        */

        /* Tune sensitivity for all the widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapTouch_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapTouch_SENSE_METHOD_CSD_E ==
                CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)
                          CapTouch_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                CapTouch_SsSetWidgetSenseClkSrc(wdgtIndex, ptrWdgt);

                /* Calibrate CSD widget to the default calibration target */
                autoTuneStatus |= CapTouch_CSDCalibrateWidget(wdgtIndex, CapTouch_CSD_AUTOTUNE_CAL_LEVEL);

                #if (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN)
                    CapTouch_DsUpdateWidgetCrc(wdgtIndex);
                #endif /* (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN) */
            }
        }

        return autoTuneStatus;
    }
#endif /* (CapTouch_CSD_SS_DIS != CapTouch_CSD_AUTOTUNE)) */


#if (CapTouch_DISABLE == CapTouch_CSDV2)
    #if ( ((CapTouch_ENABLE == CapTouch_CSX_EN) \
        || (CapTouch_ENABLE == CapTouch_ISX_EN)) ||\
         (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG))
        /*******************************************************************************
        * Function Name: CapTouch_SsTrimIdacsSinking
        ****************************************************************************//**
        *
        * \brief
        *  This function loads trim values from SFLASH rows to calibrate
        *  IDAC1 and IDAC2 for Sinking CSD Mode
        *
        * \details
        *  Function reads trim value from SFLASH and loads it into IDAC trim register.
        *  If Compensation IDAC is disabled the function loads trim values for IDAC1 only.
        *
        *******************************************************************************/
        static void CapTouch_SsTrimIdacsSinking(void)
        {
            uint32 trimValue;

            /* Mod iDAC Sinking Mode */
            trimValue = CY_GET_REG32(CapTouch_TRIM2_PTR) & ~CapTouch_IDAC_TRIM2_MOD_SNK_MASK;
            trimValue |= ((uint32)CY_GET_REG8(CapTouch_SFLASH_TRIM2_PTR) &
                                                      CapTouch_SFLASH_TRIM_IDAC_MOD_MASK) ;

            #if (CapTouch_ENABLE == CapTouch_CSD_IDAC_COMP_EN)
                /* Comp iDAC Sinking Mode */
                trimValue &= ~CapTouch_IDAC_TRIM2_COMP_SNK_MASK;
                trimValue |= ((uint32)CY_GET_REG8(CapTouch_SFLASH_TRIM2_PTR) &
                                                          CapTouch_SFLASH_TRIM_IDAC_COMP_MASK);
            #endif /* (CapTouch_ENABLE == CapTouch_CSD_IDAC_COMP_EN) */

            /* Update IDAC trim bits for gain control in current sink mode */
            CY_SET_REG32(CapTouch_TRIM2_PTR, trimValue);
        }
    #endif /* ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
               (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG)) */


    #if ( ((CapTouch_ENABLE == CapTouch_CSX_EN) \
        || (CapTouch_ENABLE == CapTouch_ISX_EN)) || \
         (CapTouch_IDAC_SOURCING == CapTouch_CSD_IDAC_CONFIG))
        /*******************************************************************************
        * Function Name: CapTouch_SsTrimIdacsSourcing
        ****************************************************************************//**
        *
        * \brief
        *  This function loads trim values from SFLASH rows to calibrate
        *  IDAC1 and IDAC2 for Sourcing CSD Mode
        *
        * \details
        *  Function reads trim value from SFLASH and loads it into IDAC trim register.
        *  If Compensation IDAC is disabled the function loads trim values for IDAC1 only.
        *
        *******************************************************************************/
        static void CapTouch_SsTrimIdacsSourcing(void)
        {
            uint32 trimValue;

            /* Mod iDAC Sourcing Mode */
            trimValue = CY_GET_REG32(CapTouch_TRIM1_PTR) & ~CapTouch_IDAC_TRIM1_MOD_SRC_MASK;
            trimValue |= ((uint32)CY_GET_REG8(CapTouch_SFLASH_TRIM1_PTR) &
                                                      CapTouch_SFLASH_TRIM_IDAC_MOD_MASK);

            #if (CapTouch_ENABLE == CapTouch_CSD_IDAC_COMP_EN)
                 /* Comp iDAC Sourcing Mode */
                trimValue &= ~CapTouch_IDAC_TRIM1_COMP_SRC_MASK;
                trimValue |= ((uint32)CY_GET_REG8(CapTouch_SFLASH_TRIM1_PTR) &
                                                          CapTouch_SFLASH_TRIM_IDAC_COMP_MASK);
            #endif /* (CapTouch_ENABLE == CapTouch_CSD_IDAC_COMP_EN) */

            /* Update IDAC trim bits for gain control in current source mode */
            CY_SET_REG32(CapTouch_TRIM1_PTR, trimValue);
        }
    #endif /* ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
               (CapTouch_IDAC_SOURCING == CapTouch_CSD_IDAC_CONFIG)) */


    /*******************************************************************************
    * Function Name: CapTouch_SsTrimIdacs
    ****************************************************************************//**
    *
    * \brief
    *  This function loads trim values from SFLASH rows to calibrate
    *  IDAC1 and IDAC2 for CSD Mode
    *
    * \details
    *  If CSX mode is enabled the function loads trim values for both sink and source
    *  mode. If CSX mode is disabled the function loads trim values for sink or
    *  source mode based on sink/source mode configuration.
    *  If Compensation IDAC is disabled the function loads trim values for IDAC1 only.
    *
    *******************************************************************************/
    static void CapTouch_SsTrimIdacs(void)
    {
        #if (CapTouch_ENABLE == CapTouch_CSX_EN) || (CapTouch_ENABLE == CapTouch_ISX_EN)
            CapTouch_SsTrimIdacsSinking();
            CapTouch_SsTrimIdacsSourcing();
        #elif (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG)
            CapTouch_SsTrimIdacsSinking();
        #elif (CapTouch_IDAC_SOURCING == CapTouch_CSD_IDAC_CONFIG)
            CapTouch_SsTrimIdacsSourcing();
        #else
            #error "Not supported Mode, Component cannot work in this mode"
        #endif /* (CapTouch_ENABLE == CapTouch_CSX_EN) || (CapTouch_ENABLE == CapTouch_ISX_EN) */
    }
#endif  /* (CapTouch_DISABLE == CapTouch_CSDV2) */


#if (CapTouch_ENABLE == CapTouch_MULTI_FREQ_SCAN_EN)
    /*******************************************************************************
    * Function Name: CapTouch_SsChangeImoFreq
    ****************************************************************************//**
    *
    * \brief
    *  This function changes the IMO frequency.
    *
    * \details
    *  The IMO frequency can have three offsets: 0%, -5% and +5%. The frequency
    *  trims are contained in the CapTouch_immunity[value] array for each
    *  frequency channel.
    *
    * \param value The frequency channel ID.
    *
    *******************************************************************************/
    void CapTouch_SsChangeImoFreq(uint32 value)
    {
        CY_SET_REG32(CY_SYS_CLK_IMO_TRIM1_PTR, CapTouch_immunity[value]);
    }

    /*******************************************************************************
    * Function Name: CapTouch_SsChangeClkFreq
    ****************************************************************************//**
    *
    * \brief
    *  This function changes the sensor clock frequency by configuring
    *  the corresponding divider.
    *
    * \details
    *  This function changes the sensor clock frequency by configuring
    *  the corresponding divider.
    *
    * \param chId
    *  The frequency channel ID.
    *
    *******************************************************************************/
    void CapTouch_SsChangeClkFreq(uint32 chId)
    {
        uint32 snsClkDivider;
        uint32 freqOffset1 = 0u;
        uint32 freqOffset2 = 0u;

        #if (0u != CapTouch_TOTAL_CSD_WIDGETS)
            #if (CapTouch_ENABLE == CapTouch_CSDV2)
                uint32 conversionsNum;
            #endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

            #if((CapTouch_CLK_SOURCE_PRS8  == CapTouch_CSD_SNS_CLK_SOURCE) ||\
                (CapTouch_CLK_SOURCE_PRS12 == CapTouch_CSD_SNS_CLK_SOURCE) ||\
                (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE))
                uint32 snsClkSrc;
            #endif
        #endif

        #if ((0u != CapTouch_TOTAL_CSD_WIDGETS) || \
             ((CapTouch_DISABLE == CapTouch_CSX_COMMON_TX_CLK_EN) && (0u != CapTouch_TOTAL_CSX_WIDGETS)))
            CapTouch_FLASH_WD_STRUCT const *ptrFlashWdgt = &CapTouch_dsFlash.wdgtArray[CapTouch_widgetIndex];
            CapTouch_RAM_WD_BASE_STRUCT const *ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)ptrFlashWdgt->ptr2WdgtRam;
        #endif

        switch(CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[CapTouch_widgetIndex]))
        {
        #if (0u != CapTouch_TOTAL_CSD_WIDGETS)
            case CapTouch_SENSE_METHOD_CSD_E:
                /* Get sensor clock divider value */
                #if (CapTouch_ENABLE == CapTouch_CSD_COMMON_SNS_CLK_EN)
                    snsClkDivider = CapTouch_dsRam.snsCsdClk;
                #else /* (CapTouch_ENABLE == CapTouch_CSD_COMMON_SNS_CLK_EN) */
                    #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
                        /* Get SnsClck divider for rows or columns */
                        if (CapTouch_dsFlash.wdgtArray[CapTouch_widgetIndex].numCols <= CapTouch_sensorIndex)
                        {
                            snsClkDivider = ptrWdgt->rowSnsClk;
                        }
                        else
                        {
                            snsClkDivider = ptrWdgt->snsClk;
                        }
                    #else
                        snsClkDivider = ptrWdgt->snsClk;
                    #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */
                #endif /* (CapTouch_ENABLE == CapTouch_CSD_COMMON_SNS_CLK_EN) */

                freqOffset1 = CapTouch_CSD_MFS_DIVIDER_OFFSET_F1;
                freqOffset2 = CapTouch_CSD_MFS_DIVIDER_OFFSET_F2;

                #if((CapTouch_CLK_SOURCE_PRS8  == CapTouch_CSD_SNS_CLK_SOURCE) ||\
                    (CapTouch_CLK_SOURCE_PRS12 == CapTouch_CSD_SNS_CLK_SOURCE) ||\
                    (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE))
                    /* Get sense clk source */
                    #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
                        /* Get SnsClc Source for rows or columns */
                        if (CapTouch_dsFlash.wdgtArray[CapTouch_widgetIndex].numCols <= CapTouch_sensorIndex)
                        {
                            snsClkSrc = (uint32)ptrWdgt->rowSnsClkSource;
                        }
                        else
                        {
                            snsClkSrc = (uint32)ptrWdgt->snsClkSource;
                        }
                    #else
                        snsClkSrc = (uint32)ptrWdgt->snsClkSource;
                    #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */

                    switch (snsClkSrc)
                    {
                    case CapTouch_CLK_SOURCE_PRS8:
                    case CapTouch_CLK_SOURCE_PRS12:
                        /* Multiply by 2 for PRS8/PRS12 mode */
                        freqOffset1 <<= 1u;
                        freqOffset2 <<= 1u;
                        break;

                    default:
                        break;
                    }
                #endif

                /* Change the divider based on the chId */
                switch (chId)
                {
                    case CapTouch_FREQ_CHANNEL_1:
                    {
                        snsClkDivider += freqOffset1;
                        break;
                    }
                    case CapTouch_FREQ_CHANNEL_2:
                    {
                        snsClkDivider += freqOffset2;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                #if (CapTouch_ENABLE == CapTouch_CSDV2)
                    /* Set Number Of Conversions based on scanning resolution */
                    conversionsNum = CapTouch_SsCSDGetNumberOfConversions(snsClkDivider, (uint32)ptrWdgt->resolution, (uint32)ptrWdgt->snsClkSource);
                    CY_SET_REG32(CapTouch_SEQ_NORM_CNT_PTR, (conversionsNum & CapTouch_SEQ_NORM_CNT_CONV_CNT_MASK));
                #endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

                #if((CapTouch_CLK_SOURCE_PRS8  == CapTouch_CSD_SNS_CLK_SOURCE) ||\
                    (CapTouch_CLK_SOURCE_PRS12 == CapTouch_CSD_SNS_CLK_SOURCE) ||\
                    (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE))
                    switch (snsClkSrc)
                    {
                    case CapTouch_CLK_SOURCE_PRS8:
                    case CapTouch_CLK_SOURCE_PRS12:
                        /* Divide by 2 for PRS8/PRS12 mode */
                        snsClkDivider >>= 1;
                        if (snsClkDivider == 0u)
                        {
                            snsClkDivider = 1u;
                        }
                        break;

                    default:
                        break;
                    }
                #endif

                /* Configure the new divider */
                CapTouch_SsSetSnsClockDivider(snsClkDivider);

                break;
        #endif /* #if (0u != CapTouch_TOTAL_CSD_WIDGETS) */

        #if (0u != CapTouch_TOTAL_CSX_WIDGETS)
            case CapTouch_SENSE_METHOD_CSX_E:
                #if (CapTouch_ENABLE == CapTouch_CSX_COMMON_TX_CLK_EN)
                    snsClkDivider = CapTouch_dsRam.snsCsxClk;
                #else /* (CapTouch_ENABLE == CapTouch_CSX_COMMON_TX_CLK_EN) */
                    snsClkDivider = ptrWdgt->snsClk;
                #endif /* (CapTouch_ENABLE == CapTouch_CSX_COMMON_TX_CLK_EN) */
                freqOffset1 = CapTouch_CSX_MFS_DIVIDER_OFFSET_F1;
                freqOffset2 = CapTouch_CSX_MFS_DIVIDER_OFFSET_F2;

                /* Change the divider based on the chId */
                switch (chId)
                {
                    case CapTouch_FREQ_CHANNEL_1:
                    {
                        snsClkDivider += freqOffset1;
                        break;
                    }
                    case CapTouch_FREQ_CHANNEL_2:
                    {
                        snsClkDivider += freqOffset2;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                /* Configure the new divider */
                CapTouch_SsSetSnsClockDivider(snsClkDivider);

                break;
        #endif /* #if (0u != CapTouch_TOTAL_CSX_WIDGETS) */

        default:
            CYASSERT(0);
            break;
        }
    }
#endif  /* (CapTouch_ENABLE == CapTouch_MULTI_FREQ_SCAN_EN) */


#if((CapTouch_ENABLE == CapTouch_CSD_EN) || \
    ((CapTouch_ENABLE == CapTouch_CSX_EN) && \
     (CapTouch_ENABLE == CapTouch_CSDV2)))
/*******************************************************************************
* Function Name: CapTouch_SsInitializeSourceSenseClk
****************************************************************************//**
*
* \brief
*  Sets a source for Sense Clk for all CSD widgets.
*
* \details
*  Updates snsClkSource and rowSnsClkSource with a source for the sense Clk.
*  for all CSD widgets.
*
*******************************************************************************/
void CapTouch_SsInitializeSourceSenseClk(void)
{
    uint32 wdgtIndex;
    CapTouch_RAM_WD_BASE_STRUCT *ptrWdgt;

    for (wdgtIndex = 0u; wdgtIndex < CapTouch_TOTAL_WIDGETS; wdgtIndex++)
    {
        ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)CapTouch_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

        switch (CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[wdgtIndex]))
        {
        #if (CapTouch_ENABLE == CapTouch_CSD_EN)
            case CapTouch_SENSE_METHOD_CSD_E:
                CapTouch_SsSetWidgetSenseClkSrc(wdgtIndex, ptrWdgt);
                break;
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */
        #if ((CapTouch_ENABLE == CapTouch_CSX_EN) && (CapTouch_ENABLE == CapTouch_CSDV2))
            case CapTouch_SENSE_METHOD_CSX_E:
                CapTouch_SsSetWidgetTxClkSrc(wdgtIndex, ptrWdgt);
                break;
        #endif /* (((CapTouch_ENABLE == CapTouch_CSX_EN) || (CapTouch_ENABLE == CapTouch_ISX_EN)) && (CapTouch_ENABLE == CapTouch_CSDV2)) */
        default:
            break;
        }
        #if (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN)
            CapTouch_DsUpdateWidgetCrc(wdgtIndex);
        #endif /* (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN) */
    }
}
#endif /* ((CapTouch_ENABLE == CapTouch_CSD_EN) || \
           (((CapTouch_ENABLE == CapTouch_CSX_EN) || \
           (CapTouch_ENABLE == CapTouch_ISX_EN)) && \
           (CapTouch_ENABLE == CapTouch_CSDV2))) */


#if (CapTouch_ENABLE == CapTouch_CSD_EN)
    /*******************************************************************************
    * Function Name: CapTouch_SsSetWidgetSenseClkSrc
    ****************************************************************************//**
    *
    * \brief
    *  Sets a source for the sense clock for a widget.
    *
    * \param wdgtIndex
    *  Specifies the ID of the widget.
    * \param ptrWdgt
    *  The pointer to the RAM_WD_BASE_STRUCT structure.
    *
    * \details
    *  Updates snsClkSource and rowSnsClkSource with a source for the sense Clk for a
    *  widget.
    *
    *******************************************************************************/
    static void CapTouch_SsSetWidgetSenseClkSrc(uint32 wdgtIndex, CapTouch_RAM_WD_BASE_STRUCT * ptrWdgt)
    {
        uint8 lfsrSize;
        uint8 lfsrScale;

        #if(CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE)
            #if(CapTouch_ENABLE == CapTouch_CSDV2)
                uint32 conversionsNum;
            #endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */
            uint32 snsClkDivider;
        #endif /* (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE) */

        #if(CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE)
            snsClkDivider = CapTouch_SsCSDGetColSnsClkDivider(wdgtIndex);

            #if(CapTouch_ENABLE == CapTouch_CSDV2)
                conversionsNum = CapTouch_SsCSDGetNumberOfConversions(snsClkDivider, (uint32)ptrWdgt->resolution,
                                                                                             CapTouch_CLK_SOURCE_DIRECT);
                lfsrSize = CapTouch_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                if (CapTouch_CLK_SOURCE_DIRECT == lfsrSize)
                {
                    /*
                    * Multiplying of the snsClkDivider by 2 is needed to pass the
                    * average PRS frequency through the argument.
                    */
                    lfsrSize = CapTouch_SsCSDCalcPrsSize(snsClkDivider << 1uL, (uint32)ptrWdgt->resolution);
                }
                lfsrScale = CapTouch_SsCalcLfsrScale(snsClkDivider, lfsrSize);
            #else
                /*
                * Multiplying of the snsClkDivider by 2 is needed to pass the
                * average PRS frequency through the argument.
                */
                lfsrSize = CapTouch_SsCSDCalcPrsSize(snsClkDivider << 1uL, (uint32)ptrWdgt->resolution);
                lfsrScale = 0u;
            #endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */
        #else
            lfsrSize = (uint8)CapTouch_DEFAULT_MODULATION_MODE;
            lfsrScale = 0u;
            (void)wdgtIndex; /* This parameter is unused in such configurations */
        #endif /* (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE) */

        ptrWdgt->snsClkSource = lfsrSize | (uint8)(lfsrScale << CapTouch_CLK_SOURCE_LFSR_SCALE_OFFSET);

        #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
            if ((CapTouch_WD_TOUCHPAD_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                (CapTouch_WD_MATRIX_BUTTON_E == (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[wdgtIndex].wdgtType))
            {
                #if(CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE)
                    snsClkDivider = CapTouch_SsCSDGetRowSnsClkDivider(wdgtIndex);

                    #if(CapTouch_ENABLE == CapTouch_CSDV2)
                        lfsrSize = CapTouch_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                        if (CapTouch_CLK_SOURCE_DIRECT == lfsrSize)
                        {
                            /*
                            * Multiplying of the snsClkDivider by 2 is needed to pass the
                            * average PRS frequency through the argument.
                            */
                            lfsrSize = CapTouch_SsCSDCalcPrsSize(snsClkDivider << 1uL, (uint32)ptrWdgt->resolution);
                        }
                        lfsrScale = CapTouch_SsCalcLfsrScale(snsClkDivider, lfsrSize);
                    #else
                        /*
                        * Multiplying of the snsClkDivider by 2 is needed to pass the
                        * average PRS frequency through the argument.
                        */
                        lfsrSize = CapTouch_SsCSDCalcPrsSize(snsClkDivider << 1uL, (uint32)ptrWdgt->resolution);
                        lfsrScale = 0u;
                    #endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */
                #else
                    lfsrSize = (uint8)CapTouch_DEFAULT_MODULATION_MODE;
                    lfsrScale = 0u;
                #endif /* (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE) */
                ptrWdgt->rowSnsClkSource = lfsrSize | (uint8)(lfsrScale << CapTouch_CLK_SOURCE_LFSR_SCALE_OFFSET);
            }
        #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */
    }
#endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */


#if ((CapTouch_ENABLE == CapTouch_CSX_EN) && (CapTouch_ENABLE == CapTouch_CSDV2))
    /*******************************************************************************
    * Function Name: CapTouch_SsSetWidgetTxClkSrc
    ****************************************************************************//**
    *
    * \brief
    *  Sets a source for the Tx clock for a widget.
    *
    * \param wdgtIndex
    *  Specifies the ID of the widget.
    * \param ptrWdgt
    *  The pointer to the RAM_WD_BASE_STRUCT structure.
    *
    * \details
    *  Updates snsClkSource with a source for Tx Clk for a widget.
    *
    *******************************************************************************/
    static void CapTouch_SsSetWidgetTxClkSrc(uint32 wdgtIndex, CapTouch_RAM_WD_BASE_STRUCT * ptrWdgt)
    {
        uint8 lfsrSize;
        uint8 lfsrScale;

        #if ((CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSX_TX_CLK_SOURCE) && \
             (CapTouch_DISABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES))
            uint32 conversionsNum;
            uint32 snsClkDivider;
        #endif

        #if(CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSX_TX_CLK_SOURCE)
            #if (CapTouch_DISABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES)
                conversionsNum = (uint32)ptrWdgt->resolution;
                snsClkDivider = CapTouch_SsCSXGetTxClkDivider(wdgtIndex);
                lfsrSize = CapTouch_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                lfsrScale = CapTouch_SsCalcLfsrScale(snsClkDivider, lfsrSize);
            #else
                lfsrSize = (uint8)CapTouch_CLK_SOURCE_DIRECT;
                lfsrScale = 0u;
                /* Unused function argument */
                (void)wdgtIndex;
            #endif /* (CapTouch_ENABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES) */
        #else
            lfsrSize = (uint8)CapTouch_CSX_TX_CLK_SOURCE;
            lfsrScale = 0u;
            /* Unused function argument */
            (void)wdgtIndex;
        #endif /* (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSX_TX_CLK_SOURCE) */

        ptrWdgt->snsClkSource = lfsrSize | (uint8)(lfsrScale << CapTouch_CLK_SOURCE_LFSR_SCALE_OFFSET);
    }
#endif /* ((CapTouch_ENABLE == CapTouch_CSX_EN) && (CapTouch_ENABLE == CapTouch_CSDV2)) */


#if(((CapTouch_ENABLE == CapTouch_CSX_EN) && \
     (CapTouch_DISABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES) && \
     (CapTouch_ENABLE == CapTouch_CSDV2) && \
     (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSX_TX_CLK_SOURCE)) ||\
    ((CapTouch_ENABLE == CapTouch_CSD_EN) && \
     (CapTouch_ENABLE == CapTouch_CSDV2) && \
     (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE)))
/*******************************************************************************
* Function Name: CapTouch_SsCalcLfsrSize
****************************************************************************//**
*
* \brief
*  This is an internal function that finds a SSC polynomial size in the Auto mode.
*
* \details
*  The SSC polynomial size in the auto mode is found based on the following
*  requirements:
*  - an LFSR value should be selected so that the max clock dither is limited with +/-10%.
*  - at least one full spread spectrum polynomial should pass during the scan time.
*  - the value of the number of conversions should be an integer multiple of the
*    repeat period of the programmed LFSR_SIZE.
*
* \param
*  snsClkDivider The divider value for the sense clock.
*  resolution The widget resolution.
*
* \return lfsrSize The LFSRSIZE value for the SENSE_PERIOD register.
*
*******************************************************************************/
static uint8 CapTouch_SsCalcLfsrSize(uint32 snsClkDivider, uint32 conversionsNum)
{
    uint8 lfsrSize = 0u;

    /* Find LFSR value */
    if((CapTouch_SNSCLK_SSC4_THRESHOLD <= snsClkDivider) &&
       (CapTouch_SNSCLK_SSC4_PERIOD <= conversionsNum) &&
       (0uL == (conversionsNum % CapTouch_SNSCLK_SSC4_PERIOD)))
    {
        lfsrSize = CapTouch_CLK_SOURCE_SSC4;
    }
    else if((CapTouch_SNSCLK_SSC3_THRESHOLD <= snsClkDivider) &&
            (CapTouch_SNSCLK_SSC3_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapTouch_SNSCLK_SSC3_PERIOD)))
    {
        lfsrSize = CapTouch_CLK_SOURCE_SSC3;
    }
    else if((CapTouch_SNSCLK_SSC2_THRESHOLD <= snsClkDivider) &&
            (CapTouch_SNSCLK_SSC2_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapTouch_SNSCLK_SSC2_PERIOD)))
    {
        lfsrSize = CapTouch_CLK_SOURCE_SSC2;
    }
    else if((CapTouch_SNSCLK_SSC1_THRESHOLD <= snsClkDivider) &&
            (CapTouch_SNSCLK_SSC1_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapTouch_SNSCLK_SSC1_PERIOD)))
    {
        lfsrSize = CapTouch_CLK_SOURCE_SSC1;
    }
    else
    {
        lfsrSize = (uint8)CapTouch_CLK_SOURCE_DIRECT;
    }

    return (lfsrSize);
}


/*******************************************************************************
* Function Name: CapTouch_SsCalcLfsrScale
****************************************************************************//**
*
* \brief
*  This is an internal function that calculates the LFSR scale value.
*
* \details
*  The LFSR scale value is used to increase the clock dither if the desired dither
*  is wider than can be achieved with the current Sense Clock Divider and current LFSR
*  period.
*
*  This returns the LFSR scale value needed to keep the clock dither in
*  range +/-10%.
*
* \param
*  snsClkDivider The divider value for the sense clock.
*  lfsrSize The period of the LFSR sequence.
*          For devices with CapTouch_CSDV2_REF9P6UA_EN = TRUE, the
*          mode parameters can take the following values:
*          CapTouch_CLK_SOURCE_DIRECT The spreadspectrum is not used.
*          CapTouch_CLK_SOURCE_SSC1   The length of LFSR sequence is 63 cycles.
*          CapTouch_CLK_SOURCE_SSC2   The length of LFSR sequence is 127 cycles.
*          CapTouch_CLK_SOURCE_SSC3   The length of LFSR sequence is 255 cycles.
*          CapTouch_CLK_SOURCE_SSC4   The length of LFSR sequence is 511 cycles.
*
*          For devices with CapTouch_CSDV2_REF9P6UA_EN = TRUE, the
*          mode parameters can take the following values:
*          CapTouch_CLK_SOURCE_DIRECT The spreadspectrum is not used.
*          CapTouch_CLK_SOURCE_SSC1   The length of LFSR sequence is 3 cycles.
*          CapTouch_CLK_SOURCE_SSC2   The length of LFSR sequence is 7 cycles.
*          CapTouch_CLK_SOURCE_SSC3   The length of LFSR sequence is 15 cycles.
*          CapTouch_CLK_SOURCE_SSC4   The length of LFSR sequence is 31 cycles.
*
* \return
*  Returns the LFSR scale value needed to keep the clock dither in range +/-10%.
*
*******************************************************************************/
static uint8 CapTouch_SsCalcLfsrScale(uint32 snsClkDivider, uint8 lfsrSize)
{
    uint32 lfsrScale;
    uint32 lfsrRange;
    uint32 lfsrDither;

    /* Initialize the lfsrSize variable with the LFSR Range for given Lfsr Size. */
    switch(lfsrSize)
    {
        case CapTouch_CLK_SOURCE_SSC1:
        {
            lfsrRange = CapTouch_SNSCLK_SSC1_RANGE;
            break;
        }
        case CapTouch_CLK_SOURCE_SSC2:
        {
            lfsrRange = CapTouch_SNSCLK_SSC2_RANGE;
            break;
        }
        case CapTouch_CLK_SOURCE_SSC3:
        {
            lfsrRange = CapTouch_SNSCLK_SSC3_RANGE;
            break;
        }
        case CapTouch_CLK_SOURCE_SSC4:
        {
            lfsrRange = CapTouch_SNSCLK_SSC4_RANGE;
            break;
        }
        default:
        {
            lfsrRange = 0u;
            break;
        }
    }

    /* Calculate the LFSR Scale value that is required to keep the Clock dither
     * as close as possible to the +/-10% limit of the used frequency.
     */
    if((lfsrSize != CapTouch_CLK_SOURCE_DIRECT) && (0u != lfsrRange))
    {
        /* Calculate the LFSR Dither in percents. */
        lfsrDither  = ((lfsrRange * 100uL) / snsClkDivider);
        lfsrScale = 0uL;

        while(lfsrDither < CapTouch_LFSR_DITHER_PERCENTAGE)
        {
            lfsrScale++;
            lfsrDither <<=1uL;
        }

        if(lfsrDither > CapTouch_LFSR_DITHER_PERCENTAGE)
        {
            lfsrScale--;
        }
    }
    else
    {
        lfsrScale = 0uL;
    }

    return ((uint8)lfsrScale);
}

#endif /* (((CapTouch_ENABLE == CapTouch_CSX_EN) && \
           (CapTouch_DISABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES) && \
           (CapTouch_ENABLE == CapTouch_CSDV2) && \
           (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSX_TX_CLK_SOURCE)) ||\
          ((CapTouch_ENABLE == CapTouch_CSD_EN) && \
           (CapTouch_ENABLE == CapTouch_CSDV2) && \
           (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE))) */


#if (CapTouch_ENABLE == CapTouch_CSD_EN)
    /*******************************************************************************
    * Function Name: CapTouch_SsClearCSDSensors
    ****************************************************************************//**
    *
    * \brief
    *  Resets all the CSD sensors to the non-sampling state by sequentially
    *  disconnecting all the sensors from the Analog MUX bus and putting them to
    *  an inactive state.
    *
    * \details
    *  The function goes through all the widgets and updates appropriate bits in
    *  the IO HSIOM, PC and DR registers depending on the Inactive sensor connection
    *  parameter. DR register bits are set to zero when the Inactive sensor
    *  connection is Ground or Hi-Z.
    *
    *******************************************************************************/
    void CapTouch_SsClearCSDSensors(void)
    {
        uint32 wdgtIndex;
        uint32 snsIndex;

        #if (CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN)
            uint32 pinIndex;
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN) */

        #if (CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN)
            /* Declare ptr to sensor IO structure */
            CapTouch_FLASH_IO_STRUCT const *curDedicatedSnsIOPtr;
            /* Pointer to Flash structure holding info of sensor to be scanned */
            CapTouch_FLASH_SNS_STRUCT const *curFlashSnsPtr;
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN) */
        CapTouch_FLASH_IO_STRUCT const *curSnsIOPtr;

        for (wdgtIndex = 0u; wdgtIndex < CapTouch_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapTouch_SENSE_METHOD_CSD_E ==
                CapTouch_GET_SENSE_METHOD(&CapTouch_dsFlash.wdgtArray[wdgtIndex]))
            {
                curSnsIOPtr = (CapTouch_FLASH_IO_STRUCT const *)
                                                CapTouch_dsFlash.wdgtArray[wdgtIndex].ptr2SnsFlash;

                /* Go through all the sensors in widget */
                for (snsIndex = 0u; snsIndex < (uint8)CapTouch_dsFlash.wdgtArray[wdgtIndex].totalNumSns; snsIndex++)
                {
                    #if (CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN)
                        /* Check ganged sns flag */
                        if (CapTouch_GANGED_SNS_MASK == (CapTouch_dsFlash.wdgtArray[wdgtIndex].staticConfig &
                                                                 CapTouch_GANGED_SNS_MASK))
                        {
                            /* Get sns pointer */
                            curFlashSnsPtr = (CapTouch_FLASH_SNS_STRUCT const *)
                                                               CapTouch_dsFlash.wdgtArray[wdgtIndex].ptr2SnsFlash +
                                                               snsIndex;

                            for(pinIndex = 0u; pinIndex < curFlashSnsPtr->numPins; pinIndex++)
                            {
                                /* Get IO pointer for dedicated pin */
                                curDedicatedSnsIOPtr = &CapTouch_ioList[curFlashSnsPtr->firstPinId + pinIndex];

                                /* Disconnect dedicated pin */
                                CapTouch_CSDDisconnectSns(curDedicatedSnsIOPtr);
                            }
                        }
                        else
                        {
                            /* Disable sensor */
                            CapTouch_CSDDisconnectSns(&curSnsIOPtr[snsIndex]);
                        }
                    #else
                        /* Disable sensor */
                        CapTouch_CSDDisconnectSns(&curSnsIOPtr[snsIndex]);
                    #endif /* (CapTouch_ENABLE == CapTouch_CSD_GANGED_SNS_EN) */
                }
            }
        }
    }

    /*******************************************************************************
    * Function Name: CapTouch_SsCSDGetColSnsClkDivider
    ****************************************************************************//**
    *
    * \brief
    *  This function gets the Sense Clock Divider value for one-dimension widgets
    *  and Column Sense Clock divider value for two-dimension widgets.
    *
    * \details
    *  This function gets the Sense Clock Divider value based on the Component
    *  configuration. The function is applicable for one-dimension widgets and for
    *  two-dimension widgets.
    *
    * \param widgetId
    *  Specifies the ID of the widget.
    *
    * \return
    *  Returns the Sense Clock Divider value for one-dimension widgets
    *  and the Column Sense Clock divider value for two-dimension widgets.
    *
    *******************************************************************************/
    uint32 CapTouch_SsCSDGetColSnsClkDivider(uint32 widgetId)
    {
        uint32 retVal;

        /* Get sense divider based on configuration */
        #if (CapTouch_ENABLE != CapTouch_CSD_COMMON_SNS_CLK_EN)
            CapTouch_RAM_WD_BASE_STRUCT *ptrWdgt;

            ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)
            CapTouch_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

            retVal = (uint32)(ptrWdgt->snsClk);
        #else
            retVal = (uint32)CapTouch_dsRam.snsCsdClk;

            (void)widgetId; /* This parameter is unused in such configurations */
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_COMMON_SNS_CLK_EN) */

        return (retVal);
    }


    /*******************************************************************************
    * Function Name: CapTouch_SsCSDSetColSnsClkDivider
    ****************************************************************************//**
    *
    * \brief
    *  This function sets the Sense Clock Divider value for one-dimension widgets
    *  and Column Sense Clock divider value for two-dimension widgets.
    *
    * \details
    *  This function sets the Sense Clock Divider value based on the Component
    *  configuration. The function is applicable for one-dimension widgets and for
    *  two-dimension widgets.
    *
    * \param
    *  widgetId Specifies the ID of the widget.
    *
    * \return The Sense Clock Divider value for one-dimension widgets
    *        and the Column Sense Clock divider value for two-dimension widgets.
    *
    *******************************************************************************/
    void CapTouch_SsCSDSetColSnsClkDivider(uint32 widgetId, uint32 dividerVal)
    {
        /* Get sense divider based on configuration */
        #if (CapTouch_ENABLE != CapTouch_CSD_COMMON_SNS_CLK_EN)
            CapTouch_RAM_WD_BASE_STRUCT *ptrWdgt;

            ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)
            CapTouch_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

            ptrWdgt->snsClk = (uint16)dividerVal;
        #else
            CapTouch_dsRam.snsCsdClk = (uint16)dividerVal;

            (void)widgetId; /* This parameter is unused in such configurations */
        #endif /* (CapTouch_ENABLE == CapTouch_CSD_COMMON_SNS_CLK_EN) */
    }


    #if (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN)
        /*******************************************************************************
        * Function Name: CapTouch_SsCSDGetRowSnsClkDivider
        ****************************************************************************//**
        *
        * \brief
        *  This function gets the Sense Clock Divider value for one-dimension widgets
        *  and the Column Sense Clock divider value for two-dimension widgets.
        *
        * \details
        *  This function gets the Sense Clock Divider value based on the Component
        *  configuration. The function is applicable for one-dimension widgets and for
        *  two-dimension widgets.
        *
        * \param
        *  widgetId Specifies the ID of the widget.
        *
        * \return
        *  Returns the sense clock divider value for one-dimension widgets
        *  and column sense clock divider value for two-dimension widgets.
        *
        *******************************************************************************/
        uint32 CapTouch_SsCSDGetRowSnsClkDivider(uint32 widgetId)
        {
            uint32 retVal;

            /* Get sense divider based on configuration */
            #if (CapTouch_ENABLE != CapTouch_CSD_COMMON_SNS_CLK_EN)
                CapTouch_RAM_WD_BASE_STRUCT *ptrWdgt;

                ptrWdgt = (CapTouch_RAM_WD_BASE_STRUCT *)
                CapTouch_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

                retVal = ptrWdgt->rowSnsClk;
            #else
                retVal = (uint32)CapTouch_dsRam.snsCsdClk;

                (void)widgetId; /* This parameter is unused in such configurations */
            #endif /* (CapTouch_ENABLE == CapTouch_CSD_COMMON_SNS_CLK_EN) */

            return (retVal);
        }

    #endif /* (CapTouch_CSD_MATRIX_WIDGET_EN || CapTouch_CSD_TOUCHPAD_WIDGET_EN) */


    #if (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE)
        /*******************************************************************************
        * Function Name: CapTouch_SsCSDCalcPrsSize
        ****************************************************************************//**
        *
        * \brief
        *  The function finds PRS polynomial size in the Auto mode.
        *
        * \details
        *  The PRS polynomial size in the Auto mode is found based on the following
        *  requirements:
        *  - at least one full spread spectrum polynomial should pass during scan time.
        *
        * \param
        *  snsClkDivider The divider value for the sense clock.
        *  resolution The widget resolution.
        *
        * \return prsSize PRS value for SENSE_PERIOD register.
        *
        *******************************************************************************/
        uint8 CapTouch_SsCSDCalcPrsSize(uint32 snsClkDivider, uint32 resolution)
        {
            uint32 prsSize;
            uint32 modClkDivider = 1u;

            #if ((CapTouch_ENABLE != CapTouch_CSDV2) && \
                 (CapTouch_ENABLE == CapTouch_IS_M0S8PERI_BLOCK))
                    modClkDivider = (uint32)CapTouch_dsRam.modCsdClk;
            #endif

            if ((snsClkDivider * CapTouch_PRS_LENGTH_12_BITS) <= (modClkDivider * ((0x00000001Lu << resolution) - 1u)))
            {
                /* Set PRS12 mode */
                prsSize = CapTouch_PRS_12_CONFIG;
            }
            else if ((snsClkDivider * CapTouch_PRS_LENGTH_8_BITS) <= (modClkDivider * ((0x00000001Lu << resolution) - 1u)))
            {
                /* Set PRS8 mode */
                prsSize = CapTouch_PRS_8_CONFIG;
            }
            else
            {
                /* Set Direct clock mode */
                prsSize = CapTouch_CLK_SOURCE_DIRECT;
            }

            return (uint8)prsSize;
        }
    #endif /* (CapTouch_CLK_SOURCE_PRSAUTO == CapTouch_CSD_SNS_CLK_SOURCE) */
#endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */


#if (CapTouch_ENABLE == CapTouch_CSDV2)
    /*******************************************************************************
    * Function Name: CapTouch_BistDischargeExtCapacitors
    ****************************************************************************//**
    *
    * \brief
    *  The function discharge available external capacitors.
    *
    * \details
    *  The function discharge available external capacitors by connection them
    *  to GND using STRONG GPIO drive mode. Additionaly, the function disconnects 
    *  the capacitors from analog mux buses if connected.
    *  Note: the function does not restore the connection to analog mux busses 
    *  and supposes that all the capacitors belong to a single device port.
    *
    *******************************************************************************/
    void CapTouch_BistDischargeExtCapacitors(void)
    {
        uint32 newRegValue;
        uint8  interruptState;
        
        /* Disconnect Ext Cap from AMUXBUS-A using HSIOM registers */
        interruptState = CyEnterCriticalSection();
        newRegValue = CY_GET_REG32(CapTouch_EXT_CAP_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapTouch_EXT_CAP_HSIOM_MASK);
        CY_SET_REG32(CapTouch_EXT_CAP_HSIOM_PTR, newRegValue);
        CyExitCriticalSection(interruptState);

        /* Set output port register to 0 to connect to GND */
        interruptState = CyEnterCriticalSection();
        newRegValue = CY_GET_REG32(CapTouch_EXT_CAP_DR_PTR);
        newRegValue &= (uint32)(~(uint32)CapTouch_EXT_CAP_DR_MASK);
        CY_SET_REG32(CapTouch_EXT_CAP_DR_PTR, newRegValue);
        CyExitCriticalSection(interruptState);

        /* Set port configuration register (drive mode) in STRONG mode */
        interruptState = CyEnterCriticalSection();
        newRegValue = CY_GET_REG32(CapTouch_EXT_CAP_PC_PTR);
        newRegValue &= (uint32)(~((uint32)CapTouch_EXT_CAP_PC_MASK));
        newRegValue |= (CapTouch_EXT_CAP_PC_STRONG_CFG);
        CY_SET_REG32(CapTouch_EXT_CAP_PC_PTR, newRegValue);
        CyExitCriticalSection(interruptState);

        /* Delay to discharge external capacitors */
        CyDelayUs(CapTouch_EXT_CAP_DISCHARGE_TIME);

        /* Set port configuration register (drive mode) in STRONG mode */
        interruptState = CyEnterCriticalSection();
        newRegValue = CY_GET_REG32(CapTouch_EXT_CAP_PC_PTR);
        newRegValue &= (uint32)(~((uint32)CapTouch_EXT_CAP_PC_MASK));
        CY_SET_REG32(CapTouch_EXT_CAP_PC_PTR, newRegValue);
        CyExitCriticalSection(interruptState);
    }
#endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */


/* [] END OF FILE */
