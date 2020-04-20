/***************************************************************************//**
* \file CapTouch_Structure.h
* \version 6.0
*
* \brief
*   This file provides the top level declarations of the Component data structure.
*   Also, the file declares the high-level and low-level APIs for data access.
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

#if !defined(CY_SENSE_CapTouch_STRUCTURE_H)
#define CY_SENSE_CapTouch_STRUCTURE_H

#include <cytypes.h>
#include "cyfitter.h"
#include "CapTouch_Configuration.h"
#if (CapTouch_CSD_SS_DIS != CapTouch_CSD_AUTOTUNE)
    #include "CapTouch_SmartSense_LL.h"
#endif

#if (CapTouch_ENABLE == CapTouch_ALP_FILTER_EN)
    #include "CapTouch_AlpFilter_LL.h"
#endif

#if ((CapTouch_ENABLE == CapTouch_GES_GLOBAL_EN) ||\
     (CapTouch_ENABLE == CapTouch_BALLISTIC_MULTIPLIER_EN))
    #include "CapTouch_TMG.h"
#endif

#if (CapTouch_ENABLE == CapTouch_CENTROID_5X5_CSD_EN)
    #include "CapTouch_AdvancedCentroid_LL.h"
#endif

#if (CapTouch_ENABLE == CapTouch_POS_ADAPTIVE_IIR_FILTER_EN)
    #include "CapTouch_AdaptiveFilter_LL.h"
#endif

/*******************************************************************************
* Constant Definitions
*******************************************************************************/

/* Defines size of Sensor Status Register in Data Structure */
#define CapTouch_SNS_STS_TYPE               uint8

/* No touch condition for slider position report */
#define CapTouch_SLIDER_NO_TOUCH            (0xFFFFu)
#define CapTouch_TOUCHPAD_NO_TOUCH          (0xFFFFFFFFLu)

#define CapTouch_SLIDER_POS_NONE            (0xFFFFu)
#define CapTouch_TOUCHPAD_POS_NONE          (0xFFFFu)

#define CapTouch_MATRIX_POS_NONE            (0xFFu)
#define CapTouch_MATRIX_POS_MULTI           (0xFEu)

#define CapTouch_PROX_STS_MASK              (3Lu)
#define CapTouch_PROX_STS_OFFSET(proxId)    ((proxId) << 1u)

#define CapTouch_MATRIX_BUTTONS_TOUCHED     (0x80000000Lu)

/*******************************************************************************
* Enumeration types definition
*******************************************************************************/

/***************************************************************************//**
* \brief Defines widget types
*******************************************************************************/
typedef enum
{
    CapTouch_WD_BUTTON_E        = 0x01u,
    CapTouch_WD_LINEAR_SLIDER_E = 0x02u,
    CapTouch_WD_RADIAL_SLIDER_E = 0x03u,
    CapTouch_WD_MATRIX_BUTTON_E = 0x04u,
    CapTouch_WD_TOUCHPAD_E      = 0x05u,
    CapTouch_WD_PROXIMITY_E     = 0x06u,
    CapTouch_WD_ENCODERDIAL_E   = 0x07u
} CapTouch_WD_TYPE_ENUM;


/***************************************************************************//**
* \brief Defines sensing methods types
*******************************************************************************/
typedef enum
{
    CapTouch_UNDEFINED_E            = 0x00u,
    CapTouch_SENSE_METHOD_CSD_E     = 0x01u,
    CapTouch_SENSE_METHOD_CSX_E     = 0x02u,
    CapTouch_SENSE_METHOD_BIST_E    = 0x03u,
    CapTouch_SENSE_METHOD_ISX_E     = 0x04u,
} CapTouch_SENSE_METHOD_ENUM;

/***************************************************************************//**
* \brief Defines electrode types
*******************************************************************************/
typedef enum
{
    CapTouch_ELTD_TYPE_SELF_E   = 0x01u,
    CapTouch_ELTD_TYPE_MUT_TX_E = 0x02u,
    CapTouch_ELTD_TYPE_MUT_RX_E = 0x03u
} CapTouch_ELTD_TYPE_ENUM;

/**
* \cond SECTION_STRUCTURES
* \addtogroup group_structures
* \{
*/
/*******************************************************************************
* Declares RAM structures for all used widgets
*******************************************************************************/

/***************************************************************************//**
* \brief Declares common widget RAM parameters
*******************************************************************************/
typedef struct
{
    /**
     *  CRC for the whole Widget Object in RAM (not only the common part)
     */
    uint16 crc;

    /**
     *  Provides scan resolution or number of sub-conversions.
     */
    uint16 resolution;

    /**
     *  Widget Finger Threshold.
     */
    CapTouch_THRESHOLD_TYPE fingerTh;

    /**
     *  Widget Noise Threshold.
     */
    uint8  noiseTh;

    /**
     *  Widget Negative Noise Threshold.
     */
    uint8  nNoiseTh;

    /**
     *  Widget Hysteresis for the signal crossing finger or touch/proximity 
     *  threshold.
     */
    uint8  hysteresis;

    /**
     *  Widget Debounce for the signal above the finger or touch/proximity 
     *  threshold. OFF to ON.
     */
    uint8  onDebounce;

    /**
     *  The widget low baseline reset count. Specifies the number of 
     *  samples the sensor has to be below the Negative Noise 
     *  Threshold to trigger a baseline reset.
     */
    CapTouch_LOW_BSLN_RST_TYPE lowBslnRst;

    /**
     *  Specifies the sense clock divider. Present only if individual 
     *  clock dividers are enabled. Specifies the sense clock divider 
     *  for the Column sensors for the Matrix Buttons and Touchpad 
     *  widgets. Sets Tx clock divider for CSX Widgets.
     */
    uint16 snsClk;

    /**
     *  Register for internal use
     */
    uint8  snsClkSource;
} CapTouch_RAM_WD_BASE_STRUCT;

