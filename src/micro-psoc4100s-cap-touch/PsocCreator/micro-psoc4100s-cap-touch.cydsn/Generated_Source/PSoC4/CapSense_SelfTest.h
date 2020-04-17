/***************************************************************************//**
* \file CapSense_SelfTest.h
* \version 6.0
*
* \brief
*   This file provides the function prototypes for the Built-In Self-Test
*   library.
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

#if !defined(CY_SENSE_CapSense_SELFTEST_H)
#define CY_SENSE_CapSense_SELFTEST_H

#include "cytypes.h"
#include "CapSense_Configuration.h"
#include "CapSense_Sensing.h"

#if (CapSense_ENABLE == CapSense_SELF_TEST_EN)

/*******************************************************************************
* Function Prototypes - High-level API
*******************************************************************************/

/**
* \cond (SECTION_C_HIGH_LEVEL)
* \addtogroup group_c_high_level
* \{
*/

uint32 CapSense_RunSelfTest(uint32 testEnMask);

/** \}
* \endcond */

/*******************************************************************************
* Function Prototypes - Low-level API
*******************************************************************************/

/**
* \cond (SECTION_C_LOW_LEVEL)
* \addtogroup group_c_low_level
* \{
*/

/* CRC test group */
#if (CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN)
    uint32 CapSense_CheckGlobalCRC(void);
#endif /*(CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN) */
#if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
    uint32 CapSense_CheckWidgetCRC(uint32 widgetId);
#endif /*(CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */

/* Baseline test group */
#if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
    uint32 CapSense_CheckBaselineDuplication(uint32 widgetId, uint32 sensorId);
#endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */
#if (CapSense_ENABLE == CapSense_TST_BSLN_RAW_OUT_RANGE_EN)
    uint32 CapSense_CheckBaselineRawcountRange(uint32 widgetId, uint32 sensorId,
                                    CapSense_BSLN_RAW_RANGE_STRUCT *ranges);
#endif /* (CapSense_ENABLE == CapSense_TST_BSLN_RAW_OUT_RANGE_EN) */

/* Short test group */
#if (CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN)
    uint32 CapSense_CheckSensorShort(uint32 widgetId, uint32 sensorId);
#endif /*(CapSense_ENABLE == CapSense_TST_SNS_SHORT_EN) */
#if (CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN)
    uint32 CapSense_CheckSns2SnsShort(uint32 widgetId, uint32 sensorId);
#endif /*(CapSense_ENABLE == CapSense_TST_SNS2SNS_SHORT_EN) */

/* Capacitance measurement test group */
#if (CapSense_ENABLE == CapSense_TST_SNS_CAP_EN)
    uint32 CapSense_GetSensorCapacitance(uint32 widgetId, uint32 sensorId);
#endif /*(CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) */
#if (CapSense_ENABLE == CapSense_TST_SH_CAP_EN)
        uint32 CapSense_GetShieldCapacitance(void);
#endif /*(CapSense_ENABLE == CapSense__TST_SH_CAP_EN) */
#if (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN)
    uint32 CapSense_GetExtCapCapacitance(uint32 extCapId);
#endif /*(CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN) */

/* Vdda measurement */
#if ((CapSense_ENABLE == CapSense_TST_VDDA_EN) && (CapSense_ENABLE == CapSense_CSDV2))
    uint16 CapSense_GetVdda(void);
#endif /* ((CapSense_ENABLE == CapSense_TST_VDDA_EN) && (CapSense_ENABLE == CapSense_CSDV2)) */

/** \}
* \endcond */

/*******************************************************************************
* Function Prototypes - Internal Functions
*******************************************************************************/

/**
* \cond SECTION_C_INTERNAL
* \addtogroup group_c_internal
* \{
*/
#if (CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN)
    void CapSense_DsUpdateGlobalCrc(void);
#endif /*(CapSense_ENABLE == CapSense_TST_GLOBAL_CRC_EN) */
#if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
    void CapSense_DsUpdateWidgetCrc(uint32 widgetId);
#endif /*(CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */
#if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
    void CapSense_UpdateTestResultBaselineDuplication(uint32 widgetId, uint32 sensorId);
#endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */

void CapSense_BistInitialize(void);
void CapSense_BistDisableMode(void);

#if ((CapSense_ENABLE == CapSense_TST_SNS_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_SH_CAP_EN) || \
     (CapSense_ENABLE == CapSense_TST_EXTERNAL_CAP_EN))
    CY_ISR_PROTO(CapSense_BistPostSingleScan);
#endif

/** \}
* \endcond */

/***********************************************************************************************************************
* Local definition
***********************************************************************************************************************/

/* Defines the mask for baseline data processing failure */
#define CapSense_PROCESS_BASELINE_FAILED                        (0x00000001uL << 31u)

/* Defines constants for Self-Test library */
#define CapSense_TST_LSBYTE                                     (0x000000FFuL)
#define CapSense_TST_FAILED                                     (0x0000FFFFuL)
#define CapSense_TST_BAD_PARAM                                  (0x00000001uL << 30u)
#define CapSense_TST_NOT_EXECUTED                               (0x00000001uL << 31u)

    /* Defines external capacitor ID */
    #define CapSense_TST_CMOD_MAP                               (0u)
    #define CapSense_TST_CSH_MAP                                (1u)
    #define CapSense_TST_CINTA_MAP                              (2u)
    #define CapSense_TST_CINTB_MAP                              (3u)

    #define CapSense_08_BIT_SHIFT                               (8uL)

    #define CapSense_BIST_SNS_CAP_MAX_CP                        ((0x00000001uL << 8uL) - 1uL)
    #define CapSense_BIST_SH_CAP_MAX_CP                         ((0x00000001uL << 16uL) - 1uL)
    #define CapSense_BIST_SNS_CAP_UNIT_SCALE                    (1000uL)
    #define CapSense_BIST_10_BIT_MASK                           ((0x00000001uL << 10uL) - 1uL)
    #define CapSense_BIST_CALIBRATION_TARGET                    ((CapSense_BIST_10_BIT_MASK * \
                                                                          CapSense_CSD_RAWCOUNT_CAL_LEVEL) / \
                                                                          CapSense_PERCENTAGE_100)

    #define CapSense_BIST_AVG_CYCLES_PER_LOOP                   (5u)
    #define CapSense_BIST_MEASURE_MAX_TIME_US                   (3000u)
    #define CapSense_BIST_PRECHARGE_MAX_TIME_US                 (250u)

    #define CapSense_BIST_MEASURE_WATCHDOG_CYCLES_NUM           (((CYDEV_BCLK__HFCLK__MHZ) * (CapSense_BIST_MEASURE_MAX_TIME_US)) /\
                                                                        (CapSense_BIST_AVG_CYCLES_PER_LOOP))
    #define CapSense_BIST_PRECHARGE_WATCHDOG_CYCLES_NUM         (((CYDEV_BCLK__HFCLK__MHZ) * (CapSense_BIST_PRECHARGE_MAX_TIME_US)) /\
                                                                        (CapSense_BIST_AVG_CYCLES_PER_LOOP))

    /* Min idac code at which we can guarantee 10% of raw count step per idac changing for one */
    #define CapSense_BIST_MIN_IDAC_VALUE                        (12uL)
    #define CapSense_BIST_MAX_IDAC_VALUE                        ((1uL << CapSense_IDAC_BITS_USED) - 5uL)
    #define CapSense_BIST_MAX_MODCLK_DIVIDER                    (0x000000FFuL)

#if (CapSense_ENABLE == CapSense_CSDV2)
    #define CapSense_TST_EXT_CAP_LOW_RANGE                  (5uL)
    #define CapSense_TST_EXT_CAP_RESOLUTION                 (0x00000001uL << 10uL)
    #define CapSense_TST_EXT_CAP_DURATION                   (1uL)
    #define CapSense_TST_EXT_CAP_MODCLK_MHZ                 (4uL)
    #define CapSense_SENSE_DUTY_TST_EXT_CAP_WIDTH           (10uL)
    #define CapSense_SEQ_INIT_CNT_FINE_INIT_SKIP            (0x00000000uL)
    #define CapSense_IDAC_BITS_USED                         (7u)
