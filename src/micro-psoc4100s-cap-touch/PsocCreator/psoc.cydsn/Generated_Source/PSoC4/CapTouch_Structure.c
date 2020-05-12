/***************************************************************************//**
* \file CapTouch_Structure.c
* \version 6.0
*
* \brief
*   This file defines the data structure global variables and provides implementation
*   for the high-level and low-level APIs of the Data Structure module.
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

#include <cytypes.h>
#include "CyLib.h"
#include "CapTouch_Structure.h"
#include "CapTouch_Configuration.h"

#if (0u != CapTouch_ADC_EN)
    #include "CapTouch_Adc.h"
#endif /* (0u != CapTouch_ADC_EN) */

#if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)
    #include "CapTouch_SelfTest.h"
#endif

#if (CapTouch_ENABLE == CapTouch_CENTROID_5X5_CSD_EN)
    #include "CapTouch_AdvancedCentroid_LL.h"
#endif

#if (CapTouch_ENABLE == CapTouch_GES_GLOBAL_EN)
    #include "CapTouch_Gesture.h"
#endif /* (CapTouch_ENABLE == CapTouch_GES_GLOBAL_EN) */

/*******************************************************************************
* Defines the RAM Data Structure variables and their init data in flash
*******************************************************************************/
/**
* \cond SECTION_GLOBAL_VARIABLES
* \addtogroup group_global_variables
* \{
*/

/**
* The variable that contains the CapTouch configuration, settings and
* scanning results. CapTouch_dsRam represents RAM Data Structure.
*/
CapTouch_RAM_STRUCT CapTouch_dsRam;
/** \}
* \endcond */

/*******************************************************************************
* Declares Widget's De-bounce Counters
*******************************************************************************/
static uint8 CapTouch_debounceU102[CapTouch_U102_NUM_SENSORS];
static uint8 CapTouch_debounceU103[CapTouch_U103_NUM_SENSORS];
static uint8 CapTouch_debounceU104[CapTouch_U104_NUM_SENSORS];
static uint8 CapTouch_debounceU105[CapTouch_U105_NUM_SENSORS];
static uint8 CapTouch_debounceU106[CapTouch_U106_NUM_SENSORS];
static uint8 CapTouch_debounceU107[CapTouch_U107_NUM_SENSORS];
static uint8 CapTouch_debounceU108[CapTouch_U108_NUM_SENSORS];
static uint8 CapTouch_debounceU109[CapTouch_U109_NUM_SENSORS];
static uint8 CapTouch_debounceU110[CapTouch_U110_NUM_SENSORS];
static uint8 CapTouch_debounceU111[CapTouch_U111_NUM_SENSORS];
static uint8 CapTouch_debounceU112[CapTouch_U112_NUM_SENSORS];
static uint8 CapTouch_debounceU113[CapTouch_U113_NUM_SENSORS];
static uint8 CapTouch_debounceU114[CapTouch_U114_NUM_SENSORS];


