/*******************************************************************************
* \file CapTouch_Configuration.h
* \version 6.0
*
* \brief
*   This file provides the customizer parameters definitions.
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

#if !defined(CY_SENSE_CapTouch_CONFIGURATION_H)
#define CY_SENSE_CapTouch_CONFIGURATION_H

#include <cytypes.h>

/*******************************************************************************
* Customizer-generated defines
*******************************************************************************/
#define CapTouch_ENABLE                             (1u)
#define CapTouch_DISABLE                            (0u)

#define CapTouch_THIRD_GENERATION_BLOCK             (1u)
#define CapTouch_FOURTH_GENERATION_BLOCK            (2u)

#define CapTouch_GENERATION_BLOCK_VERSION           (2u)

/*******************************************************************************
* HW IP block global defines
*******************************************************************************/

#if (CapTouch_GENERATION_BLOCK_VERSION == CapTouch_THIRD_GENERATION_BLOCK)
    #define CapTouch_CSDV1                          (CapTouch_ENABLE)
    
    #ifdef CYIPBLOCK_m0s8csd_VERSION
        #if (0u == CYIPBLOCK_m0s8csd_VERSION)
            #define CapTouch_CSDV1_VER1             (CapTouch_ENABLE)
            #define CapTouch_CSDV1_VER2             (CapTouch_DISABLE)
        #else
            #define CapTouch_CSDV1_VER1             (CapTouch_DISABLE)
            #define CapTouch_CSDV1_VER2             (CapTouch_ENABLE)
        #endif
    #else
        #error "HW IP block version is not specified"
    #endif
#else
    #define CapTouch_CSDV1                          (CapTouch_DISABLE)
    #define CapTouch_CSDV1_VER1                     (CapTouch_DISABLE)
    #define CapTouch_CSDV1_VER2                     (CapTouch_DISABLE)
#endif

#if (CapTouch_GENERATION_BLOCK_VERSION == CapTouch_FOURTH_GENERATION_BLOCK)
    #define CapTouch_CSDV2                          (CapTouch_ENABLE)
    
    #ifdef CYIPBLOCK_m0s8csdv2_VERSION
        #if (1u == CYIPBLOCK_m0s8csdv2_VERSION)
            #define CapTouch_CSDV2_VER1             (CapTouch_ENABLE)
        #else
            #define CapTouch_CSDV2_VER1             (CapTouch_DISABLE)
        #endif
        #if (2u == CYIPBLOCK_m0s8csdv2_VERSION)
            #define CapTouch_CSDV2_VER2             (CapTouch_ENABLE)
        #else
            #define CapTouch_CSDV2_VER2             (CapTouch_DISABLE)
        #endif
    #else
        #error "HW IP block version is not specified"
    #endif
#else
    #define CapTouch_CSDV2                          (CapTouch_DISABLE)
    #define CapTouch_CSDV2_VER1                     (CapTouch_DISABLE)
    #define CapTouch_CSDV2_VER2                     (CapTouch_DISABLE)
#endif

/*******************************************************************************
* Project-global defines
*******************************************************************************/

#define CapTouch_2000_MV                            (2000u)

#ifdef CYDEV_VDDA_MV
    #define CapTouch_CYDEV_VDDA_MV                  (CYDEV_VDDA_MV)
#else
    #ifdef CYDEV_VDD_MV
        #define CapTouch_CYDEV_VDDA_MV              (CYDEV_VDD_MV)
    #endif
#endif

#define CapTouch_BAD_CONVERSIONS_NUM                (1u)
#define CapTouch_RESAMPLING_CYCLES_MAX_NUMBER       (1u)


/*******************************************************************************
* Enabled Scan Methods
*******************************************************************************/
#define CapTouch_CSD_EN                             (0u)
#define CapTouch_CSX_EN                             (1u)
#define CapTouch_ISX_EN                             (0u)
#define CapTouch_CSD_CSX_EN                         (CapTouch_CSD_EN && CapTouch_CSX_EN)

#define CapTouch_MANY_SENSE_MODES_EN                ((CapTouch_CSD_EN && CapTouch_CSX_EN) || \
                                                             (CapTouch_CSD_EN && CapTouch_ISX_EN) || \
                                                             (CapTouch_CSX_EN && CapTouch_ISX_EN) || \
                                                             (CapTouch_SELF_TEST_EN))

#define CapTouch_MANY_WIDGET_METHODS_EN             ((CapTouch_CSD_EN && CapTouch_CSX_EN) || \
                                                             (CapTouch_CSD_EN && CapTouch_ISX_EN) || \
                                                             (CapTouch_CSX_EN && CapTouch_ISX_EN))

#define CapTouch_CSD2X_EN                           (0u)
#define CapTouch_CSX2X_EN                           (0u)

/*******************************************************************************
* Definitions for number of widgets and sensors
*******************************************************************************/
#define CapTouch_TOTAL_WIDGETS                      (13u)
#define CapTouch_TOTAL_CSD_WIDGETS                  (0u)
#define CapTouch_TOTAL_CSD_SENSORS                  (0u)
#define CapTouch_TOTAL_CSX_WIDGETS                  (13u)
#define CapTouch_TOTAL_ISX_WIDGETS                  (0u)
#define CapTouch_TOTAL_CSX_NODES                    (13u)
#define CapTouch_TOTAL_ISX_NODES                    (0u)

/*******************************************************************************
* Total number of CSD sensors + CSX nodes
*******************************************************************************/
#define CapTouch_TOTAL_SENSORS            (CapTouch_TOTAL_CSD_SENSORS + \
                                                   CapTouch_TOTAL_CSX_NODES+ \
                                                   CapTouch_TOTAL_ISX_NODES)

/*******************************************************************************
* Total number of scan slots (used only when dual-channel scan is enabled)
*******************************************************************************/
#define CapTouch_TOTAL_SCAN_SLOTS         (26u)

/*******************************************************************************
* Defines widget IDs
*******************************************************************************/
#define CapTouch_U102_WDGT_ID                   (0u)
#define CapTouch_U103_WDGT_ID                   (1u)
#define CapTouch_U104_WDGT_ID                   (2u)
#define CapTouch_U105_WDGT_ID                   (3u)
#define CapTouch_U106_WDGT_ID                   (4u)
#define CapTouch_U107_WDGT_ID                   (5u)
#define CapTouch_U108_WDGT_ID                   (6u)
#define CapTouch_U109_WDGT_ID                   (7u)
#define CapTouch_U110_WDGT_ID                   (8u)
#define CapTouch_U111_WDGT_ID                   (9u)
#define CapTouch_U112_WDGT_ID                   (10u)
#define CapTouch_U113_WDGT_ID                   (11u)
#define CapTouch_U114_WDGT_ID                   (12u)

/*******************************************************************************
* Defines sensor IDs
*******************************************************************************/

/* U102 sensor names */
#define CapTouch_U102_SNS0_ID                   (0u)

