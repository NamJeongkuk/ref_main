/***************************************************************************//**
* \file CapSense_SelfTest.c
* \version 6.0
*
* \brief
*   This file provides the source code for the Built-In Self-Test library.
*
* \see CapSense v6.0 Datasheet
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

#include "cytypes.h"
#include "CyLib.h"
#include "CapSense_Configuration.h"
#include "CapSense_Structure.h"
#include "CapSense_Sensing.h"
#include "CapSense_SelfTest.h"

/***********************************************************************************************************************
* Local definition
***********************************************************************************************************************/
#define CapSense_TST_CRC_GROUP_EN                       (CapSense_TST_GLOBAL_CRC_EN || \
                                                                 CapSense_TST_WDGT_CRC_EN)

#define CapSense_TST_BASELINE_GROUP_EN                  (CapSense_TST_BSLN_DUPLICATION_EN || \
                                                                 CapSense_TST_BSLN_RAW_OUT_RANGE_EN)

#define CapSense_TST_HW_GROUP_EN                        (CapSense_TST_SHORT_GROUP_EN || \
                                                                 CapSense_TST_MEASUREMENT_GROUP_EN)

#define CapSense_TST_SHORT_GROUP_EN                     (CapSense_TST_SNS_SHORT_EN || \
                                                                 CapSense_TST_SNS2SNS_SHORT_EN)

#define CapSense_TST_MEASUREMENT_GROUP_EN               (CapSense_TST_SNS_CAP_EN || \
                                                                 CapSense_TST_SH_CAP_EN || \
                                                                 CapSense_TST_EXTERNAL_CAP_EN)

#define CapSense_TST_SW_WATCHDOG_CYCLES_NUM             (0x0000FFFFLu)
/***********************************************************************************************************************
* Definition of IO register modes
***********************************************************************************************************************/

/* Port Select HSIOM Register */
#define CapSense_TST_HSIOM_GPIO                         (0Lu)
#define CapSense_TST_HSIOM_CSD_SENSE                    (4Lu)
#define CapSense_TST_HSIOM_CSD_SHIELD                   (5Lu)
#define CapSense_TST_HSIOM_AMUXA                        (6Lu)
#define CapSense_TST_HSIOM_AMUXB                        (7Lu)

/* Port Configuration Register */
#define CapSense_TST_PC_HIGHZA                          (0Lu)
#define CapSense_TST_PC_PULLUP                          (2Lu)
#define CapSense_TST_PC_PULLDOWN                        (3Lu)
#define CapSense_TST_PC_STRONG                          (6Lu)

/* Port Data Register */
#define CapSense_TST_DR_PIN2GND                         (0Lu)
#define CapSense_TST_DR_PIN2VDD                         (1Lu)

/* Defining the drive mode of pins depending on the Inactive sensor connection setting in the Component customizer. */
#if(CapSense_SNS_CONNECTION_GROUND == CapSense_CSD_INACTIVE_SNS_CONNECTION)
    #define CapSense_TST_INACTIVE_SNS_GPIO_DM               (CapSense_TST_PC_STRONG)
#elif(CapSense_SNS_CONNECTION_HIGHZ == CapSense_CSD_INACTIVE_SNS_CONNECTION)
    #define CapSense_TST_INACTIVE_SNS_GPIO_DM               (CapSense_TST_PC_HIGHZA)
#elif(CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)
    #define CapSense_TST_INACTIVE_SNS_GPIO_DM               (CapSense_TST_PC_HIGHZA)
#else
    #error "Unsupported inactive connection for the inactive sensors."
#endif /* (CS_123_SNS_CONNECTION_GROUND == CS_123_CSD_INACTIVE_SNS_CONNECTION) */

/***************************************************************************//**
* \brief Defines Self-test HW Configuration
*
* \detail
* Defines the HW configuration depended on the test type. It includes the
* HW CSD block configuration, clock configuration, IDAC configuration and
* all electrode configuration.
*
*******************************************************************************/
typedef enum
{
    CapSense_BIST_HW_UNDEFINED_E        = 0x00u,
    CapSense_BIST_HW_SNS_SHORT_E        = 0x01u,
    CapSense_BIST_HW_S2S_SHORT_E        = 0x02u,
    CapSense_BIST_HW_SNS_CAP_CSD_E      = 0x03u,
    CapSense_BIST_HW_SNS_CAP_CSX_E      = 0x04u,
    CapSense_BIST_HW_SH_CAP_E           = 0x05u,
    CapSense_BIST_HW_EXT_CAP_E          = 0x06u,
    CapSense_BIST_HW_VDDA_E             = 0x07u
} CapSense_BIST_HW_CONFIG_ENUM;

/***************************************************************************//**
* \brief Defines Self-test IO Configuration
*
* \detail
* Defines the IO configuration depends on the test type.
*
*******************************************************************************/
typedef enum
{
    /*
    * The previous state
    */
    CapSense_BIST_IO_UNDEFINED_E        = 0x00u,
    /*
    * Sensor2Sensor Short
    * Sensor capacitance measurement of CSX widget
    */
    CapSense_BIST_IO_STRONG_E           = 0x01u,
    /*
    * Sensor Short
    * Ext capacitor measurement
    */
    CapSense_BIST_IO_HIGHZA_E           = 0x02u,
    /*
    * Sensor capacitance measurement of CSD widget:
    *     All CSX sensors are set to Strong
    *     All CSD sensors depends on Inactive sensor parameter and could be:
    *       High-Z Analog | Strong | CSD_Shield
    */
    CapSense_BIST_IO_CSD_E              = 0x03u,
    /*
    * Shield capacitance measurement
    *     All CSX sensors is set to Strong
    *     All CSD sensors depends on Inactive sensor parameter and could be:
    *     High-Z Analog | Strong | CSD_Sense
    */
    CapSense_BIST_IO_SHIELD_E           = 0x04u
} CapSense_BIST_IO_CONFIG_ENUM;

/***********************************************************************************************************************
* Local variables
***********************************************************************************************************************/
#if (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN)
static CapSense_BIST_HW_CONFIG_ENUM CapSense_bistHwConfig = CapSense_BIST_HW_UNDEFINED_E;
static CapSense_BIST_IO_CONFIG_ENUM CapSense_bistIoConfig = CapSense_BIST_IO_UNDEFINED_E;
#endif /* (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN) */

#if (CapSense_ENABLE == CapSense_TST_MEASUREMENT_GROUP_EN)
    static volatile uint16 CapSense_bistRawcount;
    static volatile uint32 CapSense_bistConfigCsd = CapSense_BIST_CSD_CONFIG;

    #if (CapSense_ENABLE != CapSense_CSDV2)
        static volatile uint32 CapSense_bistCounterResolution = CapSense_BIST_SCAN_DURATION;
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
#endif /* (CapSense_ENABLE == CapSense_TST_MEASUREMENT_GROUP_EN) */

/***********************************************************************************************************************
* Static function prototypes
***********************************************************************************************************************/

/**

* \cond SECTION_C_INTERNAL
* \addtogroup group_c_internal
* \{
*/

/* CRC group function prototypes */
#if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
    static uint32 CapSense_CheckAllWidgetCRC(void);
#endif

#if (CapSense_ENABLE == CapSense_TST_CRC_GROUP_EN)
    static uint16 CapSense_DsCalculateCrc16(uint8 *ptrData, uint32 len);
#endif

/* Baseline group function prototypes */
#if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
    static uint32 CapSense_CheckAllBaselineDuplication(void);
#endif

/* Sensor short test function prototypes */
#if (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN)
    static uint32 CapSense_CheckAllSensorShort(void);
    static void CapSense_UpdateTestResultSnsShort(uint32 wdgtId, uint32 snsId);
    static uint32 CapSense_ExecuteSnsShort(uint32 wdgtId, uint32 snsId);
    static uint32 CapSense_ExecuteSnsShortExt(uint32 wdgtId, uint32 ioSnsId);
#endif /*(CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN) */

/* Sensor to sensor test function prototypes */
#if (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN)
    static uint32 CapSense_CheckAllSns2SnsShort(void);
    static void CapSense_UpdateTestResultSns2SnsShort(uint32 wdgtId, uint32 snsId);
    static uint32 CapSense_ExecuteSns2SnsShort(uint32 wdgtId, uint32 snsId);
    static uint32 CapSense_ExecuteSns2SnsShortExt(uint32 wdgtId, uint32 ioSnsId);
#endif /*(CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN) */

/* Short group function prototypes */
#if (CapSense_ENABLE == CapSense_TST_SHORT_GROUP_EN)
    static uint32 CapSense_ReadElectrodeState(CapSense_FLASH_IO_STRUCT const *ioPtr);
#endif /* (CapSense_ENABLE == CapSense_TST_SHORT_GROUP_EN) */

/* Shield capacitance measurement test function prototypes */
#if ((CapSense_ENABLE == CapSense_TST_SH_CAP_EN) && \
     (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))
    static void CapSense_BistConnectShieldAsSensor(void);
#endif /* ((CapSense_ENABLE == CapSense_TST_SH_CAP_EN) && \
           (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */

/* Sensor capacitance measurement test function prototypes */
#if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
    static uint32 CapSense_GetSensorCapacitanceExt(uint32 wdgtId, uint32 ioSnsId);
    static uint32 CapSense_MeasureAllSensorCapacitance(void);
    static void CapSense_BistConnectSensor(uint32 wdgtId, uint32 ioSnsId);
    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #if (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)
            static void CapSense_BistEnableShieldElectrodes(void);
        #endif /* (0u != CapSense_CSD_TOTAL_SHIELD_COUNT) */
        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)
            static void CapSense_BistEnableShieldTank(void);
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) */
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
#endif /*(CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */

/* External capacitor measurement test function prototypes */
#if (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN)
    static uint32 CapSense_MeasureAllExtCapCapacitance(void);
    static uint32 CapSense_MeasureExtCapCapacitance(uint32 extCapId);
    static void CapSense_BistConnectExtCapacitor(uint32 extCapId);
    static void CapSense_BistDisconnectExtCapacitor(uint32 extCapId);
    static void CapSense_GetExtCapCapacitanceExt(uint32 modClkDivider, uint32 iDacVal);
#endif /*(CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN) */

/* Sensor and Shield capacitance measurement test function prototypes */
#if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_SH_CAP_EN))
    static void CapSense_BistDisableHwSnsCap(void);
    static uint32 CapSense_GetCapacitanceExt(void);
    static void CapSense_BistSetInactiveSensorConnection(uint32 wdgtId, uint32 ioSnsId);
    #if (CapSense_ENABLE == CapSense_CSD_EN)
        static void CapSense_BistSetAllInactiveSensorConnection(void);
    #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #if (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)
            static void CapSense_BistDisableShieldElectrodes(void);
        #endif /* (0u != CapSense_CSD_TOTAL_SHIELD_COUNT) */
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
#endif /* ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
           (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)) */

/* Measurement test group function prototypes */
#if (CapSense_ENABLE == CapSense_TST_MEASUREMENT_GROUP_EN)
    /* Capturing/Releasing HW resources by measurement test group */
    static void CapSense_BistEnableHwSnsCap(void);
    /* Scanning specific function */
    static void CapSense_BistCmodPrecharge(void);
    static void CapSense_BistStartSample(void);
    static void CapSense_BistConfigClock(uint32 snsClk);
    static void CapSense_BistSetScanDuration(void);
    static void CapSense_BistSetIdac(uint32 idacValue);
    static void CapSense_BistTriggerScan(void);
    static void CapSense_WaitEndOfScan(uint32 timeout);
    #if (CapSense_DISABLE == CapSense_CSDV2)
        static void CapSense_BistClockRestart(void);
    #endif /* (CapSense_DISABLE == CapSense_CSDV2) */
#endif /*(CapSense_ENABLE == CapSense_TST_MEASUREMENT_GROUP_EN) */

#if (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN)
    /* HW Config Switching function */
    static void CapSense_BistSwitchHwConfig(CapSense_BIST_HW_CONFIG_ENUM config);
    static void CapSense_SwitchAllIOState(CapSense_BIST_IO_CONFIG_ENUM config);
    /* IOs configuration function */
    static void CapSense_SetAllIODrState(uint32 value);
    static void CapSense_SetAllIOPcState(uint32 value);
    static void CapSense_SetElectrodePc(CapSense_FLASH_IO_STRUCT const *ioPtr, uint32 value);
    static void CapSense_SetElectrodeDr(CapSense_FLASH_IO_STRUCT const *ioPtr, uint32 value);
    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
        (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))
        static void CapSense_SetAllIOHsiomState(uint32 value);
    #endif
#endif

#if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) || \
    ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
     (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)))
    static void CapSense_SetElectrodeHsiom(CapSense_FLASH_IO_STRUCT const *ioPtr, uint32 value);
#endif /* ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
           (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) || \
          ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
           (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))) */
#if ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
     (CapSense_ENABLE == CapSense_CSDV2))
    static void CapSense_BistEnableVddaMeasurement(void);
    static uint16 CapSense_BistVddaRunMeasurement(void);
#endif /* ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
           (CapSense_ENABLE == CapSense_CSDV2)) */

/** \}
* \endcond */

/*******************************************************************************
* Function Name: CapSense_RunSelfTest
****************************************************************************//**
*
* \brief
*  Runs built-in self-tests specified by the test enable mask.
*
* \details
*  The function performs various self-tests on all the enabled widgets
*  and sensors in the Component. The required set of tests can be selected
*  using the bit-mask in testEnMask parameter.
*
*  Use CapSense_TST_RUN_SELF_TEST_MASK to execute all the self-tests or
*  any combination of the masks (defined in testEnMask parameter) to specify the
*  test list.
*
*  To execute a single-element test (i.e. for one widget or sensor), the following
*  functions available:
*  - CapSense_CheckGlobalCRC()
*  - CapSense_CheckWidgetCRC()
*  - CapSense_CheckBaselineDuplication()
*  - CapSense_CheckSensorShort()
*  - CapSense_CheckSns2SnsShort()
*  - CapSense_GetSensorCapacitance()
*  - CapSense_GetShieldCapacitance()
*  - CapSense_GetExtCapCapacitance()
*  - CapSense_GetVdda()
*
*  Refer to these functions for detail information on the corresponding test.
*
* \param testEnMask
*  Specifies the tests to be executed. Each bit corresponds to one test. It is
*  possible to launch the function with any combination of the available tests.
*  - CapSense_TST_GLOBAL_CRC       - Verifies the RAM structure CRC of global parameters
*  - CapSense_TST_WDGT_CRC         - Verifies the RAM widget structure CRC for all the widgets
*  - CapSense_TST_BSLN_DUPLICATION - Verifies the baseline consistency of all the sensors (inverse copy)
*  - CapSense_TST_SNS_SHORT        - Checks all the sensors for a short to GND or VDD
*  - CapSense_TST_SNS2SNS_SHORT    - Checks all the sensors for a short to other sensors
*  - CapSense_TST_SNS_CAP          - Measures all the sensors capacitance
*  - CapSense_TST_SH_CAP           - Measures the shield capacitance
*  - CapSense_TST_EXTERNAL_CAP     - Measures the capacitance of the available external capacitors
*  - CapSense_TST_VDDA             - Measures the Vdda voltage
*  - CapSense_TST_RUN_SELF_TEST_MASK - Executes all available tests.
*
* \return
*  Returns a bit-mask with a status of execution of the specified tests:
*  - CY_RET_SUCCESS - All the tests passed.
*  - CapSense_TST_NOT_EXECUTED - The previously triggered scanning is not
*    completed.
*  - CapSense_TST_BAD_PARAM - A non-defined test was requested in the
*    testEnMask parameter.
*  - The bit-mask of the failed tests.
*
*******************************************************************************/
uint32 CapSense_RunSelfTest(uint32 testEnMask)
{
    uint32 result = CYRET_SUCCESS;

    if(CapSense_NOT_BUSY != CapSense_IsBusy())
    {
        result = CapSense_TST_NOT_EXECUTED;
    }
    else
    {
        if (0u == (testEnMask & (~CapSense_TST_RUN_SELF_TEST_MASK)))
        {
            #if (CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN)
                if (0u != (CapSense_TST_GLOBAL_CRC & testEnMask))
                {
                    result |= CapSense_CheckGlobalCRC();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN) */

            #if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
                if (0u != (CapSense_TST_WDGT_CRC & testEnMask))
                {
                    result |= CapSense_CheckAllWidgetCRC();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */

            #if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
                if (0u != (CapSense_TST_BSLN_DUPLICATION & testEnMask))
                {
                    result |= CapSense_CheckAllBaselineDuplication();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */

            /* All the next tests are hardware dependent, so requesting to switch the sense method */
            CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);

            #if (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN)
                if (0u != (CapSense_TST_SNS_SHORT & testEnMask))
                {
                    result |= CapSense_CheckAllSensorShort();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN) */

            #if (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN)
                if (0u != (CapSense_TST_SNS2SNS_SHORT & testEnMask))
                {
                    result |= CapSense_CheckAllSns2SnsShort();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN) */

            #if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
                if (0u != (CapSense_TST_SNS_CAP & testEnMask))
                {
                    result |= CapSense_MeasureAllSensorCapacitance();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */

            #if (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)
                if (0u != (CapSense_TST_SH_CAP & testEnMask))
                {
                    (void)CapSense_GetShieldCapacitance();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) */

            #if (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN)
                if (0u != (CapSense_TST_EXTERNAL_CAP & testEnMask))
                {
                    result |= CapSense_MeasureAllExtCapCapacitance();
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN) */

            #if ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
                 (CapSense_ENABLE == CapSense_CSDV2))
                if (0u != (CapSense_TST_VDDA & testEnMask))
                {
                    result |= CapSense_GetVdda();
                }
            #endif /* ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
                       (CapSense_ENABLE == CapSense_CSDV2)) */
        }
        else
        {
            result |= CapSense_TST_BAD_PARAM;
        }
    }

    return result;
}

#if (CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN)