#else /* (CapSense_ENABLE == CapSense_CSDV2) */
    #define CapSense_TST_EXT_CAP_LOW_RANGE                  (5uL << 3uL)
    #define CapSense_TST_EXT_CAP_RESOLUTION                 (0xFFuL)
    #define CapSense_TST_EXT_CAP_SNSCLK_DIVIDER             (0xFFuL)
    #define CapSense_TST_EXT_CAP_DURATION                   (CapSense_TST_EXT_CAP_RESOLUTION << \
                                                                     CapSense_RESOLUTION_OFFSET)
    #define CapSense_TST_EXT_CAP_MODCLK_MHZ                 (2uL)
    #define CapSense_IDAC_BITS_USED                         (8u)
#endif /* (CapSense_ENABLE == CapSense_CSDV2) */

#define CapSense_BIST_CAL_MIDDLE_BIT                        (1uL << (CapSense_IDAC_BITS_USED - 1u))
#define CapSense_TST_EXT_CAP_RESOLUTION_75                  ((CapSense_TST_EXT_CAP_RESOLUTION >> 1uL) +\
                                                                     (CapSense_TST_EXT_CAP_RESOLUTION >> 2uL))
#define CapSense_TST_EXT_CAP_MLTPLR                         (2u)
#define CapSense_CSDCMP_TST_CAP_MEAS                        (0x00000201uL)