/* U102 node names */
#define CapTouch_U102_RX0_TX0_ID                (0u)

/* U102 sensor element IDs */
#define CapTouch_U102_RX0_ID                    (0u)
#define CapTouch_U102_TX0_ID                    (1u)

/* U103 sensor names */
#define CapTouch_U103_SNS0_ID                   (0u)

/* U103 node names */
#define CapTouch_U103_RX0_TX0_ID                (0u)

/* U103 sensor element IDs */
#define CapTouch_U103_RX0_ID                    (0u)
#define CapTouch_U103_TX0_ID                    (1u)

/* U104 sensor names */
#define CapTouch_U104_SNS0_ID                   (0u)

/* U104 node names */
#define CapTouch_U104_RX0_TX0_ID                (0u)

/* U104 sensor element IDs */
#define CapTouch_U104_RX0_ID                    (0u)
#define CapTouch_U104_TX0_ID                    (1u)

/* U105 sensor names */
#define CapTouch_U105_SNS0_ID                   (0u)

/* U105 node names */
#define CapTouch_U105_RX0_TX0_ID                (0u)

/* U105 sensor element IDs */
#define CapTouch_U105_RX0_ID                    (0u)
#define CapTouch_U105_TX0_ID                    (1u)

/* U106 sensor names */
#define CapTouch_U106_SNS0_ID                   (0u)

/* U106 node names */
#define CapTouch_U106_RX0_TX0_ID                (0u)

/* U106 sensor element IDs */
#define CapTouch_U106_RX0_ID                    (0u)
#define CapTouch_U106_TX0_ID                    (1u)

/* U107 sensor names */
#define CapTouch_U107_SNS0_ID                   (0u)

/* U107 node names */
#define CapTouch_U107_RX0_TX0_ID                (0u)

/* U107 sensor element IDs */
#define CapTouch_U107_RX0_ID                    (0u)
#define CapTouch_U107_TX0_ID                    (1u)

/* U108 sensor names */
#define CapTouch_U108_SNS0_ID                   (0u)

/* U108 node names */
#define CapTouch_U108_RX0_TX0_ID                (0u)

/* U108 sensor element IDs */
#define CapTouch_U108_RX0_ID                    (0u)
#define CapTouch_U108_TX0_ID                    (1u)

/* U109 sensor names */
#define CapTouch_U109_SNS0_ID                   (0u)

/* U109 node names */
#define CapTouch_U109_RX0_TX0_ID                (0u)

/* U109 sensor element IDs */
#define CapTouch_U109_RX0_ID                    (0u)
#define CapTouch_U109_TX0_ID                    (1u)

/* U110 sensor names */
#define CapTouch_U110_SNS0_ID                   (0u)

/* U110 node names */
#define CapTouch_U110_RX0_TX0_ID                (0u)

/* U110 sensor element IDs */
#define CapTouch_U110_RX0_ID                    (0u)
#define CapTouch_U110_TX0_ID                    (1u)

/* U111 sensor names */
#define CapTouch_U111_SNS0_ID                   (0u)

/* U111 node names */
#define CapTouch_U111_RX0_TX0_ID                (0u)

/* U111 sensor element IDs */
#define CapTouch_U111_RX0_ID                    (0u)
#define CapTouch_U111_TX0_ID                    (1u)

/* U112 sensor names */
#define CapTouch_U112_SNS0_ID                   (0u)

/* U112 node names */
#define CapTouch_U112_RX0_TX0_ID                (0u)

/* U112 sensor element IDs */
#define CapTouch_U112_RX0_ID                    (0u)
#define CapTouch_U112_TX0_ID                    (1u)

/* U113 sensor names */
#define CapTouch_U113_SNS0_ID                   (0u)

/* U113 node names */
#define CapTouch_U113_RX0_TX0_ID                (0u)

/* U113 sensor element IDs */
#define CapTouch_U113_RX0_ID                    (0u)
#define CapTouch_U113_TX0_ID                    (1u)

/* U114 sensor names */
#define CapTouch_U114_SNS0_ID                   (0u)

/* U114 node names */
#define CapTouch_U114_RX0_TX0_ID                (0u)

/* U114 sensor element IDs */
#define CapTouch_U114_RX0_ID                    (0u)
#define CapTouch_U114_TX0_ID                    (1u)



/*******************************************************************************
* Enabled widget types
*******************************************************************************/
#define CapTouch_BUTTON_WIDGET_EN         (1u)
#define CapTouch_SLIDER_WIDGET_EN         (0u)
#define CapTouch_MATRIX_WIDGET_EN         (0u)
#define CapTouch_PROXIMITY_WIDGET_EN      (0u)
#define CapTouch_TOUCHPAD_WIDGET_EN       (0u)
#define CapTouch_ENCODERDIAL_WIDGET_EN    (0u)

#define CapTouch_CSD_MATRIX_WIDGET_EN     (0u)
#define CapTouch_CSD_TOUCHPAD_WIDGET_EN   (0u)

#define CapTouch_CSX_MATRIX_WIDGET_EN     (0u)
#define CapTouch_CSX_TOUCHPAD_WIDGET_EN   (0u)

/*******************************************************************************
* Centroid APIs
*******************************************************************************/
#define CapTouch_CENTROID_EN              (0u)
#define CapTouch_TOTAL_DIPLEXED_SLIDERS   (0u)
#define CapTouch_TOTAL_LINEAR_SLIDERS     (0u)
#define CapTouch_TOTAL_RADIAL_SLIDERS     (0u)
#define CapTouch_TOTAL_TOUCHPADS          (0u)
#define CapTouch_MAX_CENTROID_LENGTH      (0u)
#define CapTouch_SLIDER_MULT_METHOD       (0u)
#define CapTouch_TOUCHPAD_MULT_METHOD     (0u)

/*******************************************************************************
* Enabled sensor types
*******************************************************************************/
#define CapTouch_REGULAR_SENSOR_EN        (1u)
#define CapTouch_PROXIMITY_SENSOR_EN      (0u)

/*******************************************************************************
* Sensor ganging
*******************************************************************************/
#define CapTouch_GANGED_SNS_EN            (0u)
#define CapTouch_CSD_GANGED_SNS_EN        (0u)
#define CapTouch_CSX_GANGED_SNS_EN        (0u)

/*******************************************************************************
* Max number of sensors used among all the widgets
*******************************************************************************/
#define CapTouch_MAX_SENSORS_PER_WIDGET   (1u)
#define CapTouch_MAX_SENSORS_PER_5X5_TOUCHPAD (1u)

/*******************************************************************************
* Total number of all used electrodes (NOT unique)
*******************************************************************************/
#define CapTouch_TOTAL_ELECTRODES         (26u)
/* Obsolete */
#define CapTouch_TOTAL_SENSOR_IOS         CapTouch_TOTAL_ELECTRODES