/*******************************************************************************
* Function Name: CapSense_CheckGlobalCRC
****************************************************************************//**
*
* \brief
*  Checks the stored CRC of the CapSense_RAM_STRUCT data structure.
*
* \details
*  This function validates the data integrity of the
*  CapSense_RAM_STRUCT data structure by calculating the CRC and
*  comparing it with the stored CRC value (i.e. CapSense_GLB_CRC_VALUE).
*
*  If the stored and calculated CRC values differ, the calculated CRC
*  is stored to the CapSense_GLB_CRC_CALC_VALUE register and
*  the CapSense_TST_GLOBAL_CRC bit is set in the
*  CapSense_TEST_RESULT_MASK_VALUE register. The function never
*  clears the CapSense_TST_GLOBAL_CRC bit.
*
*  It is recommended to use the CapSense_SetParam() function to
*  change a value of CapSense_RAM_STRUCT data structure register/elements
*  as CRC is updated by the CapSense_SetParam() function.
*
*  This test also can be initiated by using CapSense_RunSelfTest()
*  function with the CapSense_TST_GLOBAL_CRC mask input.
*
* \return
*  Returns a status of the executed test:
*  - CY_RET_SUCCESS - The stored CRC matches the calculated CRC
*  - CapSense_TST_GLOBAL_CRC - The stored CRC is wrong.
*
*******************************************************************************/
uint32 CapSense_CheckGlobalCRC(void)
{
    uint32 result = CapSense_TST_GLOBAL_CRC;
    uint16 crcValue = CapSense_DsCalculateCrc16((uint8 *)&CapSense_dsRam +
                                    CapSense_GLOBAL_CRC_AREA_START,
                                    CapSense_GLOBAL_CRC_AREA_SIZE);
    if (CapSense_dsRam.glbCrc == crcValue)
    {
        result = CYRET_SUCCESS;
    }
    else
    {
        CapSense_dsRam.selfTest.glbCrcCalc = crcValue;
        CapSense_dsRam.selfTest.testResultMask |= CapSense_TST_GLOBAL_CRC;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_DsUpdateGlobalCrc
****************************************************************************//**
*
* \brief
*   Recalculates the CRC for the global Component data (certain registers)
*   and writes it to the Data Structure.
*
* \details
*  Uses the function CapSense_DsCalculateCrc16.
*
*******************************************************************************/
void CapSense_DsUpdateGlobalCrc(void)
{
    CapSense_dsRam.glbCrc = CapSense_DsCalculateCrc16((uint8 *)&CapSense_dsRam +
                                                        CapSense_GLOBAL_CRC_AREA_START,
                                                        CapSense_GLOBAL_CRC_AREA_SIZE);
}
#endif /*(CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN) */

#if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)

/*******************************************************************************
* Function Name: CapSense_CheckWidgetCRC
****************************************************************************//**
*
* \brief
*  Checks the stored CRC of the CapSense_RAM_WD_BASE_STRUCT data
*  structure of the specified widget.
*
* \details
*  This function validates the data integrity of the
*  CapSense_RAM_WD_BASE_STRUCT data structure of the specified widget
*  by calculating the CRC and comparing it with the stored CRC value
*  (i.e. CapSense_<WidgetName>_CRC_VALUE).
*
*  If the stored and calculated CRC values differ:
*  1. The calculated CRC is stored to the CapSense_WDGT_CRC_CALC_VALUE register
*  2. The widget ID is stored to the CapSense_WDGT_CRC_ID_VALUE register
*  3. The CapSense_TST_WDGT_CRC bit is set in the
*     CapSense_TEST_RESULT_MASK_VALUE register.
*
*  The function never clears the CapSense_TST_WDGT_CRC bit.
*  If the CapSense_TST_WDGT_CRC bit is set, the CapSense_WDGT_CRC_CALC_VALUE
*  and CapSense_WDGT_CRC_ID_VALUE registers are not updated.
*
*  It is recommended to use the CapSense_SetParam() function to change
*  a value of CapSense_RAM_WD_BASE_STRUCT data structure register/elements
*  as the CRC is updated by CapSense_SetParam() function.
*
*  This test can be initiated by CapSense_RunSelfTest() function with
*  the CapSense_TST_WDGT_CRC mask as an input.
*
*  The function updates the wdgtWorking register
*  CapSense_WDGT_WORKING<Number>_VALUE by clearing the widget-corresponding bit.
*  Those non-working widgets are skipped by the high-level API.
*  Restoring a widget to its working state should be done by the application level.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*  A macro for the widget ID can be found in the CapSense Configuration
*  header file defined as CapSense_<WidgetName>_WDGT_ID.
*
* \return
*  Returns a status of the test execution:
*  - CY_RET_SUCCESS - The stored CRC matches the calculated CRC.
*  - CapSense_TST_WDGT_CRC - The widget CRC is wrong.
*  - CapSense_TST_BAD_PARAM - The input parameter is invalid.
*
*******************************************************************************/
uint32 CapSense_CheckWidgetCRC(uint32 widgetId)
{
    uint32 result = CapSense_TST_WDGT_CRC;
    uint16 crcValue = 0u;
    CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

    if (CapSense_TOTAL_WIDGETS > widgetId)
    {
        /* Get pointer to RAM object data */
        ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;
        crcValue = CapSense_DsCalculateCrc16((uint8 *)ptrWdgt +
                                    CapSense_WIDGET_CRC_AREA_START,
                                    CapSense_WIDGET_CRC_AREA_SIZE);
        if ((ptrWdgt->crc) != crcValue)
        {
            /* Write to self-test data structure wdgtId of the first bad tested widget */
            if (0Lu == (CapSense_dsRam.selfTest.testResultMask &
                       CapSense_TST_WDGT_CRC))
            {
                CapSense_dsRam.selfTest.wdgtCrcCalc = crcValue;
                CapSense_dsRam.selfTest.wdgtCrcId = (uint8)widgetId;
                CapSense_dsRam.selfTest.testResultMask |= CapSense_TST_WDGT_CRC;
            }
            else
            {
            }
            CapSense_dsRam.wdgtWorking[CapSense_GET_WDGT_STATUS_INDEX(widgetId)] &=
                                             ~(CapSense_GET_WDGT_STATUS_MASK(widgetId));
        }
        else
        {
            result = CYRET_SUCCESS;
        }
    }
    else
    {
        result = CapSense_TST_BAD_PARAM;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_CheckAllWidgetCRC
****************************************************************************//**
*
* \brief
*  The internal fuction that checks CRC of all widget structures.
*
* \details
*  The function calculates CRC of all widget structures and compare it
*  to the stored CRCs.
*  In case of failed comparison the function updates
*  testResultMask and returns the status.
*  The function use CapSense_CheckWidgetCRC.
*
* \return
*  Returns the status of the test processing:
*  - CYRET_SUCCESS if all widget CRC are OK;
*  - CapSense_TST_WDGT_CRC if any widget CRC is wrong.
*  - CapSense_TST_NOT_EXECUTED if any widget is unavailable.
*
*******************************************************************************/
static uint32 CapSense_CheckAllWidgetCRC(void)
{
    uint32 result = CYRET_SUCCESS;
    uint32 wdgtIndex = 0u;

    for (wdgtIndex = 0u;
         wdgtIndex < CapSense_TOTAL_WIDGETS;
         wdgtIndex++)
    {
        if (0u != (CapSense_CheckWidgetCRC(wdgtIndex)))
        {
            result |= CapSense_TST_WDGT_CRC;
        }
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_DsUpdateWidgetCrc
****************************************************************************//**
*
* \brief
*   Recalculates the CRC for the specified widgetId and
*   writes it to the Data Structure.
*
* \details
*  The maximum size of the RAM Widget Object is 192 bits. So, the best suitable
*  CRC polynomial is 0xAC9A that has hamming distance 5 for the data up to 241 bits.
*
*  Reference:  "P. Koopman, T. Chakravarthy,
*  "Cyclic Redundancy Code (CRC) Polynomial Selection for Embedded Networks",
*  The International Conference on Dependable Systems and Networks, DSN-2004"
*
* \param widgetId  The widget index number.
*
*******************************************************************************/
void CapSense_DsUpdateWidgetCrc(uint32 widgetId)
{
    uint32 len;
    uint8 *ptrData;
    CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

    if (widgetId < CapSense_TOTAL_WIDGETS)
    {
        /* Get pointer to RAM object data */
        ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;
        ptrData = (uint8 *)CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;
        len = (uint32)CapSense_WIDGET_CRC_AREA_SIZE;

        /* Skip CRC in RAM object structure for CRC calculation */
        ptrData += CapSense_WIDGET_CRC_AREA_START;

        ptrWdgt->crc = CapSense_DsCalculateCrc16(ptrData, len);
    }
}
#endif /* (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */

#if (CapSense_ENABLE == CapSense_TST_CRC_GROUP_EN)

/*******************************************************************************
* Function Name: DsCalculateCrc16
****************************************************************************//**
*
* \brief
*   Calculates CRC for the specified buffer and length. CRC Poly: 0xAC9A
*
* \details
*  This API is used for the CRC protection of the RAM Widget Object.
*  The maximum size of the RAM Widget Object is 192 bits. So, the best suitable
*  CRC polynomial is 0xAC9A. It has a Hamming distance 5 for data words up to 241 bits.
*
*  Reference:  "P. Koopman, T. Chakravarthy,
*  "Cyclic Redundancy Code (CRC) Polynomial Selection for Embedded Networks",
*  The International Conference on Dependable Systems and Networks, DSN-2004"
*
* \param ptrData The pointer to the data.
* \param len     The length of the data in bytes.
*
* \return A calculated CRC-16 value.
*
*******************************************************************************/
static uint16 CapSense_DsCalculateCrc16(uint8 *ptrData, uint32 len)
{
    uint32 idx;
    uint32 actualCrc = 0Lu;
    const uint16 crcTable[] =
    {
        0x0000u, 0xAC9Au, 0xF5AEu, 0x5934u, 0x47C6u, 0xEB5Cu, 0xB268u, 0x1EF2u,
        0x8F8Cu, 0x2316u, 0x7A22u, 0xD6B8u, 0xC84Au, 0x64D0u, 0x3DE4u, 0x917Eu
    };

    for (;len-- > 0u;)
    {
        /* Process HI Nibble */
        idx = ((actualCrc >> 12u) ^ (((uint32)*ptrData) >> 4u)) & 0xFLu;
        actualCrc = crcTable[idx] ^ (actualCrc << 4u);

        /* Process LO Nibble */
        idx = ((actualCrc >> 12u) ^ (uint32)*ptrData) & 0xFLu;
        actualCrc = crcTable[idx] ^ (actualCrc << 4u);

        ptrData++;
    }

    return (uint16)actualCrc;
}
#endif /* (CapSense_ENABLE == CapSense_TST_CRC_GROUP_EN) */

#if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
/*******************************************************************************
* Function Name: CapSense_CheckBaselineDuplication
****************************************************************************//**
*
* \brief
*  Checks that the baseline of the specified widget/sensor is not corrupted by
*  comparing it with a baseline inverse copy.
*
* \details
*  This function validates the integrity of baseline of sensor by comparing
*  the conformity of the baseline and its inversion.
*
*  If the baseline does not match its inverse copy:
*  1. The widget ID is stored to the CapSense_INV_BSLN_WDGT_ID_VALUE register
*  2. The sensor ID is stored to the CapSense_INV_BSLN_SNS_ID_VALUE register
*  3. The CapSense_TST_BSLN_DUPLICATION bit is set in the
*     CapSense_TEST_RESULT_MASK_VALUE register.
*
*  The function never clears the CapSense_TST_BSLN_DUPLICATION bit.
*  If the CapSense_TST_BSLN_DUPLICATION bit is set, the CapSense_INV_BSLN_WDGT_ID_VALUE
*  and CapSense_INV_BSLN_SNS_ID_VALUE registers are not updated.
*
*  It is possible to execute a test for all the widgets using CapSense_RunSelfTest() function
*  with the CapSense_TST_BSLN_DUPLICATION mask.
*  In this case, the CapSense_INV_BSLN_WDGT_ID_VALUE and
*  CapSense_INV_BSLN_SNS_ID_VALUE registers contain the widget and
*  sensor ID of the first detected fail.
*
*  The function updates the wdgtWorking register CapSense_WDGT_WORKING<Number>_VALUE
*  by clearing the widget-corresponding bit. Those non-working widgets are skipped by
*  the high-level API. Restoring a widget to its working state should be done by the application level.
*
*  The test is integrated into the CapSense Component. All CapSense
*  processing functions like CapSense_ProcessAllWidgets()
*  or CapSense_UpdateSensorBaseline() automatically verify the baseline
*  value before using it and update its inverse copy after processing.
*  If fail is detected during a baseline update a CYRET_BAD_DATA result
*  is returned. The baseline initialization functions do not verify the
*  baseline and update the baseline inverse copy.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget.
*  A macro for the sensor ID within the specified widget can be found in
*  the CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*
* \return
*  Returns the status of the test execution:
*  - CY_RET_SUCCESS - The baseline matches its inverse copy.
*  - CapSense_TST_BSLN_DUPLICATION - The test failed.
*  - CapSense_TST_BAD_PARAM - The input parameters are invalid.
*
*******************************************************************************/
uint32 CapSense_CheckBaselineDuplication(uint32 widgetId, uint32 sensorId)
{
    uint32 result = CYRET_SUCCESS;
    uint32 freqChannel;
    CapSense_RAM_SNS_STRUCT *ptrSensor = NULL;
    CapSense_FLASH_WD_STRUCT const *ptrWidget = NULL;

    if ((CapSense_TOTAL_WIDGETS > widgetId) && ((CapSense_GET_SENSOR_COUNT(widgetId)) > sensorId))
    {
        ptrWidget = &CapSense_dsFlash.wdgtArray[widgetId];
        /* Find pointer to specified sensor object */
        ptrSensor = ptrWidget->ptr2SnsRam;
        ptrSensor = &ptrSensor[sensorId];
        /* Check baseline inversion of every channel in sensor */
        for(freqChannel = 0u; freqChannel < CapSense_NUM_SCAN_FREQS; freqChannel++)
        {
            if (ptrSensor->bsln[freqChannel] != ((uint16) ~(ptrSensor->bslnInv[freqChannel])))
            {
                CapSense_UpdateTestResultBaselineDuplication(widgetId, sensorId);
                result |= CapSense_TST_BSLN_DUPLICATION;
            }
        }
    }
    else
    {
        result |= CapSense_TST_BAD_PARAM;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_CheckAllBaselineDuplication
****************************************************************************//**
*
* \brief
*  The internal fuction that checks baseline duplication of all the sensors.
*
* \details
*  The function checks baseline duplication of all the sensors by using
*  CapSense_CheckBaselineDuplication() function.
*
* \return
*  Returns the status of the test processing:
*    - CYRET_SUCCESS if the test is successfully completed;
*    - CapSense_TST_BSLN_DUPLICATION if any sensor baseline duplication
       is wrong;
*******************************************************************************/
static uint32 CapSense_CheckAllBaselineDuplication(void)
{
    uint32 result = CYRET_SUCCESS;
    uint32 wdgtIndex;
    uint32 snsIndex;

    for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
    {
        for (snsIndex = 0u; snsIndex < (CapSense_GET_SENSOR_COUNT(wdgtIndex)); snsIndex++)
        {
            if (CYRET_SUCCESS != CapSense_CheckBaselineDuplication(wdgtIndex, snsIndex))
            {
                result |= CapSense_TST_BSLN_DUPLICATION;
                break;
            }
        }
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_UpdateTestResultBaselineDuplication
****************************************************************************//**
*
* \brief
*  The internal fuction that updates SefTest data structure and
*  widgetWorking register.
*
* \details
*  The function resets a corresponding bit in widgetWorking register,
*  checks TestResultMask for CapSense_TST_BSLN_DUPLICATION
*  bit and if it was not setted to 1 sets it and memorizes wdgtId and snsId
*  in registers invBslnWdgtId and invBslnSnsId of the SefTest data structure.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget which
*  will be processed.
*
*******************************************************************************/
void CapSense_UpdateTestResultBaselineDuplication(uint32 widgetId, uint32 sensorId)
{
    CapSense_dsRam.wdgtWorking[CapSense_GET_WDGT_STATUS_INDEX(widgetId)] &=
                                     ~(CapSense_GET_WDGT_STATUS_MASK(widgetId));
    /* Write to self-test data structure wdgtId and snsId of the first sensor with a damaged baseline inversion */
    if (0Lu == (CapSense_dsRam.selfTest.testResultMask &
                CapSense_TST_BSLN_DUPLICATION))
    {
        CapSense_dsRam.selfTest.testResultMask |= CapSense_TST_BSLN_DUPLICATION;
        CapSense_dsRam.selfTest.invBslnWdgtId = (uint8)widgetId;
        CapSense_dsRam.selfTest.invBslnSnsId = (uint8)sensorId;
    }
}
#endif /*(CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)*/

#if (CapSense_ENABLE == CapSense_TST_BSLN_RAW_OUT_RANGE_EN)

/*******************************************************************************
* Function Name: CapSense_CheckBaselineRawcountRange
****************************************************************************//**
*
* \brief
*  Checks that raw count and baseline of the specified widget/sensor are
*  within the specified range.
*
* \details
*  The baseline and raw count shall be within specific range (based on
*  calibration target) for good units. The function checks whether or not
*  the baseline and raw count are within the limits defined by the user in the
*  ranges function argument. If baseline or raw count are out of limits this
*  function sets the CapSense_TST_BSLN_RAW_OUT_RANGE bit in the
*  CapSense_TEST_RESULT_MASK_VALUE register.
*
*  Unlike other tests, this test does not update
*  CapSense_WDGT_WORKING<Number>_VALUE register and is not available in
*  the CapSense_RunSelfTest() function.
*
*  Use this function to verify the uniformity of sensors, for example, at
*  mass-production or during an operation phase.
*
* \param widgetId
*  Specifies the ID number of the widget.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget.
*  A macro for the sensor ID within the specified widget can be found in
*  the CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*
* \param *ranges
*  Specifies the pointer to the CapSense_BSLN_RAW_RANGE_STRUCT structure
*  with valid ranges for the raw count and baseline.
*
* \return
*  Returns a status of the test execution:
*  - CY_RET_SUCCESS - The raw count and baseline are within the specified range
*  - CapSense_TST_BSLN_RAW_OUT_RANGE - The test failed and baseline or
     raw count or both are out of the specified limit.
*  - CapSense_TST_BAD_PARAM - The input parameters are invalid.
*
*******************************************************************************/
uint32 CapSense_CheckBaselineRawcountRange(uint32 widgetId, uint32 sensorId,CapSense_BSLN_RAW_RANGE_STRUCT *ranges)
{
    uint32 result = CYRET_SUCCESS;
    uint32 freqChannel;
    CapSense_RAM_SNS_STRUCT *ptrSensor = NULL;
    CapSense_FLASH_WD_STRUCT const *ptrWidget = NULL;


    if ((CapSense_TOTAL_WIDGETS > widgetId) &&
       ((CapSense_GET_SENSOR_COUNT(widgetId)) > sensorId))
    {
        ptrWidget = &CapSense_dsFlash.wdgtArray[widgetId];
        /* Find pointer to specified sensor object */
        ptrSensor = ptrWidget->ptr2SnsRam;
        ptrSensor = &ptrSensor[sensorId];

        #if(CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
        /* Check baseline and rawcont of every channel in sensor */
            for(freqChannel = 0u;
                freqChannel < CapSense_NUM_SCAN_FREQS;
                freqChannel++)
            {
        #else
            freqChannel = 0u;
        #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

                if ((ptrSensor->bsln[freqChannel] > ranges->bslnHiLim) ||
                    (ptrSensor->bsln[freqChannel] < ranges->bslnLoLim) ||
                    (ptrSensor->raw[freqChannel]  > ranges->rawHiLim)  ||
                    (ptrSensor->raw[freqChannel]  < ranges->rawLoLim))
                {
                    CapSense_dsRam.selfTest.testResultMask |= CapSense_TST_BSLN_RAW_OUT_RANGE;
                    result = CapSense_TST_BSLN_RAW_OUT_RANGE;
                }

        #if(CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
            }
        #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

    }
    else
    {
        result |= CapSense_TST_BAD_PARAM;
    }
    return result;
}
#endif /* (CapSense_ENABLE == CapSense_TST_BSLN_RAW_OUT_RANGE_EN) */

#if (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN)

/*******************************************************************************
* Function Name: CapSense_CheckSensorShort
****************************************************************************//**
*
* \brief
*  Checks the specified widget/sensor for shorts to GND or VDD.
*
* \details
*  The function performs test to check for a short to GND or VDD on
*  the specified sensor. The resistance of electrical short must be
*  less than 1100 Ohm including series resistors on sensor for the short to
*  be detected. The CapSense_GetSensorCapacitance() function can be
*  used to check an electrical short with resistance higher than 1100 Ohm or
*  when the specified ganged sensor consists of two or more electrodes.
*
*  This function performs the following tasks:
*  - If a short is detected, the widget ID is stored to the
*    CapSense_SHORTED_WDGT_ID_VALUE register.
*  - If a short is detected, the sensor ID is stored to the
*    CapSense_SHORTED_SNS_ID_VALUE register.
*  - If a short is detected, the CapSense_TST_SNS_SHORT bit is
*    set in the CapSense_TEST_RESULT_MASK_VALUE register.
*  - If a short is detected, the bit corresponding to the specified widget
*    (CapSense_WDGT_WORKING<Number>_VALUE) is cleared in the
*    wdgtWorking register to indicate fault with the widget. Once the bit
*    is cleared, the widget is treated as non-working widget by the
*    high-level functions and further processing are skipped. Restoring
*    the bit corresponding the widget can be done by application layer
*    to restore the operation of the high-level functions.
*  - If CapSense_TST_SNS_SHORT is already set due to previously
*    detected fault on any of the sensor, the CapSense_TST_SNS_SHORT
*    register is not cleared by this function and
*    CapSense_SHORTED_WDGT_ID_VALUE and
*    CapSense_SHORTED_SNS_ID_VALUE registers are not updated.
*    For this reason, remember to read details of defective sensor and clear
*    CapSense_TST_SNS_SHORT prior to calling this function on the
*    same or different sensor.
*
*  This function performs the test on one specific sensor. The
*  CapSense_RunSelfTest() function with the
*  CapSense_TST_SNS_SHORT mask, performs the short test on all
*  the widgets and sensors in the Component. In this case,
*  CapSense_SHORTED_WDGT_ID_VALUE and
*  CapSense_SHORTED_SNS_ID_VALUE registers stores the widget and sensor
*  ID of the first faulty sensor.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget.
*  A macro for the sensor ID within the specified widget can be found in
*  the CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*
* \return
*  Returns a status of the test execution:
*  - CY_RET_SUCCESS - The sensor of the widget does not have a short to VDD or
*    GND and is in working condition.
*  - CapSense_TST_SNS_SHORT - A short is detected on the specified sensor.
*  - CapSense_TST_BAD_PARAM - The input parameters are invalid.
*
*******************************************************************************/
uint32 CapSense_CheckSensorShort(uint32 widgetId, uint32 sensorId)
{
    uint32 result = CYRET_SUCCESS;

    if ((CapSense_TOTAL_WIDGETS > widgetId) && ((CapSense_GET_SENSOR_COUNT(widgetId)) > sensorId))
    {
        /* Release previously captured HW resources by other mode and capture them for BIST */
        CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);
        /* Switch HW resource configuration to sensor short test */
        CapSense_BistSwitchHwConfig(CapSense_BIST_HW_SNS_SHORT_E);
        /* Execute the test */
        result = CapSense_ExecuteSnsShort(widgetId, sensorId);
    }
    else
    {
        result = CapSense_TST_BAD_PARAM;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_CheckAllSensorShort
****************************************************************************//**
*
* \brief
*  The internal fuction that checks all the sensors short to GND or VDD.
*
* \details
*  The fuction that checks for shorts on Vdd/GND of all sensor (not
*  electrode) by using CapSense_ExecuteSnsShort. The function returns
*  the status and updates testResultMask and  wdgtWorking register
*  in a case of any shorts detection.
*
* \return
*  Returns the status of the test processing:
*  - CYRET_SUCCESS if test passed successfully;
*  - CapSense_TST_SNS_SHORT if any sensor of any widget is
*    shorted to Vdd or GND.
*
*******************************************************************************/
static uint32 CapSense_CheckAllSensorShort(void)
{
    uint32 wdgtIndex;
    uint32 snsIndex;
    uint32 result = CYRET_SUCCESS;
    uint32 resultWidget = CYRET_SUCCESS;

    /* Releasing of previously captured HW resources by other mode was done in RunSelfTest function */
    /* Switch HW resource configuration to sensor short test */
    CapSense_BistSwitchHwConfig(CapSense_BIST_HW_SNS_SHORT_E);
    /* Execute the test */
    for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
    {
        resultWidget = CYRET_SUCCESS;
        for (snsIndex = 0u; snsIndex < (CapSense_GET_SENSOR_COUNT(wdgtIndex)); snsIndex++)
        {
            resultWidget = CapSense_ExecuteSnsShort(wdgtIndex, snsIndex);
            if (CYRET_SUCCESS != resultWidget)
            {
                break;
            }
        }
        result |= resultWidget;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_UpdateTestResultSnsShort
****************************************************************************//**
* \brief
*  The internal fuction that updates SefTest data structure and
*  widgetWorking register.
*
* \details
*  The function resets a corresponding bit in widgetWorking register,
*  checks TestResultMask for CapSense_TST_SNS_SHORT bit and if
*  it was not setted to 1 sets it and memorizes wdgtId and snsId
*  in corresponding registers of the SefTest data structure.
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param snsId
*  Specifies the ID number of the sensor within the widget which
*  will be processed.
*******************************************************************************/
static void CapSense_UpdateTestResultSnsShort(uint32 wdgtId, uint32 snsId)
{
    CapSense_dsRam.wdgtWorking[CapSense_GET_WDGT_STATUS_INDEX(wdgtId)] &=
                                     ~(CapSense_GET_WDGT_STATUS_MASK(wdgtId));
    if (0Lu == (CapSense_dsRam.selfTest.testResultMask & CapSense_TST_SNS_SHORT))
    {
        /* Write to self-test data structure wdgtId and snsId of the first shorted sensor */
        CapSense_dsRam.selfTest.testResultMask |= CapSense_TST_SNS_SHORT;
        CapSense_dsRam.selfTest.shortedWdgtId = (uint8)wdgtId;
        CapSense_dsRam.selfTest.shortedSnsId = (uint8)snsId;
    }
}

/*******************************************************************************
* Function Name: CapSense_ExecuteSnsShort
****************************************************************************//**
*
* \brief
*  The internal fuction that checks one sensor for shorts on GND/VDD.
*
* \details
*  The function that checks for shorts on GND/VDD of a certain sensor for CSD
*  widgets or of Rx and Tx electrodes of a certain sensor for CSX widgets.
*  The function uses CapSense_ExecuteSnsShortExt.
*  In a case of a shorts detection the function returns failed result.
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param snsId
*  Specifies the ID number of the sensor within the widget to be processed.
*
* \return
*  Returns the status of the test processing:
*  - CYRET_SUCCESS if any sensor's electrode of the widget isn't shorted to VDD or GND;
*  - CapSense_TST_SNS_SHORT if the sensor is shorted to VDD or GND;
*
*******************************************************************************/
static uint32 CapSense_ExecuteSnsShort(uint32 wdgtId, uint32 snsId)
{
    uint32 ioSnsId;
    uint32 result = CYRET_SUCCESS;

    #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
        /* For CSX widgets get an index of Rx electrode, for CSD widgets an electrode index is the same as snsId */
        if (CapSense_SENSE_METHOD_CSX_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtId]))
        {
            ioSnsId = snsId / CapSense_dsFlash.wdgtArray[wdgtId].numRows;
        }
        else
        {
            ioSnsId = snsId;
        }
    #elif (CapSense_ENABLE == CapSense_CSX_EN)
        ioSnsId = snsId / CapSense_dsFlash.wdgtArray[wdgtId].numRows;
    #else
        ioSnsId = snsId;
    #endif

    result = CapSense_ExecuteSnsShortExt(wdgtId, ioSnsId);

    /* For CSX widgets, get an index of a Tx electrode and repeat the test */
    #if (CapSense_ENABLE == CapSense_CSX_EN)
        if (CYRET_SUCCESS == result)
        {
            if (CapSense_SENSE_METHOD_CSX_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtId]))
            {
                ioSnsId = (uint32)(snsId % CapSense_dsFlash.wdgtArray[wdgtId].numRows) +
                    (uint32)CapSense_dsFlash.wdgtArray[wdgtId].numCols;
                result = CapSense_ExecuteSnsShortExt(wdgtId, ioSnsId);
            }
        }
    #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

    if (CYRET_SUCCESS != result)
    {
        CapSense_UpdateTestResultSnsShort(wdgtId, snsId);
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_ExecuteSnsShortExt
****************************************************************************//**
*
* \brief
*  The internal fuction that checks one sensor for shorts on GND/VDD.
*
* \details
*  The function that checks for shorts on GND/VDD of a certain sensor (electrode)
*  for CSD widgets or of Rx or Tx electrode of a certain sensor for CSX widgets.
*  In a case of a shorts detection the function returns failed result.
*  The function proceeds ganged sensors (electrodes).
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param snsId
*  Specifies the ID number of the sensor (electrode) within the widget to be processed.
*
* \return
*  Returns the status of the test processing:
*  - CYRET_SUCCESS if the electrode of the widget isn't shorted to VDD or GND;
*  - CapSense_TST_SNS_SHORT if the sensor or Rx or Tx electrode
*    is shorted to VDD or GND.
*
*******************************************************************************/
static uint32 CapSense_ExecuteSnsShortExt(uint32 wdgtId, uint32 ioSnsId)
{
    uint32 eltdNum;
    uint32 eltdIndex;
    uint32 result = CYRET_SUCCESS;
    CapSense_FLASH_IO_STRUCT const *ioPtr;
    CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;

    /* Check ganged sns flag */
    if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[wdgtId].staticConfig & CapSense_GANGED_SNS_MASK))
    {
        curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash;
        curFlashSnsPtr += ioSnsId;
        ioPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId];
        eltdNum = curFlashSnsPtr->numPins;
    }
    else
    {
        ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash + ioSnsId;
        eltdNum = 1u;
    }

    /* Loop through all electrodes of specified sensor */
    for (eltdIndex = 0u; eltdIndex < eltdNum; eltdIndex++)
    {
        /* Set electrode to pull up drive mode and check that electrode state is logical 1 */
        CapSense_SetElectrodeDr(ioPtr, CapSense_TST_DR_PIN2VDD);
        CapSense_SetElectrodePc(ioPtr, CapSense_TST_PC_PULLUP);
        if (0uL == CapSense_ReadElectrodeState(ioPtr))
        {
            result = CapSense_TST_SNS_SHORT;
        }
        /* Set electrode to pull down drive mode and check that electrode state is logical 0 */
        CapSense_SetElectrodeDr(ioPtr, CapSense_TST_DR_PIN2GND);
        CapSense_SetElectrodePc(ioPtr, CapSense_TST_PC_PULLDOWN);
        if (0uL != CapSense_ReadElectrodeState(ioPtr))
        {
            result = CapSense_TST_SNS_SHORT;
        }
        /* Revert electrode to default drive mode */
        CapSense_SetElectrodePc(ioPtr, CapSense_TST_PC_HIGHZA);

        if (CapSense_TST_SNS_SHORT == result)
        {
            break;
        }

        /* Get the next electrode */
        ioPtr++;
    }
    return result;
}
#endif /* (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN) */

#if (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN)
/*******************************************************************************
* Function Name: CapSense_CheckSns2SnsShort
****************************************************************************//**
*
* \brief
*  Checks the specified widget/sensor for shorts to any other CapSense
*  sensors.
*
* \details
*  The function performs the test on the specified sensor to check for a short
*  to other sensors in the Component. The resistance of an electrical short
*  must be less than 1100 Ohm including the series resistors on the sensor for
*  the short to be detected. The CapSense_GetSensorCapacitance()
*  function can be used to check the electrical short with the resistance
*  higher than 1100 Ohm or when the specified ganged sensor consists of two or
*  more electrodes.
*
*  This function performs the following tasks:
*  - If a short is detected, widget ID is stored to the
*    CapSense_P2P_WDGT_ID_VALUE register.
*  - If a short is detected, sensor ID is stored to the
*    CapSense_P2P_SNS_ID_VALUE register.
*  - If a short is detected, CapSense_TST_SNS2SNS_SHORT bit is set
*    in the CapSense_TEST_RESULT_MASK_VALUE register.
*  - If a short is detected, the bit corresponding to widget
*    (CapSense_WDGT_WORKING<Number>_VALUE) is cleared in the
*    wdgtWorking register to indicate fault with widget. Once the bit is
*    cleared, the widget is treated as a non-working widget by the high-level
*    functions and further processing are skipped. Restoring the bit
*    corresponding to the widget can be done by application layer to restore
*    the operation of high-level functions.
*  - If CapSense_TST_SNS2SNS_SHORT is already set due to the
*    previously detected fault on any of the sensor, the
*    CapSense_TST_SNS2SNS_SHORT register is not cleared by this
*    function and CapSense_P2P_WDGT_ID_VALUE and
*    CapSense_P2P_SNS_ID_VALUE registers are not updated.
*    For this reason, remember to read details of defective sensor and clear
*    CapSense_TST_SNS2SNS_SHORT prior to calling this function on the
*    same or different sensor.
*
*  This function performs the test on one specific sensor.
*  The CapSense_RunSelfTest() function with the
*  CapSense_TST_SNS2SNS_SHORT mask performs the short test on all the
*  widgets and sensors in the Component. In this case,
*  CapSense_P2P_WDGT_ID_VALUE and
*  CapSense_P2P_SNS_ID_VALUE registers store the widget and sensor ID
*  of the first faulty sensor.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget.
*  A macro for the sensor ID within the specified widget can be found in
*  the CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*
* \return
*  Returns a status of the test execution:
*  - CY_RET_SUCCESS - The sensor is not shorted to any other sensor is in
*    working condition.
*  - CapSense_TST_SNS2SNS_SHORT - A short is detected with one or
*    more sensors in the Component.
*  - CapSense_TST_BAD_PARAM - The input parameters are invalid.
*
*******************************************************************************/
uint32 CapSense_CheckSns2SnsShort(uint32 widgetId, uint32 sensorId)
{
    uint32 result = CYRET_SUCCESS;

    if ((CapSense_TOTAL_WIDGETS > widgetId) && ((CapSense_GET_SENSOR_COUNT(widgetId)) > sensorId))
    {
        /* Release previously captured HW resources by other mode and capture them for BIST */
        CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);
        /* Switch HW resource configuration to sensor to sensor short test */
        CapSense_BistSwitchHwConfig(CapSense_BIST_HW_S2S_SHORT_E);
        /* Execute the test */
        result = CapSense_ExecuteSns2SnsShort(widgetId, sensorId);
    }
    else
    {
        result |= CapSense_TST_BAD_PARAM;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_CheckAllSns2SnsShort
****************************************************************************//**
*
* \brief
*  The internal fuction that checks for shorts between all of
*  CapSense sensors.
*
* \details
*  The function checks for all sensor shorts between them by using
*  CapSense_ExecuteSns2SnsShort.
*  The function returns the status and updates testResultMask and
*  wdgtWorking register in a case of any shorts detection.
*
* \return
*  Returns the status of the test processing:
*  - CYRET_SUCCESS - All the sensors of all the widgets aren't shorted to
*    any other sensors.
*  - CapSense_TST_SNS_SHORT - Any sensor of any widget is
*    shorted on any other sensor.
*
*******************************************************************************/
static uint32 CapSense_CheckAllSns2SnsShort(void)
{
    uint32 wdgtIndex;
    uint32 snsIndex;
    uint32 result = CYRET_SUCCESS;
    uint32 resultWidget = CYRET_SUCCESS;

    /* Releasing of previously captured HW resources by other mode was done in RunSelfTest function */
    /* Switch HW resource configuration to sensor to sensor to sensor short test */
    CapSense_BistSwitchHwConfig(CapSense_BIST_HW_S2S_SHORT_E);

    /* Execute the test */
    for (wdgtIndex = 0uL; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
    {
        resultWidget = CYRET_SUCCESS;
        for (snsIndex = 0u; snsIndex < (CapSense_GET_SENSOR_COUNT(wdgtIndex)); snsIndex++)
        {
            resultWidget = CapSense_ExecuteSns2SnsShort(wdgtIndex, snsIndex);
            if (CYRET_SUCCESS != resultWidget)
            {
                break;
            }
        }
        result |= resultWidget;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_UpdateTestResultSns2SnsShort
****************************************************************************//**
* \brief
*  The internal fuction that updates self-test data structure and
*  widgetWorking register.
*
* \details
*  The function resets a corresponding bit in widgetWorking register,
*  checks TestResultMask for CapSense_TST_SNS2SNS_SHORT bit and
*  if it was not setted to 1 sets it and memorizes wdgtId and snsId
*  in corresponding registers of the SefTest data structure.
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param snsId
*  Specifies the ID number of the sensor within the widget which
*  will be processed.
*******************************************************************************/
static void CapSense_UpdateTestResultSns2SnsShort(uint32 wdgtId, uint32 snsId)
{
    CapSense_dsRam.wdgtWorking[CapSense_GET_WDGT_STATUS_INDEX(wdgtId)] &=
                                     ~(CapSense_GET_WDGT_STATUS_MASK(wdgtId));
    /* Write to self-test data structure wdgtId and snsId of the first shorted sensor2sensor */
    if (0uL == (CapSense_dsRam.selfTest.testResultMask & CapSense_TST_SNS2SNS_SHORT))
    {
        CapSense_dsRam.selfTest.testResultMask |= CapSense_TST_SNS2SNS_SHORT;
        CapSense_dsRam.selfTest.p2pWdgtId = (uint8)wdgtId;
        CapSense_dsRam.selfTest.p2pSnsId = (uint8)snsId;
    }
}

/*******************************************************************************
* Function Name: CapSense_ExecuteSns2SnsShort
****************************************************************************//**
*
* \brief
*  The internal fuction that checks one sensor for shorts to another sensor.
*
* \details
*  The function that checks for shorts to another sensor of a certain sensor
*  for CSD widgets or Rx and Tx electrodes of a certain sensor for CSX widgets.
*  The function uses CapSense_ExecuteSnsShortExt.
*  In a case of a shorts detection the function returns failed result.
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param snsId
*  Specifies the ID number of the sensor within the widget to be processed.
*
* \return
*  Returns the status of the test processing:
*  - CYRET_SUCCESS if the electrode of the widget isn't shorted to another sensor;
*  - CapSense_TST_SNS2SNS_SHORT if the sensor is shorted to another sensor.
*
*******************************************************************************/
static uint32 CapSense_ExecuteSns2SnsShort(uint32 wdgtId, uint32 snsId)
{
    uint32 ioSnsId;
    uint32 result = CYRET_SUCCESS;

    #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
        /* For CSX widgets get an index of Rx electrode, for CSD widgets an electrode index is the same as snsId */
        if (CapSense_SENSE_METHOD_CSX_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtId]))
        {
            ioSnsId = snsId / CapSense_dsFlash.wdgtArray[wdgtId].numRows;
        }
        else
        {
            ioSnsId = snsId;
        }
    #elif (CapSense_ENABLE == CapSense_CSX_EN)
        ioSnsId = snsId / CapSense_dsFlash.wdgtArray[wdgtId].numRows;
    #else
        ioSnsId = snsId;
    #endif

    result = CapSense_ExecuteSns2SnsShortExt(wdgtId, ioSnsId);

    /* For CSX widgets get an index of Tx electrode and repeat the test */
    #if (CapSense_ENABLE == CapSense_CSX_EN)
        if (CYRET_SUCCESS == result)
        {
            if (CapSense_SENSE_METHOD_CSX_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtId]))
            {
                ioSnsId = (uint32)(snsId % CapSense_dsFlash.wdgtArray[wdgtId].numRows) +
                    (uint32)CapSense_dsFlash.wdgtArray[wdgtId].numCols;
                result = CapSense_ExecuteSns2SnsShortExt(wdgtId, ioSnsId);
            }
        }
    #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

    if (CYRET_SUCCESS != result)
    {
        CapSense_UpdateTestResultSns2SnsShort(wdgtId, snsId);
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_ExecuteSns2SnsShortExt
****************************************************************************//**
*
* \brief
*  The internal fuction that checks one sensor for shorts to another sensor.
*
* \details
*  The function that checks for shorts to another sensor of a certain sensor(electrode)
*  for CSD widgets or of Rx or Tx electrode of a certain sensor for CSX widgets.
*  In a case of a shorts detection the function returns failed result.
*  The function proceeds ganged sensors(electrodes).
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param snsId
*  Specifies the ID number of the sensor within the widget to be processed.
*
* \return
*  Returns the status of the test processing:
*  - CYRET_SUCCESS if the electrode of the widget isn't shorted to another sensor;
*  - CapSense_TST_SNS2SNS_SHORT if the sensor or Rx or Tx electrode
*    is shorted to another sensor.
*
*******************************************************************************/
static uint32 CapSense_ExecuteSns2SnsShortExt(uint32 wdgtId, uint32 ioSnsId)
{
    uint32 eltdNum;
    uint32 eltdIndex;
    uint32 result = CYRET_SUCCESS;
    CapSense_FLASH_IO_STRUCT const *ioPtr;
    CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;

    /* Check ganged sns flag */
    if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[wdgtId].staticConfig & CapSense_GANGED_SNS_MASK))
    {
        curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash;
        curFlashSnsPtr += ioSnsId;
        ioPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId];
        eltdNum = curFlashSnsPtr->numPins;
    }
    else
    {
        ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash + ioSnsId;
        eltdNum = 1uL;
    }

    /* Loop through all electrodes of specified sensor and set pull up drive mode */
    for (eltdIndex = 0uL; eltdIndex < eltdNum; eltdIndex++)
    {
        /* Set electrode to pull up drive mode and check that electrode state is logical 1 */
        CapSense_SetElectrodeDr(ioPtr + eltdIndex, CapSense_TST_DR_PIN2VDD);
        CapSense_SetElectrodePc(ioPtr + eltdIndex, CapSense_TST_PC_PULLUP);

        if (0uL == CapSense_ReadElectrodeState(ioPtr + eltdIndex))
        {
            result = CapSense_TST_SNS2SNS_SHORT;
            break;
        }

        /* Revert electrode to default drive mode */
        CapSense_SetElectrodeDr(ioPtr + eltdIndex, CapSense_TST_DR_PIN2GND);
        CapSense_SetElectrodePc(ioPtr + eltdIndex, CapSense_TST_PC_STRONG);
    }

    return(result);
}
#endif /* (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN) */

#if (CapSense_ENABLE == CapSense_TST_SHORT_GROUP_EN)

/*******************************************************************************
* Function Name: CapSense_ReadElectrodeState
****************************************************************************//**
*
* \brief
*  The internal function that reds a certain electrode state.
*
* \details
*  The function activates an input buffer for the specified electrode and reads
*  the correspondent input bit. After that it disables the input buffer.
*
* \param *ioPtr
*  A pointer to IO data structure of the specified electrode.
*
* \return
*  Returns the state of the input buffer of the electrode (0 or non 0).
*
*******************************************************************************/
static uint32 CapSense_ReadElectrodeState(CapSense_FLASH_IO_STRUCT const *ioPtr)
{
    uint32 newRegisterValue;
    uint8  interruptState;
    uint32 result;

    /* Enable the input buffer for the specified electrode */
    interruptState = CyEnterCriticalSection();
    newRegisterValue = CY_GET_REG32(ioPtr->pc2Ptr);
    newRegisterValue &= ~(ioPtr->mask);
    CY_SET_REG32(ioPtr->pc2Ptr, newRegisterValue);
    CyExitCriticalSection(interruptState);

    /* Read the bit from the IO pad state register (PS) */
    result = CY_GET_REG32(ioPtr->psPtr) & ioPtr->mask;

    /* Disable the input buffer for this pin (set PC2 bit to logic 1) */
    interruptState = CyEnterCriticalSection();
    newRegisterValue = CY_GET_REG32(ioPtr->pc2Ptr);
    newRegisterValue |= ioPtr->mask;
    CY_SET_REG32(ioPtr->pc2Ptr, newRegisterValue);
    CyExitCriticalSection(interruptState);

    return(result);
}
#endif /* (CapSense_ENABLE == CapSense_TST_SHORT_GROUP_EN) */

#if (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN)
/*******************************************************************************
* Function Name: CapSense_GetExtCapCapacitance
****************************************************************************//**
*
* \brief
*  Measures the capacitance of the specified external capacitor.
*
* \details
*  The function measures the capacitance of the specified external capacitor
*  such as Cmod and returns the result, alternatively the result is stored in
*  the CapSense_EXT_CAP<EXT_CAP_ID>_VALUE register in data structure.
*
*  The measurable capacitance range using this function is
*  from 200pF to 60,000pF with measurement accuracy of 10%.
*
*  This test can be executed for all the external capacitors at once using
*  the CapSense_RunSelfTest() function with the
*  CapSense_TST_EXTERNAL_CAP mask.
*
* \param extCapId
*  Specifies the ID number of the external capacitor to be measured:
*  - CapSense_TST_CMOD_ID - Cmod capacitor
*  - CapSense_TST_CSH_ID - Csh capacitor
*  - CapSense_TST_CINTA_ID - CintA capacitor
*  - CapSense_TST_CINTB_ID - CintB capacitor
*
* \return
*  Returns a status of the test execution:
*  - The capacitance (in pF) of the specified external capacitor
*  - CapSense_TST_BAD_PARAM if the input parameter is invalid.
*
*******************************************************************************/
uint32 CapSense_GetExtCapCapacitance(uint32 extCapId)
{
    uint32 result = CYRET_SUCCESS;

    if (CapSense_TST_EXT_CAPS_NUM > extCapId)
    {
        CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);
        CapSense_BistSwitchHwConfig(CapSense_BIST_HW_EXT_CAP_E);
        CapSense_BistConfigClock((uint32)CapSense_TST_EXT_CAP_RESOLUTION);
        /* Set resolution to fixed value */
        CapSense_BistSetScanDuration();
        result = CapSense_MeasureExtCapCapacitance(extCapId);
    }
    else
    {
        result |= CapSense_TST_BAD_PARAM;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_MeasureExtCapCapacitance
****************************************************************************//**
*
* \brief
*  The internal function that measures a capacitance of an external capacitor
*  when HW block is configured for an external capasitors measuring.
*
* \details
*  The function connect an external capacitor (Cmod, Csh, CintA/CintB)
*  to CSD block, defines a modclock frequency divider and an Idac code
*  and measures a capacitance in range of 0,5 to 5,0 nF.
*  If the result exceeds 75 % of a range high bound the function divides modClk,
*  multiply Idac code to the fixed value to wide a measurement range and performs
*  another measuring cycle for a capacitance in range of 5,0 to 60,0 nF.
*  After that the fuction disconnect the external capacitor, calculate a result
*  and stores it to a correspondent register of the self-test data structure.
*
* \param extCapId
*  An index of the external cap to measure.
*
* \return
*  - Cext of the external capacitor in pF units.
*
*******************************************************************************/
static uint32 CapSense_MeasureExtCapCapacitance(uint32 extCapId)
{
    uint32 modClkDivider;
    uint32 iDacVal;
    uint32 result = 0uL;

    /* Connect an extCap */
    CapSense_BistConnectExtCapacitor(extCapId);
     /* Set modClk and Idac to the fixed value for a fine scan */
    #if (CYDEV_BCLK__HFCLK__MHZ < CapSense_TST_EXT_CAP_MODCLK_MHZ)
        modClkDivider = 1uL;
    #else
        modClkDivider = CYDEV_BCLK__HFCLK__MHZ / CapSense_TST_EXT_CAP_MODCLK_MHZ;
    #endif /* (CYDEV_BCLK__HFCLK__MHZ < CapSense_TST_EXT_CAP_MODCLK_MHZ) */

    iDacVal = (((CapSense_TST_EXT_CAP_LOW_RANGE * CapSense_CSD_VREF_MV * CYDEV_BCLK__HFCLK__MHZ) /
              CapSense_TST_SNS_CAP_IDAC_GAIN) / modClkDivider) + 1uL;
    CapSense_GetExtCapCapacitanceExt(modClkDivider, iDacVal);

    /* For results more then 75 % of the resolution limit perform a second wide range scan */
    if (CapSense_TST_EXT_CAP_RESOLUTION_75 < CapSense_bistRawcount)
    {
         /* Divide modClk and Multiply Idac to the fixed value to wide a measurement range */
        modClkDivider <<= 2u;

        #if (CapSense_ENABLE == CapSense_CSDV2)
            iDacVal <<= 2u;
        #else /* (CapSense_ENABLE == CapSense_CSDV2) */
            iDacVal = CapSense_BIST_MAX_IDAC_VALUE;
        #endif /* (CapSense_ENABLE == CapSense_CSDV2) */

        CapSense_GetExtCapCapacitanceExt(modClkDivider, iDacVal);
    }
    /* Disconnect ext capacitor */
    CapSense_BistDisconnectExtCapacitor(extCapId);
    /* Calculate result in 2 steps to prevent overflow */
    result = (iDacVal * modClkDivider * CapSense_bistRawcount) / CYDEV_BCLK__HFCLK__MHZ;
    result = (result * CapSense_TST_SNS_CAP_IDAC_GAIN) / CapSense_CSD_VREF_MV;
    CapSense_dsRam.selfTest.extCap[extCapId] = (uint16)result;

    return result;
}

/*******************************************************************************
* Function Name: CapSense_GetExtCapCapacitanceExt
****************************************************************************//**
*
* \brief
*  The internal function that performs a scan for an external capacitor measuring.
*
* \details
*  The function sets the modclock and Idac to defined values and performs a scan
*  by using CSD mode for an external capacitor (Cmod, Csh, or CintA/CintB).
*
* \param modClkDivider
*  Specifies a divider for a modulation clock frequency.
*
* \param iDacVal
*  Specifies an Idac modulation code.
*
*******************************************************************************/
static void CapSense_GetExtCapCapacitanceExt(uint32 modClkDivider, uint32 iDacVal)
{
    #if ((CapSense_ENABLE != CapSense_CSDV2) &&\
         (CapSense_ENABLE == CapSense_IS_M0S8PERI_BLOCK))
        CapSense_BistConfigClock(CapSense_TST_EXT_CAP_RESOLUTION * modClkDivider);
    #endif /* ((CapSense_ENABLE != CapSense_CSDV2) &&\
               (CapSense_ENABLE == CapSense_IS_M0S8PERI_BLOCK)) */

    CapSense_SsSetModClkClockDivider(modClkDivider);
    CapSense_BistSetIdac(iDacVal);
    /* Start the scan */
    CapSense_dsRam.status |= CapSense_SW_STS_BUSY;
    CapSense_BistStartSample();
    /* Initialize Watchdog Counter with time interval which is enough to charge 100 nF capacitor */
    CapSense_WaitEndOfScan(CapSense_BIST_MEASURE_WATCHDOG_CYCLES_NUM);
}

/*******************************************************************************
* Function Name: CapSense_BistConnectExtCapacitor
****************************************************************************//**
*
* \brief
*  The internal function that connects an external capacitor to the
*  CapSense block.
*
* \details
*  The function that that connects an external capacitor to the CapSense
*  block by using HSIOM in CSD mode.
*
* \param extCapId
*  An index of the external cap to connect.
*
*******************************************************************************/
static void CapSense_BistConnectExtCapacitor(uint32 extCapId)
{
    uint32 newRegValue;
    uint8  interruptState;

    #if (CapSense_ENABLE == CapSense_CSDV2)
        /* Connect AMUXBUS-A to CSDCOMP(sense path) */
    CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFMA_STATIC_CLOSE);
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */

    switch(CapSense_extCapMap[extCapId])
    {
        #if (CapSense_ENABLE == CapSense_CSD_EN)
            case CapSense_TST_CMOD_MAP:
            {
                /* Connect Cmod pin using HSIOM registers */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_CSD_SENSE << CapSense_CMOD_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);
                break;
            }
                #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                     (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
                    case CapSense_TST_CSH_MAP:
                    {
                        /* Connect CTANK to AMUXBUS-A */
                        interruptState = CyEnterCriticalSection();
                        newRegValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
                        newRegValue &= (uint32)(~(uint32)CapSense_CSH_HSIOM_MASK);
                        newRegValue |= (CapSense_HSIOM_SEL_CSD_SENSE << CapSense_CSH_HSIOM_SHIFT);
                        CY_SET_REG32(CapSense_CSH_HSIOM_PTR, newRegValue);
                        CyExitCriticalSection(interruptState);
                        break;
                    }
                #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                      (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

        #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */

        #if (CapSense_ENABLE == CapSense_CSX_EN)
            case CapSense_TST_CINTA_MAP:
            {
                /* Connect the CintA pin using HSIOM registers */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintA_HSIOM_PTR);
                newRegValue &= ~CapSense_CintA_HSIOM_MASK;
                newRegValue |= (CapSense_HSIOM_SEL_CSD_SENSE << CapSense_CintA_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CintA_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);
                break;
            }
            case CapSense_TST_CINTB_MAP:
            {
                /* Connect CintB to AMUXBUS-A using HSIOM registers */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_HSIOM_PTR);
                newRegValue &= ~CapSense_CintB_HSIOM_MASK;
                newRegValue |= (CapSense_HSIOM_SEL_CSD_SENSE << CapSense_CintB_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CintB_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);
                break;
            }
        #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

        default:
        {
            break;
        }
    }
}

/*******************************************************************************
* Function Name: CapSense_BistDisconnectExtCapacitor
****************************************************************************//**
*
* \brief
*  The internal function that disconnects an external capacitor from the
*  CapSense block.
*
* \details
*  The function that that disconnects an external capacitor from the
*  CapSense block.
*  After that the function dicharges the specified capacitor to GND to provide
*  next regular scans.
*
* \param extCapId
*  An index of the external cap to disconnect.
*
*******************************************************************************/
static void CapSense_BistDisconnectExtCapacitor(uint32 extCapId)
{
    uint32 newRegValue;
    uint8  interruptState;

    #if (CapSense_ENABLE == CapSense_CSDV2)
        /* Disconnect AMUXBUS-A from CSDCOMP(sense path) */
        CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFMA_STATIC_OPEN);
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */

    switch(CapSense_extCapMap[extCapId])
    {
        #if (CapSense_ENABLE == CapSense_CSD_EN)
            case CapSense_TST_CMOD_MAP:
            {
                /* Disconnect Cmod from AMUXBUS-A using HSIOM registers */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
                CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                #if (CapSense_ENABLE == CapSense_CSDV2)
                    /* Disconnect CSDCMP from CSDBUS-A (parallel pass via SFCA switch) */
                    newRegValue = CY_GET_REG32(CapSense_SW_CMP_P_SEL_PTR);
                    newRegValue &= (uint32) (~CapSense_SW_CMP_P_SEL_SW_SFCA_MASK);
                    CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, newRegValue);
                #endif /* (CapSense_ENABLE == CapSense_CSDV2) */

                /* Discharge CMOD by using GPIO mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_GPIO << CapSense_CMOD_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Set output port register for Cmod to 0 to connect Cmod to GND */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_DR_PTR);
                newRegValue &= (uint32)(~(uint32) (1uL << CapSense_CMOD_DR_SHIFT));
                CY_SET_REG32(CapSense_CMOD_DR_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Set port configuration register (drive mode) for Cmod in STRONG mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_PC_PTR);
                newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << CapSense_CMOD_PC_SHIFT));
                newRegValue |= (CapSense_GPIO_STRGDRV << CapSense_CMOD_PC_SHIFT);
                CY_SET_REG32(CapSense_CMOD_PC_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Delay 10 uS to discharge Cmod */
                CyDelayUs(10u);

                /* Set port configuration register (drive mode) for Cmod in HIGHZ-A mode (PC=000) */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_PC_PTR);
                newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << CapSense_CMOD_PC_SHIFT));
                CY_SET_REG32(CapSense_CMOD_PC_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                break;
            }

            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                 (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
                case CapSense_TST_CSH_MAP:
                {
                    /* Disconnect CTANK from AMUXBUS-A */
                    interruptState = CyEnterCriticalSection();
                    newRegValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
                    newRegValue &= (uint32)(~(uint32)CapSense_CSH_HSIOM_MASK);
                    CY_SET_REG32(CapSense_CSH_HSIOM_PTR, newRegValue);
                    CyExitCriticalSection(interruptState);

                    /* Discharge CTANK by using GPIO mode */
                    interruptState = CyEnterCriticalSection();
                    newRegValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
                    newRegValue &= (uint32)(~(uint32)CapSense_CSH_HSIOM_MASK);
                    newRegValue |= (CapSense_HSIOM_SEL_GPIO << CapSense_CSH_HSIOM_SHIFT);
                    CY_SET_REG32(CapSense_CSH_HSIOM_PTR, newRegValue);
                    CyExitCriticalSection(interruptState);

                    /* Set output port register for Cmod to 0 to connect Cmod to GND */
                    interruptState = CyEnterCriticalSection();
                    newRegValue = CY_GET_REG32(CapSense_CSH_DR_PTR);
                    newRegValue &= (uint32)(~(uint32) (1uL << CapSense_CSH_SHIFT));
                    CY_SET_REG32(CapSense_CSH_DR_PTR, newRegValue);
                    CyExitCriticalSection(interruptState);

                    /* Set port configuration register (drive mode) for CTANK in STRONG mode */
                    interruptState = CyEnterCriticalSection();
                    newRegValue = CY_GET_REG32(CapSense_CSH_PC_PTR);
                    newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << CapSense_CSH_PC_SHIFT));
                    newRegValue |= (CapSense_GPIO_STRGDRV << CapSense_CSH_PC_SHIFT);
                    CY_SET_REG32(CapSense_CSH_PC_PTR, newRegValue);
                    CyExitCriticalSection(interruptState);

                    /* Delay 10 uS to discharge CTANK */
                    CyDelayUs(10u);

                    /* Set port configuration register (drive mode) for CTANK in HIGHZ-A mode */
                    interruptState = CyEnterCriticalSection();
                    newRegValue = CY_GET_REG32(CapSense_CSH_PC_PTR);
                    newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << CapSense_CSH_PC_SHIFT));
                    CY_SET_REG32(CapSense_CSH_PC_PTR, newRegValue);
                    CyExitCriticalSection(interruptState);

                    break;
                }
            #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                       (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

        #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */

        #if (CapSense_ENABLE == CapSense_CSX_EN)
            case CapSense_TST_CINTA_MAP:
            {
                /* Disconnect CintA from AMUXBUS-A using HSIOM registers. */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintA_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CintA_HSIOM_MASK);
                CY_SET_REG32(CapSense_CintA_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Discharge CintA by using GPIO mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintA_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CintA_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_GPIO << CapSense_CintA_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CintA_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Set output port register for CintA to 0 to connect CintA to GND */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintA_DR_PTR);
                newRegValue &= (uint32)(~(uint32) (1uL << CapSense_CintA_SHIFT));
                CY_SET_REG32(CapSense_CintA_DR_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Set port configuration register (drive mode) for CintA in STRONG mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintA_PC_PTR);
                newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << (CapSense_CintA_SHIFT * 3uL)));
                newRegValue |= (CapSense_GPIO_STRGDRV << (CapSense_CintA_SHIFT * 3uL));
                CY_SET_REG32(CapSense_CintA_PC_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Delay 10 uS to discharge CintA */
                CyDelayUs(10u);

                /* Set port configuration register (drive mode) for CintA in HIGHZ-A mode (PC=000) */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintA_PC_PTR);
                newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << (CapSense_CintA_SHIFT * 3uL)));
                CY_SET_REG32(CapSense_CintA_PC_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                break;
            }
            case CapSense_TST_CINTB_MAP:
            {
                /* Discharge CintB by using GPIO mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CintB_HSIOM_MASK);
                CY_SET_REG32(CapSense_CintB_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Set a CintB HSIOM register to the GPIO mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CintB_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_GPIO << CapSense_CintB_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CintB_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Set output port register for CintB to 0 to connect CintB to GND */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_DR_PTR);
                newRegValue &= (uint32)(~(uint32) (1uL << CapSense_CintB_SHIFT));
                CY_SET_REG32(CapSense_CintB_DR_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Set port configuration register (drive mode) for CintB in STRONG mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_PC_PTR);
                newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << (CapSense_CintB_SHIFT * 3uL)));
                newRegValue |= (CapSense_GPIO_STRGDRV << (CapSense_CintB_SHIFT * 3uL));
                CY_SET_REG32(CapSense_CintB_PC_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Delay 10 uS to discharge CintB */
                CyDelayUs(10u);

                /* Set port configuration register (drive mode) for CintB in HIGHZ-A mode */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_PC_PTR);
                newRegValue &= (uint32)(~((uint32)CapSense_GPIO_PC_MASK << (CapSense_CintB_SHIFT * 3uL)));
                CY_SET_REG32(CapSense_CintB_PC_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                break;
            }
        #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

        default:
        {
            break;
        }
    }
}