/***********************************************************************************************************************
* HW CSD Block Config
***********************************************************************************************************************/
#if (CapSense_ENABLE == CapSense_CSDV2)

    #define CapSense_BIST_INTR_SET_CFG                          (0x00000000uL)
    #define CapSense_BIST_SW_FW_TANK_SEL_CFG                    (0x00000000uL)
    #define CapSense_BIST_CAP_SENSE_DUTY_SEL                    (0x00010000uL)
    #define CapSense_BIST_AMBUF_PWR_MODE_OFF                    (CapSense_AMBUF_PWR_MODE_OFF)
    #define CapSense_BIST_AUTO_ZERO_TIME                        (CapSense_CSD_AUTO_ZERO_TIME)

    #if (CapSense_ENABLE == CapSense_CSD_EN)
        #if (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION)
            #define CapSense_BIST_SW_DSI_SEL_CMODPAD                (CapSense_SW_DSI_CMOD)
        #elif (CapSense_CSD__CSH_TANK_PAD == CapSense_CMOD_CONNECTION)
            #define CapSense_BIST_SW_DSI_SEL_CMODPAD                (CapSense_SW_DSI_CTANK)
        #else
            #define CapSense_BIST_SW_DSI_SEL_CMODPAD                (0x00000000uL)
        #endif

        #if (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_BIST_SW_DSI_SEL_TANKPAD                (CapSense_SW_DSI_CTANK)
        #elif (CapSense_CSD__CMOD_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_BIST_SW_DSI_SEL_TANKPAD                (CapSense_SW_DSI_CMOD)
        #else
            #define CapSense_BIST_SW_DSI_SEL_TANKPAD                (0x00000000uL)
        #endif
    #else
        #define CapSense_BIST_SW_DSI_SEL_CMODPAD                (CapSense_SW_DSI_CMOD)
        #define CapSense_BIST_SW_DSI_SEL_TANKPAD                (CapSense_SW_DSI_CTANK)
    #endif

    #define CapSense_BIST_DEFAULT_SW_DSI_SEL                    (CapSense_BIST_SW_DSI_SEL_CMODPAD | \
                                                                         CapSense_BIST_SW_DSI_SEL_TANKPAD)
    #define CapSense_BIST_ADC_CTL_CFG                           (0x00000000uL)
    /* Shield switch default config */
    #define CapSense_BIST_SW_SHIELD_SEL_INIT                 (CapSense_SW_SHIELD_SEL_SW_HCAV_HSCMP)
    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
         (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
        #if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
            #define CapSense_BIST_SW_SHIELD_SEL_CFG          (CapSense_SW_SHIELD_SEL_SW_HCBG_HSCMP)
        #else
            #define CapSense_BIST_SW_SHIELD_SEL_CFG          (CapSense_SW_SHIELD_SEL_SW_HCBV_HSCMP)
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */
    #elif(CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
            #define CapSense_BIST_SW_SHIELD_SEL_CFG          (CapSense_SW_SHIELD_SEL_SW_HCBV_PHI1 | \
                                                                      CapSense_SW_SHIELD_SEL_SW_HCBG_PHI2_HSCMP)
        #else
            #define CapSense_BIST_SW_SHIELD_SEL_CFG          (CapSense_SW_SHIELD_SEL_SW_HCBG_PHI1 | \
                                                                      CapSense_SW_SHIELD_SEL_SW_HCBV_PHI2_HSCMP)
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */
    #else
        #define CapSense_BIST_SW_SHIELD_SEL_CFG              (0x00000000uL)
    #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
               (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

    /* SW_HS_P_SEL switches state for Coarse initialization of CMOD (sense path) */
    #if (CapSense_ENABLE == CapSense_CSD_EN)
        #if (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION)
            #define CapSense_BIST_HS_P_SEL_COARSE_CMOD               (CapSense_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
        #elif (CapSense_CSD__CSHIELD_PAD == CapSense_CMOD_CONNECTION)
            #define CapSense_BIST_HS_P_SEL_COARSE_CMOD               (CapSense_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
        #else
            #define CapSense_BIST_HS_P_SEL_COARSE_CMOD               (CapSense_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
        #endif /* (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) */
    #else
        #define CapSense_BIST_HS_P_SEL_COARSE_CMOD                   (CapSense_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
    #endif /* ((CapSense_ENABLE == CapSense_CSD_CSX_EN) && (CapSense_ENABLE == CapSense_CSD_EN)) */

    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
        (CapSense_ENABLE == CapSense_CSD_SHIELD_EN))
        /* SW_HS_P_SEL switches state for Coarse initialization of CTANK (sense path) */
        #if (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_BIST_HS_P_SEL_COARSE_TANK               (CapSense_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
            #define CapSense_BIST_HS_P_SEL_SCAN_TANK                (CapSense_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
        #elif (CapSense_CSD__CSHIELD_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_BIST_HS_P_SEL_COARSE_TANK               (CapSense_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
            #define CapSense_BIST_HS_P_SEL_SCAN_TANK                (CapSense_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
        #elif (CapSense_CSD__CMOD_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_BIST_HS_P_SEL_COARSE_TANK               (CapSense_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
            #define CapSense_BIST_HS_P_SEL_SCAN_TANK                (CapSense_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
        #else
            #define CapSense_BIST_HS_P_SEL_COARSE_TANK               (CapSense_SW_HS_P_SEL_SW_HMMA_STATIC_CLOSE)
            #define CapSense_BIST_HS_P_SEL_SCAN_TANK                (CapSense_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE)
        #endif /* (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION) */
    #else
        #define CapSense_BIST_HS_P_SEL_COARSE_TANK                   (CapSense_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
        #define CapSense_BIST_HS_P_SEL_SCAN_TANK                     (CapSense_STATIC_OPEN)
    #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
               (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)) */

    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
        (CapSense_ENABLE == CapSense_CSD_SHIELD_EN))
        #define CapSense_BIST_HS_P_SEL_SCAN                       (CapSense_BIST_HS_P_SEL_SCAN_TANK)
    #elif(CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #define CapSense_BIST_HS_P_SEL_SCAN                       (CapSense_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE)
    #else
        #define CapSense_BIST_HS_P_SEL_SCAN                       (CapSense_STATIC_OPEN)
    #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
               (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)) */

    #define CapSense_BIST_SW_HS_P_SEL_COARSE                    (CapSense_BIST_HS_P_SEL_COARSE_CMOD | CapSense_BIST_HS_P_SEL_COARSE_TANK)
    #define CapSense_BIST_SW_HS_P_SEL_CFG                       (0x00000000uL)
    #define CapSense_BIST_SW_HS_N_SEL_CFG                       (0x00000000uL)

    #if (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN)
        #define CapSense_BIST_AZ_ENABLE_CFG                  (CapSense_CSD_AZ_EN_MASK)
    #else
        #define CapSense_BIST_AZ_ENABLE_CFG                  (0uL)
    #endif /* (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN) */

    #define CapSense_BIST_HSCMP_CFG                             (CapSense_HSCMP_EN_MASK | CapSense_BIST_AZ_ENABLE_CFG)
    #define CapSense_BIST_CSDCMP_INIT                           (CapSense_CSDCMP_CSDCMP_DISABLED)

    #define CapSense_BIST_BLOCK_ON_DELAY                        (3uL * CYDEV_BCLK__HFCLK__MHZ)
    #define CapSense_BIST_HSCMP_ON_DELAY                        (1uL * CYDEV_BCLK__HFCLK__MHZ)

    #if (CapSense_ENABLE == CapSense_CSD_EN)
        #if (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION)
            #define CapSense_BIST_SW_FW_MOD_SEL_INIT    (CapSense_SW_FW_MOD_SEL_SW_F1PM_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_MOD_SEL_SW_F1MA_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_MOD_SEL_SW_F1CA_STATIC_CLOSE)
        #else
            #define CapSense_BIST_SW_FW_MOD_SEL_INIT    (0x00000000uL)
        #endif /* (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) */

        #if (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_BIST_SW_FW_TANK_SEL_INIT   (CapSense_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_TANK_SEL_SW_F2MA_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_TANK_SEL_SW_F2CA_STATIC_CLOSE)
        #else
            #define CapSense_BIST_SW_FW_TANK_SEL_INIT   (0x00000000uL)
        #endif /* (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION) */

    #elif (CapSense_ENABLE == CapSense_CSX_EN)
        #define CapSense_BIST_SW_FW_MOD_SEL_INIT        (CapSense_SW_FW_MOD_SEL_SW_F1PM_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_MOD_SEL_SW_F1MA_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_MOD_SEL_SW_F1CA_STATIC_CLOSE)

        #define CapSense_BIST_SW_FW_TANK_SEL_INIT       (CapSense_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_TANK_SEL_SW_F2MA_STATIC_CLOSE |\
                                                                 CapSense_SW_FW_TANK_SEL_SW_F2CA_STATIC_CLOSE)
    #else
        #define CapSense_BIST_SW_FW_MOD_SEL_INIT        (0x00000000uL)
        #define CapSense_BIST_SW_FW_TANK_SEL_INIT       (0x00000000uL)
    #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


    #define CapSense_BIST_SW_RES_INIT                           (CapSense_CSD_INIT_SWITCH_RES << CYFLD_CSD_RES_HCAV__OFFSET)
    #define CapSense_BIST_SW_FW_MOD_SEL_SCAN                    (0x00000000uL)
    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
         (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
         (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION))
        #define CapSense_BIST_SW_FW_TANK_SEL_SCAN               (CapSense_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                         CapSense_SW_FW_TANK_SEL_SW_F2CB_PHI2)
    #else
        #define CapSense_BIST_SW_FW_TANK_SEL_SCAN               (0x00000000uL)
    #endif

    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
         (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
         (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION))
        #define CapSense_BIST_SW_SHIELD_SEL                     (CapSense_SW_SHIELD_SEL_SW_HCBV_HSCMP)
    #elif((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
          (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
        #define CapSense_BIST_SW_SHIELD_SEL                     (CapSense_SW_SHIELD_SEL_SW_HCBV_HSCMP)
    #elif(CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #define CapSense_BIST_SW_SHIELD_SEL                     (CapSense_SW_SHIELD_SEL_SW_HCBG_PHI1 | \
                                                                         CapSense_SW_SHIELD_SEL_SW_HCBV_PHI2_HSCMP)
    #else
        #define CapSense_BIST_SW_SHIELD_SEL                     (0x00000000uL)
    #endif
    #define CapSense_BIST_SW_RES_SCAN                           ((CapSense_CSD_SHIELD_SWITCH_RES << CYFLD_CSD_RES_HCBV__OFFSET) |\
                                                                         (CapSense_CSD_SHIELD_SWITCH_RES << CYFLD_CSD_RES_HCBG__OFFSET))
    #define CapSense_BIST_HSCMP_SCAN_MASK                       (CapSense_HSCMP_EN_MASK)
    #define CapSense_BIST_IDACB_CFG                             (0x00000000uL)
    #define CapSense_BIST_IDACA_CFG                             (0x01830000uL)

    /* IDAC Gain in nA */
    #define CapSense_TST_SNS_CAP_IDAC_GAIN                      (2400uL)

    #if ((CYDEV_BCLK__HFCLK__HZ / CapSense_TST_MOD_CLK_DIVIDER) <= CapSense_MOD_CSD_CLK_12MHZ)
        #define CapSense_FILTER_DELAY_CFG                       (CapSense_CONFIG_FILTER_DELAY_12MHZ)
    #elif ((CYDEV_BCLK__HFCLK__HZ / CapSense_TST_MOD_CLK_DIVIDER) <= CapSense_MOD_CSD_CLK_24MHZ)
        #define CapSense_FILTER_DELAY_CFG                       (CapSense_CONFIG_FILTER_DELAY_24MHZ)
    #else
        /* ((CYDEV_BCLK__HFCLK__HZ / CapSense_TST_MOD_CLK_DIVIDER) <= CapSense_MOD_CSD_CLK_48MHZ) */
        #define CapSense_FILTER_DELAY_CFG                       (CapSense_CONFIG_FILTER_DELAY_48MHZ)
    #endif

    #define CapSense_BIST_CSD_CONFIG                            (CapSense_CONFIG_FILTER_DELAY_2_CYCLES)
    #define CapSense_BIST_SW_REFGEN_SEL_CFG                     (CapSense_SW_REFGEN_SEL_SW_SGR_MASK)

    #define CapSense_TST_MEASMODE_VREF                              (0x1uL << CYFLD_CSD_ADC_MODE__OFFSET)
    #define CapSense_TST_MEASMODE_VREFBY2                           (0x2uL << CYFLD_CSD_ADC_MODE__OFFSET)
    #define CapSense_TST_MEASMODE_VIN                               (0x3uL << CYFLD_CSD_ADC_MODE__OFFSET)

    /* Clock defines */
    #define CapSense_TST_VDDA_MODCLK_DIV_DEFAULT                    (CapSense_TST_MOD_CLK_DIVIDER)
    #define CapSense_TST_VDDA_SENSE_DIV_DEFAULT                     (0x4uL)
    #define CapSense_TST_VDDA_TOTAL_CLOCK_DIV                       (CapSense_TST_VDDA_MODCLK_DIV_DEFAULT * \
                                                                             CapSense_TST_VDDA_SENSE_DIV_DEFAULT)

    /* Acquisition time definitions: ADC_CTL */
    #define CapSense_VDDA_ACQUISITION_TIME_US                       (10uL)
    #define CapSense_VDDA_ACQUISITION_BASE                          ((CapSense_VDDA_ACQUISITION_TIME_US * \
                                                                            (CYDEV_BCLK__HFCLK__MHZ)) / \
                                                                            CapSense_TST_VDDA_TOTAL_CLOCK_DIV)

    #define CapSense_TST_VDDA_ADC_AZ_TIME                           (5uL)
    #define CapSense_TST_VDDA_SEQ_TIME_BASE                         (((CYDEV_BCLK__HFCLK__HZ * CapSense_TST_VDDA_ADC_AZ_TIME) / \
                                                                               CapSense_TST_VDDA_TOTAL_CLOCK_DIV) / 1000000uL)

    #if (0 == CapSense_TST_VDDA_SEQ_TIME_BASE)
        #define CapSense_TST_VDDA_SEQ_TIME_DEFAULT                  (1u)
    #else
        #define CapSense_TST_VDDA_SEQ_TIME_DEFAULT                  (CapSense_TST_VDDA_SEQ_TIME_BASE)
    #endif




    #define CapSense_TST_VDDA_FILTER_DELAY                          (2uL)
    #define CapSense_TST_VDDA_FINE_INIT_TIME                        (CapSense_TST_FINE_INIT_TIME)
    #define CapSense_TST_VDDA_SCAN_DURATION                         (2uL)

    #define CapSense_TST_VDDA_VREF_TRIM_MAX_DEVIATION               (20uL)

    /* The reference voltage macros */
    #define CapSense_TST_VDDA_VREF_CALIB_USED                       (2400uL)

    #define CapSense_TST_VDDA_BAD_RESULT                            (0xFFFFu)


    #define CapSense_TST_VDDA_CONFIG_DEFAULT_CFG                    (CapSense_CONFIG_ENABLE_MASK |\
                                                                             CapSense_CONFIG_SAMPLE_SYNC_MASK |\
                                                                             CapSense_CONFIG_SENSE_EN_MASK |\
                                                                             CapSense_CONFIG_DSI_COUNT_SEL_MASK)

    #define CapSense_TST_VDDA_IDACA_DEFAULT_CFG                     (0x00000000uL)
    #define CapSense_TST_VDDA_IDACB_DEFAULT_CFG                     (CapSense_TST_VDDA_IDAC_DEFAULT |\
                                                                            (CapSense_IDAC_COMP_POL_DYN_DYNAMIC << \
                                                                              CYFLD_CSD_POL_DYN__OFFSET) |\
                                                                              CapSense_IDAC_COMP_LEG3_EN_MASK)

    #define CapSense_TST_CSDCMP_DEFAULT_CFG                         (0x00000000uL)
    #define CapSense_TST_SW_DSI_SEL_DEFAULT_CFG                     (0x00000000uL)
    #define CapSense_TST_SENSE_DUTY_VDDA_CFG                        (0x00000000uL)
    #define CapSense_TST_SEQ_INIT_CNT_DEFAULT_CFG                   (1u)
    #define CapSense_TST_SEQ_NORM_CNT_DEFAULT_CFG                   (CapSense_TST_VDDA_SCAN_DURATION)
    #define CapSense_TST_SW_HS_P_SEL_DEFAULT_CFG                    (CapSense_SW_HS_P_SEL_SW_HMRH_STATIC_CLOSE)
    #define CapSense_TST_SW_HS_N_SEL_DEFAULT_CFG                    (CapSense_SW_HS_N_SEL_SW_HCCD_STATIC_CLOSE)
    #define CapSense_TST_SW_SHIELD_SEL_DEFAULT_CFG                  (0x00000000uL)
    #define CapSense_TST_SW_BYP_SEL_DEFAULT_CFG                     (CapSense_SW_BYP_SEL_SW_BYA_MASK |\
                                                                             CapSense_SW_BYP_SEL_SW_BYB_MASK)
    #define CapSense_TST_SW_CMP_P_SEL_DEFAULT_CFG                   (0x00000000uL)
    #define CapSense_TST_SW_CMP_N_SEL_DEFAULT_CFG                   (0x00000000uL)
    #define CapSense_TST_SW_FW_MOD_SEL_DEFAULT_CFG                  (CapSense_SW_FW_MOD_SEL_SW_C1CC_STATIC_CLOSE |\
                                                                             CapSense_SW_FW_MOD_SEL_SW_C1CD_STATIC_CLOSE)
    #define CapSense_TST_SW_FW_TANK_SEL_DEFAULT_CFG                 (CapSense_SW_FW_TANK_SEL_SW_C2CC_STATIC_CLOSE |\
                                                                             CapSense_SW_FW_TANK_SEL_SW_C2CD_STATIC_CLOSE)
    #define CapSense_TST_SW_REFGEN_SEL_DEFAULT_CFG                  (CapSense_SW_REFGEN_SEL_SW_SGR_MASK)
    #define CapSense_TST_REFGEN_DEFAULT_CFG                         (CapSense_REFGEN_REFGEN_EN_MASK |\
                                                                             CapSense_REFGEN_RES_EN_MASK |\
                                                                            ((uint32)CapSense_TST_VDDA_VREF_GAIN << \
                                                                             CYFLD_CSD_GAIN__OFFSET))
    #define CapSense_TST_SW_AMUXBUF_SEL_DEFAULT_CFG                 (0x00000000uL)
    #define CapSense_TST_HSCMP_DEFAULT_CFG                          (CapSense_HSCMP_EN_MASK |\
                                                                             CapSense_CSD_AZ_EN_MASK)

    #define CapSense_TST_VDDA_SEQ_START_MEASURE_CFG                 (CapSense_SEQ_START_AZ0_SKIP_MASK |\
                                                                            CapSense_SEQ_START_START_MASK)