/*******************************************************************************
* Total number of used physical IOs (unique)
*******************************************************************************/
#define CapTouch_TOTAL_IO_CNT             (8u)

/*******************************************************************************
* Array length for widget status registers
*******************************************************************************/
#define CapTouch_WDGT_STATUS_WORDS        \
                        (((uint8)((CapTouch_TOTAL_WIDGETS - 1u) / 32u)) + 1u)


/*******************************************************************************
* Auto-tuning mode selection
*******************************************************************************/
#define CapTouch_CSD_SS_DIS         (0x00ul)
#define CapTouch_CSD_SS_HW_EN       (0x01ul)
#define CapTouch_CSD_SS_TH_EN       (0x02ul)
#define CapTouch_CSD_SS_HWTH_EN     (CapTouch_CSD_SS_HW_EN | \
                                             CapTouch_CSD_SS_TH_EN)

#define CapTouch_CSD_AUTOTUNE       CapTouch_CSD_SS_DIS


/*******************************************************************************
* General settings
*******************************************************************************/

#define CapTouch_AUTO_RESET_METHOD_LEGACY (0u)
#define CapTouch_AUTO_RESET_METHOD_SAMPLE (1u)

#define CapTouch_MULTI_FREQ_SCAN_EN       (0u)
#define CapTouch_SENSOR_AUTO_RESET_EN     (0u)
#define CapTouch_SENSOR_AUTO_RESET_METHOD (0u)
#define CapTouch_NUM_CENTROIDS            (1u)
#define CapTouch_4PTS_LOCAL_MAX_EN        (0u)
#define CapTouch_OFF_DEBOUNCE_EN          (0u)
#define CapTouch_CUSTOM_DS_RAM_SIZE       (0u)

/* Defines power status of HW block after scanning */
#define CapTouch_BLOCK_OFF_AFTER_SCAN_EN  (0u)

/* Defines number of scan frequencies */
#if (CapTouch_DISABLE != CapTouch_MULTI_FREQ_SCAN_EN)
    #define CapTouch_NUM_SCAN_FREQS       (3u)
#else
    #define CapTouch_NUM_SCAN_FREQS       (1u)
#endif /* #if (CapTouch_DISABLE != CapTouch_MULTI_FREQ_SCAN_EN) */

/* Data size for thresholds / low baseline reset */
#define CapTouch_SIZE_8BITS               (8u)
#define CapTouch_SIZE_16BITS              (16u)

#define CapTouch_THRESHOLD_SIZE           CapTouch_SIZE_16BITS
typedef uint16 CapTouch_THRESHOLD_TYPE;

#if (CapTouch_AUTO_RESET_METHOD_LEGACY == CapTouch_SENSOR_AUTO_RESET_METHOD)
    #define CapTouch_LOW_BSLN_RST_SIZE        CapTouch_SIZE_8BITS
    typedef uint8 CapTouch_LOW_BSLN_RST_TYPE;
#else
    #define CapTouch_LOW_BSLN_RST_SIZE    (16u)
    typedef uint16 CapTouch_LOW_BSLN_RST_TYPE;
#endif /* #if (CapTouch_AUTO_RESET_METHOD_LEGACY == CapTouch_SENSOR_AUTO_RESET_METHOD) */

/* Coefficient to define touch threshold for proximity sensors */
#define CapTouch_PROX_TOUCH_COEFF         (300u)

/*******************************************************************************
* General Filter Constants
*******************************************************************************/

/* Baseline algorithm options */
#define CapTouch_IIR_BASELINE                 (0u)
#define CapTouch_BUCKET_BASELINE              (1u)

#define CapTouch_BASELINE_TYPE                CapTouch_IIR_BASELINE

/* IIR baseline filter algorithm for regular sensors*/
#define CapTouch_REGULAR_IIR_BL_TYPE          CapTouch_IIR_FILTER_PERFORMANCE

/* IIR baseline coefficients for regular sensors */
#define CapTouch_REGULAR_IIR_BL_N             (1u)
#define CapTouch_REGULAR_IIR_BL_SHIFT         (8u)

/* IIR baseline filter algorithm for proximity sensors*/
#define CapTouch_PROX_IIR_BL_TYPE             CapTouch_IIR_FILTER_PERFORMANCE

/* IIR baseline coefficients for proximity sensors */
#define CapTouch_PROX_IIR_BL_N                (1u)
#define CapTouch_PROX_IIR_BL_SHIFT            (8u)


/* IIR filter constants */
#define CapTouch_IIR_COEFFICIENT_K            (256u)

/* IIR filter type */
#define CapTouch_IIR_FILTER_STANDARD          (1u)
#define CapTouch_IIR_FILTER_PERFORMANCE       (2u)
#define CapTouch_IIR_FILTER_MEMORY            (3u)

/* Regular sensor raw count filters */
#define CapTouch_REGULAR_RC_FILTER_EN         (1u)
#define CapTouch_REGULAR_RC_IIR_FILTER_EN     (1u)
#define CapTouch_REGULAR_RC_MEDIAN_FILTER_EN  (0u)
#define CapTouch_REGULAR_RC_AVERAGE_FILTER_EN (0u)
#define CapTouch_REGULAR_RC_CUSTOM_FILTER_EN  (0u)
#define CapTouch_REGULAR_RC_ALP_FILTER_EN     (0u)

/* Proximity sensor raw count filters */
#define CapTouch_PROX_RC_FILTER_EN            (0u)
#define CapTouch_PROX_RC_IIR_FILTER_EN        (0u)
#define CapTouch_PROX_RC_MEDIAN_FILTER_EN     (0u)
#define CapTouch_PROX_RC_AVERAGE_FILTER_EN    (0u)
#define CapTouch_PROX_RC_CUSTOM_FILTER_EN     (0u)
#define CapTouch_PROX_RC_ALP_FILTER_EN        (0u)

#define CapTouch_ALP_FILTER_EN                (0u)
#define CapTouch_REGULAR_RC_ALP_FILTER_COEFF  (2u)
#define CapTouch_PROX_RC_ALP_FILTER_COEFF     (2u)

/* Raw count filters */
#define CapTouch_RC_FILTER_EN                 (CapTouch_REGULAR_RC_FILTER_EN || CapTouch_PROX_RC_FILTER_EN)

/* IIR raw count filter algorithm for regular sensors */
#define CapTouch_REGULAR_IIR_RC_TYPE          (CapTouch_IIR_FILTER_STANDARD)

/* IIR raw count filter coefficients for regular sensors */
#define CapTouch_REGULAR_IIR_RC_N             (128u)
#define CapTouch_REGULAR_IIR_RC_SHIFT         (0u)

/* IIR raw count filter algorithm for proximity sensors*/
#define CapTouch_PROX_IIR_RC_TYPE             (CapTouch_IIR_FILTER_STANDARD)

/* IIR raw count filter coefficients for proximity sensors */
#define CapTouch_PROX_IIR_RC_N                (128u)
#define CapTouch_PROX_IIR_RC_SHIFT            (0u)