/***************************************************************************//**
* \brief Declares RAM parameters for the CSX Button
*******************************************************************************/
typedef struct
{
    /**
     *  CRC for the whole Widget Object in RAM (not only the common part)
     */
    uint16 crc;

    /**
     *  Provides scan resolution or number of sub-conversions.
     */
    uint16 resolution;

    /**
     *  Widget Finger Threshold.
     */
    CapTouch_THRESHOLD_TYPE fingerTh;

    /**
     *  Widget Noise Threshold.
     */
    uint8  noiseTh;

    /**
     *  Widget Negative Noise Threshold.
     */
    uint8  nNoiseTh;

    /**
     *  Widget Hysteresis for the signal crossing finger or touch/proximity 
     *  threshold.
     */
    uint8  hysteresis;

    /**
     *  Widget Debounce for the signal above the finger or touch/proximity 
     *  threshold. OFF to ON.
     */
    uint8  onDebounce;

    /**
     *  The widget low baseline reset count. Specifies the number of 
     *  samples the sensor has to be below the Negative Noise 
     *  Threshold to trigger a baseline reset.
     */
    CapTouch_LOW_BSLN_RST_TYPE lowBslnRst;

    /**
     *  Specifies the sense clock divider. Present only if individual 
     *  clock dividers are enabled. Specifies the sense clock divider 
     *  for the Column sensors for the Matrix Buttons and Touchpad 
     *  widgets. Sets Tx clock divider for CSX Widgets.
     */
    uint16 snsClk;

    /**
     *  Register for internal use
     */
    uint8  snsClkSource;
} CapTouch_RAM_WD_BUTTON_STRUCT;


/***************************************************************************//**
* \brief Declares RAM structure with all defined widgets
*******************************************************************************/
typedef struct
{
    /**
     *  U102 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u102;

    /**
     *  U103 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u103;

    /**
     *  U104 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u104;

    /**
     *  U105 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u105;

    /**
     *  U106 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u106;

    /**
     *  U107 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u107;

    /**
     *  U108 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u108;

    /**
     *  U109 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u109;

    /**
     *  U110 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u110;

    /**
     *  U111 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u111;

    /**
     *  U112 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u112;

    /**
     *  U113 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u113;

    /**
     *  U114 widget RAM structure
     */
    CapTouch_RAM_WD_BUTTON_STRUCT u114;
} CapTouch_RAM_WD_LIST_STRUCT;


/***************************************************************************//**
* \brief Declares RAM structure for sensors
*******************************************************************************/
typedef struct
{
    /**
     *  The sensor raw counts.
     */
    uint16 raw [CapTouch_NUM_SCAN_FREQS];

    /**
     *  The sensor baseline.
     */
    uint16 bsln [CapTouch_NUM_SCAN_FREQS];

    /**
     *  The bit inverted baseline
     */
    uint16 bslnInv [CapTouch_NUM_SCAN_FREQS];

    /**
     *  For the bucket baseline algorithm holds the bucket state, 
     *  For the IIR baseline keeps LSB of the baseline value.
     */
    uint8  bslnExt [CapTouch_NUM_SCAN_FREQS];

    /**
     *  Sensor differences.
     */
    CapTouch_THRESHOLD_TYPE diff;

    /**
     *  The baseline reset counter for the low baseline reset function.
     */
    CapTouch_LOW_BSLN_RST_TYPE negBslnRstCnt [CapTouch_NUM_SCAN_FREQS];

    /**
     *  The compensation IDAC value or the balancing IDAC value.
     */
    uint8  idacComp [CapTouch_NUM_SCAN_FREQS];
} CapTouch_RAM_SNS_STRUCT;


/***************************************************************************//**
* \brief Declares RAM structure with all defined sensors
*******************************************************************************/
typedef struct
{
    /**
     *  U102 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u102 [CapTouch_U102_NUM_SENSORS];

    /**
     *  U103 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u103 [CapTouch_U103_NUM_SENSORS];

    /**
     *  U104 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u104 [CapTouch_U104_NUM_SENSORS];

    /**
     *  U105 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u105 [CapTouch_U105_NUM_SENSORS];

    /**
     *  U106 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u106 [CapTouch_U106_NUM_SENSORS];

    /**
     *  U107 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u107 [CapTouch_U107_NUM_SENSORS];

    /**
     *  U108 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u108 [CapTouch_U108_NUM_SENSORS];

    /**
     *  U109 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u109 [CapTouch_U109_NUM_SENSORS];

    /**
     *  U110 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u110 [CapTouch_U110_NUM_SENSORS];

    /**
     *  U111 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u111 [CapTouch_U111_NUM_SENSORS];

    /**
     *  U112 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u112 [CapTouch_U112_NUM_SENSORS];

    /**
     *  U113 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u113 [CapTouch_U113_NUM_SENSORS];

    /**
     *  U114 sensors RAM structures array
     */
    CapTouch_RAM_SNS_STRUCT u114 [CapTouch_U114_NUM_SENSORS];
} CapTouch_RAM_SNS_LIST_STRUCT;


/***************************************************************************//**
* \brief Declares self test data structure
*******************************************************************************/
typedef struct
{
    /**
     *  Bit mask of test results (PASS/FAIL)
     */
    uint32 testResultMask;

    /**
     *  The capacitance of an external capacitor
     */
    uint16 extCap [CapTouch_TST_EXT_CAPS_NUM];

    /**
     *  The result of Vdda measurement (mV)
     */
    uint16 vddaVoltage;

    /**
     *  A calculated CRC for global Component Data
     */
    uint16 glbCrcCalc;

    /**
     *  The widget data structure calculated CRC if the correspondent 
     *  test result bit is set
     */
    uint16 wdgtCrcCalc;

    /**
     *  The global data structure calculated CRC if the correspondent 
     *  test result bit is set
     */
    uint8  wdgtCrcId;

    /**
     *  The first widget ID with mismatched baseline
     */
    uint8  invBslnWdgtId;

    /**
     *  The first sensor ID with mismatched baseline
     */
    uint8  invBslnSnsId;

    /**
     *  The first shorted to GND/VDDA widget ID
     */
    uint8  shortedWdgtId;

    /**
     *  The first shorted to GND/VDDA sensor ID
     */
    uint8  shortedSnsId;

    /**
     *  The first widget ID with a sensor shorted to another sensor
     */
    uint8  p2pWdgtId;

    /**
     *  The first sensor ID shorted to another sensor
     */
    uint8  p2pSnsId;
} CapTouch_RAM_SELF_TEST_STRUCT;