/*******************************************************************************
* Function Name: CapSense_MeasureAllExtCapCapacitance
****************************************************************************//**
*
* \brief
*  The internal function that measures a capacitance of all external
*  capacitors.
*
* \details
*  The function measures capacitances of all external  capacitor (Cmod,
*  Csh, CintA, CintB).
*  The function stores cap values in corresponding registers.
*  The function does not affect on another Component parameters or
*  functionalities.
*
* \return
*  Returns the status of the measuring process:
*  - CYRET_SUCCESS if all measurings is successfull;
*  - CapSense_TST_EXTERNAL_CAP if any measuring is failed.
*
*******************************************************************************/
static uint32 CapSense_MeasureAllExtCapCapacitance(void)
{
    uint32 extCapIndex;

    CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);
    CapSense_BistSwitchHwConfig(CapSense_BIST_HW_EXT_CAP_E);
    CapSense_BistConfigClock(CapSense_TST_EXT_CAP_RESOLUTION);
    /* Set resolution to fixed value) */
    CapSense_BistSetScanDuration();

    for (extCapIndex = 0u; extCapIndex < CapSense_TST_EXT_CAPS_NUM; extCapIndex++)
    {
        (void)CapSense_MeasureExtCapCapacitance(extCapIndex);
    }

    return (CYRET_SUCCESS);
}
#endif /*(CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN) */