/* Median filter constants */

/* Order of regular sensor median filter */
#define CapTouch_REGULAR_MEDIAN_LEN           (2u)

/* Order of proximity sensor median filter */
#define CapTouch_PROX_MEDIAN_LEN              (2u)

/* Average filter constants*/
#define CapTouch_AVERAGE_FILTER_LEN_2         (1u)
#define CapTouch_AVERAGE_FILTER_LEN_4         (3u)

/* Order of regular sensor average filter */
#define CapTouch_REGULAR_AVERAGE_LEN          (CapTouch_AVERAGE_FILTER_LEN_4)

/* Order of proximity sensor average filter */
#define CapTouch_PROX_AVERAGE_LEN             (CapTouch_AVERAGE_FILTER_LEN_4)

/* Widget baseline coefficient enable */
#define CapTouch_WD_BSLN_COEFF_EN             (0u)

/* Centroid position filters */
#define CapTouch_POSITION_FILTER_EN           (0u)
#define CapTouch_POS_MEDIAN_FILTER_EN         (0u)
#define CapTouch_POS_IIR_FILTER_EN            (0u)
#define CapTouch_POS_ADAPTIVE_IIR_FILTER_EN   (0u)
#define CapTouch_POS_AVERAGE_FILTER_EN        (0u)
#define CapTouch_POS_JITTER_FILTER_EN         (0u)
#define CapTouch_BALLISTIC_MULTIPLIER_EN      (0u)
#define CapTouch_CENTROID_3X3_CSD_EN          (0u)
#define CapTouch_CENTROID_5X5_CSD_EN          (0u)
#define CapTouch_CSD_5X5_MAX_FINGERS          (1u)

#define CapTouch_POS_IIR_COEFF                (128u)
#define CapTouch_POS_IIR_RESET_RADIAL_SLIDER  (35u)

#define CapTouch_CSX_TOUCHPAD_UNDEFINED       (40u)

/* IDAC options */

/* Third-generation HW block IDAC gain */
#define CapTouch_IDAC_GAIN_4X                 (4u)
#define CapTouch_IDAC_GAIN_8X                 (8u)

/* Fourth-generation HW block IDAC gain */
#define CapTouch_IDAC_GAIN_LOW                (0uL)
#define CapTouch_IDAC_GAIN_MEDIUM             (1uL)
#define CapTouch_IDAC_GAIN_HIGH               (2uL)

#define CapTouch_IDAC_SOURCING                (0u)
#define CapTouch_IDAC_SINKING                 (1u)

/* Shield tank capacitor precharge source */
#define CapTouch_CSH_PRECHARGE_VREF           (0u)
#define CapTouch_CSH_PRECHARGE_IO_BUF         (1u)

/* Shield electrode delay */
#define CapTouch_NO_DELAY                     (0u)

#if(CapTouch_ENABLE == CapTouch_CSDV2)
    #define CapTouch_SH_DELAY_5NS             (1u)
    #define CapTouch_SH_DELAY_10NS            (2u)
    #define CapTouch_SH_DELAY_20NS            (3u)
#else
    #if(CapTouch_ENABLE == CapTouch_CSDV1_VER2)
        #define CapTouch_SH_DELAY_10NS        (3u)
        #define CapTouch_SH_DELAY_50NS        (2u)
    #else
        #define CapTouch_SH_DELAY_1CYCLES     (1u)
        #define CapTouch_SH_DELAY_2CYCLES     (2u)
    #endif /* (CapTouch_ENABLE == CapTouch_CSDV1_VER2) */
#endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

/* Inactive sensor connection options */
#define CapTouch_SNS_CONNECTION_GROUND        (0x00000006Lu)
#define CapTouch_SNS_CONNECTION_HIGHZ         (0x00000000Lu)
#define CapTouch_SNS_CONNECTION_SHIELD        (0x00000002Lu)

/* Sense clock selection options */
#if defined(CapTouch_TAPEOUT_STAR_USED)
    #define CapTouch_CSDV2_REF9P6UA_EN            (0u)
#else
    #define CapTouch_CSDV2_REF9P6UA_EN            (1u)
#endif /* defined(CapTouch_TAPEOUT_STAR_USED) */

#define CapTouch_CLK_SOURCE_DIRECT            (0x00000000Lu)

#define CapTouch_CLK_SOURCE_SSC1              (0x01u)
#define CapTouch_CLK_SOURCE_SSC2              (0x02u)
#define CapTouch_CLK_SOURCE_SSC3              (0x03u)
#define CapTouch_CLK_SOURCE_SSC4              (0x04u)

#define CapTouch_CLK_SOURCE_PRS8              (0x05u)
#define CapTouch_CLK_SOURCE_PRS12             (0x06u)
#define CapTouch_CLK_SOURCE_PRSAUTO           (0xFFu)

#define CapTouch_MFS_IMO                      (0u)
#define CapTouch_MFS_SNS_CLK                  (1u)

/* Defines scan resolutions */
#define CapTouch_RES6BIT                      (6u)
#define CapTouch_RES7BIT                      (7u)
#define CapTouch_RES8BIT                      (8u)
#define CapTouch_RES9BIT                      (9u)
#define CapTouch_RES10BIT                     (10u)
#define CapTouch_RES11BIT                     (11u)
#define CapTouch_RES12BIT                     (12u)
#define CapTouch_RES13BIT                     (13u)
#define CapTouch_RES14BIT                     (14u)
#define CapTouch_RES15BIT                     (15u)
#define CapTouch_RES16BIT                     (16u)

/* Fourth-generation HW block: Initialization switch resistance */
#define CapTouch_INIT_SW_RES_LOW              (0x00000000Lu)
#define CapTouch_INIT_SW_RES_MEDIUM           (0x00000001Lu)
#define CapTouch_INIT_SW_RES_HIGH             (0x00000002Lu)

/* Fourth-generation HW block: Initialization switch resistance */
#define CapTouch_SCAN_SW_RES_LOW              (0x00000000Lu)
#define CapTouch_SCAN_SW_RES_MEDIUM           (0x00000001Lu)
#define CapTouch_SCAN_SW_RES_HIGH             (0x00000002Lu)

/* Fourth-generation HW block: CSD shield switch resistance */
#define CapTouch_SHIELD_SW_RES_LOW            (0x00000000Lu)
#define CapTouch_SHIELD_SW_RES_MEDIUM         (0x00000001Lu)
#define CapTouch_SHIELD_SW_RES_HIGH           (0x00000002Lu)
#define CapTouch_SHIELD_SW_RES_LOW_EMI        (0x00000003Lu)