/***************************************************************************//**
* Declares Filter History Objects
*******************************************************************************/
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU102[CapTouch_U102_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU103[CapTouch_U103_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU104[CapTouch_U104_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU105[CapTouch_U105_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU106[CapTouch_U106_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU107[CapTouch_U107_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU108[CapTouch_U108_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU109[CapTouch_U109_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU110[CapTouch_U110_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU111[CapTouch_U111_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU112[CapTouch_U112_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU113[CapTouch_U113_NUM_SENSORS];
static CapTouch_REGULAR_FLTR_STRUCT CapTouch_fltrHistoryU114[CapTouch_U114_NUM_SENSORS];

/*******************************************************************************
* Defines and initializes the Flash Data Structure
*******************************************************************************/



/**
* \cond SECTION_API_CONSTANTS
* \addtogroup group_api_constants
* \{
*/
/**
* Constant for the FLASH Data Structure
*/
const CapTouch_FLASH_STRUCT CapTouch_dsFlash =
{
    /* Flash Widget Initialization Data */
    {
        { /* U102 */
            &CapTouch_ioList[0u],
            (void *)&CapTouch_dsRam.wdgtList.u102,
            CapTouch_dsRam.snsList.u102,
            (void *)CapTouch_fltrHistoryU102,
            CapTouch_debounceU102,
            CapTouch_U102_STATIC_CONFIG,
            CapTouch_U102_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U102_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u102,
        },
        { /* U103 */
            &CapTouch_ioList[2u],
            (void *)&CapTouch_dsRam.wdgtList.u103,
            CapTouch_dsRam.snsList.u103,
            (void *)CapTouch_fltrHistoryU103,
            CapTouch_debounceU103,
            CapTouch_U103_STATIC_CONFIG,
            CapTouch_U103_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U103_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u103,
        },
        { /* U104 */
            &CapTouch_ioList[4u],
            (void *)&CapTouch_dsRam.wdgtList.u104,
            CapTouch_dsRam.snsList.u104,
            (void *)CapTouch_fltrHistoryU104,
            CapTouch_debounceU104,
            CapTouch_U104_STATIC_CONFIG,
            CapTouch_U104_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U104_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u104,
        },
        { /* U105 */
            &CapTouch_ioList[6u],
            (void *)&CapTouch_dsRam.wdgtList.u105,
            CapTouch_dsRam.snsList.u105,
            (void *)CapTouch_fltrHistoryU105,
            CapTouch_debounceU105,
            CapTouch_U105_STATIC_CONFIG,
            CapTouch_U105_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U105_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u105,
        },
        { /* U106 */
            &CapTouch_ioList[8u],
            (void *)&CapTouch_dsRam.wdgtList.u106,
            CapTouch_dsRam.snsList.u106,
            (void *)CapTouch_fltrHistoryU106,
            CapTouch_debounceU106,
            CapTouch_U106_STATIC_CONFIG,
            CapTouch_U106_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U106_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u106,
        },
        { /* U107 */
            &CapTouch_ioList[10u],
            (void *)&CapTouch_dsRam.wdgtList.u107,
            CapTouch_dsRam.snsList.u107,
            (void *)CapTouch_fltrHistoryU107,
            CapTouch_debounceU107,
            CapTouch_U107_STATIC_CONFIG,
            CapTouch_U107_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U107_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u107,
        },
        { /* U108 */
            &CapTouch_ioList[12u],
            (void *)&CapTouch_dsRam.wdgtList.u108,
            CapTouch_dsRam.snsList.u108,
            (void *)CapTouch_fltrHistoryU108,
            CapTouch_debounceU108,
            CapTouch_U108_STATIC_CONFIG,
            CapTouch_U108_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U108_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u108,
        },
        { /* U109 */
            &CapTouch_ioList[14u],
            (void *)&CapTouch_dsRam.wdgtList.u109,
            CapTouch_dsRam.snsList.u109,
            (void *)CapTouch_fltrHistoryU109,
            CapTouch_debounceU109,
            CapTouch_U109_STATIC_CONFIG,
            CapTouch_U109_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U109_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u109,
        },
        { /* U110 */
            &CapTouch_ioList[16u],
            (void *)&CapTouch_dsRam.wdgtList.u110,
            CapTouch_dsRam.snsList.u110,
            (void *)CapTouch_fltrHistoryU110,
            CapTouch_debounceU110,
            CapTouch_U110_STATIC_CONFIG,
            CapTouch_U110_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U110_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u110,
        },
        { /* U111 */
            &CapTouch_ioList[18u],
            (void *)&CapTouch_dsRam.wdgtList.u111,
            CapTouch_dsRam.snsList.u111,
            (void *)CapTouch_fltrHistoryU111,
            CapTouch_debounceU111,
            CapTouch_U111_STATIC_CONFIG,
            CapTouch_U111_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U111_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u111,
        },
        { /* U112 */
            &CapTouch_ioList[20u],
            (void *)&CapTouch_dsRam.wdgtList.u112,
            CapTouch_dsRam.snsList.u112,
            (void *)CapTouch_fltrHistoryU112,
            CapTouch_debounceU112,
            CapTouch_U112_STATIC_CONFIG,
            CapTouch_U112_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U112_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u112,
        },
        { /* U113 */
            &CapTouch_ioList[22u],
            (void *)&CapTouch_dsRam.wdgtList.u113,
            CapTouch_dsRam.snsList.u113,
            (void *)CapTouch_fltrHistoryU113,
            CapTouch_debounceU113,
            CapTouch_U113_STATIC_CONFIG,
            CapTouch_U113_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U113_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u113,
        },
        { /* U114 */
            &CapTouch_ioList[24u],
            (void *)&CapTouch_dsRam.wdgtList.u114,
            CapTouch_dsRam.snsList.u114,
            (void *)CapTouch_fltrHistoryU114,
            CapTouch_debounceU114,
            CapTouch_U114_STATIC_CONFIG,
            CapTouch_U114_NUM_SENSORS,
            (uint8)CapTouch_WD_BUTTON_E,
            CapTouch_U114_NUM_SENSORS,
            1u,
            CapTouch_dsRam.snsCp.u114,
        },
    },
};

/**
* The array of the pointers to the electrode specific register.
*/
const CapTouch_FLASH_IO_STRUCT CapTouch_ioList[CapTouch_TOTAL_ELECTRODES] =
{
    { /* 0: U102_Rx0 */
        (reg32 *)CapTouch_Rx__0__HSIOM,
        (reg32 *)CapTouch_Rx__0__PC,
        (reg32 *)CapTouch_Rx__0__PC2,
        (reg32 *)CapTouch_Rx__0__DR,
        (reg32 *)CapTouch_Rx__0__PS,
        CapTouch_Rx__0__HSIOM_MASK,
        CapTouch_Rx__0__MASK,
        CapTouch_Rx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__0__SHIFT,
        (uint8)CapTouch_Rx__0__SHIFT * 3u,
    },
    { /* 1: U102_Tx */
        (reg32 *)CapTouch_Tx__0__HSIOM,
        (reg32 *)CapTouch_Tx__0__PC,
        (reg32 *)CapTouch_Tx__0__PC2,
        (reg32 *)CapTouch_Tx__0__DR,
        (reg32 *)CapTouch_Tx__0__PS,
        CapTouch_Tx__0__HSIOM_MASK,
        CapTouch_Tx__0__MASK,
        CapTouch_Tx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT * 3u,
    },
    { /* 2: U102_Rx0 ganged to U103_Rx0 */
        (reg32 *)CapTouch_Rx__0__HSIOM,
        (reg32 *)CapTouch_Rx__0__PC,
        (reg32 *)CapTouch_Rx__0__PC2,
        (reg32 *)CapTouch_Rx__0__DR,
        (reg32 *)CapTouch_Rx__0__PS,
        CapTouch_Rx__0__HSIOM_MASK,
        CapTouch_Rx__0__MASK,
        CapTouch_Rx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__0__SHIFT,
        (uint8)CapTouch_Rx__0__SHIFT * 3u,
    },
    { /* 3: U103_Tx */
        (reg32 *)CapTouch_Tx__1__HSIOM,
        (reg32 *)CapTouch_Tx__1__PC,
        (reg32 *)CapTouch_Tx__1__PC2,
        (reg32 *)CapTouch_Tx__1__DR,
        (reg32 *)CapTouch_Tx__1__PS,
        CapTouch_Tx__1__HSIOM_MASK,
        CapTouch_Tx__1__MASK,
        CapTouch_Tx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT * 3u,
    },
    { /* 4: U102_Rx0 ganged to U104_Rx0 */
        (reg32 *)CapTouch_Rx__0__HSIOM,
        (reg32 *)CapTouch_Rx__0__PC,
        (reg32 *)CapTouch_Rx__0__PC2,
        (reg32 *)CapTouch_Rx__0__DR,
        (reg32 *)CapTouch_Rx__0__PS,
        CapTouch_Rx__0__HSIOM_MASK,
        CapTouch_Rx__0__MASK,
        CapTouch_Rx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__0__SHIFT,
        (uint8)CapTouch_Rx__0__SHIFT * 3u,
    },
    { /* 5: U104_Tx */
        (reg32 *)CapTouch_Tx__2__HSIOM,
        (reg32 *)CapTouch_Tx__2__PC,
        (reg32 *)CapTouch_Tx__2__PC2,
        (reg32 *)CapTouch_Tx__2__DR,
        (reg32 *)CapTouch_Tx__2__PS,
        CapTouch_Tx__2__HSIOM_MASK,
        CapTouch_Tx__2__MASK,
        CapTouch_Tx__2__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__2__SHIFT,
        (uint8)CapTouch_Tx__2__SHIFT * 3u,
    },
    { /* 6: U105_Rx0 */
        (reg32 *)CapTouch_Rx__1__HSIOM,
        (reg32 *)CapTouch_Rx__1__PC,
        (reg32 *)CapTouch_Rx__1__PC2,
        (reg32 *)CapTouch_Rx__1__DR,
        (reg32 *)CapTouch_Rx__1__PS,
        CapTouch_Rx__1__HSIOM_MASK,
        CapTouch_Rx__1__MASK,
        CapTouch_Rx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__1__SHIFT,
        (uint8)CapTouch_Rx__1__SHIFT * 3u,
    },
    { /* 7: U102_Tx ganged to U105_Tx */
        (reg32 *)CapTouch_Tx__0__HSIOM,
        (reg32 *)CapTouch_Tx__0__PC,
        (reg32 *)CapTouch_Tx__0__PC2,
        (reg32 *)CapTouch_Tx__0__DR,
        (reg32 *)CapTouch_Tx__0__PS,
        CapTouch_Tx__0__HSIOM_MASK,
        CapTouch_Tx__0__MASK,
        CapTouch_Tx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT * 3u,
    },
    { /* 8: U105_Rx0 ganged to U106_Rx0 */
        (reg32 *)CapTouch_Rx__1__HSIOM,
        (reg32 *)CapTouch_Rx__1__PC,
        (reg32 *)CapTouch_Rx__1__PC2,
        (reg32 *)CapTouch_Rx__1__DR,
        (reg32 *)CapTouch_Rx__1__PS,
        CapTouch_Rx__1__HSIOM_MASK,
        CapTouch_Rx__1__MASK,
        CapTouch_Rx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__1__SHIFT,
        (uint8)CapTouch_Rx__1__SHIFT * 3u,
    },
    { /* 9: U103_Tx ganged to U106_Tx */
        (reg32 *)CapTouch_Tx__1__HSIOM,
        (reg32 *)CapTouch_Tx__1__PC,
        (reg32 *)CapTouch_Tx__1__PC2,
        (reg32 *)CapTouch_Tx__1__DR,
        (reg32 *)CapTouch_Tx__1__PS,
        CapTouch_Tx__1__HSIOM_MASK,
        CapTouch_Tx__1__MASK,
        CapTouch_Tx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT * 3u,
    },
    { /* 10: U105_Rx0 ganged to U107_Rx0 */
        (reg32 *)CapTouch_Rx__1__HSIOM,
        (reg32 *)CapTouch_Rx__1__PC,
        (reg32 *)CapTouch_Rx__1__PC2,
        (reg32 *)CapTouch_Rx__1__DR,
        (reg32 *)CapTouch_Rx__1__PS,
        CapTouch_Rx__1__HSIOM_MASK,
        CapTouch_Rx__1__MASK,
        CapTouch_Rx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__1__SHIFT,
        (uint8)CapTouch_Rx__1__SHIFT * 3u,
    },
    { /* 11: U104_Tx ganged to U107_Tx */
        (reg32 *)CapTouch_Tx__2__HSIOM,
        (reg32 *)CapTouch_Tx__2__PC,
        (reg32 *)CapTouch_Tx__2__PC2,
        (reg32 *)CapTouch_Tx__2__DR,
        (reg32 *)CapTouch_Tx__2__PS,
        CapTouch_Tx__2__HSIOM_MASK,
        CapTouch_Tx__2__MASK,
        CapTouch_Tx__2__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__2__SHIFT,
        (uint8)CapTouch_Tx__2__SHIFT * 3u,
    },
    { /* 12: U108_Rx0 */
        (reg32 *)CapTouch_Rx__2__HSIOM,
        (reg32 *)CapTouch_Rx__2__PC,
        (reg32 *)CapTouch_Rx__2__PC2,
        (reg32 *)CapTouch_Rx__2__DR,
        (reg32 *)CapTouch_Rx__2__PS,
        CapTouch_Rx__2__HSIOM_MASK,
        CapTouch_Rx__2__MASK,
        CapTouch_Rx__2__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__2__SHIFT,
        (uint8)CapTouch_Rx__2__SHIFT * 3u,
    },
    { /* 13: U102_Tx ganged to U108_Tx */
        (reg32 *)CapTouch_Tx__0__HSIOM,
        (reg32 *)CapTouch_Tx__0__PC,
        (reg32 *)CapTouch_Tx__0__PC2,
        (reg32 *)CapTouch_Tx__0__DR,
        (reg32 *)CapTouch_Tx__0__PS,
        CapTouch_Tx__0__HSIOM_MASK,
        CapTouch_Tx__0__MASK,
        CapTouch_Tx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT * 3u,
    },
    { /* 14: U108_Rx0 ganged to U109_Rx0 */
        (reg32 *)CapTouch_Rx__2__HSIOM,
        (reg32 *)CapTouch_Rx__2__PC,
        (reg32 *)CapTouch_Rx__2__PC2,
        (reg32 *)CapTouch_Rx__2__DR,
        (reg32 *)CapTouch_Rx__2__PS,
        CapTouch_Rx__2__HSIOM_MASK,
        CapTouch_Rx__2__MASK,
        CapTouch_Rx__2__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__2__SHIFT,
        (uint8)CapTouch_Rx__2__SHIFT * 3u,
    },
    { /* 15: U103_Tx ganged to U109_Tx */
        (reg32 *)CapTouch_Tx__1__HSIOM,
        (reg32 *)CapTouch_Tx__1__PC,
        (reg32 *)CapTouch_Tx__1__PC2,
        (reg32 *)CapTouch_Tx__1__DR,
        (reg32 *)CapTouch_Tx__1__PS,
        CapTouch_Tx__1__HSIOM_MASK,
        CapTouch_Tx__1__MASK,
        CapTouch_Tx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT * 3u,
    },
    { /* 16: U108_Rx0 ganged to U110_Rx0 */
        (reg32 *)CapTouch_Rx__2__HSIOM,
        (reg32 *)CapTouch_Rx__2__PC,
        (reg32 *)CapTouch_Rx__2__PC2,
        (reg32 *)CapTouch_Rx__2__DR,
        (reg32 *)CapTouch_Rx__2__PS,
        CapTouch_Rx__2__HSIOM_MASK,
        CapTouch_Rx__2__MASK,
        CapTouch_Rx__2__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__2__SHIFT,
        (uint8)CapTouch_Rx__2__SHIFT * 3u,
    },
    { /* 17: U104_Tx ganged to U110_Tx */
        (reg32 *)CapTouch_Tx__2__HSIOM,
        (reg32 *)CapTouch_Tx__2__PC,
        (reg32 *)CapTouch_Tx__2__PC2,
        (reg32 *)CapTouch_Tx__2__DR,
        (reg32 *)CapTouch_Tx__2__PS,
        CapTouch_Tx__2__HSIOM_MASK,
        CapTouch_Tx__2__MASK,
        CapTouch_Tx__2__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__2__SHIFT,
        (uint8)CapTouch_Tx__2__SHIFT * 3u,
    },
    { /* 18: U111_Rx0 */
        (reg32 *)CapTouch_Rx__3__HSIOM,
        (reg32 *)CapTouch_Rx__3__PC,
        (reg32 *)CapTouch_Rx__3__PC2,
        (reg32 *)CapTouch_Rx__3__DR,
        (reg32 *)CapTouch_Rx__3__PS,
        CapTouch_Rx__3__HSIOM_MASK,
        CapTouch_Rx__3__MASK,
        CapTouch_Rx__3__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__3__SHIFT,
        (uint8)CapTouch_Rx__3__SHIFT * 3u,
    },
    { /* 19: U102_Tx ganged to U111_Tx */
        (reg32 *)CapTouch_Tx__0__HSIOM,
        (reg32 *)CapTouch_Tx__0__PC,
        (reg32 *)CapTouch_Tx__0__PC2,
        (reg32 *)CapTouch_Tx__0__DR,
        (reg32 *)CapTouch_Tx__0__PS,
        CapTouch_Tx__0__HSIOM_MASK,
        CapTouch_Tx__0__MASK,
        CapTouch_Tx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT * 3u,
    },
    { /* 20: U111_Rx0 ganged to U112_Rx0 */
        (reg32 *)CapTouch_Rx__3__HSIOM,
        (reg32 *)CapTouch_Rx__3__PC,
        (reg32 *)CapTouch_Rx__3__PC2,
        (reg32 *)CapTouch_Rx__3__DR,
        (reg32 *)CapTouch_Rx__3__PS,
        CapTouch_Rx__3__HSIOM_MASK,
        CapTouch_Rx__3__MASK,
        CapTouch_Rx__3__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__3__SHIFT,
        (uint8)CapTouch_Rx__3__SHIFT * 3u,
    },
    { /* 21: U103_Tx ganged to U112_Tx */
        (reg32 *)CapTouch_Tx__1__HSIOM,
        (reg32 *)CapTouch_Tx__1__PC,
        (reg32 *)CapTouch_Tx__1__PC2,
        (reg32 *)CapTouch_Tx__1__DR,
        (reg32 *)CapTouch_Tx__1__PS,
        CapTouch_Tx__1__HSIOM_MASK,
        CapTouch_Tx__1__MASK,
        CapTouch_Tx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT * 3u,
    },
    { /* 22: U113_Rx0 */
        (reg32 *)CapTouch_Rx__4__HSIOM,
        (reg32 *)CapTouch_Rx__4__PC,
        (reg32 *)CapTouch_Rx__4__PC2,
        (reg32 *)CapTouch_Rx__4__DR,
        (reg32 *)CapTouch_Rx__4__PS,
        CapTouch_Rx__4__HSIOM_MASK,
        CapTouch_Rx__4__MASK,
        CapTouch_Rx__4__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__4__SHIFT,
        (uint8)CapTouch_Rx__4__SHIFT * 3u,
    },
    { /* 23: U102_Tx ganged to U113_Tx */
        (reg32 *)CapTouch_Tx__0__HSIOM,
        (reg32 *)CapTouch_Tx__0__PC,
        (reg32 *)CapTouch_Tx__0__PC2,
        (reg32 *)CapTouch_Tx__0__DR,
        (reg32 *)CapTouch_Tx__0__PS,
        CapTouch_Tx__0__HSIOM_MASK,
        CapTouch_Tx__0__MASK,
        CapTouch_Tx__0__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT,
        (uint8)CapTouch_Tx__0__SHIFT * 3u,
    },
    { /* 24: U113_Rx0 ganged to U114_Rx0 */
        (reg32 *)CapTouch_Rx__4__HSIOM,
        (reg32 *)CapTouch_Rx__4__PC,
        (reg32 *)CapTouch_Rx__4__PC2,
        (reg32 *)CapTouch_Rx__4__DR,
        (reg32 *)CapTouch_Rx__4__PS,
        CapTouch_Rx__4__HSIOM_MASK,
        CapTouch_Rx__4__MASK,
        CapTouch_Rx__4__HSIOM_SHIFT,
        (uint8)CapTouch_Rx__4__SHIFT,
        (uint8)CapTouch_Rx__4__SHIFT * 3u,
    },
    { /* 25: U103_Tx ganged to U114_Tx */
        (reg32 *)CapTouch_Tx__1__HSIOM,
        (reg32 *)CapTouch_Tx__1__PC,
        (reg32 *)CapTouch_Tx__1__PC2,
        (reg32 *)CapTouch_Tx__1__DR,
        (reg32 *)CapTouch_Tx__1__PS,
        CapTouch_Tx__1__HSIOM_MASK,
        CapTouch_Tx__1__MASK,
        CapTouch_Tx__1__HSIOM_SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT,
        (uint8)CapTouch_Tx__1__SHIFT * 3u,
    },
};



/** \}
* \endcond */

/* Initialization data for RAM widget list */
const CapTouch_RAM_WD_LIST_STRUCT CapTouch_ramWidgetInit =
{
    { /* U102 */
        CapTouch_U102_CRC,
        CapTouch_U102_RESOLUTION,
        CapTouch_U102_FINGER_TH,
        CapTouch_U102_NOISE_TH,
        CapTouch_U102_NNOISE_TH,
        CapTouch_U102_HYSTERESIS,
        CapTouch_U102_ON_DEBOUNCE,
        CapTouch_U102_LOW_BSLN_RST,
        CapTouch_U102_SNS_CLK,
        CapTouch_U102_SNS_CLK_SOURCE,
    },
    { /* U103 */
        CapTouch_U103_CRC,
        CapTouch_U103_RESOLUTION,
        CapTouch_U103_FINGER_TH,
        CapTouch_U103_NOISE_TH,
        CapTouch_U103_NNOISE_TH,
        CapTouch_U103_HYSTERESIS,
        CapTouch_U103_ON_DEBOUNCE,
        CapTouch_U103_LOW_BSLN_RST,
        CapTouch_U103_SNS_CLK,
        CapTouch_U103_SNS_CLK_SOURCE,
    },
    { /* U104 */
        CapTouch_U104_CRC,
        CapTouch_U104_RESOLUTION,
        CapTouch_U104_FINGER_TH,
        CapTouch_U104_NOISE_TH,
        CapTouch_U104_NNOISE_TH,
        CapTouch_U104_HYSTERESIS,
        CapTouch_U104_ON_DEBOUNCE,
        CapTouch_U104_LOW_BSLN_RST,
        CapTouch_U104_SNS_CLK,
        CapTouch_U104_SNS_CLK_SOURCE,
    },
    { /* U105 */
        CapTouch_U105_CRC,
        CapTouch_U105_RESOLUTION,
        CapTouch_U105_FINGER_TH,
        CapTouch_U105_NOISE_TH,
        CapTouch_U105_NNOISE_TH,
        CapTouch_U105_HYSTERESIS,
        CapTouch_U105_ON_DEBOUNCE,
        CapTouch_U105_LOW_BSLN_RST,
        CapTouch_U105_SNS_CLK,
        CapTouch_U105_SNS_CLK_SOURCE,
    },
    { /* U106 */
        CapTouch_U106_CRC,
        CapTouch_U106_RESOLUTION,
        CapTouch_U106_FINGER_TH,
        CapTouch_U106_NOISE_TH,
        CapTouch_U106_NNOISE_TH,
        CapTouch_U106_HYSTERESIS,
        CapTouch_U106_ON_DEBOUNCE,
        CapTouch_U106_LOW_BSLN_RST,
        CapTouch_U106_SNS_CLK,
        CapTouch_U106_SNS_CLK_SOURCE,
    },
    { /* U107 */
        CapTouch_U107_CRC,
        CapTouch_U107_RESOLUTION,
        CapTouch_U107_FINGER_TH,
        CapTouch_U107_NOISE_TH,
        CapTouch_U107_NNOISE_TH,
        CapTouch_U107_HYSTERESIS,
        CapTouch_U107_ON_DEBOUNCE,
        CapTouch_U107_LOW_BSLN_RST,
        CapTouch_U107_SNS_CLK,
        CapTouch_U107_SNS_CLK_SOURCE,
    },
    { /* U108 */
        CapTouch_U108_CRC,
        CapTouch_U108_RESOLUTION,
        CapTouch_U108_FINGER_TH,
        CapTouch_U108_NOISE_TH,
        CapTouch_U108_NNOISE_TH,
        CapTouch_U108_HYSTERESIS,
        CapTouch_U108_ON_DEBOUNCE,
        CapTouch_U108_LOW_BSLN_RST,
        CapTouch_U108_SNS_CLK,
        CapTouch_U108_SNS_CLK_SOURCE,
    },
    { /* U109 */
        CapTouch_U109_CRC,
        CapTouch_U109_RESOLUTION,
        CapTouch_U109_FINGER_TH,
        CapTouch_U109_NOISE_TH,
        CapTouch_U109_NNOISE_TH,
        CapTouch_U109_HYSTERESIS,
        CapTouch_U109_ON_DEBOUNCE,
        CapTouch_U109_LOW_BSLN_RST,
        CapTouch_U109_SNS_CLK,
        CapTouch_U109_SNS_CLK_SOURCE,
    },
    { /* U110 */
        CapTouch_U110_CRC,
        CapTouch_U110_RESOLUTION,
        CapTouch_U110_FINGER_TH,
        CapTouch_U110_NOISE_TH,
        CapTouch_U110_NNOISE_TH,
        CapTouch_U110_HYSTERESIS,
        CapTouch_U110_ON_DEBOUNCE,
        CapTouch_U110_LOW_BSLN_RST,
        CapTouch_U110_SNS_CLK,
        CapTouch_U110_SNS_CLK_SOURCE,
    },
    { /* U111 */
        CapTouch_U111_CRC,
        CapTouch_U111_RESOLUTION,
        CapTouch_U111_FINGER_TH,
        CapTouch_U111_NOISE_TH,
        CapTouch_U111_NNOISE_TH,
        CapTouch_U111_HYSTERESIS,
        CapTouch_U111_ON_DEBOUNCE,
        CapTouch_U111_LOW_BSLN_RST,
        CapTouch_U111_SNS_CLK,
        CapTouch_U111_SNS_CLK_SOURCE,
    },
    { /* U112 */
        CapTouch_U112_CRC,
        CapTouch_U112_RESOLUTION,
        CapTouch_U112_FINGER_TH,
        CapTouch_U112_NOISE_TH,
        CapTouch_U112_NNOISE_TH,
        CapTouch_U112_HYSTERESIS,
        CapTouch_U112_ON_DEBOUNCE,
        CapTouch_U112_LOW_BSLN_RST,
        CapTouch_U112_SNS_CLK,
        CapTouch_U112_SNS_CLK_SOURCE,
    },
    { /* U113 */
        CapTouch_U113_CRC,
        CapTouch_U113_RESOLUTION,
        CapTouch_U113_FINGER_TH,
        CapTouch_U113_NOISE_TH,
        CapTouch_U113_NNOISE_TH,
        CapTouch_U113_HYSTERESIS,
        CapTouch_U113_ON_DEBOUNCE,
        CapTouch_U113_LOW_BSLN_RST,
        CapTouch_U113_SNS_CLK,
        CapTouch_U113_SNS_CLK_SOURCE,
    },
    { /* U114 */
        CapTouch_U114_CRC,
        CapTouch_U114_RESOLUTION,
        CapTouch_U114_FINGER_TH,
        CapTouch_U114_NOISE_TH,
        CapTouch_U114_NNOISE_TH,
        CapTouch_U114_HYSTERESIS,
        CapTouch_U114_ON_DEBOUNCE,
        CapTouch_U114_LOW_BSLN_RST,
        CapTouch_U114_SNS_CLK,
        CapTouch_U114_SNS_CLK_SOURCE,
    },
};


/* IDAC Initialization Data */
const uint8 CapTouch_ramIdacInit[CapTouch_TOTAL_SENSORS] =
{
    /* U102 */
    CapTouch_U102_RX0_IDAC_COMP0,

    /* U103 */
    CapTouch_U103_RX0_IDAC_COMP0,

    /* U104 */
    CapTouch_U104_RX0_IDAC_COMP0,

    /* U105 */
    CapTouch_U105_RX0_IDAC_COMP0,

    /* U106 */
    CapTouch_U106_RX0_IDAC_COMP0,

    /* U107 */
    CapTouch_U107_RX0_IDAC_COMP0,

    /* U108 */
    CapTouch_U108_RX0_IDAC_COMP0,

    /* U109 */
    CapTouch_U109_RX0_IDAC_COMP0,

    /* U110 */
    CapTouch_U110_RX0_IDAC_COMP0,

    /* U111 */
    CapTouch_U111_RX0_IDAC_COMP0,

    /* U112 */
    CapTouch_U112_RX0_IDAC_COMP0,

    /* U113 */
    CapTouch_U113_RX0_IDAC_COMP0,

    /* U114 */
    CapTouch_U114_RX0_IDAC_COMP0,
};



/*******************************************************************************
* Defines internal data types and statements
*******************************************************************************/
#define PARAM_TYPE_OFFSET       (30u)
#define PARAM_TYPE_MASK         (3Lu << PARAM_TYPE_OFFSET)
#define PARAM_RWBIT_OFFSET      (29u)
#define PARAM_RWBIT_MASK        (1Lu << PARAM_RWBIT_OFFSET)
#define PARAM_FBIT_OFFSET       (28u)
#define PARAM_FBIT_MASK         (1Lu << PARAM_FBIT_OFFSET)
#define PARAM_UBIT_OFFSET       (23u)
#define PARAM_UBIT_MASK         (1Lu << PARAM_UBIT_OFFSET)
#define PARAM_WDID_OFFSET       (16u)
#define PARAM_WDID_MASK         (0x7FLu << PARAM_WDID_OFFSET)
#define PARAM_CRC_OFFSET        (24u)
#define PARAM_CRC_MASK          (0x0FLu << PARAM_CRC_OFFSET)

/* Defines align masks for uint16 and uint32 */
#define UINT16_ALIGN_MASK       (1Lu)
#define UINT32_ALIGN_MASK       (3Lu)

/* Defines supported parameter types */
#define DS_PARAM_TYPE_UINT8     (0x01u)
#define DS_PARAM_TYPE_UINT16    (0x02u)
#define DS_PARAM_TYPE_UINT32    (0x03u)


/* Defines PARAM_ID structure */
typedef struct
{
    uint16 offset;      /* parameter offset */
    uint8  widgetId;    /* widget Id parameter belongs to */
    uint8  affectsCrc;  /* parameter affects widget CRC */
    uint8  crc;         /* parameter CRC */
    uint8  flash;       /* parameter located in FLASH flag */
    uint8  rw;          /* parameter is read/write flag */
    uint8  type;        /* parameter type: uint8/uint16/uint32 */
} PARAM_ID_STRUCT;


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
/**
* \cond SECTION_C_INTERNAL
* \addtogroup group_c_internal
* \{
*/


/* Ext Cap Mapping Data */
const uint8 CapTouch_extCapMap[CapTouch_TST_EXT_CAPS_NUM] =
{
    CapTouch_TST_CINTA_MAP,
    CapTouch_TST_CINTB_MAP,
};


static cystatus DsParseParamId(uint32 paramId, PARAM_ID_STRUCT *pData);

/** \}
* \endcond */


/*******************************************************************************
* Function Name: CapTouch_DsInitialize
****************************************************************************//**
*
* \brief
*   This function initializes the Data Structure storage.
*
* \details
*   This function copies the default widget configuration parameters
*   from the Flash storage into the RAM Data Structure.
*
*******************************************************************************/
void CapTouch_DsInitialize(void)
{
    #if ((CapTouch_ENABLE == CapTouch_CSX_EN) || \
         (CapTouch_ENABLE == CapTouch_CSD_IDAC_COMP_EN) || \
        ((CapTouch_ENABLE == CapTouch_CSX_EN) && \
         (CapTouch_ENABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES)))
        uint32 snsId;
        uint32 wdgtId;
        CapTouch_RAM_SNS_STRUCT * ptrSnsTmp;
        CapTouch_FLASH_WD_STRUCT const * ptrFlashWdgt;
    #endif

    #if ((CapTouch_ENABLE == CapTouch_CSX_EN) && \
         (CapTouch_ENABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES))
        uint16 subConvNumber;
    #endif

    #if ((0u != CapTouch_CSX_EN) || (0 != CapTouch_CSD_IDAC_COMP_EN))
        uint8 const * ptrIdacInit = CapTouch_ramIdacInit;
    #endif

    /* Reset RAM data structure content */
    (void)memset(&CapTouch_dsRam, 0, sizeof(CapTouch_dsRam));

    /* Initialize configId, deviceId and hwClock registers */
    CapTouch_dsRam.configId = CapTouch_CONFIG_ID;
    CapTouch_dsRam.deviceId = CapTouch_DEVICE_ID;
    CapTouch_dsRam.hwClock = CapTouch_HW_CLOCK;

    /* Initialize global RAM data */
    CapTouch_dsRam.csd0Config = CapTouch_CSD0_CONFIG;

    #if (0u != CapTouch_TOTAL_CSD_WIDGETS)
        CapTouch_dsRam.modCsdClk = CapTouch_CSD_SCANSPEED_DIVIDER;

        #if (0u != CapTouch_CSD_COMMON_SNS_CLK_EN)
            CapTouch_dsRam.snsCsdClk = CapTouch_CSD_SNS_CLK_DIVIDER;
        #endif /* #if (0u != CapTouch_CSD_COMMON_SNS_CLK_EN) */
    #endif /* #if (0u != CapTouch_TOTAL_CSD_WIDGETS) */

    #if (0u != CapTouch_CSX_EN)
        CapTouch_dsRam.modCsxClk = CapTouch_CSX_SCANSPEED_DIVIDER;

        #if (0u != CapTouch_CSX_COMMON_TX_CLK_EN)
            CapTouch_dsRam.snsCsxClk = CapTouch_CSX_TX_CLK_DIVIDER;
        #endif /* #if (0u != CapTouch_CSX_COMMON_TX_CLK_EN) */
    #endif /* #if (0u != CapTouch_CSX_EN) */

    #if (0u != CapTouch_ISX_EN)
        CapTouch_dsRam.modIsxClk = CapTouch_ISX_SCANSPEED_DIVIDER;
    #endif /* #if (0u != CapTouch_ISX_EN) */

    #if (0u != CapTouch_SELF_TEST_EN)
        CapTouch_dsRam.glbCrc = CapTouch_GLB_CRC;
    #endif /* #if (0u != CapTouch_SELF_TEST_EN) */

    /* Initialize RAM widget data */
    CapTouch_dsRam.wdgtList = CapTouch_ramWidgetInit;

    /* Initialize Gesture configuration structure */
    #if ((0u != CapTouch_GES_GLOBAL_EN) || (0u != CapTouch_BALLISTIC_MULTIPLIER_EN))
        CapTouch_dsRam.timestampInterval = CapTouch_TIMESTAMP_INTERVAL;
    #endif /* ((0u != CapTouch_GES_GLOBAL_EN) || (0u != CapTouch_BALLISTIC_MULTIPLIER_EN)) */
        
    #if (0u != CapTouch_GES_GLOBAL_EN)
        CapTouch_dsRam.gestures = CapTouch_ramGesturesInit;
    #endif /* (0u != CapTouch_GES_GLOBAL_EN) */

    #if (0u != CapTouch_BALLISTIC_MULTIPLIER_EN)
        CapTouch_dsRam.ballisticConfig = CapTouch_ramBallisticInit;
    #endif /* (0u != CapTouch_BALLISTIC_MULTIPLIER_EN) */

    #if (0u != CapTouch_CSX_EN) || (0 != CapTouch_CSD_IDAC_COMP_EN)
        /* Initialize IDAC data */
        ptrFlashWdgt = CapTouch_dsFlash.wdgtArray;

        for (wdgtId = CapTouch_TOTAL_WIDGETS; wdgtId-- > 0u; )
        {
            ptrSnsTmp = ptrFlashWdgt->ptr2SnsRam;

            for (snsId = CapTouch_GET_SNS_CNT_BY_PTR(ptrFlashWdgt); snsId-- > 0u;)
            {
                ptrSnsTmp->idacComp[0u] = *ptrIdacInit;
                ptrIdacInit++;

                #if (0u != CapTouch_MULTI_FREQ_SCAN_EN)
                    ptrSnsTmp->idacComp[1u] = *ptrIdacInit;
                    ptrIdacInit++;

                    ptrSnsTmp->idacComp[2u] = *ptrIdacInit;
                    ptrIdacInit++;
                #endif /* #if (0u != CapTouch_MULTI_FREQ_SCAN_EN) */

                ptrSnsTmp++;
            }

            ptrFlashWdgt++; /* Move to next widget */
        }
    #endif /*(0u != CapTouch_CSX_EN) || (0 != CapTouch_CSD_IDAC_COMP_EN)*/

    #if ((CapTouch_ENABLE == CapTouch_CSX_EN) && \
         (CapTouch_ENABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES))
        /* Initialize Sub-Conversion Number for each node of each CSX widget */
        ptrFlashWdgt = CapTouch_dsFlash.wdgtArray;
        for (wdgtId = CapTouch_TOTAL_WIDGETS; wdgtId-- > 0u;)
        {
            /* Check for widget sense method */
            if (CapTouch_SENSE_METHOD_CSX_E == CapTouch_GET_SENSE_METHOD(ptrFlashWdgt))
            {
                /* Take the sub-convesion number of a CSX widget and initialize each node of this widget */
                subConvNumber = ((CapTouch_RAM_WD_BASE_STRUCT *)(ptrFlashWdgt->ptr2WdgtRam))->resolution;
                ptrSnsTmp = ptrFlashWdgt->ptr2SnsRam;
                for (snsId = CapTouch_GET_SNS_CNT_BY_PTR(ptrFlashWdgt); snsId-- > 0u;)
                {
                    ptrSnsTmp->subConvNum = subConvNumber;
                    ptrSnsTmp++;
                }
            }
            /* Move to the next widget */
            ptrFlashWdgt++;
        }
    #endif /* ((CapTouch_ENABLE == CapTouch_CSX_EN) && \
               (CapTouch_ENABLE == CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES)) */

    #if (0u != CapTouch_ADC_EN)
        CapTouch_AdcDsInitialize();
    #endif /* (0u != CapTouch_ADC_EN) */
}


#if (0u != CapTouch_ADC_EN)
    /*******************************************************************************
    * Function Name: CapTouch_AdcDsInitialize
    ****************************************************************************//**
    *
    * \brief
    *   Configures the initial Adc datastructure members.
    *
    *******************************************************************************/
    void CapTouch_AdcDsInitialize(void)
    {
        CapTouch_dsRam.adcResolution = CapTouch_ADC_RESOLUTION;
        CapTouch_dsRam.adcIdac = (uint8)(CapTouch_ADC_IDAC_DEFAULT);
        CapTouch_dsRam.adcActiveCh = CapTouch_AdcNO_CHANNEL;
    }
#endif /* (0u != CapTouch_ADC_EN) */


/*******************************************************************************
* Function Name: DsParseParamId
****************************************************************************//**
*
* \brief
*   This function parses the parameter ID and checks its correctness.
*
* \details
*   This function checks the parameter CRC if the self-test is enabled,
*   validates the offset value and parameter type.
*
* \param paramId The parameter to parse.
* \param pData   The pointer to the structure that will hold parsed data.
*
* \return CYRET_SUCCESS     If the parameter is valid
*         CYRET_BAD_PARAM   If the parameter is invalid
*
*******************************************************************************/
static cystatus DsParseParamId(uint32 paramId, PARAM_ID_STRUCT *pData)
{
    cystatus result = CYRET_SUCCESS;

    #if (0u != CapTouch_SELF_TEST_EN)
        const uint8 crcTable[] =
        {
            0x00u, 0x09u, 0x0Bu, 0x02u, 0x0Fu, 0x06u, 0x04u, 0x0Du,
            0x07u, 0x0Eu, 0x0Cu, 0x05u, 0x08u, 0x01u, 0x03u, 0x0Au
        };

        uint32 i;
        uint32 actualCrc = 0u;
        uint32 crcIndex;
    #endif /* #if (0u != CapTouch_SELF_TEST_EN) */

    /* Extract parameter data */
    pData->offset     = LO16(paramId);
    pData->affectsCrc = LO8((paramId & PARAM_UBIT_MASK)  >> PARAM_UBIT_OFFSET);
    pData->widgetId   = LO8((paramId & PARAM_WDID_MASK)  >> PARAM_WDID_OFFSET);
    pData->type       = LO8((paramId & PARAM_TYPE_MASK)  >> PARAM_TYPE_OFFSET);
    pData->flash      = LO8((paramId & PARAM_FBIT_MASK)  >> PARAM_FBIT_OFFSET);
    pData->rw         = LO8((paramId & PARAM_RWBIT_MASK) >> PARAM_RWBIT_OFFSET);
    pData->crc        = LO8((paramId & PARAM_CRC_MASK)   >> PARAM_CRC_OFFSET);

    /* Check parameter CRC if self-test is enabled */
    #if (0u != CapTouch_SELF_TEST_EN)

        /* Calculate CRC for bits 0..24.
         * The CRC is calculated using nibbles (4-bits).
         * So for 0..24 bits there are in total 6 nibbles. */
        for (i = 6u; i > 0u; i--)
        {
            crcIndex = actualCrc ^ (paramId & 0x0Fu);
            actualCrc = crcTable[crcIndex];
            paramId >>= 4u;
        }

        /* Add bits 28..32 */
        crcIndex = actualCrc ^ ((paramId >> 4u) & 0x0Fu);
        actualCrc = crcTable[crcIndex];

        /* Add bits 24..28 (CRC) */
        crcIndex = actualCrc ^ (paramId & 0x0Fu);
        actualCrc = crcTable[crcIndex];

        if (0u != actualCrc)
        {
            /* CRC mismatch */
            result = CYRET_BAD_PARAM;
        }

        if ((CYRET_SUCCESS == result) &&
            (0u != pData->affectsCrc) && (pData->widgetId >= CapTouch_TOTAL_WIDGETS ))
        {
            /* Wrong widgetId for protected parameter */
            result = CYRET_BAD_PARAM;
        }

        /* Check offset value range */
        if (CYRET_SUCCESS == result)
        {
            if (0u == pData->flash)
            {
                /* Check offset for RAM Data Structure range */
                if (pData->offset >= sizeof(CapTouch_dsRam))
                {
                    result = CYRET_BAD_PARAM;
                }
            }
            else
            {
                /* Check offset for Flash Data Structure range */
                if (pData->offset >= sizeof(CapTouch_dsFlash))
                {
                    result = CYRET_BAD_PARAM;
                }

                /* Check if it is marked as Read Only */
                if (0u != pData->rw)
                {
                    result = CYRET_BAD_PARAM;
                }
            }
        }
    #else /* #if (0u != CapTouch_SELF_TEST_EN) */
        /* Check offset value range */
        if (0u == pData->flash)
        {
            /* Check offset for RAM Data Structure range */
            if (pData->offset >= sizeof(CapTouch_dsRam))
            {
                result = CYRET_BAD_PARAM;
            }
        }
        else
        {
            /* Check offset for Flash Data Structure range */
            if (pData->offset >= sizeof(CapTouch_dsFlash))
            {
                result = CYRET_BAD_PARAM;
            }

            /* Check if it is marked as Read Only */
            if (0u != pData->rw)
            {
                result = CYRET_BAD_PARAM;
            }
        }
    #endif /* #if (0u != CapTouch_SELF_TEST_EN) */

    if (CYRET_SUCCESS == result)
    {
        /* Check parameter type, offset alignment */
        switch (pData->type)
        {
        case DS_PARAM_TYPE_UINT16:
            if (0u != (pData->offset & UINT16_ALIGN_MASK))
            {
                result = CYRET_BAD_PARAM;
            }
            break;

        case DS_PARAM_TYPE_UINT32:
            if (0u != (pData->offset & UINT32_ALIGN_MASK))
            {
                result = CYRET_BAD_PARAM;
            }
            break;

        case DS_PARAM_TYPE_UINT8:
            break;

        default:
            result = CYRET_BAD_PARAM;
            break;
        }
    }

    return result;
}

/*******************************************************************************
* Function Name: CapTouch_GetParam
****************************************************************************//**
*
* \brief
*  Gets the specified parameter value from the \ref group_structures.
*
* \details
*  This function gets the value of the specified parameter by the paramId
*  argument. The paramId for each register is available in the
*  CapTouch RegisterMap header file as
*  CapTouch_<ParameterName>_PARAM_ID. The paramId is a special
*  enumerated value generated by the customizer. The format of paramId is as
*  follows:
*    1. [ byte 3 byte 2 byte 1 byte 0 ]
*    2. [ TTWFCCCC UIIIIIII MMMMMMMM LLLLLLLL ]
*    3. T - encodes the parameter type:
*      - 01b: uint8
*      - 10b: uint16
*      - 11b: uint32
*    4. W - indicates whether the parameter is writable:
*      - 0: ReadOnly
*      - 1: Read/Write
*    5. C - 4 bit CRC (X^3 + 1) of the whole paramId word, the C bits are
*       filled with 0s when the CRC is calculated.
*    6. U - indicates if the parameter affects the RAM Widget Object CRC.
*    7. I - specifies that the widgetId parameter belongs to
*    8. M,L - the parameter offset MSB and LSB accordingly in:
*      - Flash Data Structure if W bit is 0.
*      - RAM Data Structure if W bit is 1.
*
*  Refer to the \ref group_structures section for details of the data structure
*  organization and examples of its register access.
*
* \param paramId
*  Specifies the ID of parameter to get its value.
*  A macro for the parameter ID can be found in the CapTouch RegisterMap header
*  file defined as CapTouch_<ParameterName>_PARAM_ID.
*
* \param value
*  The pointer to a variable to be updated with the obtained value.
*
* \return
*  Returns the status of the operation:
*    - CYRET_SUCCESS - The operation is successfully completed.
*    - CYRET_BAD_PARAM - The input parameter is invalid.
*
*******************************************************************************/
cystatus CapTouch_GetParam(uint32 paramId, uint32 *value)
{
    PARAM_ID_STRUCT pData;
    cystatus result;

    union
    {
        volatile void   const * ptr;
        volatile uint8  const * ptrUint8;
        volatile uint16 const * ptrUint16;
        volatile uint32 const * ptrUint32;
    } ptrData;

    result = DsParseParamId(paramId, &pData);

    /* Parse and validate paramId */
    if (CYRET_SUCCESS == result)
    {
        /* Get base address of parameter */
        if (0u == pData.flash)
        {
            ptrData.ptr = &CapTouch_dsRam;
        }
        else
        {
            ptrData.ptr = &CapTouch_dsFlash;
        }

        /* Add offset to base address */
        ptrData.ptrUint8 += pData.offset;

        /* Read data */
        if (DS_PARAM_TYPE_UINT8 == pData.type)
        {
            *value = (uint32) (*ptrData.ptrUint8);
        }
        else if (DS_PARAM_TYPE_UINT16 == pData.type)
        {
            *value = (uint32) (*ptrData.ptrUint16);
        }
        else if (DS_PARAM_TYPE_UINT32 == pData.type)
        {
            *value = *ptrData.ptrUint32;
        }
        else
        {
            /* Parameter comes here already validated. */
            CYASSERT(0u);
        }
    }

    return result;
}


/*******************************************************************************
* Function Name: CapTouch_SetParam
****************************************************************************//**
*
* \brief
*  Sets a new value for the specified parameter in the \ref group_structures.
*
* \details
*  This function sets the value of the specified parameter by the paramId
*  argument. The paramId for each register is available in the
*  CapTouch RegisterMap header file as
*  CapTouch_<ParameterName>_PARAM_ID. The paramId is a special
*  enumerated value generated by the customizer. The format of paramId is as
*  follows:
*    1. [ byte 3 byte 2 byte 1 byte 0 ]
*    2. [ TTWFCCCC UIIIIIII MMMMMMMM LLLLLLLL ]
*    3. T - encodes the parameter type:
*      - 01b: uint8
*      - 10b: uint16
*      - 11b: uint32
*    4. W - indicates whether the parameter is writable:
*      - 0: ReadOnly
*      - 1: Read/Write
*    5. C - 4 bit CRC (X^3 + 1) of the whole paramId word, the C bits are
*       filled with 0s when the CRC is calculated.
*    6. U - indicates if the parameter affects the RAM Widget Object CRC.
*    7. I - specifies that the widgetId parameter belongs to
*    8. M,L - the parameter offset MSB and LSB accordingly in:
*      - Flash Data Structure if W bit is 0.
*      - RAM Data Structure if W bit is 1.
*
*  Refer to the \ref group_structures section for details of the data structure
*  organization and examples of its register access.
*
*  This function writes specified value into the desired register without
*  other registers update. It is application layer responsibility to keep all
*  the data structure registers aligned. Repeated call of
*  CapTouch_Start() function helps aligning dependent register values.
*
* \param paramId
*  Specifies the ID of parameter to set its value.
*  A macro for the parameter ID can be found in the CapTouch RegisterMap header
*  file defined as CapTouch_<ParameterName>_PARAM_ID.

* \param value
*  Specifies the new parameter's value.
*
* \return
*  Returns the status of the operation:
*    - CYRET_SUCCESS - The operation is successfully completed.
*    - CYRET_BAD_PARAM - The input parameter is invalid.
*
*******************************************************************************/
cystatus CapTouch_SetParam(uint32 paramId, uint32 value)
{
    cystatus result;
    PARAM_ID_STRUCT pData;

    union
    {
        volatile void   * ptr;
        volatile uint8  * ptrUint8;
        volatile uint16 * ptrUint16;
        volatile uint32 * ptrUint32;
    } ptrData;

    result = DsParseParamId(paramId, &pData);

    /* verify if parameter is writable */
    if (CYRET_SUCCESS == result)
    {
        if (0u == pData.rw)
        {
            result = CYRET_BAD_PARAM;
        }
    }

    /* Parse and validate paramId */
    if (CYRET_SUCCESS == result)
    {
        /* Get base address of parameter */
        ptrData.ptr = &CapTouch_dsRam;

        /* Add offset to base address */
        ptrData.ptrUint8 += pData.offset;

        /* Write data */
        if (DS_PARAM_TYPE_UINT8 == pData.type)
        {
            *ptrData.ptrUint8 = LO8(value);
        }
        else if (DS_PARAM_TYPE_UINT16 == pData.type)
        {
            *ptrData.ptrUint16 = LO16(value);
        }
        else if (DS_PARAM_TYPE_UINT32 == pData.type)
        {
            *ptrData.ptrUint32 = value;
        }
        else
        {
            /* Parameter comes here already validated. */
            CYASSERT(0u);
        }

        #if (CapTouch_ENABLE == (CapTouch_TST_WDGT_CRC_EN || CapTouch_TST_GLOBAL_CRC_EN))
            /* Update widget or global CRC if self-test is enabled and parameter affects it */
            if (0u != pData.affectsCrc)
            {
                if ((ptrData.ptrUint16 >= &CapTouch_dsRam.csd0Config) &&
                    (ptrData.ptrUint16 <  &CapTouch_dsRam.glbCrc))
                {

                    #if (CapTouch_ENABLE == CapTouch_TST_GLOBAL_CRC_EN)
                        CapTouch_DsUpdateGlobalCrc();
                    #endif /* (CapTouch_ENABLE == CapTouch_TST_GLOBAL_CRC_EN) */

                }
                else
                {
                    #if (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN)
                        CapTouch_DsUpdateWidgetCrc((uint32)pData.widgetId);
                    #endif /* (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN) */
                }
            }
        #endif /* (CapTouch_ENABLE == (CapTouch_TST_WDGT_CRC_EN || CapTouch_TST_GLOBAL_CRC_EN)) */
    }

    return result;
}

/*******************************************************************************
* Function Name: CapTouch_IsAnyWidgetActive
****************************************************************************//**
*
* \brief
*  Reports if any widget has detected a touch.
*
* \details
*  This function reports if any widget has detected a touch or not by extracting
*  information from the wdgtStatus registers
*  (CapTouch_WDGT_STATUS<X>_VALUE). This function does not process a
*  widget but extracts processed results from the \ref group_structures.
*
* \return
*   Returns the touch detection status of all the widgets:
*     - Zero - No touch is detected in all the widgets or sensors.
*     - Non-zero - At least one widget or sensor detected a touch.
*
*******************************************************************************/
uint32 CapTouch_IsAnyWidgetActive(void)
{
    uint32 result = 0Lu;
    uint32 wdWord;

    for (wdWord = CapTouch_WDGT_STATUS_WORDS; wdWord-- > 0u;)
    {
        result |= CapTouch_dsRam.wdgtStatus[wdWord];
    }

    return result;
}


/*******************************************************************************
* Function Name: CapTouch_IsWidgetActive
****************************************************************************//**
*
* \brief
*  Reports if the specified widget detects a touch on any of its sensors.
*
* \details
*  This function reports if the specified widget has detected a touch or not by
*  extracting information from the wdgtStatus registers
*  (CapTouch_WDGT_STATUS<X>_VALUE). This function does not process the
*  widget but extracts processed results from the \ref group_structures.
*
* \param widgetId
*  Specifies the ID number of the widget to get its status.
*  A macro for the widget ID can be found in the CapTouch Configuration header
*  file defined as CapTouch_<WidgetName>_WDGT_ID.
*
* \return
*  Returns the touch detection status of the specified widgets:
*    - Zero - No touch is detected in the specified widget or a wrong widgetId
*      is specified.
*    - Non-zero if at least one sensor of the specified widget is active, i.e.
*      a touch is detected.
*
*******************************************************************************/
uint32 CapTouch_IsWidgetActive(uint32 widgetId)
{
    uint32 result = 0Lu;

    if (widgetId < CapTouch_TOTAL_WIDGETS)
    {
        result = CapTouch_GET_WIDGET_ACTIVE_STATUS(widgetId);
    }
    return result;
}


/*******************************************************************************
* Function Name: CapTouch_IsSensorActive
****************************************************************************//**
*
* \brief
*  Reports if the specified sensor in the widget detects a touch.
*
* \details
*  This function reports if the specified sensor in the widget has detected a
*  touch or not by extracting information from the wdgtStatus registers
*  (CapTouch_WDGT_STATUS<X>_VALUE). This function does not process the
*  widget or sensor but extracts processed results from the \ref group_structures.
*
*  For proximity sensors, this function returns the proximity detection status. To
*  get the finger touch status of proximity sensors, use the
*  CapTouch_IsProximitySensorActive() function.
*
* \param widgetId
*  Specifies the ID number of the widget.
*  A macro for the widget ID can be found in the CapTouch Configuration header
*  file defined as CapTouch_<WidgetName>_WDGT_ID.

* \param sensorId
*  Specifies the ID number of the sensor within the widget to get its touch
*  detection status.
*  A macro for the sensor ID within the specified widget can be found in the
*  CapTouch Configuration header file defined as
*  CapTouch_<WidgetName>_SNS<SensorNumber>_ID.
*
* \return
*  Returns the touch detection status of the specified sensor / widget:
*    - Zero if no touch is detected in the specified sensor / widget or a wrong
*      widget ID / sensor ID is specified.
*    - Non-zero if the specified sensor is active i.e. touch is detected. If the
*      specific sensor belongs to a proximity widget, the proximity detection
*      status is returned.
*
*******************************************************************************/
uint32 CapTouch_IsSensorActive(uint32 widgetId, uint32 sensorId)
{
    uint32 result = 0Lu;

    if ((widgetId < CapTouch_TOTAL_WIDGETS) &&
        (sensorId < CapTouch_GET_SENSOR_COUNT(widgetId)))
    {
        #if (CapTouch_ENABLE == CapTouch_PROXIMITY_WIDGET_EN)
            if (CapTouch_WD_PROXIMITY_E ==
                (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[widgetId].wdgtType)
            {
                sensorId = CapTouch_PROX_STS_OFFSET(sensorId);
            }
        #endif /* (CapTouch_ENABLE != CapTouch_PROXIMITY_WIDGET_EN) */
        result = CapTouch_dsRam.snsStatus[widgetId] & (1Lu << sensorId);
    }

    return result;
}

#if (0u != CapTouch_MATRIX_WIDGET_EN)
/*******************************************************************************
* Function Name: CapTouch_IsMatrixButtonsActive
****************************************************************************//**
*
* \brief
*  Reports the status of the specified matrix button widget.
*
* \details
*  This function reports if the specified matrix widget has detected a touch or
*  not by extracting information from the wdgtStatus registers
*  (CapTouch_WDGT_STATUS<X>_VALUE for the CSD widgets and
*  CapTouch_SNS_STATUS<WidgetId>_VALUE for CSX widget). In addition, the
*  function provides details of the active sensor including active rows/columns
*  for the CSD widgets. This function is used only with the matrix button
*  widgets. This function does not process the widget but extracts
*  processed results from the \ref group_structures.
*
* \param widgetId
*  Specifies the ID number of the matrix button widget to check the status of its
*  sensors.
*  A macro for the widget ID can be found in the CapTouch Configuration header file
*  defined as CapTouch_<WidgetName>_WDGT_ID
*
* \return
*  Returns the touch detection status of the sensors in the specified matrix
*  buttons widget. Zero indicates that no touch is detected in the specified
*  widget or a wrong widgetId is specified.
*    1. For the matrix buttons widgets with the CSD sensing mode:
*      - Bit [31] if set, indicates that one or more sensors in the widget
*        detected a touch.
*      - Bits [30..24] are reserved
*      - Bits [23..16] indicate the logical sensor number of the sensor that
*        detected a touch. If more than one sensor detected a touch for the CSD
*        widget, no status is reported because more than one touch is invalid for the
*        CSD matrix buttons widgets.
*      - Bits [15..8] indicate the active row number.
*      - Bits [7..0] indicate the active column number.
*    2. For the matrix buttons widgets with the CSX widgets, each bit (31..0)
*      corresponds to the TX/RX intersection.
*
*******************************************************************************/
uint32 CapTouch_IsMatrixButtonsActive(uint32 widgetId)
{
    uint32 result = 0Lu;
    cystatus state = CYRET_SUCCESS;
    CapTouch_FLASH_WD_STRUCT const *ptrFlashWdgt = 0u;

    #if (0u != CapTouch_CSD_MATRIX_WIDGET_EN)
        CapTouch_RAM_WD_CSD_MATRIX_STRUCT *wdCsdMatrix;
    #endif

    if (widgetId >= CapTouch_TOTAL_WIDGETS)
    {
        state = CYRET_BAD_PARAM;
    }
    else
    {
        ptrFlashWdgt = &CapTouch_dsFlash.wdgtArray[widgetId];

        if ((CapTouch_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType != CapTouch_WD_MATRIX_BUTTON_E)
        {
            state = CYRET_BAD_PARAM;
        }
        else if (0u == CapTouch_GET_WIDGET_ACTIVE_STATUS(widgetId))
        {
            state = CYRET_BAD_PARAM;
        }
        else
        {
            /* input parameters are OK */
        }
    }

    if (CYRET_SUCCESS == state)
    {
        switch(CapTouch_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CapTouch_CSD_MATRIX_WIDGET_EN)
            case CapTouch_SENSE_METHOD_CSD_E:
                wdCsdMatrix = ptrFlashWdgt->ptr2WdgtRam;

                result = CapTouch_MATRIX_BUTTONS_TOUCHED |
                         ((uint32)wdCsdMatrix->posSnsId << 16u)  |
                         ((uint32)wdCsdMatrix->posRow   << 8u)   |
                         (uint32)wdCsdMatrix->posCol;
                break;
        #endif /* #if (0u != CapTouch_CSD_MATRIX_WIDGET_EN) */

        #if (0u != CapTouch_CSX_MATRIX_WIDGET_EN)
            case CapTouch_SENSE_METHOD_CSX_E:
                result = CapTouch_dsRam.snsStatus[widgetId];
                break;
        #endif /* #if (0u != CapTouch_CSX_MATRIX_WIDGET_EN) */

        default:
            CYASSERT(0u);
            break;
        }
    }

    return result;
}
#endif /* #if (0u != CapTouch_MATRIX_WIDGET_EN) */

#if (0u != CapTouch_PROXIMITY_WIDGET_EN)
/*******************************************************************************
* Function Name: CapTouch_IsProximitySensorActive
****************************************************************************//**
*
* \brief
*  Reports the finger detection status of the specified proximity widget/sensor.
*
* \details
*  This function reports if the specified proximity sensor has detected a touch
*  or not by extracting information from the wdgtStatus registers
*  (CapTouch_SNS_STATUS<WidgetId>_VALUE). This function is used
*  only with proximity sensor widgets. This function does not process the
*  widget but extracts processed results from the \ref group_structures.
*
* \param widgetId
*  Specifies the ID number of the proximity widget.
*  A macro for the widget ID can be found in the CapTouch Configuration header
*  file defined as CapTouch_<WidgetName>_WDGT_ID
*
* \param proxId
*  Specifies the ID number of the proximity sensor within the proximity widget to
*  get its touch detection status.
*  A macro for the proximity ID within a specified widget can be found in the
*  CapTouch Configuration header file defined as
*  CapTouch_<WidgetName>_SNS<SensorNumber>_ID
*
* \return
*  Returns the status of the specified sensor of the proximity widget. Zero
*  indicates that no touch is detected in the specified sensor / widget or a
*  wrong widgetId / proxId is specified.
*    - Bits [31..2] are reserved.
*    - Bit [1] indicates that a touch is detected.
*    - Bit [0] indicates that a proximity is detected.
*
*******************************************************************************/
uint32 CapTouch_IsProximitySensorActive(uint32 widgetId, uint32 proxId)
{
    uint32 result = 0Lu;

    if ((widgetId < CapTouch_TOTAL_WIDGETS) && (proxId < CapTouch_GET_SENSOR_COUNT(widgetId)) &&
        (CapTouch_WD_PROXIMITY_E ==
            (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[widgetId].wdgtType))
    {
        result = CapTouch_dsRam.snsStatus[widgetId];
        result >>= CapTouch_PROX_STS_OFFSET(proxId);
        result &=  CapTouch_PROX_STS_MASK;
    }

    return result;
}
#endif /* #if (0u != CapTouch_PROXIMITY_WIDGET_EN) */


#if (0u != CapTouch_SLIDER_WIDGET_EN)
/*******************************************************************************
* Function Name: CapTouch_GetCentroidPos
****************************************************************************//**
*
* \brief
*  Reports the centroid position for the specified slider widget.
*
* \details
*  This function reports the centroid value of a specified radial or linear
*  slider widget by extracting information from the wdgtStatus registers
*  (CapTouch_<WidgetName>_POSITION<X>_VALUE). This function is
*  used only with radial or linear slider widgets. This function does not
*  process the widget but extracts processed results from the \ref group_structures.
*
* \param widgetId
*  Specifies the ID number of a slider widget to get the centroid of the
*  detected touch.
*  A macro for the widget ID can be found in the
*  CapTouch Configuration header file defined as
*  CapTouch_<WidgetName>_WDGT_ID
*
* \return
*  Returns the centroid position of a specified slider widget:
*    - The centroid position if a touch is detected.
*    - CapTouch_SLIDER_NO_TOUCH - No touch is detected or a wrong
*      widgetId is specified.
*
*******************************************************************************/
uint32 CapTouch_GetCentroidPos(uint32 widgetId)
{
    uint32 result = CapTouch_SLIDER_NO_TOUCH;
    CapTouch_RAM_WD_SLIDER_STRUCT *wdSlider;

    if ((widgetId < CapTouch_TOTAL_WIDGETS) &&
        ((CapTouch_WD_LINEAR_SLIDER_E ==
            (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[widgetId].wdgtType) ||
         (CapTouch_WD_RADIAL_SLIDER_E ==
            (CapTouch_WD_TYPE_ENUM)CapTouch_dsFlash.wdgtArray[widgetId].wdgtType)))
    {
        wdSlider = CapTouch_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;
        result = (uint32)wdSlider->position[0u];
    }

    return result;
}
#endif /* #if (0u != CapTouch_SLIDER_WIDGET_EN) */


#if (0u != CapTouch_TOUCHPAD_WIDGET_EN)
/*******************************************************************************
* Function Name: CapTouch_GetXYCoordinates
****************************************************************************//**
*
* \brief
*  Reports the X/Y position detected for the specified touchpad widget.
*
* \details
*  This function reports a touch position (X and Y coordinates) value of a
*  specified touchpad widget by extracting information from the wdgtStatus
*  registers (CapTouch_<WidgetName>_POS_Y_VALUE). This function should
*  be used only with the touchpad widgets. This function does not process the
*  widget but extracts processed results from the \ref group_structures.
*
* \param widgetId
*  Specifies the ID number of a touchpad widget to get the X/Y position of a
*  detected touch.
*  A macro for the widget ID can be found in the
*  CapTouch Configuration header file defined as
*  CapTouch_<WidgetName>_WDGT_ID.
*
* \return
*  Returns the touch position of a specified touchpad widget:
*    1. If a touch is detected:
*      - Bits [31..16] indicate the Y coordinate.
*      - Bits [15..0] indicate the X coordinate.
*    2. If no touch is detected or a wrong widgetId is specified:
*      - CapTouch_TOUCHPAD_NO_TOUCH.
*
*******************************************************************************/
uint32 CapTouch_GetXYCoordinates(uint32 widgetId)
{
    CapTouch_FLASH_WD_STRUCT const *ptrFlashWdgt = NULL;

    #if (0u != CapTouch_CSD_TOUCHPAD_WIDGET_EN)
        CapTouch_RAM_WD_CSD_TOUCHPAD_STRUCT *wdCsdTouchpad;
    #endif /* #if (0u != CapTouch_CSD_TOUCHPAD_WIDGET_EN) */

    #if (0u != CapTouch_CSX_TOUCHPAD_WIDGET_EN)
        CapTouch_RAM_WD_CSX_TOUCHPAD_STRUCT *wdCsxTouchpad;
    #endif /* #if (0u != CapTouch_CSX_TOUCHPAD_WIDGET_EN) */

    cystatus state = CYRET_SUCCESS;
    uint32 result = CapTouch_TOUCHPAD_NO_TOUCH;

    if (widgetId >= CapTouch_TOTAL_WIDGETS)
    {
        state = CYRET_BAD_PARAM;
    }
    else
    {
        ptrFlashWdgt = &CapTouch_dsFlash.wdgtArray[widgetId];

        if ((CapTouch_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType != CapTouch_WD_TOUCHPAD_E)
        {
            state = CYRET_BAD_PARAM;
        }
    }

    if (CYRET_SUCCESS == state)
    {
        switch(CapTouch_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CapTouch_CSD_TOUCHPAD_WIDGET_EN)
            case CapTouch_SENSE_METHOD_CSD_E:
                wdCsdTouchpad = ptrFlashWdgt->ptr2WdgtRam;
                #if (CapTouch_ENABLE == CapTouch_CENTROID_5X5_CSD_EN)
                    #if (CapTouch_ENABLE == CapTouch_CENTROID_3X3_CSD_EN)
                        if (0 != (ptrFlashWdgt->staticConfig & CapTouch_CENTROID_5X5_MASK))
                        {
                    #endif
                        result = ((uint32)wdCsdTouchpad->position.pos[0u].x) |
                                 ((uint32)wdCsdTouchpad->position.pos[0u].y << 16u);
                    #if (CapTouch_ENABLE == CapTouch_CENTROID_3X3_CSD_EN)
                        }
                    #endif
                #endif

                #if (CapTouch_ENABLE == CapTouch_CENTROID_3X3_CSD_EN)
                    #if (CapTouch_ENABLE == CapTouch_CENTROID_5X5_CSD_EN)
                        if (0 != (ptrFlashWdgt->staticConfig & CapTouch_CENTROID_3X3_MASK))
                        {
                    #endif
                        result = ((uint32)wdCsdTouchpad->posX) |
                                 ((uint32)wdCsdTouchpad->posY << 16u);
                    #if (CapTouch_ENABLE == CapTouch_CENTROID_5X5_CSD_EN)
                        }
                    #endif
                #endif

                break;
        #endif /* #if (0u != CapTouch_CSD_TOUCHPAD_WIDGET_EN) */

        #if (0u != CapTouch_CSX_TOUCHPAD_WIDGET_EN)
            case CapTouch_SENSE_METHOD_CSX_E:
                wdCsxTouchpad = ptrFlashWdgt->ptr2WdgtRam;

                result = ((uint32)wdCsxTouchpad->touch[0u].x) |
                         ((uint32)wdCsxTouchpad->touch[0u].y << 16u);
                break;
        #endif /* #if (0u != CapTouch_CSX_TOUCHPAD_WIDGET_EN) */

        default:
            CYASSERT(0u);
            break;
        }
    }

    return result;
}
#endif /* #if (0u != CapTouch_TOUCHPAD_WIDGET_EN) */


/* [] END OF FILE */