/*******************************************************************************
* Function Name: CapSense_BistDisableMode
****************************************************************************//**
*
* \brief
*  Releases captured HW resources.
*
* \details
*  Releases captured HW resources to be used by other modes.
*
*******************************************************************************/
void CapSense_BistDisableMode(void)
{
    #if (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN)
        CapSense_BistSwitchHwConfig(CapSense_BIST_HW_UNDEFINED_E);
        CapSense_bistIoConfig = CapSense_BIST_IO_UNDEFINED_E;
    #endif /* (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN) */
}

/*******************************************************************************
* Function Name: CapSense_BistInitialize
****************************************************************************//**
*
* \brief
*  Initialize the HW resources to Built-in Self-test mode.
*
* \details
*  Preserved the function to be consistent with other modes (CSX or CSD).
*  In practice resource capturing and initialization are performed later when
*  defined which test type should be executed. Depends on the test type HW
*  resources initialized differently. Only disconnection of all IO from AMUX
*  is common for all the test types.
*
*******************************************************************************/
void CapSense_BistInitialize(void)
{
    #if (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN)
        #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
            (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))
            /* Disconnect all the IOs from analog mux busses */
            CapSense_SetAllIOHsiomState(CapSense_TST_HSIOM_GPIO);
        #endif
        /* Set data register to GND that is applicable for all the tests */
        CapSense_SetAllIODrState(CapSense_TST_DR_PIN2GND);
    #endif /* (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN) */
}

#if ((CapSense_ENABLE == CapSense_TST_SH_CAP_EN) && \
     (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))