/* Fourth-generation HW block: CSD shield switch resistance */
#define CapTouch_INIT_SHIELD_SW_RES_LOW       (0x00000000Lu)
#define CapTouch_INIT_SHIELD_SW_RES_MEDIUM    (0x00000001Lu)
#define CapTouch_INIT_SHIELD_SW_RES_HIGH      (0x00000002Lu)
#define CapTouch_INIT_SHIELD_SW_RES_LOW_EMI   (0x00000003Lu)

/* Fourth-generation HW block: CSD shield switch resistance */
#define CapTouch_SCAN_SHIELD_SW_RES_LOW       (0x00000000Lu)
#define CapTouch_SCAN_SHIELD_SW_RES_MEDIUM    (0x00000001Lu)
#define CapTouch_SCAN_SHIELD_SW_RES_HIGH      (0x00000002Lu)
#define CapTouch_SCAN_SHIELD_SW_RES_LOW_EMI   (0x00000003Lu)

/* Sensing method */
#define CapTouch_SENSING_LEGACY               (0x00000000Lu)
#define CapTouch_SENSING_LOW_EMI              (0x00000001Lu)
#define CapTouch_SENSING_FULL_WAVE            (0x00000002Lu)


/*******************************************************************************
* CSD/CSX Common settings
*******************************************************************************/

#define CapTouch_BLOCK_ANALOG_WAKEUP_DELAY_US (0u)

#define CapTouch_MFS_METHOD                   (0u)
#define CapTouch_IMO_FREQUENCY_OFFSET_F1      (20u)
#define CapTouch_IMO_FREQUENCY_OFFSET_F2      (20u)

/*******************************************************************************
* CSD Specific settings
*******************************************************************************/

/* CSD scan method settings */
#define CapTouch_CSD_IDAC_AUTOCAL_EN          (0u)
#define CapTouch_CSD_IDAC_GAIN                (CapTouch_IDAC_GAIN_HIGH)
#define CapTouch_CSD_SHIELD_EN                (0u)
#define CapTouch_CSD_SHIELD_TANK_EN           (0u)
#define CapTouch_CSD_CSH_PRECHARGE_SRC        (CapTouch_CSH_PRECHARGE_VREF)
#define CapTouch_CSD_SHIELD_DELAY             (CapTouch_NO_DELAY)
#define CapTouch_CSD_TOTAL_SHIELD_COUNT       (0u)
#define CapTouch_CSD_SCANSPEED_DIVIDER        (1u)
#define CapTouch_CSD_COMMON_SNS_CLK_EN        (0u)
#define CapTouch_CSD_SNS_CLK_SOURCE           (CapTouch_CLK_SOURCE_PRSAUTO)
#define CapTouch_CSD_SNS_CLK_DIVIDER          (8u)
#define CapTouch_CSD_INACTIVE_SNS_CONNECTION  (CapTouch_SNS_CONNECTION_GROUND)
#define CapTouch_CSD_IDAC_COMP_EN             (0u)
#define CapTouch_CSD_IDAC_CONFIG              (CapTouch_IDAC_SOURCING)
#define CapTouch_CSD_RAWCOUNT_CAL_LEVEL       (85u)
#define CapTouch_CSD_DUALIDAC_LEVEL           (50u)
#define CapTouch_CSD_PRESCAN_SETTLING_TIME    (5u)
#define CapTouch_CSD_SNSCLK_R_CONST           (1000u)
#define CapTouch_CSD_VREF_MV                  (2743u)

/* CSD settings - Fourth-generation HW block */
#define CapTouch_CSD_ANALOG_STARTUP_DELAY_US  (10u)
#define CapTouch_CSD_FINE_INIT_TIME           (10u)
#define CapTouch_CSD_DEDICATED_IDAC_COMP_EN   (0u)
#define CapTouch_CSD_AUTO_ZERO_EN             (0u)
#define CapTouch_CSD_AUTO_ZERO_TIME           (15Lu)
#define CapTouch_CSD_NOISE_METRIC_EN          (0u)
#define CapTouch_CSD_NOISE_METRIC_TH          (1Lu)
#define CapTouch_CSD_INIT_SWITCH_RES          (CapTouch_INIT_SW_RES_MEDIUM)
#define CapTouch_CSD_SENSING_METHOD           (0)
#define CapTouch_CSD_SHIELD_SWITCH_RES        (CapTouch_SHIELD_SW_RES_MEDIUM)
#define CapTouch_CSD_GAIN                     (13Lu)

#define CapTouch_CSD_MFS_METHOD               (u)
#define CapTouch_CSD_MFS_DIVIDER_OFFSET_F1    (1u)
#define CapTouch_CSD_MFS_DIVIDER_OFFSET_F2    (2u)

/*******************************************************************************
* CSX Specific settings
*******************************************************************************/

/* CSX scan method settings */
#define CapTouch_CSX_SCANSPEED_DIVIDER        (1u)
#define CapTouch_CSX_COMMON_TX_CLK_EN         (0u)
#define CapTouch_CSX_TX_CLK_SOURCE            (CapTouch_CLK_SOURCE_PRSAUTO)
#define CapTouch_CSX_TX_CLK_DIVIDER           (80u)
#define CapTouch_CSX_MAX_FINGERS              (1u)
#define CapTouch_CSX_MAX_LOCAL_PEAKS          (5u)
#define CapTouch_CSX_IDAC_AUTOCAL_EN          (1u)
#define CapTouch_CSX_IDAC_BITS_USED           (7u)
#define CapTouch_CSX_RAWCOUNT_CAL_LEVEL       (30u)
#define CapTouch_CSX_IDAC_GAIN                (CapTouch_IDAC_GAIN_MEDIUM)
#define CapTouch_CSX_SKIP_OVSMPL_SPECIFIC_NODES (0u)
#define CapTouch_CSX_MULTIPHASE_TX_EN         (0u)
#define CapTouch_CSX_MAX_TX_PHASE_LENGTH      (0u)

/* CSX settings - Fourth-generation HW block */
#define CapTouch_CSX_ANALOG_STARTUP_DELAY_US  (10u)
#define CapTouch_CSX_AUTO_ZERO_EN             (0u)
#define CapTouch_CSX_AUTO_ZERO_TIME           (15u)
#define CapTouch_CSX_FINE_INIT_TIME           (4u)
#define CapTouch_CSX_NOISE_METRIC_EN          (0u)
#define CapTouch_CSX_NOISE_METRIC_TH          (1u)
#define CapTouch_CSX_INIT_SWITCH_RES          (CapTouch_INIT_SW_RES_MEDIUM)
#define CapTouch_CSX_SCAN_SWITCH_RES          (CapTouch_SCAN_SW_RES_LOW)
#define CapTouch_CSX_INIT_SHIELD_SWITCH_RES   (CapTouch_INIT_SHIELD_SW_RES_HIGH)
#define CapTouch_CSX_SCAN_SHIELD_SWITCH_RES   (CapTouch_SCAN_SHIELD_SW_RES_LOW)