#else /* (CapSense_ENABLE == CapSense_CSDV2) */

#define CapSense_BIST_CSD_CONFIG                            (CapSense_CONFIG_SENSE_COMP_BW_MASK | \
                                                                     CapSense_CONFIG_SENSE_INSEL_MASK | \
                                                                     CapSense_CONFIG_REFBUF_DRV_MASK)

#define CapSense_BIST_CMOD_PRECHARGE_CONFIG                 (CapSense_BIST_CSD_CONFIG | \
                                                                     CapSense_CONFIG_REFBUF_EN_MASK | \
                                                                     CapSense_CONFIG_COMP_PIN_MASK)

#define CapSense_BIST_CMOD_PRECHARGE_CONFIG_CSD_EN          (CapSense_BIST_CMOD_PRECHARGE_CONFIG | \
                                                                     CapSense_CSD_ENABLE_MASK)

/* Third-generation HW block Ctank pre-charge mode configuration */
#if(CapSense_CSD_CSH_PRECHARGE_SRC == CapSense_CSH_PRECHARGE_VREF)
    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #define  CapSense_BIST_CTANK_PRECHARGE_CONFIG       (CapSense_BIST_CSD_CONFIG | \
                                                                     CapSense_CONFIG_REFBUF_EN_MASK | \
                                                                     CapSense_CONFIG_PRS_CLEAR_MASK | \
                                                                     CapSense_CONFIG_REFBUF_OUTSEL_MASK)
    #else
        #define  CapSense_BIST_CTANK_PRECHARGE_CONFIG       (CapSense_BIST_CSD_CONFIG | \
                                                                     CapSense_CONFIG_REFBUF_OUTSEL_MASK | \
                                                                     CapSense_CONFIG_PRS_CLEAR_MASK)
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
#else
    #define  CapSense_BIST_CTANK_PRECHARGE_CONFIG           (CapSense_BIST_CSD_CONFIG |\
                                                                     CapSense_CONFIG_REFBUF_OUTSEL_MASK |\
                                                                     CapSense_CONFIG_REFBUF_EN_MASK |\
                                                                     CapSense_CONFIG_COMP_MODE_MASK |\
                                                                     CapSense_CONFIG_PRS_CLEAR_MASK |\
                                                                     CapSense_CONFIG_COMP_PIN_MASK)