/*******************************************************************************
* Function Name: CapSense_BistConnectShieldAsSensor
****************************************************************************//**
*
* \brief
*  Connects (a) shield electrode(s) to the HW block via the AMUX bus.
*
* \details
*   The function gets the IO configuration registers addresses, their shifts and
*   masks from the SHIELD_IO_STRUCT object. Basing on this data, it updates
*   the HSIOM and PC registers.
*
*******************************************************************************/
static void CapSense_BistConnectShieldAsSensor(void)
{
    uint8 interruptState;
    uint32 regValue;
    uint32 eltdIndex;
    CapSense_SHIELD_IO_STRUCT const *ioPtr;

    /* Loop through all electrodes of Shield */
    for (eltdIndex = 0u; eltdIndex < CapSense_CSD_TOTAL_SHIELD_COUNT; eltdIndex++)
    {
        ioPtr = &CapSense_shieldIoList[eltdIndex];

        /* Set electrode to High-Z Analog */
        interruptState = CyEnterCriticalSection();
        regValue = CY_GET_REG32(ioPtr->pcPtr);
        regValue &= ~(CapSense_GPIO_PC_MASK << ioPtr->shift);
        CY_SET_REG32(ioPtr->pcPtr, regValue);
        CyExitCriticalSection(interruptState);

        /* Update HSIOM port select register */
        interruptState = CyEnterCriticalSection();
        regValue = CY_GET_REG32(ioPtr->hsiomPtr);
        regValue &= ~(ioPtr->hsiomMask);
        regValue |= (CapSense_TST_HSIOM_CSD_SENSE << ioPtr->hsiomShift);
        CY_SET_REG32(ioPtr->hsiomPtr, regValue);
        CyExitCriticalSection(interruptState);
    }
}
#endif /* ((CapSense_ENABLE == CapSense_TST_SH_CAP_EN) && \
           (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */

#if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
     (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
     (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))

/*******************************************************************************
* Function Name: CapSense_BistEnableShieldElectrodes
****************************************************************************//**
*
* \brief
*  This internal function initializes Shield Electrodes.
*
* \details
*  The function sets the bit in the HSIOM register which enables the shield electrode
*  functionality on the pin. The port and pin configurations are stored in
*  the CapSense_shieldIoList structure.
*
*******************************************************************************/
static void CapSense_BistEnableShieldElectrodes(void)
{
    uint32 newRegValue;
    uint32 shieldIndex;

    for (shieldIndex = 0u; shieldIndex < CapSense_CSD_TOTAL_SHIELD_COUNT; shieldIndex++)
    {
        /* Set drive mode to Analog */
        CY_SET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr, CY_GET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr) &
                   ~(CapSense_GPIO_PC_MASK <<(CapSense_shieldIoList[shieldIndex].shift)));

        /* Set HSIOM register mode to CSD_SHIELD or AMUXBUS B connection */
        newRegValue = CY_GET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr);
        newRegValue &= ~(CapSense_HSIOM_SEL_MASK << (CapSense_shieldIoList[shieldIndex].hsiomShift));

        /* Set HSIOM register mode to CSD_SHIELD */
        newRegValue |= (CapSense_HSIOM_SEL_CSD_SHIELD << (CapSense_shieldIoList[shieldIndex].hsiomShift));

        CY_SET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr, newRegValue);
    }
}
#endif /* ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
           (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
           (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */

#if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
     (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
     (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))

/*******************************************************************************
* Function Name: CapSense_BistEnableShieldTank
****************************************************************************//**
*
* \brief
*   This function configures Csh pin
*
* \details
*   The function updates the CSH_HSIOM register to connect Csh to AMUXBUS-B.
*   For Third-generation HW block it additionally sets Strong drive mode for Csh
*   and applies High level to Csh pin when precharge is set to IO buffer.
*
*******************************************************************************/
static void CapSense_BistEnableShieldTank(void)
{
    uint32 newRegValue;

    /* Update the CSH_HSIOM register to connect Csh to AMUXBUS-B */
    newRegValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
    newRegValue &= (uint32)(~(uint32)CapSense_CSH_HSIOM_MASK);
    newRegValue |= (CapSense_CSH_TO_AMUXBUS_B_MASK << CapSense_CSH_HSIOM_SHIFT);
    CY_SET_REG32(CapSense_CSH_HSIOM_PTR, newRegValue);

    #if ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
         (CapSense_DISABLE == CapSense_CSDV2))
        /* Set the Strong drive mode for Csh */
        newRegValue = CY_GET_REG32(CapSense_CSH_PC_PTR);
        newRegValue &= (uint32)(~(CapSense_CSH_PC_MASK << CapSense_CSH_PC_SHIFT));
        newRegValue |= (CapSense_CSH_PC_STRONG_MODE << CapSense_CSH_PC_SHIFT);
        CY_SET_REG32(CapSense_CSH_PC_PTR, newRegValue);

        /* Apply High level to the Csh pin for Csh */
        newRegValue = CY_GET_REG32(CapSense_CSH_DR_PTR);
        newRegValue &= (uint32)(~(CapSense_DR_MASK << CapSense_CSH_SHIFT));
        newRegValue |= (uint32)CapSense_CSH_DR_CONFIG;
        CY_SET_REG32(CapSense_CSH_DR_PTR, newRegValue);
    #endif /* ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
               (CapSense_DISABLE == CapSense_CSDV2)) */
}
#endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
           (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
           (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

/** \cond SECTION_C_LOW_LEVEL */
#if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)

/*******************************************************************************
* Function Name: CapSense_GetSensorCapacitance
****************************************************************************//**
*
* \brief
*  Measures the specified widget/sensor capacitance.
*
* \details
*  The function measures capacitance of the specified sensor and returns the
*  result, alternatively the result is stored in the Component data structure.
*
*  For CSD sensors, the capacitance of the specified sensor is measured.
*  For CSX sensors, the capacitance the both Rx and Tx electrodes of the sensor
*  is measured. For ganged sensor, total capacitance of all electrodes associated
*  with the sensor is measured. The capacitance measurement result is independent
*  on the Component or sensor tuning parameters and neither tuning parameter nor
*  the capacitance measurement function creates interference to other.
*
*  While measuring capacitance of a CSX sensor electrode, all the non-measured
*  electrodes and CSD shield electrodes (if enabled) are set to active low (GND).
*  While measuring capacitance of a CSD sensor electrode, all the CSX sensor
*  electrodes are set to active low (GND) and all CSD sensor electrodes are
*  set to the state defined by the inactive sensor state parameter in the
*  Component CSD Setting tab of the customizer. If the shield electrode
*  is enabled, it is enabled during CSD sensor capacitance measurement.
*
*  The measurable capacitance range using this function is from 5pF to 255pF.
*  If a returned value is 255, the sensor capacitance can be higher.
*
*  The measured capacitance is stored in the CapSense_RAM_SNS_CP_STRUCT structure.
*  The CapSense_<WidgetName>_PTR2SNS_CP_VALUE register contains a pointer to
*  the array of the specified widget with the sensor capacitance.
*
*  This test can be executed for all the sensors at once using the
*  CapSense_RunSelfTest() function along with the
*  CapSense_TST_SNS_CAP mask.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget.
*  A macro for the sensor ID within the specified widget can be found in
*  the CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*
* \return
*  Returns a result of the test execution:
*  - Bits [7..0] The capacitance (in pF) of the CSD electrode or the capacitance
*    of Rx electrode of CSX sensor.
*  - Bits [15..8] The capacitance (in pF) of Tx electrode of CSX sensor.
*  - Bit [30] CapSense_TST_BAD_PARAM The input parameters are invalid.
*
*******************************************************************************/
uint32 CapSense_GetSensorCapacitance(uint32 widgetId, uint32 sensorId)
{
    uint32 result = 0uL;
    uint32 ioSnsId = sensorId;

    if ((CapSense_TOTAL_WIDGETS > widgetId) &&
        (CapSense_dsFlash.wdgtArray[widgetId].totalNumSns > sensorId))
    {
        CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);

        switch(CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
                CapSense_BistSwitchHwConfig(CapSense_BIST_HW_SNS_CAP_CSD_E);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:
                CapSense_BistSwitchHwConfig(CapSense_BIST_HW_SNS_CAP_CSX_E);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CYASSERT(0u);
            break;
        }

        /* For CSX widgets get an index of Rx electrode, for CSD widgets an electrode index is the same as snsId */
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            if (CapSense_SENSE_METHOD_CSX_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
            {
                ioSnsId = sensorId / CapSense_dsFlash.wdgtArray[widgetId].numRows;
            }
            else
            {
                ioSnsId = sensorId;
            }
        #elif (CapSense_ENABLE == CapSense_CSX_EN)
            ioSnsId = sensorId / CapSense_dsFlash.wdgtArray[widgetId].numRows;
        #else
            ioSnsId = sensorId;
        #endif
        result = CapSense_GetSensorCapacitanceExt(widgetId, ioSnsId);

        /* For CSX widgets get an index of Tx electrode and repeat the test */
        #if (CapSense_ENABLE == CapSense_CSX_EN)
            if (CapSense_SENSE_METHOD_CSX_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
            {
                ioSnsId = (sensorId % CapSense_dsFlash.wdgtArray[widgetId].numRows) +
                                   CapSense_dsFlash.wdgtArray[widgetId].numCols;

                result |= (CapSense_GetSensorCapacitanceExt(widgetId, ioSnsId) << CapSense_08_BIT_SHIFT);

            }
        #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
    }
    else
    {
        result = CapSense_TST_BAD_PARAM;
    }

    return result;
}

/*******************************************************************************
* Function Name: CapSense_GetSensorCapacitanceExt
****************************************************************************//**
*
* \brief
*  The internal function that measures a certain sensor
*  (electrode for CSX widgets) capacitance.
*
* \details
*  The function connects a certain sensor (electrode for CSX widgets)
*  to the previously configured CapSense HW block by using the CSD mode.
*  Then measures a Cp by using the CapSense_GetCapacitanceExt function.
*  After measuring the function sets an inactive connection of the sensor
*  (electrode for CSX widgets) and stores the Cp value in the corresponding
*  register of the CapSense_RAM_SNS_STRUCT.
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param snsId
*  Specifies the ID number of the sensor within the widget
*  which capacitance will be measured.
*
* \return
*  - Cp of a certain capacitor in pF.
*
*******************************************************************************/
static uint32 CapSense_GetSensorCapacitanceExt(uint32 wdgtId, uint32 ioSnsId)
{
    uint32 result;
    uint8 * cpPtr = CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsCpArr + ioSnsId;

    /* Connect the specified sensor */
    CapSense_BistConnectSensor(wdgtId, ioSnsId);

    result = CapSense_GetCapacitanceExt();

    /* Disconnect the sensor */
    CapSense_BistSetInactiveSensorConnection(wdgtId, ioSnsId);

    if (CapSense_BIST_SNS_CAP_MAX_CP < result)
    {
        result = CapSense_BIST_SNS_CAP_MAX_CP;
    }

    /* Save cp to data structure */
    *cpPtr = (uint8)result;

    return result;
}

/*******************************************************************************
* Function Name: CapSense_BistConnectSensor
****************************************************************************//**
*
* \brief
*  Connects a sensor (including ganged) port-pin to the HW block via the AMUX
*  bus.
*
* \details
*   The function gets the IO configuration registers addresses, their shifts and
*   masks from the FLASH_IO_STRUCT object. Basing on this data, it updates the HSIOM and
*   PC registers.
*
* \param
*   wdgtIndex Specifies ID of the widget.
*   sensorIndex Specifies ID of the sensor in the widget.
*
*******************************************************************************/
static void CapSense_BistConnectSensor(uint32 wdgtId, uint32 ioSnsId)
{
    uint32 eltdNum;
    uint32 eltdIndex;
    CapSense_FLASH_IO_STRUCT const *ioPtr;
    CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;

    /* Check ganged sns flag */
    if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[wdgtId].staticConfig & CapSense_GANGED_SNS_MASK))
    {
        curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash;
        curFlashSnsPtr += ioSnsId;
        ioPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId];
        eltdNum = curFlashSnsPtr->numPins;
    }
    else
    {
        ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash + ioSnsId;
        eltdNum = 1u;
    }

    /* Loop through all electrodes of specified sensor */
    for (eltdIndex = 0u; eltdIndex < eltdNum; eltdIndex++)
    {
        /* Set electrode to CSD_SENSE mode to be scanned */
        CapSense_SetElectrodePc(ioPtr, CapSense_TST_PC_HIGHZA);
        CapSense_SetElectrodeHsiom(ioPtr, CapSense_TST_HSIOM_CSD_SENSE);
        ioPtr++;
    }
}

/*******************************************************************************
* Function Name: CapSense_MeasureAllSensorCapacitance
****************************************************************************//**
*
* \brief
*  The internal function that measures all the sensors Cp capacitance.
*
* \details
*  The function that measures Cp of all the sensors by using
*  CapSense_GetSensorCapacitance function.
*  The function stores the Cp values in the corresponding BIST data
*  structure registers.
*
* \return
*  Returns the status of the measuring process:
*  - CYRET_SUCCESS.
*
*******************************************************************************/
static uint32 CapSense_MeasureAllSensorCapacitance(void)
{
    uint32 wdgtId;
    uint32 ioSnsId;
    uint32 totalNumIoSns;
    uint32 result = CYRET_SUCCESS;

    /* Loop through all the widgets */
    for (wdgtId = 0u; wdgtId < CapSense_TOTAL_WIDGETS; wdgtId++)
    {
        switch(CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtId]))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
                CapSense_BistSwitchHwConfig(CapSense_BIST_HW_SNS_CAP_CSD_E);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:
                CapSense_BistSwitchHwConfig(CapSense_BIST_HW_SNS_CAP_CSX_E);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CYASSERT(0u);
            break;
        }

        /* Get a total number of widget elements: for CSX it is numRows + numCols, for CSD it is totalNumSns */
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            if (CapSense_SENSE_METHOD_CSX_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtId]))
            {
                totalNumIoSns = (uint32)CapSense_dsFlash.wdgtArray[wdgtId].numRows + CapSense_dsFlash.wdgtArray[wdgtId].numCols;
            }
            else
            {
                totalNumIoSns = (uint32)CapSense_dsFlash.wdgtArray[wdgtId].totalNumSns;
            }
        #elif (CapSense_ENABLE == CapSense_CSX_EN)
            totalNumIoSns = (uint32)CapSense_dsFlash.wdgtArray[wdgtId].numRows + CapSense_dsFlash.wdgtArray[wdgtId].numCols;
        #else
            totalNumIoSns = (uint32)CapSense_dsFlash.wdgtArray[wdgtId].totalNumSns;
        #endif

        /* Loop through all the sensors */
        for (ioSnsId = 0u; ioSnsId < totalNumIoSns; ioSnsId++)
        {
            (void)CapSense_GetSensorCapacitanceExt(wdgtId, ioSnsId);
        }
    }
    return result;
}
#endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */

#if (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)

/*******************************************************************************
* Function Name: CapSense_GetShieldCapacitance
****************************************************************************//**
*
* \brief
*  Measures the shield electrode capacitance.
*
* \details
*  The function measures capacitance of the shield electrode and returns the
*  result, alternatively the result is stored in
*  CapSense_SHIELD_CAP_VALUE of data structure. If the shield consists
*  of several electrodes, total capacitance of all shield electrodes is
*  reported.
*
*  While measuring capacitance of shield electrode, the sensor states are
*  inherited from the Component configuration. All the CSX electrodes are
*  set to active low (GND) and all the CSD electrodes are set to state defined
*  by the inactive sensor state parameter in the Component CSD Setting tab
*  of the customizer.
*
*  The measurable capacitance range using this function is from 5pF to 255pF.
*
*  This test can be executed for all the sensors at once using the
*  CapSense_RunSelfTest() function with the CapSense_TST_SH_CAP
*  mask.
*
* \return
*  The shield electrode capacitance (in pF)
*
*******************************************************************************/
uint32 CapSense_GetShieldCapacitance(void)
{
    uint32 result;

    CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);
    CapSense_BistSwitchHwConfig(CapSense_BIST_HW_SH_CAP_E);

    #if (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)
        CapSense_BistConnectShieldAsSensor();
    #endif /* (0u != CapSense_CSD_TOTAL_SHIELD_COUNT) */

    result = CapSense_GetCapacitanceExt();

    /* Disconnect shield electrodes */
    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
         (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))
        CapSense_BistDisableShieldElectrodes();
    #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
               (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */

    if (CapSense_BIST_SNS_CAP_MAX_CP < result)
    {
        result = CapSense_BIST_SNS_CAP_MAX_CP;
    }

    /* Save a capacitance value to the data structure */
    CapSense_dsRam.selfTest.shieldCap = (uint16)result;

    return result;
}
#endif /* (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) */

#if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_SH_CAP_EN))

/*******************************************************************************
* Function Name: CapSense_GetCapacitanceExt
****************************************************************************//**
*
* \brief
*  The internal function that measures a capacitance attached to AMUXBUS.
*
* \details
*  The function measures Cp of a certain sensor or shield by using the
*  CSD mode and defined Idac mode, sense clock frequency and resolution.
*  The range for sensor measuring is 5 to 250 pF, for shield 10 to 250 pf.
*  The function calibrates the Idac for reach a defined target of raw counts.
*  If reached Idac will be lower then a certain value the function sets
*  the higher sense clock frequency.
*  The function stores the Cp value in the corresponding register of
*  CapSense_RAM_SNS_STRUCT.
*
* \return
*  - Cp of  a certain sensor or shield in pF.
*
*******************************************************************************/
static uint32 CapSense_GetCapacitanceExt(void)
{
    uint32 cp;
    uint32 idacValue = 0uL;
    uint32 snsClkIncrementIndex;
    uint32 idacMask = ((uint32)CapSense_BIST_CAL_MIDDLE_BIT << 1uL);

    if (CapSense_BIST_HW_SH_CAP_E == CapSense_bistHwConfig)
    {
        /* Set SnsClk frequency for shield measurement up to 500pF */
        snsClkIncrementIndex = 0uL;
    }
    else
    {
        /* Set SnsClk frequency for sensor measurement up to 250pF */
        snsClkIncrementIndex = 1uL;
    }

    /* Initialize raw count for first cycle pass */
    CapSense_bistRawcount = 0u;

    CapSense_BistSetScanDuration();

    /* Finding optimal IDAC and Frequency */
    while (0uL != idacMask)
    {
        /* Decrease IDAC until rawData reaches the calibration target */
        if (CapSense_BIST_CALIBRATION_TARGET > CapSense_bistRawcount)
        {
            idacValue &= (uint32)(~idacMask);
        }
        idacMask >>= 1uL;
        idacValue |= idacMask;
        if (0uL == idacValue)
        {
            idacValue++;
        }

        /* Revert back Idac value and increase SnsClk frequency if Idac is too small */
        if (idacValue < CapSense_BIST_MIN_IDAC_VALUE)
        {
            if ((CapSense_TST_BASE_SNS_CLK_DIVIDER >> (snsClkIncrementIndex + 1uL)) >= CapSense_TST_MIN_SNS_CLK_DIVIDER)
            {
                snsClkIncrementIndex++;
                idacValue &= (uint32)(~idacMask);
                idacMask = (uint32)(CapSense_BIST_CAL_MIDDLE_BIT);
                idacValue = idacMask;
            }
        }

        /*
        * Setup scan parameters:
        * - IDAC
        * - SnsClk
        * - Resolution (conversion number)
        */
        CapSense_BistSetIdac(idacValue);
        CapSense_BistConfigClock((uint32)CapSense_TST_BASE_SNS_CLK_DIVIDER >> snsClkIncrementIndex);
        #if (CapSense_ENABLE == CapSense_CSDV2)
            CY_SET_REG32(CapSense_SEQ_NORM_CNT_PTR, ((uint32)CapSense_BIST_10_BIT_MASK /
                                            ((uint32)CapSense_TST_BASE_SNS_CLK_DIVIDER >> snsClkIncrementIndex)) &
                                            CapSense_SEQ_NORM_CNT_CONV_CNT_MASK);
        #endif

        /* Perform scanning */
        CapSense_dsRam.status |= CapSense_SW_STS_BUSY;
        CapSense_BistStartSample();
        CapSense_WaitEndOfScan(CapSense_BIST_MEASURE_WATCHDOG_CYCLES_NUM);
    }

    /*
    * Capacitance calculation:
    *   cp(pF) = Rawcount * IdacCode * IdacGain(nA) * 1000 / (MaxRaw * SnsClk(kHz) * Vref(mV))
    * Details:
    *   - The order of calculation is changed to avoid the potential overflow
    *   - The half of the last divider is added to implement the value rounding
    */
    cp = (CapSense_TST_SNS_CAP_IDAC_GAIN * CapSense_BIST_SNS_CAP_UNIT_SCALE) / CapSense_CSD_VREF_MV;
    cp = (cp * idacValue) / CapSense_BIST_10_BIT_MASK;
    cp = cp * (uint32)CapSense_bistRawcount;
    cp = (cp + (((uint32)CapSense_TST_BASE_SNS_CLK_KHZ << snsClkIncrementIndex) >> 1uL)) /
                ((uint32)CapSense_TST_BASE_SNS_CLK_KHZ << snsClkIncrementIndex);

    return cp;
}
#endif /* #if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)) */
/** \endcond */
#if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_SH_CAP_EN))
/*******************************************************************************
* Function Name: CapSense_BistDisableHwSnsCap
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
static void CapSense_BistDisableHwSnsCap(void)
{
    uint32 regValue;
    uint8  interruptState;

    /* Disconnect Cmod from AMUXBUS-A using HSIOM registers */
    #if (CapSense_ENABLE == CapSense_CSD_EN)
        interruptState = CyEnterCriticalSection();
        regValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
        regValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
        CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, regValue);
        CyExitCriticalSection(interruptState);
    #else
        interruptState = CyEnterCriticalSection();
        regValue = CY_GET_REG32(CapSense_CintA_HSIOM_PTR);
        regValue &= (uint32)(~(uint32)CapSense_CintA_HSIOM_MASK);
        regValue |= (CapSense_HSIOM_SEL_GPIO << CapSense_CintA_HSIOM_SHIFT);
        CY_SET_REG32(CapSense_CintA_HSIOM_PTR, regValue);
        CyExitCriticalSection(interruptState);

        interruptState = CyEnterCriticalSection();
        regValue = CY_GET_REG32(CapSense_CintB_HSIOM_PTR);
        regValue &= (uint32)(~(uint32)CapSense_CintB_HSIOM_MASK);
        regValue |= (CapSense_HSIOM_SEL_GPIO << CapSense_CintB_HSIOM_SHIFT);
        CY_SET_REG32(CapSense_CintB_HSIOM_PTR, regValue);
        CyExitCriticalSection(interruptState);
    #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */

    #if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
        /* Verification of the bist mode */
        if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
        {
            /* Disconnect Csh from AMUXBUS-B */
            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                 (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
                interruptState = CyEnterCriticalSection();
                regValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
                regValue &= (uint32)(~(uint32)(CapSense_CSH_TO_AMUXBUS_B_MASK << CapSense_CSH_HSIOM_SHIFT));
                CY_SET_REG32(CapSense_CSH_HSIOM_PTR, regValue);
                CyExitCriticalSection(interruptState);
            #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                       (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

            /* Disconnect shield electrodes */
            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                 (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))
                CapSense_BistDisableShieldElectrodes();
            #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                       (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */

            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))
                /*
                * Disconnect all IOs from AMUXBUS_B as all rest tests suppose the electrodes are not
                * connected to any bus. It is done for performance optiomization:
                * not to write HSIOM register each time at test type switching.
                */
                CapSense_SetAllIOHsiomState(CapSense_TST_HSIOM_GPIO);
            #endif
        }
    #endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */
}