#define CapTouch_CSX_MFS_METHOD               (u)
#define CapTouch_CSX_MFS_DIVIDER_OFFSET_F1    (1u)
#define CapTouch_CSX_MFS_DIVIDER_OFFSET_F2    (2u)

/* Gesture parameters */
#define CapTouch_GES_GLOBAL_EN                (0u)

/*******************************************************************************
* ISX Specific settings
*******************************************************************************/

/* ISX scan method settings */
#define CapTouch_ISX_SCANSPEED_DIVIDER        (1u)
#define CapTouch_ISX_LX_CLK_DIVIDER           (80u)
#define CapTouch_ISX_IDAC_AUTOCAL_EN          (0u)
#define CapTouch_ISX_IDAC_BITS_USED           (7u)
#define CapTouch_ISX_RAWCOUNT_CAL_LEVEL       (30u)
#define CapTouch_ISX_IDAC_GAIN                (CapTouch_IDAC_GAIN_MEDIUM)
#define CapTouch_ISX_SKIP_OVSMPL_SPECIFIC_NODES (0u)
#define CapTouch_ISX_MAX_TX_PHASE_LENGTH      (0u)
#define CapTouch_ISX_PIN_COUNT_LX             (u)
/* ISX settings - Fourth-generation HW block */
#define CapTouch_ISX_AUTO_ZERO_EN             (0u)
#define CapTouch_ISX_AUTO_ZERO_TIME           (15u)
#define CapTouch_ISX_FINE_INIT_TIME           (20u)
#define CapTouch_ISX_NOISE_METRIC_EN          (0u)
#define CapTouch_ISX_NOISE_METRIC_TH          (1u)
#define CapTouch_ISX_INIT_SWITCH_RES          (CapTouch_INIT_SW_RES_MEDIUM)
#define CapTouch_ISX_SCAN_SWITCH_RES          (CapTouch_SCAN_SW_RES_LOW)
#define CapTouch_ISX_INIT_SHIELD_SWITCH_RES   (CapTouch_INIT_SHIELD_SW_RES_HIGH)
#define CapTouch_ISX_SCAN_SHIELD_SWITCH_RES   (CapTouch_SCAN_SHIELD_SW_RES_LOW)
#define CapTouch_ISX_SAMPLE_PHASE_DEG         (30u)

/*******************************************************************************
* Global Parameter Definitions
*******************************************************************************/

/* Compound section definitions */
#define CapTouch_ANY_NONSS_AUTOCAL ((0u != CapTouch_CSX_IDAC_AUTOCAL_EN) || \
                                       (0u != CapTouch_ISX_IDAC_AUTOCAL_EN) || \
                                      ((CapTouch_CSD_AUTOTUNE == CapTouch_CSD_SS_DIS) && (0u != CapTouch_CSD_IDAC_AUTOCAL_EN)))
#define CapTouch_ANYMODE_AUTOCAL (((0u != CapTouch_CSX_IDAC_AUTOCAL_EN) \
                                       || (0u != CapTouch_ISX_IDAC_AUTOCAL_EN)) \
                                       || (0u != CapTouch_CSD_IDAC_AUTOCAL_EN))
/* RAM Global Parameters Definitions */
#define CapTouch_CONFIG_ID                      (0x6DF0u)
#define CapTouch_DEVICE_ID                      (0x0100u)
#define CapTouch_HW_CLOCK                       (0x0BB8u)
#define CapTouch_CSD0_CONFIG                    (0x0008u)
#define CapTouch_GLB_CRC                        (0x7444u)