/***************************************************************************//**
* \brief Declares sensor Cp data structure
*******************************************************************************/
typedef struct
{
    /**
     *  Sensor Cp data for U102 widget
     */
    uint8 u102[CapTouch_U102_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U103 widget
     */
    uint8 u103[CapTouch_U103_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U104 widget
     */
    uint8 u104[CapTouch_U104_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U105 widget
     */
    uint8 u105[CapTouch_U105_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U106 widget
     */
    uint8 u106[CapTouch_U106_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U107 widget
     */
    uint8 u107[CapTouch_U107_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U108 widget
     */
    uint8 u108[CapTouch_U108_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U109 widget
     */
    uint8 u109[CapTouch_U109_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U110 widget
     */
    uint8 u110[CapTouch_U110_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U111 widget
     */
    uint8 u111[CapTouch_U111_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U112 widget
     */
    uint8 u112[CapTouch_U112_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U113 widget
     */
    uint8 u113[CapTouch_U113_NUM_SENSORS + 1u];
    /**
     *  Sensor Cp data for U114 widget
     */
    uint8 u114[CapTouch_U114_NUM_SENSORS + 1u];
} CapTouch_RAM_SNS_CP_STRUCT;

/***************************************************************************//**
* \brief Declares the top-level RAM Data Structure
*******************************************************************************/
typedef struct
{
    /**
     *  16-bit CRC calculated by the customizer for the component 
     *  configuration. Used by the Tuner application to identify if 
     *  the FW corresponds to the specific user configuration.
     */
    uint16 configId;

    /**
     *  Used by the Tuner application to identify device-specific configuration.
     */
    uint16 deviceId;

    /**
     *  Used by the Tuner application to identify the system clock frequency.
     */
    uint16 hwClock;

    /**
     *  Tuner Command Register. Used for the communication 
     *  between the Tuner GUI and the component.
     */
    uint16 tunerCmd;

    /**
     *  This counter gets incremented after each scan.
     */
    uint16 scanCounter;

    /**
     *  Status information: Current Widget, Scan active, Error code.
     */
    volatile uint32 status;

    /**
     *  The bitmask that sets which Widgets are enabled and 
     *  scanned, each bit corresponds to one widget.
     */
    uint32 wdgtEnable [CapTouch_WDGT_STATUS_WORDS];

    /**
     *  The bitmask that reports the self-test status of all Widgets, 
     *  each bit corresponds to one widget.
     */
    uint32 wdgtWorking [CapTouch_WDGT_STATUS_WORDS];

    /**
     *  The bitmask that reports activated Widgets (widgets that 
     *  detect a touch signal above the threshold), each bit 
     *  corresponds to one widget.
     */
    uint32 wdgtStatus [CapTouch_WDGT_STATUS_WORDS];

    /**
     *  For Buttons, Sliders, Matrix Buttons and CSD Touchpad each bit 
     *  reports status of the individual sensor of the widget: 1 - active 
     *  (above the finger threshold); 0 - inactive; For the CSD Touchpad 
     *  and CSD Matrix Buttons, the column sensors occupy the least 
     *  significant bits. For the Proximity widget, each sensor uses two 
     *  bits with the following meaning: 00 - Not active; 01 - Proximity 
     *  detected (signal above finger threshold); 11 - A finger touch 
     *  detected (signal above the touch threshold); For the CSX Touchpad 
     *  Widget, this register provides a number of the detected touches. 
     *  The array size is equal to the total number of widgets. The size of 
     *  the array element depends on the max number of sensors per 
     *  widget used in the current design. It could be 1, 2 or 4 bytes.
     */
    CapTouch_SNS_STS_TYPE snsStatus [CapTouch_TOTAL_WIDGETS];

    /**
     *  The configuration register for global parameters of the 
     *  SENSE_HW0 block.
     */
    uint16 csd0Config;

    /**
     *  The modulator clock divider for the CSX widgets.
     */
    uint8  modCsxClk;

    /**
     *  CRC for global data.
     */
    uint16 glbCrc;

    /**
     *  RAM Widget Objects.
     */
    CapTouch_RAM_WD_LIST_STRUCT wdgtList;

    /**
     *  RAM Sensor Objects.
     */
    CapTouch_RAM_SNS_LIST_STRUCT snsList;

    /**
     *  The self-test data structure.
     */
    CapTouch_RAM_SELF_TEST_STRUCT selfTest;

    /**
     *  The sensor Cp Measurement data structures.
     */
    CapTouch_RAM_SNS_CP_STRUCT snsCp;

    /**
     *  The selected widget ID.
     */
    uint8  snrTestWidgetId;

    /**
     *  The selected sensor ID.
     */
    uint8  snrTestSensorId;

    /**
     *  The scan counter.
     */
    uint16 snrTestScanCounter;

    /**
     *  The sensor raw counts.
     */
    uint16 snrTestRawCount [CapTouch_NUM_SCAN_FREQS];
} CapTouch_RAM_STRUCT;


/***************************************************************************//**
* \brief Declares the Flash IO object
*******************************************************************************/
typedef struct
{
    /**
     *  Pointer to the HSIOM configuration register of the IO.
     */
    reg32  * hsiomPtr;

    /**
     *  Pointer to the port configuration register of the IO.
     */
    reg32  * pcPtr;

    /**
     *  The pointer to the port configuration register of the IO.
     */
    reg32  * pc2Ptr;

    /**
     *  Pointer to the port data register of the IO.
     */
    reg32  * drPtr;

    /**
     *  Pointer to the pin state data register of the IO.
     */
    reg32  * psPtr;

    /**
     *  IO mask in the HSIOM configuration register.
     */
    uint32   hsiomMask;

    /**
     *  IO mask in the DR and PS registers.
     */
    uint32   mask;

    /**
     *  Position of the IO configuration bits in the HSIOM register.
     */
    uint8    hsiomShift;

    /**
     *  Position of the IO configuration bits in the DR and PS registers.
     */
    uint8    drShift;

    /**
     *  Position of the IO configuration bits in the PC register.
     */
    uint8    shift;
} CapTouch_FLASH_IO_STRUCT;


/***************************************************************************//**
* \brief Declares the Flash Electrode object
*******************************************************************************/
typedef struct
{
    /**
     *  Index of the first IO in the Flash IO Object Array.
     */
    uint16 firstPinId;

    /**
     *  Total number of IOs in this sensor.
     */
    uint8  numPins;

    /**
     * Sensor type:
     * \if SECTION_C_DS
     * - ELTD_TYPE_SELF_E - CSD sensor;
     * - ELTD_TYPE_MUT_TX_E - CSX Tx;
     * - ELTD_TYPE_MUT_RX_E - CSX Rx;
     * \endif
     * \if SECTION_I_DS
     * - ELTD_TYPE_MUT_TX_E - ISX Lx sensor;
     * - ELTD_TYPE_MUT_RX_E - ISX Rx sensor;
     * \endif
     */
    uint8  type;
} CapTouch_FLASH_SNS_STRUCT;


/***************************************************************************//**
* \brief Declares the structure with all Flash electrode objects
*******************************************************************************/
typedef struct
{
    /**
     *  No ganged sensors available
     */
    uint8 notUsed;
} CapTouch_FLASH_SNS_LIST_STRUCT;




/***************************************************************************//**
* \brief Declares Flash widget object
*******************************************************************************/
typedef struct
{
    /**
     *  Points to the array of the FLASH Sensor Objects or FLASH IO 
     *  Objects that belong to this widget. Sensing block uses this 
     *  pointer to access and configure IOs for the scanning. Bit #2 in 
     *  WD_STATIC_CONFIG field indicates the type of array: 
     *  1 - Sensor Object; 0 - IO Object.
     */
    void const * ptr2SnsFlash;

    /**
     *  Points to the Widget Object in RAM. Sensing block uses it to 
     *  access scan parameters. Processing uses it to access threshold 
     *  and widget specific data.
     */
    void * ptr2WdgtRam;

    /**
     *  Points to the array of Sensor Objects in RAM. The sensing and 
     *  processing blocks use it to access the scan data.
     */
    CapTouch_RAM_SNS_STRUCT * ptr2SnsRam;

    /**
     *  Points to the array of the Filter History Objects in RAM that 
     *  belongs to this widget.
     */
    void * ptr2FltrHistory;

    /**
     *  Points to the array of the debounce counters. The size of the 
     *  debounce counter is 8 bits. These arrays are not part of the 
     *  data structure.
     */
    uint8 * ptr2DebounceArr;

    /**
     *  Miscellaneous configuration flags.
     */
    uint32 staticConfig;

    /**
     *  The total number of sensors. 
     *  For CSD widgets: WD_NUM_ROWS + WD_NUM_COLS. 
     *  For CSX widgets: WD_NUM_ROWS * WD_NUM_COLS.
     */
    uint16 totalNumSns;

    /**
     *  Specifies one of the following widget types: 
     *  WD_BUTTON_E, WD_LINEAR_SLIDER_E, WD_RADIAL_SLIDER_E, 
     *  WD_MATRIX_BUTTON_E, WD_TOUCHPAD_E, WD_PROXIMITY_E
     */
    uint8  wdgtType;

    /**
     *  For CSD Button and Proximity Widgets, the number of sensors. 
     *  For CSD Slider Widget, the number of segments. 
     *  For CSD Touchpad and Matrix Button, the number of the 
     *  column sensors. 
     *  For CSX Button, Touchpad and Matrix Button, the number 
     *  of the Rx electrodes.
     */
    uint8  numCols;

    /**
     *  For CSD Touchpad and Matrix Buttons, the number of the row 
     *  sensors. For the CSX Button, the number of the Tx electrodes 
     *  (constant 1u). For CSX Touchpad and Matrix Button, the 
     *  number of the Tx electrodes.
     */
    uint8  numRows;

    /**
     *  The pointer to the array with the electrode capacitance value in pF.
     */
    uint8 * ptr2SnsCpArr;
} CapTouch_FLASH_WD_STRUCT;


/***************************************************************************//**
* \brief Declares top-level Flash Data Structure
*******************************************************************************/
typedef struct
{
    /**
     *  Array of flash widget objects
     */
    CapTouch_FLASH_WD_STRUCT wdgtArray[CapTouch_TOTAL_WIDGETS];
} CapTouch_FLASH_STRUCT;


#if (CapTouch_ENABLE == CapTouch_TST_BSLN_RAW_OUT_RANGE_EN)
    /***************************************************************************//**
    * \brief Defines the structure for test of baseline and raw count limits which
    * will be determined by user for every sensor grounding on the
    * manufacturing specific data
    *******************************************************************************/
    typedef struct
    {
        /**
         *  Upper limit of a sensor baseline.
         */
        uint16 bslnHiLim;
        /**
         *  Lower limit of a sensor baseline.
         */
        uint16 bslnLoLim;
        /**
         *  Upper limit of a sensor raw count.
         */
        uint16 rawHiLim;
        /**
         *  Lower limit of a sensor raw count.
         */
        uint16 rawLoLim;
    } CapTouch_BSLN_RAW_RANGE_STRUCT;
#endif /* (CapTouch_ENABLE == CapTouch_TST_BSLN_RAW_OUT_RANGE_EN) */

/** \}
* \endcond */


/***************************************************************************//**
* Declares Dual-channel scan structure
*******************************************************************************/
typedef struct
{
    uint8 csd0WidgetId;
    uint8 csd0SensorId;
    uint8 csd1WidgetId;
    uint8 csd1SensorId;
} CapTouch_SCAN_SLOT_STRUCT;


/***************************************************************************//**
* Declares Filter module structures
*******************************************************************************/

/***************************************************************************//**
* \brief Declares filter channel structure for regular sensors
*******************************************************************************/
typedef struct
{
    /**
     *  IIR filter history
     */
    uint16 iirHistory;
} CapTouch_REGULAR_FLTR_CHANNEL_STRUCT;

/***************************************************************************//**
* \brief Declares filter structure for regular sensors
*******************************************************************************/
typedef struct
{
    /**
     *  Array of CapTouch_REGULAR_FLTR_CHANNEL_STRUCT for each available scan frequency
     */
    CapTouch_REGULAR_FLTR_CHANNEL_STRUCT regularChannel[CapTouch_NUM_SCAN_FREQS];
} CapTouch_REGULAR_FLTR_STRUCT;

/***************************************************************************//**
* \brief Declares union for filter structure variants
*******************************************************************************/
typedef union
{
    /**
     *  Pointer to void type
     */
    void *ptr;

    /**
     *  Pointer to CapTouch_REGULAR_FLTR_STRUCT type
     */
    CapTouch_REGULAR_FLTR_STRUCT *ptrRegular;
} CapTouch_PTR_FILTER_VARIANT;


#if (0u != CapTouch_POSITION_FILTER_EN)
typedef struct
{
    #if (0u != CapTouch_POS_MEDIAN_FILTER_EN)
        uint16 posMedianZ1;
        uint16 posMedianZ2;
    #endif /* #if (0u != CapTouch_POS_MEDIAN_FILTER_EN) */

    #if (0u != CapTouch_POS_IIR_FILTER_EN)
        uint16 posIIR;
    #endif /* #if (0u != CapTouch_POS_IIR_FILTER_EN) */

    #if (0u != CapTouch_POS_ADAPTIVE_IIR_FILTER_EN)
        uint16 posAIIR;
    #endif /* (0u != CapTouch_POS_ADAPTIVE_IIR_FILTER_EN) */

    #if (0u != CapTouch_POS_AVERAGE_FILTER_EN)
        uint16 posAverage;
    #endif /* #if (0u != CapTouch_POS_AVERAGE_FILTER_EN) */

    #if (0u != CapTouch_POS_JITTER_FILTER_EN)
        uint16 posJitter;
    #endif /* #if (0u != CapTouch_POS_JITTER_FILTER_EN) */

    #if (0u != CapTouch_POS_ADAPTIVE_IIR_FILTER_EN)
        uint8 posAIIRCoeff;
    #endif /* (0u != CapTouch_POS_ADAPTIVE_IIR_FILTER_EN) */
} CapTouch_SLIDER_POS_HISTORY_STRUCT;

typedef struct
{
    CapTouch_SLIDER_POS_HISTORY_STRUCT xPos;
    CapTouch_SLIDER_POS_HISTORY_STRUCT yPos;
} CapTouch_TOUCHPAD_POS_HISTORY_STRUCT;
#endif /* (0u != CapTouch_POSITION_FILTER_EN) */

/*******************************************************************************
* API Constants
*******************************************************************************/

/*******************************************************************************
* Defines shift/mask values for data structure register bit fields
*******************************************************************************/

/* CSD0_CONFIG bit fields */

/* Bit 0: The multi-frequency scan enable/disable at runtime. This bit 
 * is functional only if the multi-frequency scan functionality 
 * has been activated in general component configuration. */
#define CapTouch_MULTI_FREQ_EN_SIZE       (0x00000001Lu)
#define CapTouch_MULTI_FREQ_EN_SHIFT      (0u)
#define CapTouch_MULTI_FREQ_EN_MASK       (CapTouch_MULTI_FREQ_EN_SIZE << CapTouch_MULTI_FREQ_EN_SHIFT)

/* Bit 1: The sensor auto-reset is enabled */
#define CapTouch_SNS_ARST_SIZE            (0x00000001Lu)
#define CapTouch_SNS_ARST_SHIFT           (1u)
#define CapTouch_SNS_ARST_MASK            (CapTouch_SNS_ARST_SIZE << CapTouch_SNS_ARST_SHIFT)

/* Bits 2-3: The IDAC range setting (4x/8x) */
#define CapTouch_IDAC_RANGE_SIZE          (0x00000003Lu)
#define CapTouch_IDAC_RANGE_SHIFT         (2u)
#define CapTouch_IDAC_RANGE_MASK          (CapTouch_IDAC_RANGE_SIZE << CapTouch_IDAC_RANGE_SHIFT)

/* Bits 4-7: Reserved */

/* Bit 8: The shield electrode signal Enable/Disable */
#define CapTouch_SHLD_EN_SIZE             (0x00000001Lu)
#define CapTouch_SHLD_EN_SHIFT            (8u)
#define CapTouch_SHLD_EN_MASK             (CapTouch_SHLD_EN_SIZE << CapTouch_SHLD_EN_SHIFT)

/* Bits 9-10: Selects the delay by which the csd_shield is delayed relative 
 * to csd_sense */
#define CapTouch_SHLD_DLY_SIZE            (0x00000003Lu)
#define CapTouch_SHLD_DLY_SHIFT           (9u)
#define CapTouch_SHLD_DLY_MASK            (CapTouch_SHLD_DLY_SIZE << CapTouch_SHLD_DLY_SHIFT)


/* STATUS bit fields */

/* Bits 0-6: The widget currently selected for SENSE_HW 0 */
#define CapTouch_STATUS_WDGT0_SIZE        (0x0000007FLu)
#define CapTouch_STATUS_WDGT0_SHIFT       (0u)
#define CapTouch_STATUS_WDGT0_MASK        (CapTouch_STATUS_WDGT0_SIZE << CapTouch_STATUS_WDGT0_SHIFT)

/* Bit 7: The SENSE_HW0 block status */
#define CapTouch_STATUS_CSD0_SIZE         (0x00000001Lu)
#define CapTouch_STATUS_CSD0_SHIFT        (7u)
#define CapTouch_STATUS_CSD0_MASK         (CapTouch_STATUS_CSD0_SIZE << CapTouch_STATUS_CSD0_SHIFT)

/* Bits 8-14: The widget currently selected for SENSE_HW1 */
#define CapTouch_STATUS_WDGT1_SIZE        (0x0000007FLu)
#define CapTouch_STATUS_WDGT1_SHIFT       (8u)
#define CapTouch_STATUS_WDGT1_MASK        (CapTouch_STATUS_WDGT1_SIZE << CapTouch_STATUS_WDGT1_SHIFT)

/* Bit 15: The SENSE_HW1 block status */
#define CapTouch_STATUS_CSD1_SIZE         (0x00000001Lu)
#define CapTouch_STATUS_CSD1_SHIFT        (15u)
#define CapTouch_STATUS_CSD1_MASK         (CapTouch_STATUS_CSD1_SIZE << CapTouch_STATUS_CSD1_SHIFT)

/* Bit 16: The WIDGET BUSY flag status for SENSE_HW0 */
#define CapTouch_STATUS_WDGT0_BUSY_SIZE   (0x00000001Lu)
#define CapTouch_STATUS_WDGT0_BUSY_SHIFT  (16u)
#define CapTouch_STATUS_WDGT0_BUSY_MASK   (CapTouch_STATUS_WDGT0_BUSY_SIZE << CapTouch_STATUS_WDGT0_BUSY_SHIFT)

/* Bit 17: The WIDGET BUSY flag status for SENSE_HW1 */
#define CapTouch_STATUS_WDGT1_BUSY_SIZE   (0x00000001Lu)
#define CapTouch_STATUS_WDGT1_BUSY_SHIFT  (17u)
#define CapTouch_STATUS_WDGT1_BUSY_MASK   (CapTouch_STATUS_WDGT1_BUSY_SIZE << CapTouch_STATUS_WDGT1_BUSY_SHIFT)

/* Bits 18-23: Reserved */

/* Bits 24-31: Component error code */
#define CapTouch_STATUS_ERR_SIZE          (0x000000FFLu)
#define CapTouch_STATUS_ERR_SHIFT         (24u)
#define CapTouch_STATUS_ERR_MASK          (CapTouch_STATUS_ERR_SIZE << CapTouch_STATUS_ERR_SHIFT)


/* WD_STATIC_CONFIG bit fields */

/* Bit 0: The sense/Tx frequency selection: 0 - Common, 1 - Individual 
 * (per widget). */
#define CapTouch_SNS_FREQ_SIZE            (0x00000001Lu)
#define CapTouch_SNS_FREQ_SHIFT           (0u)
#define CapTouch_SNS_FREQ_MASK            (CapTouch_SNS_FREQ_SIZE << CapTouch_SNS_FREQ_SHIFT)

/* Bit 1: Duplexing Enable/Disable for linear sliders. */
#define CapTouch_DIPLEXING_SIZE           (0x00000001Lu)
#define CapTouch_DIPLEXING_SHIFT          (1u)
#define CapTouch_DIPLEXING_MASK           (CapTouch_DIPLEXING_SIZE << CapTouch_DIPLEXING_SHIFT)

/* Bit 2: The widget contains a sensor ganged to other sensors. */
#define CapTouch_GANGED_SNS_SIZE          (0x00000001Lu)
#define CapTouch_GANGED_SNS_SHIFT         (2u)
#define CapTouch_GANGED_SNS_MASK          (CapTouch_GANGED_SNS_SIZE << CapTouch_GANGED_SNS_SHIFT)

/* Bit 3: Some pin from this widget is used by other sensors to make 
 * a ganged sensor. */
#define CapTouch_SHARED_IO_SIZE           (0x00000001Lu)
#define CapTouch_SHARED_IO_SHIFT          (3u)
#define CapTouch_SHARED_IO_MASK           (CapTouch_SHARED_IO_SIZE << CapTouch_SHARED_IO_SHIFT)

/* Bit 4: The centroid position IIR filter Enable/Disable. */
#define CapTouch_POS_IIR_FILTER_SIZE      (0x00000001Lu)
#define CapTouch_POS_IIR_FILTER_SHIFT     (4u)
#define CapTouch_POS_IIR_FILTER_MASK      (CapTouch_POS_IIR_FILTER_SIZE << CapTouch_POS_IIR_FILTER_SHIFT)

/* Bit 5: The centroid position median filter Enable/Disable. */
#define CapTouch_POS_MEDIAN_FILTER_SIZE   (0x00000001Lu)
#define CapTouch_POS_MEDIAN_FILTER_SHIFT  (5u)
#define CapTouch_POS_MEDIAN_FILTER_MASK   (CapTouch_POS_MEDIAN_FILTER_SIZE << CapTouch_POS_MEDIAN_FILTER_SHIFT)

/* Bit 6: The centroid position average filter Enable/Disable. */
#define CapTouch_POS_AVERAGE_FILTER_SIZE  (0x00000001Lu)
#define CapTouch_POS_AVERAGE_FILTER_SHIFT (6u)
#define CapTouch_POS_AVERAGE_FILTER_MASK  (CapTouch_POS_AVERAGE_FILTER_SIZE << CapTouch_POS_AVERAGE_FILTER_SHIFT)

/* Bit 7: The centroid position jitter filter Enable/Disable. */
#define CapTouch_POS_JITTER_FILTER_SIZE   (0x00000001Lu)
#define CapTouch_POS_JITTER_FILTER_SHIFT  (7u)
#define CapTouch_POS_JITTER_FILTER_MASK   (CapTouch_POS_JITTER_FILTER_SIZE << CapTouch_POS_JITTER_FILTER_SHIFT)

/* Bit 8: The multiphase Tx scan Enable (CSX widgets only). */
#define CapTouch_MULTIPHASE_TX_SIZE       (0x00000001Lu)
#define CapTouch_MULTIPHASE_TX_SHIFT      (8u)
#define CapTouch_MULTIPHASE_TX_MASK       (CapTouch_MULTIPHASE_TX_SIZE << CapTouch_MULTIPHASE_TX_SHIFT)

/* Bit 9: The centroid position adaptive IIR filter Enable/Disable. */
#define CapTouch_AIIR_FILTER_SIZE         (0x00000001Lu)
#define CapTouch_AIIR_FILTER_SHIFT        (9u)
#define CapTouch_AIIR_FILTER_MASK         (CapTouch_AIIR_FILTER_SIZE << CapTouch_AIIR_FILTER_SHIFT)

/* Bit 10: Ballistic multiplier Enable/Disable. */
#define CapTouch_BALLISTIC_SIZE           (0x00000001Lu)
#define CapTouch_BALLISTIC_SHIFT          (10u)
#define CapTouch_BALLISTIC_MASK           (CapTouch_BALLISTIC_SIZE << CapTouch_BALLISTIC_SHIFT)

/* Bit 11: 3x3 centroid Enable/Disable. */
#define CapTouch_CENTROID_3X3_SIZE        (0x00000001Lu)
#define CapTouch_CENTROID_3X3_SHIFT       (11u)
#define CapTouch_CENTROID_3X3_MASK        (CapTouch_CENTROID_3X3_SIZE << CapTouch_CENTROID_3X3_SHIFT)

/* Bit 12: 5x5 centroid Enable/Disable. */
#define CapTouch_CENTROID_5X5_SIZE        (0x00000001Lu)
#define CapTouch_CENTROID_5X5_SHIFT       (12u)
#define CapTouch_CENTROID_5X5_MASK        (CapTouch_CENTROID_5X5_SIZE << CapTouch_CENTROID_5X5_SHIFT)

/* Bit 13: Edge correction Enable/Disable. */
#define CapTouch_EDGE_CORRECTION_SIZE     (0x00000001Lu)
#define CapTouch_EDGE_CORRECTION_SHIFT    (13u)
#define CapTouch_EDGE_CORRECTION_MASK     (CapTouch_EDGE_CORRECTION_SIZE << CapTouch_EDGE_CORRECTION_SHIFT)

/* Bit 14: Two finger detection Enable/Disable. */
#define CapTouch_TWO_FINGER_DETECTION_SIZE (0x00000001Lu)
#define CapTouch_TWO_FINGER_DETECTION_SHIFT (14u)
#define CapTouch_TWO_FINGER_DETECTION_MASK (CapTouch_TWO_FINGER_DETECTION_SIZE << CapTouch_TWO_FINGER_DETECTION_SHIFT)


/*******************************************************************************
* Defines Data Structure Macro helpers
*******************************************************************************/

/*******************************************************************************
* Determines the widget specific word in the wdgtStatus array by widget ID
*******************************************************************************/
/* Divide by 32 - size of the wdgtStatus word in bits to find the word index */
#define CapTouch_GET_WDGT_STATUS_INDEX(wdgtId)  ((wdgtId) >> 5u)

/*******************************************************************************
* Determines the widget bitmask in wdgtStatus word by widget ID
*******************************************************************************/
/* Take the least 5 bits of widget id to find the bit number */
#define CapTouch_GET_WDGT_STATUS_MASK(wdgtId)   (1Lu << ((wdgtId) & 0x1FLu))

/*******************************************************************************
* Determines the sense method of the widget
*******************************************************************************/
/* 
* If there are multiple sensing methods, get senseMethod from flash structure,
* otherwise, hardcode it for speed. 
*/
#if (CapTouch_MANY_WIDGET_METHODS_EN)
    #define CapTouch_GET_SENSE_METHOD(ptrFlash) ((CapTouch_SENSE_METHOD_ENUM)(ptrFlash)->senseMethod)
#elif (0u != CapTouch_TOTAL_CSD_WIDGETS)
    #define CapTouch_GET_SENSE_METHOD(ptrFlash) CapTouch_SENSE_METHOD_CSD_E
#elif (0u != CapTouch_TOTAL_CSX_WIDGETS)
    #define CapTouch_GET_SENSE_METHOD(ptrFlash) CapTouch_SENSE_METHOD_CSX_E
#elif (0u != CapTouch_TOTAL_ISX_WIDGETS)
    #define CapTouch_GET_SENSE_METHOD(ptrFlash) CapTouch_SENSE_METHOD_ISX_E
#endif

/*******************************************************************************
* Gets the widget type
*******************************************************************************/
#define CapTouch_GET_WIDGET_TYPE(ptrFlashWdgt)  ((CapTouch_WD_TYPE_ENUM)((ptrFlashWdgt)->wdgtType))

/*******************************************************************************
* Gets the number of the widget's sensors
*******************************************************************************/
#define CapTouch_GET_SENSOR_COUNT(widgetId)         CapTouch_dsFlash.wdgtArray[(widgetId)].totalNumSns
#define CapTouch_GET_SNS_CNT_BY_PTR(ptrFlashWidget) (ptrFlashWidget)->totalNumSns

/*******************************************************************************
* Increments the pointer to the Regular Filter History Object
*******************************************************************************/
#if (0u != CapTouch_REGULAR_RC_ALP_FILTER_EN)
    #define CapTouch_INC_REG_FLTR_OBJ(fltrVariant)      \
        do {                                                    \
            (fltrVariant).ptrAlp++;                             \
        } while(0)
#elif (0u != CapTouch_REGULAR_RC_FILTER_EN)
    #define CapTouch_INC_REG_FLTR_OBJ(fltrVariant)      \
        do {                                                    \
            (fltrVariant).ptrRegular++;                         \
        } while(0)
#else
    #define CapTouch_INC_REG_FLTR_OBJ(fltrVariant)      \
        do {                                                    \
        } while(0)
#endif

/*******************************************************************************
* Increments the pointer to the Proximity Filter History Object
*******************************************************************************/
#if (0u != CapTouch_PROX_RC_ALP_FILTER_EN)
    #define CapTouch_INC_PROX_FLTR_OBJ(fltrVariant)     \
        do {                                                    \
            (fltrVariant).ptrAlp++;                             \
        } while(0)
#elif (0u != CapTouch_PROX_RC_FILTER_EN)
    #define CapTouch_INC_PROX_FLTR_OBJ(fltrVariant)     \
        do {                                                    \
            (fltrVariant).ptrProx++;                            \
        } while(0)
#else
    #define CapTouch_INC_PROX_FLTR_OBJ(fltrVariant)     \
        do {                                                    \
        } while(0)
#endif

/*******************************************************************************
* Increments the pointer to the Filter History Object Variant
*******************************************************************************/
#define CapTouch_INC_FLTR_OBJ_VARIANT(isProxHistObj, fltrVariant)   \
    do {                                                                    \
        if (0u == (isProxHistObj))                                          \
        {                                                                   \
            CapTouch_INC_REG_FLTR_OBJ(fltrVariant);                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            CapTouch_INC_PROX_FLTR_OBJ(fltrVariant);                \
        }                                                                   \
    } while(0)

/*******************************************************************************
* Gets a widget status in the global enable register dsRam.wdgtEnable[]
*******************************************************************************/
#if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)
    #define CapTouch_GET_WIDGET_EN_STATUS(wdId)                                                     \
                 (CapTouch_GET_WDGT_STATUS_MASK(wdId) &                                             \
                  CapTouch_dsRam.wdgtEnable[CapTouch_GET_WDGT_STATUS_INDEX(wdId)] &         \
                  CapTouch_dsRam.wdgtWorking[CapTouch_GET_WDGT_STATUS_INDEX(wdId)])
#else
    #define CapTouch_GET_WIDGET_EN_STATUS(wdId)                    \
                (CapTouch_GET_WDGT_STATUS_MASK(wdId) &             \
                 CapTouch_dsRam.wdgtEnable[CapTouch_GET_WDGT_STATUS_INDEX(wdId)])
#endif /* (CapTouch_ENABLE == CapTouch_SELF_TEST_EN) */

/*******************************************************************************
* Gets a widget active status from the dsRam.wdgtStatus[] register
*******************************************************************************/
#define CapTouch_GET_WIDGET_ACTIVE_STATUS(wdId)                    \
            (CapTouch_GET_WDGT_STATUS_MASK(wdId) &                 \
             CapTouch_dsRam.wdgtStatus[CapTouch_GET_WDGT_STATUS_INDEX(wdId)])

/*******************************************************************************
* Declares Flash and RAM Data Structure variables
*******************************************************************************/
extern CapTouch_RAM_STRUCT            CapTouch_dsRam;
extern const CapTouch_FLASH_STRUCT    CapTouch_dsFlash;
extern const CapTouch_FLASH_IO_STRUCT CapTouch_ioList[CapTouch_TOTAL_ELECTRODES];
extern const CapTouch_RAM_WD_LIST_STRUCT CapTouch_ramWidgetInit;
extern const uint8 CapTouch_ramIdacInit[CapTouch_TOTAL_SENSORS];






/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/*******************************************************************************
* High-level API
*******************************************************************************/

/**
* \cond (SECTION_C_HIGH_LEVEL || SECTION_I_HIGH_LEVEL)
* \addtogroup group_c_high_level
* \{
*/

uint32 CapTouch_IsAnyWidgetActive(void);
uint32 CapTouch_IsWidgetActive(uint32 widgetId);
uint32 CapTouch_IsSensorActive(uint32 widgetId, uint32 sensorId);

#if (0u != CapTouch_PROXIMITY_WIDGET_EN)
    uint32 CapTouch_IsProximitySensorActive(uint32 widgetId, uint32 proxId);
#endif /* #if (0u != CapTouch_PROXIMITY_WIDGET_EN) */

/** \}
* \endcond 
* \cond (SECTION_C_HIGH_LEVEL)
* \addtogroup group_c_high_level
* \{
*/
#if (0u != CapTouch_MATRIX_WIDGET_EN)
    uint32 CapTouch_IsMatrixButtonsActive(uint32 widgetId);
#endif /* #if (0u != CapTouch_MATRIX_WIDGET_EN) */

#if (0u != CapTouch_SLIDER_WIDGET_EN)
    uint32 CapTouch_GetCentroidPos(uint32 widgetId);
#endif /* #if (0u != CapTouch_SLIDER_WIDGET_EN) */

#if (0u != CapTouch_TOUCHPAD_WIDGET_EN)
    uint32 CapTouch_GetXYCoordinates(uint32 widgetId);
#endif /* #if (0u != CapTouch_TOUCHPAD_WIDGET_EN) */

/** \}
* \endcond */

/*******************************************************************************
* Low level API
*******************************************************************************/

/**
* \cond (SECTION_C_LOW_LEVEL || SECTION_I_LOW_LEVEL)
* \addtogroup group_c_low_level
* \{
*/

cystatus CapTouch_GetParam(uint32 paramId, uint32 *value);
cystatus CapTouch_SetParam(uint32 paramId, uint32 value);

/** \}
* \endcond */

/*******************************************************************************
* Function Prototypes - internal functions
*******************************************************************************/
/**
* \cond SECTION_C_INTERNAL
* \addtogroup group_c_internal
* \{
*/

extern const uint8 CapTouch_extCapMap[CapTouch_TST_EXT_CAPS_NUM];

void CapTouch_DsInitialize(void);
#if (0u != CapTouch_ADC_EN)
    void CapTouch_AdcDsInitialize(void);
#endif /* (0u != CapTouch_ADC_EN) */

/** \}
* \endcond */

#endif /* End CY_SENSE_CapTouch_STRUCTURE_H */


/* [] END OF FILE */