#if (CapSense_ENABLE == CapSense_CSD_EN)
    /*******************************************************************************
    * Function Name: CapSense_BistSetAllInactiveSensorConnection
    ****************************************************************************//**
    *
    * \brief
    *  Resets all the CSD sensors to the non-sampling state by sequential
    *  disconnecting all the sensors from the Analog MUX bus and putting them to
    *  an inactive state.
    *
    * \details
    *   The function goes through all the sensors of CSD widgets and updates appropriate bits in
    *   the IO HSIOM, PC and DR registers depending on the Inactive sensor connection
    *   parameter. DR register bits are set to zero when the Inactive sensor
    *   connection is Ground or Hi-Z.
    *
    *******************************************************************************/
    static void CapSense_BistSetAllInactiveSensorConnection(void)
    {
        uint32 wdgtId;
        uint32 snsId;

        for (wdgtId = 0uL; wdgtId < CapSense_TOTAL_WIDGETS; wdgtId++)
        {
            if (CapSense_SENSE_METHOD_CSD_E == CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtId]))
            {
                /* Go through all the sensors in widget */
                for (snsId = 0uL; snsId < (uint8)CapSense_dsFlash.wdgtArray[wdgtId].totalNumSns; snsId++)
                {
                    CapSense_BistSetInactiveSensorConnection(wdgtId, snsId);
                }
            }
        }
    }
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


/*******************************************************************************
* Function Name: CapSense_BistSetInactiveSensorConnection
****************************************************************************//**
*
* \brief
*  Sets an inactive state for a certain sensor (electrode).
*
* \details
*  The function updates appropriate bits in the IO HSIOM and PC registers
*  depending on the Inactive sensor connection parameter.
*  The function proceeds ganged sensors(electrodes).
*
* \param wdgtId
*  Specifies the ID number of the widget to be processed.
*
* \param ioSnsId
*  Specifies the ID number of the sensor (electrode) within the widget to be processed.
*
*******************************************************************************/
static void CapSense_BistSetInactiveSensorConnection(uint32 wdgtId, uint32 ioSnsId)
{
    uint32 eltdNum;
    uint32 eltdIndex;
    CapSense_FLASH_IO_STRUCT const *ioPtr;
    CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;

    /* Check ganged sns flag */
    if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[wdgtId].staticConfig & CapSense_GANGED_SNS_MASK))
    {
        curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash;
        curFlashSnsPtr += ioSnsId;
        ioPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId];
        eltdNum = curFlashSnsPtr->numPins;
    }
    else
    {
        ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[wdgtId].ptr2SnsFlash + ioSnsId;
        eltdNum = 1uL;
    }

    /* Loop through all electrodes of specified sensor */
    for (eltdIndex = 0uL; eltdIndex < eltdNum; eltdIndex++)
    {
        if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
        {
            #if (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)
                /* Update port configuration register (drive mode to HiZ Analog) for sensor */
                CapSense_SetElectrodePc(ioPtr, CapSense_TST_PC_HIGHZA);
                /* Connect to Shield */
                CapSense_SetElectrodeHsiom(ioPtr, CapSense_TST_HSIOM_CSD_SHIELD);
            #else
                /* Update HSIOM register to disconnect sensor */
                CapSense_SetElectrodeHsiom(ioPtr, CapSense_TST_HSIOM_GPIO);
                /* Set drive mode for elecctrode based on inactive sensor connection parameter */
                CapSense_SetElectrodePc(ioPtr, CapSense_TST_INACTIVE_SNS_GPIO_DM);
            #endif /* (CapSense_SNS_CONNECTION_SHIELD != CapSense_CSD_INACTIVE_SNS_CONNECTION) */
        }
        else if (CapSense_BIST_HW_SNS_CAP_CSX_E == CapSense_bistHwConfig)
        {
            /* Update HSIOM register to disconnect sensor */
            CapSense_SetElectrodeHsiom(ioPtr, CapSense_TST_HSIOM_GPIO);
            /* Update port configuration register (drive mode to HiZ Analog) for sensor */
            CapSense_SetElectrodePc(ioPtr, CapSense_TST_PC_STRONG);
        }
        else if (CapSense_BIST_HW_SH_CAP_E == CapSense_bistHwConfig)
        {
            #if (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)
                /* Set electrode to CSD_SENSE mode to be scanned */
                CapSense_SetElectrodePc(ioPtr, CapSense_TST_PC_HIGHZA);
                CapSense_SetElectrodeHsiom(ioPtr, CapSense_TST_HSIOM_CSD_SENSE);
            #else
                /* Update HSIOM register to disconnect sensor */
                CapSense_SetElectrodeHsiom(ioPtr, CapSense_TST_HSIOM_GPIO);
                /* Set drive mode for elecctrode based on inactive sensor connection parameter */
                CapSense_SetElectrodePc(ioPtr, CapSense_TST_INACTIVE_SNS_GPIO_DM);
            #endif /* (CapSense_SNS_CONNECTION_SHIELD != CapSense_CSD_INACTIVE_SNS_CONNECTION) */
        }
        else
        {
            /* Do nothing */
        }
        ioPtr++;
    }
}

#if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
     (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))

/*******************************************************************************
* Function Name: CapSense_BistDisableShieldElectrodes
****************************************************************************//**
*
* \brief
*   This internal function disables Shield Electrodes.
*
* \details
*   The function resets the bit in the HSIOM register which disables the shield
*   electrode functionality on the pin. The port and pin configurations are
*   stored in  the CapSense_shieldIoList structure.
*
*******************************************************************************/
static void CapSense_BistDisableShieldElectrodes(void)
{
    uint32 newRegValue;
    uint32 shieldIndex;

    for (shieldIndex = 0u; shieldIndex < CapSense_CSD_TOTAL_SHIELD_COUNT; shieldIndex++)
    {
        /* Reset HSIOM register (to GPIO state) */
        CY_SET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr, CY_GET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr) &
                     (uint32)~(uint32)(CapSense_HSIOM_SEL_MASK << CapSense_shieldIoList[shieldIndex].hsiomShift));

        /* Update port configuration register (drive mode to ground)  */
        newRegValue = CY_GET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr);
        newRegValue &= ~(CapSense_GPIO_PC_MASK << CapSense_shieldIoList[shieldIndex].shift);
        newRegValue |= (CapSense_SNS_GROUND_CONNECT << CapSense_shieldIoList[shieldIndex].shift);
        CY_SET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr, newRegValue);

        /* Set logic 0 to port data register */
        CY_SET_REG32(CapSense_shieldIoList[shieldIndex].drPtr, CY_GET_REG32(CapSense_shieldIoList[shieldIndex].drPtr) &
                   (uint32)~(uint32)((uint32)1u << CapSense_shieldIoList[shieldIndex].drShift));
    }
}
#endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
           (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */

#endif /* ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
           (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)) */

#if (CapSense_ENABLE == CapSense_TST_MEASUREMENT_GROUP_EN)

/*******************************************************************************
* Function Name: CapSense_BistPostSingleScan
****************************************************************************//**
*
* \brief
*  This is an internal ISR function for the single-sensor scanning implementation.
*
* \details
*  This ISR handler is triggered when the CSD scan for a capacitance measuring
*  is finished.
*
*  The following tasks are performed:
*    1. Disable the CSD interrupt.
*    2. Read the Counter register and update the CapSense_bistRawcount with raw data.
*    3. Reset the BUSY flag.
*    4. Connect the Vref buffer to the AMUX bus (for Third-generation HW block).
*
*******************************************************************************/
CY_ISR(CapSense_BistPostSingleScan)
{
    uint32 tmpRawData;

    /* Clear pending interrupt */
    CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
    /* Read SlotResult from Raw Counter */
    tmpRawData = (uint16)CY_GET_REG32(CapSense_COUNTER_PTR);

    #if (CapSense_ENABLE == CapSense_CSDV2)
        if((CapSense_TST_EXT_CAP_RESOLUTION - 1uL) < tmpRawData)
        {
            tmpRawData = (CapSense_TST_EXT_CAP_RESOLUTION - 1uL);
        }
    #endif

    CapSense_bistRawcount = LO16(tmpRawData);

    /* Sensor is totally scanned. Reset BUSY flag */
    CapSense_dsRam.status &= ~(CapSense_SW_STS_BUSY | CapSense_WDGT_SW_STS_BUSY);
    #if (CapSense_ENABLE == CapSense_CSDV2)
        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        /* Open HCBV and HCBG switches */
            CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR, CapSense_SW_SHIELD_SEL_SW_HCBV_STATIC_OPEN |
                                                             CapSense_SW_SHIELD_SEL_SW_HCBG_STATIC_OPEN);
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        #if (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN)
            /* Disable the Fourth-generation HW block */
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd);
        #endif /* (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) */
    #else /* (CapSense_ENABLE == CapSense_CSDV2) */
        #if (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN)
            /* Disable the Third-generation HW block. Connect the Vref buffer to AMUX bus */
            #if ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
                 (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
                CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_BIST_CMOD_PRECHARGE_CONFIG);
            #else
                CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_BIST_CTANK_PRECHARGE_CONFIG);
            #endif /* ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
                       (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */
        #else
            /* Connect the Vref buffer to AMUX bus. The Third-generation HW block is enabled */
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_BIST_CTANK_PRECHARGE_CONFIG_CSD_EN);
        #endif /* (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) */
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}

#if (CapSense_DISABLE == CapSense_CSDV2)

/*******************************************************************************
* Function Name: CapSense_BistClockRestart
****************************************************************************//**
*
* \brief
*  This function restarts the clocks.
*
* \details
*  The function performs the following:
*  1) Stops, sets dividers and starts ModClk clock.
*  2) Stops and starts the SnsClk clock.
*  3) Scans one cycle to reset the flip-flop for the Direct clock mode.
*
*******************************************************************************/
static void CapSense_BistClockRestart(void)
{
    /* Stop clocks, set dividers, and start the clock to align the clock phase */
    CapSense_SsSetModClkClockDivider(CapSense_TST_MOD_CLK_DIVIDER);

    #if (CapSense_ENABLE == CapSense_IS_M0S8PERI_BLOCK)
        /* Stop the sense clock   */
        CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                     ((uint32)CapSense_SnsClk__DIV_ID <<
                     CapSense_SNSCLK_CMD_DIV_SHIFT)|
                     ((uint32)CapSense_SNSCLK_CMD_DISABLE_MASK));

        /* Check whether the previous sense clock start command has completed */
        while(0u != (CY_GET_REG32(CapSense_SNSCLK_CMD_PTR) & CapSense_SNSCLK_CMD_ENABLE_MASK))
        {
            /* Wait until the previous sense clock start command has completed */
        }

        /* Start the sense clock aligned to a previously started modulator clock */
        CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                     (uint32)(((uint32)CapSense_SnsClk__DIV_ID << CapSense_SNSCLK_CMD_DIV_SHIFT) |
                      ((uint32)CapSense_ModClk__PA_DIV_ID << CapSense_SNSCLK_CMD_PA_DIV_SHIFT) |
                      CapSense_SNSCLK_CMD_ENABLE_MASK));
    #else
        /* Clear the bit to disable the SnsClk clock */
        CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                     CY_GET_REG32(CapSense_SNSCLK_CMD_PTR) &
                     (uint32)(~(uint32)CapSense_SnsClk__ENABLE_MASK));

        /* Set a bit to enable the SnsClk clock */
        CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                     CY_GET_REG32(CapSense_SNSCLK_CMD_PTR) |
                    CapSense_SnsClk__ENABLE_MASK);
    #endif /* (CapSense_ENABLE == CapSense_IS_M0S8PERI_BLOCK) */

    /* Scan one cycle to reset the flip-flop for the Direct clock mode */
    CyIntDisable(CapSense_ISR_NUMBER);
    CY_SET_REG32(CapSense_COUNTER_PTR, CapSense_ONE_CYCLE);

    while(0u != (CY_GET_REG32(CapSense_COUNTER_PTR) & CapSense_RESOLUTION_16_BITS))
    {
        /* Wait until scanning is complete */
    }
    CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
    CyIntClearPending(CapSense_ISR_NUMBER);
}
#endif /* (CapSense_DISABLE == CapSense_CSDV2) */

/*******************************************************************************
* Function Name: CapSense_BistStartSample
****************************************************************************//**
*
* \brief
*   Starts the CSD conversion for a capacitance measuring.
*
* \details
*   This function assumes that the CSD block is already set up using
*   the CapSense_BistEnableHwSnsCap function and the capacitance
*   is connected to the CSD block.
*   The function performs the following tasks:
*   1. Disables the CSD interrupt.
*   2. Enables CSD block if it was disabled.
*   3. Restarts the clock (only for the Third-generation HW block and only for sensor capacitance measuring).
*   3. Pre charges Cmod (only for a sensor capacitance measuring or for the Third-generation HW block).
*   4. Starts the scan.
*
*******************************************************************************/
static void CapSense_BistStartSample(void)
{
#if (CapSense_ENABLE == CapSense_CSDV2)
    /* Fourth-generation HW block section */

    /* Disable the CSD interrupt to prevent it during coarse initialization */
    CyIntDisable(CapSense_ISR_NUMBER);
    /* Enable power to sub-blocks */
    CY_SET_REG32(CapSense_HSCMP_PTR, CapSense_HSCMP_EN_MASK);

    #if (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN)
        CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd |
                                                  CapSense_CONFIG_SENSE_EN_MASK |
                                                  CapSense_CONFIG_ENABLE_MASK);

        /* Wait until CSD block power is ON */
        CyDelayCycles(CapSense_BIST_BLOCK_ON_DELAY);
    #else
        /* Wait until HSCMP power is ON */
        CyDelayCycles(CapSense_BIST_HSCMP_ON_DELAY);
    #endif /* (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) */

    if (CapSense_bistHwConfig != CapSense_BIST_HW_EXT_CAP_E)
    {
        /* Precharging Cmod and Csh */
        CapSense_BistCmodPrecharge();
    }

    /* Trigger Scan */
    CapSense_BistTriggerScan();

#else
    /* Third-generation HW block section */
    uint8 interruptState;

    interruptState = CyEnterCriticalSection();

    /* Enable the CSD block. Disconnect the Vref buffer from AMUX */
    #if ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
         (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
        CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_BIST_CMOD_PRECHARGE_CONFIG_CSD_EN);
    #else
        CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_BIST_CTANK_PRECHARGE_CONFIG_CSD_EN);
    #endif

    /* Restart the clocks. Scan one cycle to reset the flip-flop for the Direct clock mode */
    if (CapSense_bistHwConfig != CapSense_BIST_HW_EXT_CAP_E)
    {
        CapSense_BistClockRestart();
    }
    /* Pre-charging Cmod and Csh */
    CapSense_BistCmodPrecharge();
    /* Trigger Scan */
    CapSense_BistTriggerScan();

    CyExitCriticalSection(interruptState);
#endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}

/*******************************************************************************
* Function Name: CapSense_BistEnableHwSnsCap
****************************************************************************//**
*
* \brief
*  Sets up the CapSense block to perform a capacitanse measuring.
*
* \details
*   This function prepare HW of the CapSense block to CSD sensing mode
*   with BIST defined parameters.
*   The function performs the following tasks:
*   1. Sets the default CSD configuration for BIST scans
*   2. Enables shield electrodes and Csh (if defined)
*   3. Sets Idac config for BIST scans
*   4. Sets a sensor clock frequency (only for a sensor capacitance measuring)
*   5. Sets the ISR handler for BIST scans.
*
*******************************************************************************/
static void CapSense_BistEnableHwSnsCap(void)
{
    uint32 newRegValue;
    uint8  interruptState;

    #if (CapSense_ENABLE == CapSense_CSDV2)
        CapSense_BistDischargeExtCapacitors();
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
    
    #if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
        /* Verification of the bist mode */
        if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
        {
            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                /* Connect shields to AMUX-B bus (config HSIOM regs) */
                #if (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)
                    CapSense_BistEnableShieldElectrodes();
                #endif /* (0u != CapSense_CSD_TOTAL_SHIELD_COUNT) */

                #if (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)
                    /* Configure Csh */
                    CapSense_BistEnableShieldTank();
                #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) */
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        }
    #endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */

    #if (CapSense_ENABLE == CapSense_CSDV2)

        /* Initialize the unused CSD registers to defaut state */
        CY_SET_REG32(CapSense_INTR_SET_PTR,         CapSense_BIST_INTR_SET_CFG);
        CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR,   CapSense_BIST_SW_FW_TANK_SEL_CFG);
        CY_SET_REG32(CapSense_SW_DSI_SEL_PTR,       CapSense_BIST_DEFAULT_SW_DSI_SEL);
        CY_SET_REG32(CapSense_ADC_CTL_PTR,          CapSense_BIST_ADC_CTL_CFG);
        CY_SET_REG32(CapSense_AMBUF_PTR,            CapSense_BIST_AMBUF_PWR_MODE_OFF);
        CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR,    CapSense_BIST_SW_SHIELD_SEL_CFG);
        CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR,      CapSense_BIST_SW_HS_P_SEL_CFG);
        CY_SET_REG32(CapSense_SW_HS_N_SEL_PTR,      CapSense_BIST_SW_HS_N_SEL_CFG);
        CY_SET_REG32(CapSense_HSCMP_PTR,            CapSense_BIST_HSCMP_CFG);
        CY_SET_REG32(CapSense_SEQ_TIME_PTR,         CapSense_BIST_AUTO_ZERO_TIME);
        CY_SET_REG32(CapSense_SW_RES_PTR,           CapSense_BIST_SW_RES_INIT);

        /* Connect AMUXBUS-A to CSDBUS-A */
        interruptState = CyEnterCriticalSection();
        newRegValue = CY_GET_REG32(CapSense_SW_BYP_SEL_PTR);
        newRegValue |= CapSense_SW_BYP_SEL_SW_BYA_MASK;
        CY_SET_REG32(CapSense_SW_BYP_SEL_PTR, newRegValue);
        CyExitCriticalSection(interruptState);

        /* Verification of the bist mode */
        if (CapSense_BIST_HW_EXT_CAP_E != CapSense_bistHwConfig)
        {
            #if (CapSense_ENABLE == CapSense_CSD_EN)
                /* Connect CMOD to AMUXBUS-A */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CMOD_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                /* Connect CMOD to (sense path) to CSDCOMP */
                #if (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION)
                    CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFPM_STATIC_CLOSE);
                #elif (CapSense_CSD__CSHIELD_PAD == CapSense_CMOD_CONNECTION)
                    CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFPS_STATIC_CLOSE);
                #else
                    CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFPT_STATIC_CLOSE);
                #endif /* (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) */
            #else
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintA_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CintA_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CintA_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CintA_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CintB_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CintB_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CintB_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);

                CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFPT_STATIC_CLOSE);
            #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
        }

        /* Verification of the bist mode */
        if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
        {
            /* Configure shield driving path */
            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                /* Connect AMUXBUS-B to CSDBUS-B (and AMUXBUS-A to CSDBUS-A ) */
            CY_SET_REG32(CapSense_SW_BYP_SEL_PTR, CapSense_SW_BYP_SEL_SW_BYA_MASK |
                                                          CapSense_SW_BYP_SEL_SW_BYB_MASK);
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        }

        /* Connect VREF to REFGEN (IAIB switch is open) */
        CY_SET_REG32(CapSense_SW_REFGEN_SEL_PTR, CapSense_BIST_SW_REFGEN_SEL_CFG);

        #if (CapSense_2000_MV > CapSense_CYDEV_VDDA_MV)

            /* Connect VREFHI to CSDCOMP */
            CY_SET_REG32(CapSense_SW_CMP_N_SEL_PTR, CapSense_SW_CMP_N_SEL_SW_SCRH_STATIC_CLOSE);

            /* Configure REFGEN. Set reference voltage when Vdda < 2 V */
            CY_SET_REG32(CapSense_REFGEN_PTR, CapSense_REFGEN_LV);

            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                CY_SET_REG32(CapSense_AMBUF_PTR, CapSense_AMBUF_PWR_MODE_NORM);
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_IRH_STATIC_CLOSE |
                                                                  CapSense_SW_AMUXBUF_SEL_SW_IRLB_STATIC_CLOSE |
                                                                  CapSense_SW_AMUXBUF_SEL_SW_ICB_PHI2);
            #else
                CY_SET_REG32(CapSense_AMBUF_PTR, CapSense_AMBUF_PWR_MODE_NORM);
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_IRH_STATIC_CLOSE |
                                                                  CapSense_SW_AMUXBUF_SEL_SW_IRLB_STATIC_CLOSE);
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        #else

            /* Connect VREFHI (from RefGen) to CSDCOMP when Vdda >= 2 V */
            CY_SET_REG32(CapSense_SW_CMP_N_SEL_PTR, CapSense_SW_CMP_N_SEL_SW_SCRH_STATIC_CLOSE);

            /* Configure REFGEN. Set reference voltage when Vdda >= 2 V */
            CY_SET_REG32(CapSense_REFGEN_PTR, CapSense_REFGEN_HV);

            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                /* Turn on CSD_AMBUF high power level when Vdaa >= 2V */
                CY_SET_REG32(CapSense_AMBUF_PTR, CapSense_AMBUF_PWR_MODE_HI);
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_IRH_STATIC_CLOSE |
                                                                  CapSense_SW_AMUXBUF_SEL_SW_ICB_PHI2);
            #else

                CY_SET_REG32(CapSense_AMBUF_PTR, CapSense_AMBUF_PWR_MODE_OFF);

                /* Configure REFGEN. Set reference voltage when Vdda >= 2 V */
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_DEFAULT);
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        #endif /* (CapSense_2000_MV > CapSense_CYDEV_VDDA_MV) */

        if (CapSense_BIST_HW_EXT_CAP_E != CapSense_bistHwConfig)
        {
            /* Configure ModClk */
            CapSense_SsSetModClkClockDivider(CapSense_TST_MOD_CLK_DIVIDER);
        }

        /* Verification of the bist mode */
        if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
        {
            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                /* Update CSD config register shield delay bits with shield delay value */
                CapSense_bistConfigCsd &= (uint32)(~CapSense_CONFIG_SHIELD_DELAY_MASK);
                CapSense_bistConfigCsd |= ((uint32)CapSense_CSD_SHIELD_DELAY <<
                                                                       CYFLD_CSD_SHIELD_DELAY__OFFSET);
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        }

        /* Configure HW block filter delay */
        CapSense_bistConfigCsd &= (uint32)(~CapSense_CONFIG_FILTER_DELAY_MASK);
        CapSense_bistConfigCsd |= CapSense_FILTER_DELAY_CFG;

        /* Enable power to sub-blocks */
        CY_SET_REG32(CapSense_CONFIG_PTR,     CapSense_bistConfigCsd |
                                                      CapSense_CONFIG_SENSE_EN_MASK |
                                                      CapSense_CONFIG_ENABLE_MASK);

        #if(CapSense_TST_ANALOG_STARTUP_DELAY_US > 0uL)
            CyDelayUs(CapSense_TST_ANALOG_STARTUP_DELAY_US);
        #endif /* (CapSense_TST_ANALOG_STARTUP_DELAY_US > 0uL) */

        /* Set the number of dummy fine initialization cycles depending of the bist mode */
        /* Set Phi1 and Phi2 switches to non-overlapping (no low EMI) and width depending of the bist mode */
        if (CapSense_BIST_HW_EXT_CAP_E != CapSense_bistHwConfig)
        {
            CY_SET_REG32(CapSense_SEQ_INIT_CNT_PTR, CapSense_TST_FINE_INIT_TIME);
            CY_SET_REG32(CapSense_SENSE_DUTY_PTR, CapSense_SENSE_DUTY_OVERLAP_PHI1_MASK |
                                                          CapSense_SENSE_DUTY_OVERLAP_PHI2_MASK);
        }
        else
        {

            CY_SET_REG32(CapSense_SEQ_INIT_CNT_PTR, CapSense_SEQ_INIT_CNT_FINE_INIT_SKIP);
            CY_SET_REG32(CapSense_SENSE_DUTY_PTR, CapSense_SENSE_DUTY_OVERLAP_PHI1_MASK |
                                                          CapSense_SENSE_DUTY_OVERLAP_PHI2_MASK |
                                                          CapSense_SENSE_DUTY_TST_EXT_CAP_WIDTH );
        }
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */

    #if (CapSense_DISABLE == CapSense_CSDV2)

        if (CapSense_BIST_HW_EXT_CAP_E != CapSense_bistHwConfig)
        {
            #if (CapSense_ENABLE == CapSense_CSD_EN)
                /* Connect Cmod to AMUXBUS-A using the HSIOM registers */
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
                newRegValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
                newRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CMOD_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);
            #else
                interruptState = CyEnterCriticalSection();
                newRegValue = CY_GET_REG32(CapSense_CintB_HSIOM_PTR);
                newRegValue &= ~CapSense_CintB_HSIOM_MASK;
                newRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CintB_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CintB_HSIOM_PTR, newRegValue);
                CyExitCriticalSection(interruptState);
            #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
        }
        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
            /* Verification of the BIST mode */
            if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
            {
                /* Update CSD config register shield delay bits with a shield delay value */
                CapSense_bistConfigCsd &= (uint32)(~CapSense_CONFIG_SHIELD_DELAY_MASK);
                CapSense_bistConfigCsd |= ((uint32)CapSense_CSD_SHIELD_DELAY << CapSense_SHIELD_DELAY_SHIFT);
            }
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

        if (CapSense_BIST_HW_EXT_CAP_E != CapSense_bistHwConfig)
        {
            /* Enable the CSD block. Connect Vref buffer to AMUX bus to make sure that Cmod is charged before scanning in the active mode */
            #if (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN)
                CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd);
            #else
                CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_BIST_CTANK_PRECHARGE_CONFIG_CSD_EN);
            #endif /* (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) */
        }
    #endif /* (CapSense_DISABLE == CapSense_CSDV2) */

    /* Configure IDAC */
    CapSense_BistSetIdac(0uL);

    if (CapSense_BIST_HW_EXT_CAP_E != CapSense_bistHwConfig)
    {
        /* Set clock dividers and clock source mode */
        CapSense_BistConfigClock(CapSense_TST_BASE_SNS_CLK_DIVIDER);
    }
    else
    {
        #if (CapSense_DISABLE == CapSense_CSDV2)
            /* Enable the CSD block. Connect Vref buffer to AMUX bus to make sure that Cmod is charged before scanning in the active mode */
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_CONFIG_ENABLE_MASK);
        #endif /* (CapSense_DISABLE == CapSense_CSDV2) */
    }
    CapSense_ISR_StartEx(&CapSense_BistPostSingleScan);
}