/*******************************************************************************
* U102 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U102_STATIC_CONFIG             (10249u)
#define CapTouch_U102_NUM_SENSORS               (1u)

/*******************************************************************************
* U102 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U102_CRC                       (0xF522u)
#define CapTouch_U102_RESOLUTION                (150u)
#define CapTouch_U102_FINGER_TH                 (245u)
#define CapTouch_U102_NOISE_TH                  (32u)
#define CapTouch_U102_NNOISE_TH                 (32u)
#define CapTouch_U102_HYSTERESIS                (24u)
#define CapTouch_U102_ON_DEBOUNCE               (3u)
#define CapTouch_U102_LOW_BSLN_RST              (32u)
#define CapTouch_U102_SNS_CLK                   (80u)
#define CapTouch_U102_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U103 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U103_STATIC_CONFIG             (10249u)
#define CapTouch_U103_NUM_SENSORS               (1u)

/*******************************************************************************
* U103 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U103_CRC                       (0x9170u)
#define CapTouch_U103_RESOLUTION                (150u)
#define CapTouch_U103_FINGER_TH                 (225u)
#define CapTouch_U103_NOISE_TH                  (34u)
#define CapTouch_U103_NNOISE_TH                 (34u)
#define CapTouch_U103_HYSTERESIS                (25u)
#define CapTouch_U103_ON_DEBOUNCE               (3u)
#define CapTouch_U103_LOW_BSLN_RST              (34u)
#define CapTouch_U103_SNS_CLK                   (80u)
#define CapTouch_U103_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U104 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U104_STATIC_CONFIG             (10249u)
#define CapTouch_U104_NUM_SENSORS               (1u)

/*******************************************************************************
* U104 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U104_CRC                       (0x01FCu)
#define CapTouch_U104_RESOLUTION                (150u)
#define CapTouch_U104_FINGER_TH                 (270u)
#define CapTouch_U104_NOISE_TH                  (32u)
#define CapTouch_U104_NNOISE_TH                 (32u)
#define CapTouch_U104_HYSTERESIS                (30u)
#define CapTouch_U104_ON_DEBOUNCE               (3u)
#define CapTouch_U104_LOW_BSLN_RST              (32u)
#define CapTouch_U104_SNS_CLK                   (80u)
#define CapTouch_U104_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U105 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U105_STATIC_CONFIG             (10249u)
#define CapTouch_U105_NUM_SENSORS               (1u)

/*******************************************************************************
* U105 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U105_CRC                       (0xA1B8u)
#define CapTouch_U105_RESOLUTION                (150u)
#define CapTouch_U105_FINGER_TH                 (225u)
#define CapTouch_U105_NOISE_TH                  (30u)
#define CapTouch_U105_NNOISE_TH                 (30u)
#define CapTouch_U105_HYSTERESIS                (25u)
#define CapTouch_U105_ON_DEBOUNCE               (3u)
#define CapTouch_U105_LOW_BSLN_RST              (30u)
#define CapTouch_U105_SNS_CLK                   (80u)
#define CapTouch_U105_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U106 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U106_STATIC_CONFIG             (10241u)
#define CapTouch_U106_NUM_SENSORS               (1u)

/*******************************************************************************
* U106 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U106_CRC                       (0x5FB8u)
#define CapTouch_U106_RESOLUTION                (150u)
#define CapTouch_U106_FINGER_TH                 (270u)
#define CapTouch_U106_NOISE_TH                  (36u)
#define CapTouch_U106_NNOISE_TH                 (36u)
#define CapTouch_U106_HYSTERESIS                (30u)
#define CapTouch_U106_ON_DEBOUNCE               (3u)
#define CapTouch_U106_LOW_BSLN_RST              (36u)
#define CapTouch_U106_SNS_CLK                   (80u)
#define CapTouch_U106_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U107 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U107_STATIC_CONFIG             (10241u)
#define CapTouch_U107_NUM_SENSORS               (1u)

/*******************************************************************************
* U107 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U107_CRC                       (0xB180u)
#define CapTouch_U107_RESOLUTION                (200u)
#define CapTouch_U107_FINGER_TH                 (360u)
#define CapTouch_U107_NOISE_TH                  (40u)
#define CapTouch_U107_NNOISE_TH                 (40u)
#define CapTouch_U107_HYSTERESIS                (40u)
#define CapTouch_U107_ON_DEBOUNCE               (3u)
#define CapTouch_U107_LOW_BSLN_RST              (40u)
#define CapTouch_U107_SNS_CLK                   (80u)
#define CapTouch_U107_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U108 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U108_STATIC_CONFIG             (10249u)
#define CapTouch_U108_NUM_SENSORS               (1u)

/*******************************************************************************
* U108 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U108_CRC                       (0x5BC0u)
#define CapTouch_U108_RESOLUTION                (150u)
#define CapTouch_U108_FINGER_TH                 (245u)
#define CapTouch_U108_NOISE_TH                  (32u)
#define CapTouch_U108_NNOISE_TH                 (32u)
#define CapTouch_U108_HYSTERESIS                (27u)
#define CapTouch_U108_ON_DEBOUNCE               (3u)
#define CapTouch_U108_LOW_BSLN_RST              (32u)
#define CapTouch_U108_SNS_CLK                   (80u)
#define CapTouch_U108_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U109 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U109_STATIC_CONFIG             (10241u)
#define CapTouch_U109_NUM_SENSORS               (1u)

/*******************************************************************************
* U109 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U109_CRC                       (0x1022u)
#define CapTouch_U109_RESOLUTION                (200u)
#define CapTouch_U109_FINGER_TH                 (270u)
#define CapTouch_U109_NOISE_TH                  (38u)
#define CapTouch_U109_NNOISE_TH                 (38u)
#define CapTouch_U109_HYSTERESIS                (30u)
#define CapTouch_U109_ON_DEBOUNCE               (3u)
#define CapTouch_U109_LOW_BSLN_RST              (38u)
#define CapTouch_U109_SNS_CLK                   (80u)
#define CapTouch_U109_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U110 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U110_STATIC_CONFIG             (10241u)
#define CapTouch_U110_NUM_SENSORS               (1u)

/*******************************************************************************
* U110 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U110_CRC                       (0x4E66u)
#define CapTouch_U110_RESOLUTION                (200u)
#define CapTouch_U110_FINGER_TH                 (270u)
#define CapTouch_U110_NOISE_TH                  (34u)
#define CapTouch_U110_NNOISE_TH                 (34u)
#define CapTouch_U110_HYSTERESIS                (30u)
#define CapTouch_U110_ON_DEBOUNCE               (3u)
#define CapTouch_U110_LOW_BSLN_RST              (34u)
#define CapTouch_U110_SNS_CLK                   (80u)
#define CapTouch_U110_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U111 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U111_STATIC_CONFIG             (10249u)
#define CapTouch_U111_NUM_SENSORS               (1u)

/*******************************************************************************
* U111 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U111_CRC                       (0xA1B8u)
#define CapTouch_U111_RESOLUTION                (150u)
#define CapTouch_U111_FINGER_TH                 (225u)
#define CapTouch_U111_NOISE_TH                  (30u)
#define CapTouch_U111_NNOISE_TH                 (30u)
#define CapTouch_U111_HYSTERESIS                (25u)
#define CapTouch_U111_ON_DEBOUNCE               (3u)
#define CapTouch_U111_LOW_BSLN_RST              (30u)
#define CapTouch_U111_SNS_CLK                   (80u)
#define CapTouch_U111_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U112 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U112_STATIC_CONFIG             (10241u)
#define CapTouch_U112_NUM_SENSORS               (1u)

/*******************************************************************************
* U112 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U112_CRC                       (0x222Au)
#define CapTouch_U112_RESOLUTION                (200u)
#define CapTouch_U112_FINGER_TH                 (360u)
#define CapTouch_U112_NOISE_TH                  (34u)
#define CapTouch_U112_NNOISE_TH                 (34u)
#define CapTouch_U112_HYSTERESIS                (40u)
#define CapTouch_U112_ON_DEBOUNCE               (3u)
#define CapTouch_U112_LOW_BSLN_RST              (34u)
#define CapTouch_U112_SNS_CLK                   (80u)
#define CapTouch_U112_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U113 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U113_STATIC_CONFIG             (10249u)
#define CapTouch_U113_NUM_SENSORS               (1u)

/*******************************************************************************
* U113 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U113_CRC                       (0x6752u)
#define CapTouch_U113_RESOLUTION                (200u)
#define CapTouch_U113_FINGER_TH                 (315u)
#define CapTouch_U113_NOISE_TH                  (94u)
#define CapTouch_U113_NNOISE_TH                 (94u)
#define CapTouch_U113_HYSTERESIS                (35u)
#define CapTouch_U113_ON_DEBOUNCE               (3u)
#define CapTouch_U113_LOW_BSLN_RST              (94u)
#define CapTouch_U113_SNS_CLK                   (80u)
#define CapTouch_U113_SNS_CLK_SOURCE            (0u)

/*******************************************************************************
* U114 initialization values for FLASH data structure
*******************************************************************************/
#define CapTouch_U114_STATIC_CONFIG             (10241u)
#define CapTouch_U114_NUM_SENSORS               (1u)

/*******************************************************************************
* U114 initialization values for RAM data structure
*******************************************************************************/
#define CapTouch_U114_CRC                       (0xD844u)
#define CapTouch_U114_RESOLUTION                (200u)
#define CapTouch_U114_FINGER_TH                 (280u)
#define CapTouch_U114_NOISE_TH                  (96u)
#define CapTouch_U114_NNOISE_TH                 (96u)
#define CapTouch_U114_HYSTERESIS                (32u)
#define CapTouch_U114_ON_DEBOUNCE               (3u)
#define CapTouch_U114_LOW_BSLN_RST              (96u)
#define CapTouch_U114_SNS_CLK                   (80u)
#define CapTouch_U114_SNS_CLK_SOURCE            (0u)