#endif /* (CapSense_CSD_CSH_PRECHARGE_SRC == CapSense__CSH_PRECHARGE_IO_BUF) */

#define  CapSense_BIST_CTANK_PRECHARGE_CONFIG_CSD_EN        (CapSense_BIST_CTANK_PRECHARGE_CONFIG | \
                                                                     CapSense_CONFIG_ENABLE_MASK | \
                                                                     CapSense_CONFIG_SENSE_COMP_EN_MASK)

#define CapSense_BIST_IDAC_CFG                              (0x00000200uL)
#define CapSense_BIST_IDAC_MOD_MASK                         (0x000003FFuL)
#define CapSense_BIST_IDAC_MOD_VAL_MASK                     (0x000000FFuL)

/* IDAC Gain = 8x that corresponds to 1LSB = 1200 nA */
#define CapSense_TST_SNS_CAP_IDAC_GAIN                      (1200uL)

#define CapSense_BIST_SCAN_DURATION                         (CapSense_BIST_10_BIT_MASK << CapSense_RESOLUTION_OFFSET)
#define CapSense_BIST_SCAN_DURATION_255                     (0x1FFuL << CapSense_RESOLUTION_OFFSET)
#endif /* (CapSense_ENABLE == CapSense_CSDV2) */

#endif /* #if (CapSense_ENABLE == CapSense_SELF_TEST_EN) */

#endif /* End CY_SENSE_CapSense_SELFTEST_H */


/* [] END OF FILE */