/*******************************************************************************
* Function Name: CapSense_BistConfigClock
****************************************************************************//**
*
* \brief
*   Sets the sensor clock frequency for BIST scans
*
* \details
*   For a sensor or shield capasitance measurement sets a calculated value of
*   the sensor clock frequency and for an external capacitance measurement sets
*   the value for a one period scan.
*
*******************************************************************************/
static void CapSense_BistConfigClock(uint32 snsClk)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)
        uint32 newRegValue;

        /*
         * Set divider value for sense clock.
         * 1u is subtracted from snsClk because SENSE_DIV value 0 corresponds
         * to dividing by 1.
         */
        newRegValue = (snsClk - 1u) & CapSense_SENSE_PERIOD_SENSE_DIV_MASK;
        CY_SET_REG32(CapSense_SENSE_PERIOD_PTR, newRegValue);
    #else
        /* Set a sense clock divider */
        CapSense_SsSetSnsClockDivider(snsClk);
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}

/*******************************************************************************
* Function Name: CapSense_BistSetScanDuration
****************************************************************************//**
*
* \brief
*   Calculates Scan Duration which is defined by scan resolution
*
* \details
*   For Fourth-generation HW block: The function calculates the number of conversions and updates the
*   SEQ_NORM_CNT register. The number of conversions depends on the resolution and
*   snsClk divider.
*   For Third-generation HW block: The function recalculates the resolution using the following equation:
*   2^resolution - 1. The calculated value is contained in the
*   CapSense_bistCounterResolution global variable and used in the
*   CapSense_bistStartSample() function to trigger a scan process.
*
*******************************************************************************/
static void CapSense_BistSetScanDuration(void)
{
    uint32 scanDuration;

    if (CapSense_bistHwConfig == CapSense_BIST_HW_EXT_CAP_E)
    {
        #if (CapSense_ENABLE == CapSense_CSDV2)
            scanDuration = CapSense_TST_EXT_CAP_DURATION;
        #else
            scanDuration = CapSense_BIST_SCAN_DURATION_255;
        #endif
    }
    else
    {
        #if (CapSense_ENABLE == CapSense_CSDV2)
            /* Calculate scanning resolution value in register */
            scanDuration = (uint32) (CapSense_BIST_10_BIT_MASK / CapSense_TST_BASE_SNS_CLK_DIVIDER);
        #else
            scanDuration = CapSense_BIST_SCAN_DURATION;
        #endif
    }

    #if (CapSense_ENABLE == CapSense_CSDV2)
        /* Set Number Of Conversions based on scanning resolution */
        CY_SET_REG32(CapSense_SEQ_NORM_CNT_PTR, scanDuration & CapSense_SEQ_NORM_CNT_CONV_CNT_MASK);
    #else
        /* Set up a scanning resolution */
        CapSense_bistCounterResolution = scanDuration;
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}

/*******************************************************************************
* Function Name: CapSense_BistSetIdac
****************************************************************************//**
*
* \brief
*   This internal function changes the IDACs values for BIST scan.
*
* \details
*   The functions disables the compensation IDAC if enabled and sets
*   desired Idac value.
*
* \param idacValue
*   The Idac code to be set.
*
*******************************************************************************/
static void CapSense_BistSetIdac(uint32 idacValue)
{
    #if ((CapSense_ENABLE != CapSense_CSDV2) && \
        (CapSense_ENABLE != CapSense_CSD_IDAC_COMP_EN))
        uint32 regValue;
        uint8  interruptState;
    #endif

    #if (CapSense_ENABLE == CapSense_CSDV2)
        CY_SET_REG32(CapSense_IDAC_MOD_PTR, CapSense_BIST_IDACA_CFG | (idacValue & CapSense_IDAC_MOD_VAL_MASK));
        #if((CapSense_ENABLE == CapSense_ADC_EN) || (CapSense_ENABLE == CapSense_IDACB_USED))
            CY_SET_REG32(CapSense_IDAC_COMP_PTR, CapSense_BIST_IDACB_CFG);
        #endif
    #else
        #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
            CY_SET_REG32(CapSense_IDAC_PTR, CapSense_BIST_IDAC_CFG | (idacValue & CapSense_BIST_IDAC_MOD_VAL_MASK));
        #else
            interruptState = CyEnterCriticalSection();
            regValue = CY_GET_REG32(CapSense_IDAC_PTR);
            regValue &= ~(CapSense_BIST_IDAC_MOD_MASK);
            regValue |= (CapSense_BIST_IDAC_CFG | (idacValue & CapSense_BIST_IDAC_MOD_VAL_MASK));
            CY_SET_REG32(CapSense_IDAC_PTR, regValue);
            CyExitCriticalSection(interruptState);
        #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}

/*******************************************************************************
* Function Name: CapSense_BistCmodPrecharge
****************************************************************************//**
*
* \brief
*  This function initializes the Cmod charging to Vref.
*
* \details
*  For Third-generation HW block:
*  The function initializes the Cmod charging to Vref.
*  Then it waits until Cmod is completely charged
*  to Vref to have stable raw counts. The Software Watchdog Counter is implemented to
*  prevent project hanging.
*
*  For Fourth-generation HW block:
*  Coarse initialization for CMOD and Cch.
*  The coarse initialization is performed by HSCOMP.
*  The HSCOMP monitors the Cmod voltage via the Cmod sense path
*  and charges the Cmod using the HCAV switch via CSDBUS-A, AMUXBUS-A
*  and static connection of Cmod to AMUXBUS-A.
*
*******************************************************************************/
static void CapSense_BistCmodPrecharge(void)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)
        /* Fourth-generation HW block section */

        volatile uint32 watchdogCounter;
        uint32 nonVolWDC;
        
        #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && (CapSense_ENABLE == CapSense_CSD_EN))
            #if(0u != CapSense_CSD_SHIELD_TANK_EN)
                uint32 tmpRegValue;
            #endif
        #endif

        CY_SET_REG32(CapSense_CSDCMP_PTR, CapSense_BIST_CSDCMP_INIT);
        CY_SET_REG32(CapSense_SW_FW_MOD_SEL_PTR,  CapSense_BIST_SW_FW_MOD_SEL_INIT);
        CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR, CapSense_BIST_SW_FW_TANK_SEL_INIT);
        CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR,  CapSense_BIST_SW_SHIELD_SEL_INIT);

        /* Connect CMOD to (sense path) to HSCOMP: HMPM or HMPS or HMPT switches depend on Cmod connects to certain pad */
        CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, CapSense_BIST_SW_HS_P_SEL_COARSE);
        CY_SET_REG32(CapSense_SW_HS_N_SEL_PTR, CapSense_SW_HS_N_SEL_SW_HCRH_STATIC_CLOSE);

        #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && (CapSense_ENABLE == CapSense_CSD_EN))
            #if(CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)
                /* Verification of the bist mode */
                if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
                {
                    /* Connect CTANK to AMUXBUS-A */
                    tmpRegValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
                    tmpRegValue &= (uint32)(~(uint32)CapSense_CSH_HSIOM_MASK);
                    tmpRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CSH_HSIOM_SHIFT);
                    CY_SET_REG32(CapSense_CSH_HSIOM_PTR, tmpRegValue);
                }
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) */
        #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && (CapSense_ENABLE == CapSense_CSD_EN)) */

        CY_SET_REG32(CapSense_SW_RES_PTR, CapSense_BIST_SW_RES_INIT);

        /* Clear all interrupt pending requests */
        CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
        (void)CY_GET_REG32(CapSense_INTR_PTR);

        /* Unmask INIT interrupt (enable interrupt) */
        CY_SET_REG32(CapSense_INTR_MASK_PTR, CapSense_INTR_MASK_CLEAR_MASK);

        /* Start SEQUENCER for coarse initialization for Cmod */
        CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_SEQ_START_SEQ_MODE_MASK |
                                                     CapSense_SEQ_START_START_MASK);

        /* Init Watchdog Counter to prevent a hang */
        watchdogCounter = CapSense_BIST_PRECHARGE_WATCHDOG_CYCLES_NUM;
        nonVolWDC = watchdogCounter;
        /* Wait for INTR.INIT goes high */
        while((0u == (CY_GET_REG32(CapSense_INTR_PTR) & CapSense_INTR_INIT_MASK)) && (0u != nonVolWDC))
        {
            watchdogCounter--;
            nonVolWDC = watchdogCounter;
        }

        if (0u == watchdogCounter)
        {
            /* Set sequencer to idle state if coarse initialization fails */
            CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_SEQ_START_ABORT_MASK);
        }

        /* Set scanning configuration for switches */
        CY_SET_REG32(CapSense_SW_FW_MOD_SEL_PTR, CapSense_BIST_SW_FW_MOD_SEL_SCAN);
        CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR, CapSense_BIST_SW_FW_TANK_SEL_SCAN);

    #else /* (CapSense_ENABLE == CapSense_CSDV2) */

        uint32 newRegValue;
        volatile uint32 watchdogCounter;
        uint32 nonVolWDC;

        /* Save the current IDAC configuration */
        newRegValue = CY_GET_REG32(CapSense_IDAC_PTR);

        /* Wait until Cmod discharges below Vref */
        CyDelayCycles(CapSense_CMOD_DISCHARGE_CYCLES);

        /* Set the IDAC configuration for fast Cmod pre-charge to Vref */
        CY_SET_REG32(CapSense_IDAC_PTR, CapSense_CSD_IDAC_PRECHARGE_CONFIG);

        /* Initialize the watchdog counter to prevent a hang */
        watchdogCounter = CapSense_BIST_PRECHARGE_WATCHDOG_CYCLES_NUM;
        nonVolWDC = watchdogCounter;
        /* Wait until Cmod reaches Vref */
        while((0u == (CY_GET_REG32(CapSense_STATUS_PTR) & CapSense_STATUS_SAMPLE)) && (0u != nonVolWDC))
        {
            watchdogCounter--;
            nonVolWDC = watchdogCounter;
        }

        /* Restore the current IDAC configuration */
        CY_SET_REG32(CapSense_IDAC_PTR, newRegValue);

        /* Enable the sense modulator output */
        CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_bistConfigCsd | CapSense_BIST_CTANK_PRECHARGE_CONFIG_CSD_EN | CapSense_CONFIG_SENSE_EN_MASK);

        CyIntEnable(CapSense_ISR_NUMBER);

    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}

/*******************************************************************************
* Function Name: CapSense_WaitEndOfScan
****************************************************************************//**
*
* \brief
*   Waits the end of scan for capacitance measurements.
*
* \details
*   Apart the waiting end of scan the function sets a watchdog to prevent
*   a hangout
*
*******************************************************************************/
static void CapSense_WaitEndOfScan(uint32 timeout)
{
    volatile uint32 watchdogCounter = timeout;
    uint32 nonVolWDC = watchdogCounter;
    while (((*(volatile uint8 *)&CapSense_dsRam.status & CapSense_SW_STS_BUSY) != 0u) &&
           (0u != nonVolWDC))
    {
        watchdogCounter--;
        nonVolWDC = watchdogCounter;
    }
    (void)watchdogCounter;
}

/*******************************************************************************
* Function Name: CapSense_BistTriggerScan
****************************************************************************//**
*
* \brief
*  This function triggers the scanning.
*
* \details
*  For Third-generation HW block:
*  Writes a resolution to start scanning.
*
*  For Fourth-generation HW block:
*  Trigger the fine initialization (scan some dummy cycles only for sensor or
*  shield capacitance meterings) and start sampling.
*  For the fine initialization and sampling the measured capacitance is charged
*  from Idac and discharged to ground using the switches in GPIO cell.
*  The CSDCOMP monitors voltage on the measured capacitance using the sense path
*  and charges it to Vref using IDAC.
*
*******************************************************************************/
static void CapSense_BistTriggerScan(void)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)
        /* Fourth-generation HW block section */
        volatile uint32 watchdogCounter;
        uint32 nonVolWDC;
        
        /* Clear previous interrupts */
        CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
        (void)CY_GET_REG32(CapSense_INTR_PTR);

        /* Clear pending interrupts  */
        CyIntClearPending(CapSense_ISR_NUMBER);
        /* Enable the CSD interrupt */
        CyIntEnable(CapSense_ISR_NUMBER);

        CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, CapSense_BIST_HS_P_SEL_SCAN);
        #if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
        /* Verification of the bist mode */
            if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
            {
                #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                    CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR,  CapSense_BIST_SW_SHIELD_SEL_CFG);
                    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)
                        CapSense_BistEnableShieldTank();
                    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) */
                #else
                    CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR, CapSense_STATIC_OPEN);
                    CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, CapSense_STATIC_OPEN);
                #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
            }
            else
            {
                CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR, CapSense_STATIC_OPEN);
                CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR,   CapSense_STATIC_OPEN);
            }
        #else
            CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR, CapSense_STATIC_OPEN);
            CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR,   CapSense_STATIC_OPEN);
        #endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */

        CY_SET_REG32(CapSense_SW_RES_PTR, CapSense_BIST_SW_RES_SCAN);

        /* Enable CSDCMP */
        if (CapSense_BIST_HW_EXT_CAP_E == CapSense_bistHwConfig)
        {
            CY_SET_REG32(CapSense_CSDCMP_PTR,         CapSense_CSDCMP_TST_CAP_MEAS);
            CY_SET_REG32(CapSense_SW_FW_MOD_SEL_PTR,  CapSense_STATIC_OPEN);
            CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR, CapSense_STATIC_OPEN);
        }
        else
        {
            CY_SET_REG32(CapSense_CSDCMP_PTR, CapSense_CSDCMP_CSDCMP_EN_MASK);
        }

    /* Verification of the bist mode */
        if (CapSense_BIST_HW_SNS_CAP_CSD_E == CapSense_bistHwConfig)
        {
            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                CY_SET_REG32(CapSense_HSCMP_PTR, CapSense_BIST_HSCMP_SCAN_MASK);
            #else
                /* Disable HSCOMP during the sampling phase when shield is disabled */
                CY_SET_REG32(CapSense_HSCMP_PTR, 0uL);
            #endif /* (CapSense_DISABLE == CapSense_CSD_SHIELD_EN) */
        }
        else
        {
            /* Disable HSCOMP during the sampling phase when shield is disabled */
            CY_SET_REG32(CapSense_HSCMP_PTR, 0uL);
        }
        /* Enable SAMPLE interrupt */
        CY_SET_REG32(CapSense_INTR_MASK_PTR, CapSense_INTR_MASK_SAMPLE_MASK);

        /* Init Watchdog Counter to prevent a hang */
        watchdogCounter = CapSense_BIST_PRECHARGE_WATCHDOG_CYCLES_NUM;
        nonVolWDC = watchdogCounter;
        /* Wait for IDLE state of the Fourth-generation HW block sequencer */
        while((0u != (CapSense_STAT_SEQ_REG & CapSense_STAT_SEQ_SEQ_STATE_MASK)) && (0u != nonVolWDC))
        {
            watchdogCounter--;
            nonVolWDC = watchdogCounter;
        }

        /* Start SEQUENCER for fine initialization scan for Cmod and then for normal scan */
        CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_SEQ_START_AZ0_SKIP_MASK |
                                                     CapSense_SEQ_START_AZ1_SKIP_MASK |
                                                     CapSense_SEQ_START_START_MASK);

    #else

        /* Third-generation HW block section */

        /* Set a resolution to the counter register to start scanning */
        CY_SET_REG32(CapSense_COUNTER_PTR, CapSense_bistCounterResolution);

    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}
#endif /*(CapSense_ENABLE == CapSense_TST_MEASUREMENT_GROUP_EN) */

#if ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
     (CapSense_ENABLE == CapSense_CSDV2))
/*******************************************************************************
* Function Name: CapSense_GetVdda
****************************************************************************//**
*
* \brief
*  Measures and returns the VDDA voltage.
*
* \details
*  This function measures voltage on VDDA terminal of the chip and returns
*  the result, alternatively the result is stored in the
*  CapSense_VDDA_VOLTAGE_VALUE register of data structure.
*
* \return
*  The VDDA voltage in mV.
*
*******************************************************************************/
uint16 CapSense_GetVdda(void)
{
    uint16 tmpResult;

    CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_BIST_E);
    CapSense_BistSwitchHwConfig(CapSense_BIST_HW_VDDA_E);
    tmpResult = CapSense_BistVddaRunMeasurement();
    CapSense_dsRam.selfTest.vddaVoltage = tmpResult;

    return (tmpResult);
}