/* RAM Sensor Parameters Definitions */
#define CapTouch_U102_RX0_IDAC_COMP0            (32u)
#define CapTouch_U103_RX0_IDAC_COMP0            (32u)
#define CapTouch_U104_RX0_IDAC_COMP0            (32u)
#define CapTouch_U105_RX0_IDAC_COMP0            (32u)
#define CapTouch_U106_RX0_IDAC_COMP0            (32u)
#define CapTouch_U107_RX0_IDAC_COMP0            (32u)
#define CapTouch_U108_RX0_IDAC_COMP0            (32u)
#define CapTouch_U109_RX0_IDAC_COMP0            (32u)
#define CapTouch_U110_RX0_IDAC_COMP0            (32u)
#define CapTouch_U111_RX0_IDAC_COMP0            (32u)
#define CapTouch_U112_RX0_IDAC_COMP0            (32u)
#define CapTouch_U113_RX0_IDAC_COMP0            (32u)
#define CapTouch_U114_RX0_IDAC_COMP0            (32u)



/*******************************************************************************
* ADC Specific Macros
*******************************************************************************/
#define CapTouch_ADC_RES8BIT                  (8u)
#define CapTouch_ADC_RES10BIT                 (10u)

#define CapTouch_ADC_FULLRANGE_MODE           (0u)
#define CapTouch_ADC_VREF_MODE                (1u)

#define CapTouch_ADC_MIN_CHANNELS             (1u)
#define CapTouch_ADC_EN                       (0u)
#define CapTouch_ADC_STANDALONE_EN            (0u)
#define CapTouch_ADC_TOTAL_CHANNELS           (1u)
#define CapTouch_ADC_RESOLUTION               (CapTouch_ADC_RES10BIT)
#define CapTouch_ADC_AMUXB_INPUT_EN           (0u)
#define CapTouch_ADC_SELECT_AMUXB_CH          (0u)
#define CapTouch_ADC_AZ_EN                    (1Lu)
#define CapTouch_ADC_AZ_TIME                  (5u)
#define CapTouch_ADC_VREF_MV                  (3840u)
#define CapTouch_ADC_GAIN                     (9Lu)
#define CapTouch_ADC_IDAC_DEFAULT             (27u)
#define CapTouch_ADC_MODCLK_DIV_DEFAULT       (1u)
#define CapTouch_ADC_MEASURE_MODE             (CapTouch_ADC_FULLRANGE_MODE)
#define CapTouch_ADC_ANALOG_STARTUP_DELAY_US  (5u)
#define CapTouch_ADC_ACQUISITION_TIME_US      (10u)

/*******************************************************************************
* Built-In Self-Test Configuration
*******************************************************************************/
#define CapTouch_SELF_TEST_EN                   (1Lu)
#define CapTouch_TST_GLOBAL_CRC_EN              (1Lu)
#define CapTouch_TST_WDGT_CRC_EN                (1Lu)
#define CapTouch_TST_BSLN_DUPLICATION_EN        (1Lu)
#define CapTouch_TST_BSLN_RAW_OUT_RANGE_EN      (1Lu)
#define CapTouch_TST_SNS_SHORT_EN               (1Lu)
#define CapTouch_TST_SNS2SNS_SHORT_EN           (1Lu)
#define CapTouch_TST_SNS_CAP_EN                 (1Lu)
#define CapTouch_TST_SH_CAP_EN                  (0Lu)
#define CapTouch_TST_EXTERNAL_CAP_EN            (1Lu)
#define CapTouch_TST_INTERNAL_CAP_EN            (0Lu)
#define CapTouch_TST_VDDA_EN                    (1Lu)
#define CapTouch_TST_FINE_INIT_TIME             (10Lu)

#define CapTouch_TST_GLOBAL_CRC                 (1Lu << 0u)
#define CapTouch_TST_WDGT_CRC                   (1Lu << 1u)
#define CapTouch_TST_BSLN_DUPLICATION           (1Lu << 4u)
#define CapTouch_TST_BSLN_RAW_OUT_RANGE         (1Lu << 5u)
#define CapTouch_TST_SNS_SHORT                  (1Lu << 8u)
#define CapTouch_TST_SNS2SNS_SHORT              (1Lu << 9u)
#define CapTouch_TST_SNS_CAP                    (1Lu << 12u)
#define CapTouch_TST_EXTERNAL_CAP               (1Lu << 16u)
#define CapTouch_TST_VDDA                       (1Lu << 20u)
#define CapTouch_TST_RUN_SELF_TEST_MASK         (CapTouch_TST_GLOBAL_CRC | \
                                                 CapTouch_TST_WDGT_CRC | \
                                                 CapTouch_TST_BSLN_DUPLICATION | \
                                                 CapTouch_TST_SNS_SHORT | \
                                                 CapTouch_TST_SNS2SNS_SHORT | \
                                                 CapTouch_TST_SNS_CAP | \
                                                 CapTouch_TST_EXTERNAL_CAP | \
                                                 CapTouch_TST_VDDA)
#define CapTouch_TST_RUN_SELF_TEST_START_MASK   (CapTouch_TST_GLOBAL_CRC | \
                                                 CapTouch_TST_WDGT_CRC | \
                                                 CapTouch_TST_SNS_SHORT | \
                                                 CapTouch_TST_SNS2SNS_SHORT)

#define CapTouch_GLOBAL_CRC_AREA_START          (42u)
#define CapTouch_GLOBAL_CRC_AREA_SIZE           (4u)
#define CapTouch_WIDGET_CRC_AREA_START          (2u)
#define CapTouch_WIDGET_CRC_AREA_SIZE           (14u)


#define CapTouch_TST_CINTA_ID                   (0u)
#define CapTouch_TST_CINTB_ID                   (1u)
#define CapTouch_TST_EXT_CAPS_NUM               (2u)

/* The divider that provides highest possible modulator clock frequency */
#define CapTouch_TST_MOD_CLK_DIVIDER            (1u)
/* The highest possible sense clock frequency to satisfy equation:
 * SnsClkFreq < 1 / (10 * R * 500pF) */
#define CapTouch_TST_BASE_SNS_CLK_KHZ           (187u)
/* The sense clock divider for CapTouch_TST_BASE_SNS_CLK_KHZ */
#define CapTouch_TST_BASE_SNS_CLK_DIVIDER       (128u)
/* The minimum valid sense clock divider */
#define CapTouch_TST_MIN_SNS_CLK_DIVIDER        (4u)
/* VDDA measurement test configuration */
#define CapTouch_TST_VDDA_VREF_MV               (3840u)
#define CapTouch_TST_VDDA_VREF_GAIN             (9u)
#define CapTouch_TST_VDDA_IDAC_DEFAULT          (27u)

#define CapTouch_TST_ANALOG_STARTUP_DELAY_US    (23u)

/*******************************************************************************
* Gesture Configuration
*******************************************************************************/
#define CapTouch_TIMESTAMP_INTERVAL             (1Lu)
#define CapTouch_GESTURE_EN_WIDGET_ID           (0Lu)
#define CapTouch_BALLISTIC_EN_WIDGET_ID         (0Lu)


#endif /* CY_SENSE_CapTouch_CONFIGURATION_H */


/* [] END OF FILE */