/*******************************************************************************
* Function Name: CapSense_BistEnableVddaMeasurement
****************************************************************************//**
*
* \brief
*  Configures the CSD block to be used for VDDA measurement.
*
* \details
*  Configures the IDACB, HW block internal switches, REFGEN, HSCOMP and
*  interrupt.
*
*******************************************************************************/
static void CapSense_BistEnableVddaMeasurement(void)
{
    /* Configure clocks */
    CapSense_SsSetModClkClockDivider(CapSense_TST_VDDA_MODCLK_DIV_DEFAULT);
    CY_SET_REG32(CapSense_SENSE_PERIOD_PTR, (CapSense_TST_VDDA_SENSE_DIV_DEFAULT - 1uL));

    /* Configure the IDAC */
    CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_TST_VDDA_CONFIG_DEFAULT_CFG);

    #if(CapSense_TST_ANALOG_STARTUP_DELAY_US > 0uL)
        CyDelayUs(CapSense_TST_ANALOG_STARTUP_DELAY_US);
    #endif /* (CapSense_TST_ANALOG_STARTUP_DELAY_US > 0uL) */

    CY_SET_REG32(CapSense_IDAC_COMP_PTR, CapSense_TST_VDDA_IDACB_DEFAULT_CFG);

    /* Configure AZ Time */
    CY_SET_REG32(CapSense_SEQ_TIME_PTR, (CapSense_TST_VDDA_SEQ_TIME_DEFAULT - 1u));

    CY_SET_REG32(CapSense_IDAC_MOD_PTR, CapSense_TST_VDDA_IDACA_DEFAULT_CFG);
    CY_SET_REG32(CapSense_CSDCMP_PTR, CapSense_TST_CSDCMP_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_DSI_SEL_PTR, CapSense_TST_SW_DSI_SEL_DEFAULT_CFG);

    CY_SET_REG32(CapSense_SENSE_DUTY_PTR, CapSense_TST_SENSE_DUTY_VDDA_CFG);
    CY_SET_REG32(CapSense_SEQ_INIT_CNT_PTR, CapSense_TST_SEQ_INIT_CNT_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SEQ_NORM_CNT_PTR, CapSense_TST_SEQ_NORM_CNT_DEFAULT_CFG);

    /* Configure the block-level routing */
    CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, CapSense_TST_SW_HS_P_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_HS_N_SEL_PTR, CapSense_TST_SW_HS_N_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR, CapSense_TST_SW_SHIELD_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_BYP_SEL_PTR, CapSense_TST_SW_BYP_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_TST_SW_CMP_P_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_CMP_N_SEL_PTR, CapSense_TST_SW_CMP_N_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_FW_MOD_SEL_PTR, CapSense_TST_SW_FW_MOD_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR, CapSense_TST_SW_FW_TANK_SEL_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_REFGEN_SEL_PTR, CapSense_TST_SW_REFGEN_SEL_DEFAULT_CFG);

    /* Config RefGen */
    CY_SET_REG32(CapSense_REFGEN_PTR, CapSense_TST_REFGEN_DEFAULT_CFG);
    CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_TST_SW_AMUXBUF_SEL_DEFAULT_CFG);

    /* Configure HSCOMP */
    CY_SET_REG32(CapSense_HSCMP_PTR, CapSense_TST_HSCMP_DEFAULT_CFG);
    CY_SET_REG32(CapSense_INTR_MASK_PTR, CapSense_INTR_MASK_CLEAR_MASK);
}


/*******************************************************************************
* Function Name: CapSense_BistVddaRunMeasurement
****************************************************************************//**
*
* \brief
*  This function triggers the VDDA measurement.
*
* \details
*  The function initiates a conversion, waits for completion and
*  returns the result in mV.
*  The function returns voltage in millivolts or
*  CapSense_TST_VDDA_BAD_RESULT if:
*   - The ADC conversion is not started
*   - The ADC conversion watch-dog triggered.
*
*******************************************************************************/
static uint16 CapSense_BistVddaRunMeasurement(void)
{
    uint16 tVdda2Vref;
    uint16 tVssa2Vref;
    uint16 tRecover;
    uint16 tmpResult;
    uint32 tmpVrefCal;
    uint32 registerValue;
    uint32 watchdogCounter;
    uint32 nonVolWDC;
    uint8 interruptState;
    uint8 tmpFlashByte0;
    uint8 tmpFlashByte1;

    watchdogCounter = CapSense_TST_SW_WATCHDOG_CYCLES_NUM;
    nonVolWDC = watchdogCounter;
    CY_SET_REG32(CapSense_ADC_CTL_PTR, (CapSense_TST_MEASMODE_VREF | (CapSense_VDDA_ACQUISITION_BASE - 1u)));
    CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_TST_VDDA_SEQ_START_MEASURE_CFG);

    /* Wait for INTR.INIT goes high */
    while((0u == (CY_GET_REG32(CapSense_INTR_PTR) & CapSense_INTR_ADC_RES_MASK)) && (0u != nonVolWDC))
    {
        watchdogCounter--;
        nonVolWDC = watchdogCounter;
    }

    /* Clear all interrupt pending requests */
    CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
    (void)CY_GET_REG32(CapSense_INTR_PTR);

    tVssa2Vref = (uint16)CY_GET_REG32(CapSense_ADC_RES_PTR);

    if(0uL != watchdogCounter)
    {
        watchdogCounter = CapSense_TST_SW_WATCHDOG_CYCLES_NUM;
        nonVolWDC = watchdogCounter;
        CY_SET_REG32(CapSense_ADC_CTL_PTR, (CapSense_TST_MEASMODE_VREFBY2 | (CapSense_VDDA_ACQUISITION_BASE - 1u)));
        CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_TST_VDDA_SEQ_START_MEASURE_CFG);

        /* Wait for INTR.INIT goes high */
        while((0u == (CY_GET_REG32(CapSense_INTR_PTR) & CapSense_INTR_ADC_RES_MASK)) && (0u != nonVolWDC))
        {
            watchdogCounter--;
            nonVolWDC = watchdogCounter;
        }

        CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
        (void)CY_GET_REG32(CapSense_INTR_PTR);
    }
    else
    {
        /* Do nothing. */
    }

    tRecover = (uint16)CY_GET_REG32(CapSense_ADC_RES_PTR);
    if(0uL != watchdogCounter)
    {
        /* Disconnect amuxbusB, Connect VDDA to csdbusB */
        interruptState = CyEnterCriticalSection();
        registerValue = CY_GET_REG32(CapSense_SW_BYP_SEL_PTR);
        registerValue &= (uint32)(~(CapSense_SW_BYP_SEL_SW_BYA_MASK | CapSense_SW_BYP_SEL_SW_BYB_MASK));
        CY_SET_REG32(CapSense_SW_BYP_SEL_PTR, registerValue);
        CyExitCriticalSection(interruptState);
        CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR, CapSense_SW_SHIELD_SEL_SW_HCBV_STATIC_CLOSE);

        watchdogCounter = CapSense_TST_SW_WATCHDOG_CYCLES_NUM;
        nonVolWDC = watchdogCounter;
        CY_SET_REG32(CapSense_ADC_CTL_PTR, (CapSense_TST_MEASMODE_VIN | (CapSense_VDDA_ACQUISITION_BASE - 1u)));
        CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_TST_VDDA_SEQ_START_MEASURE_CFG);

        /* Wait for INTR.INIT goes high */
        while((0u == (CY_GET_REG32(CapSense_INTR_PTR) & CapSense_INTR_ADC_RES_MASK)) && (0u != nonVolWDC))
        {
            watchdogCounter--;
            nonVolWDC = watchdogCounter;
        }

        CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
        (void)CY_GET_REG32(CapSense_INTR_PTR);

        tVdda2Vref = (uint16)CY_GET_REG32(CapSense_ADC_RES_PTR);
        /* Test whether the full range is 2-bits higher than adc resolution */

        tVdda2Vref = (uint16)(((((uint32)tVdda2Vref << 1u) * tRecover) + ((uint32)tVssa2Vref >> 1u)) / tVssa2Vref);

        /* Get Vref trim-value */
        tmpFlashByte0 = CY_GET_REG8(CYREG_SFLASH_CSDV2_CSD0_ADC_TRIM1);
        tmpFlashByte1 = CY_GET_REG8(CYREG_SFLASH_CSDV2_CSD0_ADC_TRIM2);

        tmpVrefCal = (uint32)tmpFlashByte0 | (((uint32)tmpFlashByte1) << 8u);

        /* Update nominal Vref to real Vref */
        tmpVrefCal *= CapSense_TST_VDDA_VREF_MV;
        tmpVrefCal /= CapSense_TST_VDDA_VREF_CALIB_USED;

        tmpResult = (uint16)tmpVrefCal;

        /* Calculate deviation of trim register */
        if (tmpVrefCal > CapSense_TST_VDDA_VREF_MV)
        {
            tmpVrefCal = tmpVrefCal - CapSense_TST_VDDA_VREF_MV;
        }
        else
        {
            tmpVrefCal = CapSense_TST_VDDA_VREF_MV - tmpVrefCal;
        }
        tmpVrefCal = (tmpVrefCal * CapSense_PERCENTAGE_100) / CapSense_TST_VDDA_VREF_MV;

        /* Use nominal Vref if trim-value is not within the allowed range */
        if (CapSense_TST_VDDA_VREF_TRIM_MAX_DEVIATION < tmpVrefCal)
        {
            tmpResult = CapSense_TST_VDDA_VREF_MV;
        }

        tmpResult = tmpResult + (((tmpResult * tVdda2Vref) + (tVssa2Vref >> 1uL)) / tVssa2Vref);
    }
    else
    {
        tmpResult = CapSense_TST_VDDA_BAD_RESULT;
    }

    return(tmpResult);
}

#endif /* ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
           (CapSense_ENABLE == CapSense_CSDV2)) */

#if (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN)

/*******************************************************************************
* Function Name: CapSense_BistSwitchHwConfig
****************************************************************************//**
*
* \brief
*   The function switches HW configuration.
*
* \details
*   The function checks the current HW configuration of CapSense block.
*   If it differs from a desired configuration the function disables the current
*   configuration and sets the desired one.
*
* \param config
*   Specifies desired configuration.
*
*******************************************************************************/
static void CapSense_BistSwitchHwConfig(CapSense_BIST_HW_CONFIG_ENUM config)
{
    if (CapSense_bistHwConfig != config)
    {
        /* The requested config differes to the current one. Disable the current config */
        switch(CapSense_bistHwConfig)
        {
            #if (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN)
                case CapSense_BIST_HW_SNS_SHORT_E:
                {
                    /* Nothing to do */
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN) */
            #if (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN)
                case CapSense_BIST_HW_S2S_SHORT_E:
                {
                    /* Nothing to do */
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN) */
            #if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
                case CapSense_BIST_HW_SNS_CAP_CSD_E:
                {
                    CapSense_BistDisableHwSnsCap();
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */
            #if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
                 (CapSense_ENABLE == CapSense_CSX_EN))
                case CapSense_BIST_HW_SNS_CAP_CSX_E:
                {
                    CapSense_BistDisableHwSnsCap();
                    break;
                }
            #endif /* ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
                       (CapSense_ENABLE == CapSense_CSX_EN)) */
            #if (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)
                case CapSense_BIST_HW_SH_CAP_E:
                {
                    CapSense_BistDisableHwSnsCap();
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) */
            #if (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN)
                case CapSense_BIST_HW_EXT_CAP_E:
                {
                    /* Nothing to do */
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN) */
            #if (CapSense_ENABLE == CapSense_TST_VDDA_EN)
                case CapSense_BIST_HW_VDDA_E:
                {
                    /* Nothing to do */
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_VDDA_EN) */
            default:
            {
                break;
            }
        }

        CapSense_bistHwConfig = config;

        /* Enable the specified mode */
        switch(config)
        {
            #if (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN)
                case CapSense_BIST_HW_SNS_SHORT_E:
                {
                    /* No HW CSD Block changes required */
                    CapSense_SwitchAllIOState(CapSense_BIST_IO_HIGHZA_E);
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN) */
            #if (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN)
                case CapSense_BIST_HW_S2S_SHORT_E:
                {
                    /* No HW CSD Block changes required */
                    CapSense_SwitchAllIOState(CapSense_BIST_IO_STRONG_E);
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN) */
            #if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
                case CapSense_BIST_HW_SNS_CAP_CSD_E:
                {
                    CapSense_BistEnableHwSnsCap();
                    CapSense_SwitchAllIOState(CapSense_BIST_IO_CSD_E);
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */
            #if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
                 (CapSense_ENABLE == CapSense_CSX_EN))
                case CapSense_BIST_HW_SNS_CAP_CSX_E:
                {
                    CapSense_BistEnableHwSnsCap();
                    CapSense_SwitchAllIOState(CapSense_BIST_IO_STRONG_E);
                    break;
                }
            #endif /* ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) && \
                       (CapSense_ENABLE == CapSense_CSX_EN)) */
            #if (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)
                case CapSense_BIST_HW_SH_CAP_E:
                {
                    CapSense_BistEnableHwSnsCap();
                    CapSense_SwitchAllIOState(CapSense_BIST_IO_SHIELD_E);
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) */
            #if (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN)
                case CapSense_BIST_HW_EXT_CAP_E:
                {
                    CapSense_BistEnableHwSnsCap();
                    CapSense_SwitchAllIOState(CapSense_BIST_IO_HIGHZA_E);
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN) */
            #if ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
                 (CapSense_ENABLE == CapSense_CSDV2))
                case CapSense_BIST_HW_VDDA_E:
                {
                    CapSense_BistEnableVddaMeasurement();
                    break;
                }
            #endif /* ((CapSense_ENABLE == CapSense_TST_VDDA_EN) &&\
                       (CapSense_ENABLE == CapSense_CSDV2)) */
            default:
            {
                break;
            }
        }
    }
}

/*******************************************************************************
* Function Name: CapSense_SwitchAllIOState
****************************************************************************//**
*
* \brief
*   Sets all the sensors pins in a desired state.
*
* \details
*   Sets all the sensors and shield (if defined) pins in a desired state.
*
* \param config
*   Specifies desired IO configuration.
*
*******************************************************************************/
static void CapSense_SwitchAllIOState(CapSense_BIST_IO_CONFIG_ENUM config)
{
    /* Re-configure the IOs if mode is changed */
    if (CapSense_bistIoConfig != config)
    {
        switch(config)
        {
            case CapSense_BIST_IO_STRONG_E:
            {
                CapSense_SetAllIOPcState(CapSense_TST_PC_STRONG);
                break;
            }
            case CapSense_BIST_IO_HIGHZA_E:
            {
                CapSense_SetAllIOPcState(CapSense_TST_PC_HIGHZA);
                break;
            }
            #if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
                case CapSense_BIST_IO_CSD_E:
                {
                    /* Configure CSX sensors to Strong */
                    #if (0u != CapSense_CSX_EN)
                        /* Set all IO to strong (including CSX IOs). It is faster than loop through only CSX sensors */
                        CapSense_SetAllIOPcState(CapSense_TST_PC_STRONG);
                    #endif

                    #if (CapSense_ENABLE == CapSense_CSD_EN)
                        /* Reconfigure only CSD sensors to required state */
                        CapSense_BistSetAllInactiveSensorConnection();
                    #endif

                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */
            #if (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)
                case CapSense_BIST_IO_SHIELD_E:
                {
                    /* Configure CSX sensors to Strong */
                    #if (CapSense_ENABLE == CapSense_CSX_EN)
                        /* Set all IO to strong (including CSX IOs). It is faster than loop through only CSX sensors */
                        CapSense_SetAllIOPcState(CapSense_TST_PC_STRONG);
                    #endif
                        /* Reconfigure only CSD sensors to required state */
                        CapSense_BistSetAllInactiveSensorConnection();
                    break;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) */
            default:
            {
                break;
            }
        }
        /* Save the configured state */
        CapSense_bistIoConfig = config;
    }
}

/*******************************************************************************
* Function Name: CapSense_SetElectrodeDr
****************************************************************************//**
*
* \brief
*  The internal function that sets a certain electrode port output data.
*
* \details
*  The function sets an electrode port output data register (DR) in a demand state.
*
* \param *ioPtr
*  A pointer to the specified electrode in the flash IO structure.
*
* \param value
*  A port output data which will be setted for an electrode.
*
*******************************************************************************/
static void CapSense_SetElectrodeDr(CapSense_FLASH_IO_STRUCT const *ioPtr, uint32 value)
{
    uint32 regValue;
    uint8  interruptState;

    /* Set a data register */
    interruptState = CyEnterCriticalSection();
    regValue = CY_GET_REG32(ioPtr->drPtr);
    regValue &= ~(ioPtr->mask);
    regValue |= value << ioPtr->drShift;
    CY_SET_REG32(ioPtr->drPtr, regValue);
    CyExitCriticalSection(interruptState);
}

/*******************************************************************************
* Function Name: CapSense_SetElectrodePc
****************************************************************************//**
*
* \brief
*  The internal function that sets a certain electrode drive mode.
*
* \details
*  The function sets an electrode drive mode (PC register) in a demand state.
*
* \param *ioPtr
*  A pointer to the specified electrode in the flash IO structure.
*
* \param value
*  A drive mode which will be setted for an electrode.
*
*******************************************************************************/
static void CapSense_SetElectrodePc(CapSense_FLASH_IO_STRUCT const *ioPtr, uint32 value)
{
    uint32 regValue;
    uint8  interruptState;

    /* Update port configuration register (drive mode) */
    interruptState = CyEnterCriticalSection();
    regValue = CY_GET_REG32(ioPtr->pcPtr);
    regValue &= ~(CapSense_GPIO_PC_MASK << ioPtr->shift);
    regValue |= value << ioPtr->shift;
    CY_SET_REG32(ioPtr->pcPtr, regValue);
    CyExitCriticalSection(interruptState);
}

#if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
    (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))

/*******************************************************************************
* Function Name: CapSense_SetAllIOHsiomState
****************************************************************************//**
*
* \brief
*  The internal function that sets all pins in a demand HSIOM mode.
*
* \details
*  The function sets all pins to a demand HSIOM mode.
*
* \param value
*  A demand HSIOM mode.
*
*******************************************************************************/
static void CapSense_SetAllIOHsiomState(uint32 value)
{
    uint32 loopIndex;
    CapSense_FLASH_IO_STRUCT const *ioPtr = &CapSense_ioList[0u];

    /* Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < CapSense_TOTAL_ELECTRODES; loopIndex++)
    {
        CapSense_SetElectrodeHsiom(ioPtr, value);
        ioPtr++;
    }
}
#endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
          (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)) */

/*******************************************************************************
* Function Name: CapSense_SetAllIODrState
****************************************************************************//**
*
* \brief
*  The internal function that sets a specific port output data for all electrodes.
*
* \details
*  The function sets a specific port output data register (DR) in a demand state for all electrodes.
*
* \param value
*  A port output data which will be set for all electrodes.
*
*******************************************************************************/
static void CapSense_SetAllIODrState(uint32 value)
{
    uint32 loopIndex;
    CapSense_FLASH_IO_STRUCT const *ioPtr = &CapSense_ioList[0u];

    /* Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < CapSense_TOTAL_ELECTRODES; loopIndex++)
    {
        CapSense_SetElectrodeDr(ioPtr, value);
        ioPtr++;
    }
}

/*******************************************************************************
* Function Name: CapSense_SetAllIOPcState
****************************************************************************//**
*
* \brief
*  The internal function that sets a specific drive mode for all electrodes.
*
* \details
*  The function sets a specific drive mode (PC register) in a demand state for all electrodes.
*
* \param value
*  A drive mode which will be set for all electrodes.
*
*******************************************************************************/
static void CapSense_SetAllIOPcState(uint32 value)
{
    uint32 loopIndex;
    CapSense_FLASH_IO_STRUCT const *ioPtr = &CapSense_ioList[0u];

    /* Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < CapSense_TOTAL_ELECTRODES; loopIndex++)
    {
        CapSense_SetElectrodePc(ioPtr, value);
        ioPtr++;
    }
}

#endif /* (CapSense_ENABLE == CapSense_TST_HW_GROUP_EN) */

#if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) || \
    ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
     (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)))

/*******************************************************************************
* Function Name: CapSense_SetElectrodeHsiom
****************************************************************************//**
*
* \brief
*  The internal function that sets a certain electrode in a demand HSIOM mode.
*
* \details
*  The function sets an electrode to a demand HSIOM mode.
*
* \param *ioPtr
*  A pointer to the specified electrode in the flash IO structure.
*
* \param value
*  A demand HSIOM mode.
*
*******************************************************************************/
static void CapSense_SetElectrodeHsiom(CapSense_FLASH_IO_STRUCT const *ioPtr, uint32 value)
{
    uint32 regValue;
    uint8  interruptState;

    /* Update HSIOM port select register */
    interruptState = CyEnterCriticalSection();
    regValue = CY_GET_REG32(ioPtr->hsiomPtr);
    regValue &= ~(ioPtr->hsiomMask);
    regValue |= (value << ioPtr->hsiomShift);
    CY_SET_REG32(ioPtr->hsiomPtr, regValue);
    CyExitCriticalSection(interruptState);
}
#endif /* ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
           (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) || \
          ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
           (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))) */


/* [] END OF FILE */
