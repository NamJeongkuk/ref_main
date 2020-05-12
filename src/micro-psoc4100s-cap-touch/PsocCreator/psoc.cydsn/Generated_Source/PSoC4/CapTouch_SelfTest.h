/***************************************************************************//**
* \file CapTouch_SelfTest.h
* \version 6.0
*
* \brief
*   This file provides the function prototypes for the Built-In Self-Test
*   library.
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

#if !defined(CY_SENSE_CapTouch_SELFTEST_H)
#define CY_SENSE_CapTouch_SELFTEST_H

#include "cytypes.h"
#include "CapTouch_Configuration.h"
#include "CapTouch_Sensing.h"

#if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN)

/*******************************************************************************
* Function Prototypes - High-level API
*******************************************************************************/

/**
* \cond (SECTION_C_HIGH_LEVEL)
* \addtogroup group_c_high_level
* \{
*/

uint32 CapTouch_RunSelfTest(uint32 testEnMask);

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
#if (CapTouch_ENABLE == CapTouch_TST_GLOBAL_CRC_EN)
    uint32 CapTouch_CheckGlobalCRC(void);
#endif /*(CapTouch_ENABLE == CapTouch_TST_GLOBAL_CRC_EN) */
#if (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN)
    uint32 CapTouch_CheckWidgetCRC(uint32 widgetId);
#endif /*(CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN) */

/* Baseline test group */
#if (CapTouch_ENABLE == CapTouch_TST_BSLN_DUPLICATION_EN)
    uint32 CapTouch_CheckBaselineDuplication(uint32 widgetId, uint32 sensorId);
#endif /* (CapTouch_ENABLE == CapTouch_TST_BSLN_DUPLICATION_EN) */
#if (CapTouch_ENABLE == CapTouch_TST_BSLN_RAW_OUT_RANGE_EN)
    uint32 CapTouch_CheckBaselineRawcountRange(uint32 widgetId, uint32 sensorId,
                                    CapTouch_BSLN_RAW_RANGE_STRUCT *ranges);
#endif /* (CapTouch_ENABLE == CapTouch_TST_BSLN_RAW_OUT_RANGE_EN) */

/* Short test group */
#if (CapTouch_ENABLE == CapTouch_TST_SNS_SHORT_EN)
    uint32 CapTouch_CheckSensorShort(uint32 widgetId, uint32 sensorId);
#endif /*(CapTouch_ENABLE == CapTouch_TST_SNS_SHORT_EN) */
#if (CapTouch_ENABLE == CapTouch_TST_SNS2SNS_SHORT_EN)
    uint32 CapTouch_CheckSns2SnsShort(uint32 widgetId, uint32 sensorId);
#endif /*(CapTouch_ENABLE == CapTouch_TST_SNS2SNS_SHORT_EN) */

/* Capacitance measurement test group */
#if (CapTouch_ENABLE == CapTouch_TST_SNS_CAP_EN)
    uint32 CapTouch_GetSensorCapacitance(uint32 widgetId, uint32 sensorId);
#endif /*(CapTouch_ENABLE == CapTouch_TST_SNS_CAP_EN) */
#if (CapTouch_ENABLE == CapTouch_TST_SH_CAP_EN)
        uint32 CapTouch_GetShieldCapacitance(void);
#endif /*(CapTouch_ENABLE == CapTouch__TST_SH_CAP_EN) */
#if (CapTouch_ENABLE == CapTouch_TST_EXTERNAL_CAP_EN)
    uint32 CapTouch_GetExtCapCapacitance(uint32 extCapId);
#endif /*(CapTouch_ENABLE == CapTouch_TST_EXTERNAL_CAP_EN) */

/* Vdda measurement */
#if ((CapTouch_ENABLE == CapTouch_TST_VDDA_EN) && (CapTouch_ENABLE == CapTouch_CSDV2))
    uint16 CapTouch_GetVdda(void);
#endif /* ((CapTouch_ENABLE == CapTouch_TST_VDDA_EN) && (CapTouch_ENABLE == CapTouch_CSDV2)) */

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
#if (CapTouch_ENABLE == CapTouch_TST_GLOBAL_CRC_EN)
    void CapTouch_DsUpdateGlobalCrc(void);
#endif /*(CapTouch_ENABLE == CapTouch_TST_GLOBAL_CRC_EN) */
#if (CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN)
    void CapTouch_DsUpdateWidgetCrc(uint32 widgetId);
#endif /*(CapTouch_ENABLE == CapTouch_TST_WDGT_CRC_EN) */
#if (CapTouch_ENABLE == CapTouch_TST_BSLN_DUPLICATION_EN)
    void CapTouch_UpdateTestResultBaselineDuplication(uint32 widgetId, uint32 sensorId);
#endif /* (CapTouch_ENABLE == CapTouch_TST_BSLN_DUPLICATION_EN) */

void CapTouch_BistInitialize(void);
void CapTouch_BistDisableMode(void);

#if ((CapTouch_ENABLE == CapTouch_TST_SNS_CAP_EN) || \
     (CapTouch_ENABLE == CapTouch_TST_SH_CAP_EN) || \
     (CapTouch_ENABLE == CapTouch_TST_EXTERNAL_CAP_EN))
    CY_ISR_PROTO(CapTouch_BistPostSingleScan);
#endif

/** \}
* \endcond */

/***********************************************************************************************************************
* Local definition
***********************************************************************************************************************/

/* Defines the mask for baseline data processing failure */
#define CapTouch_PROCESS_BASELINE_FAILED                        (0x00000001uL << 31u)

/* Defines constants for Self-Test library */
#define CapTouch_TST_LSBYTE                                     (0x000000FFuL)
#define CapTouch_TST_FAILED                                     (0x0000FFFFuL)
#define CapTouch_TST_BAD_PARAM                                  (0x00000001uL << 30u)
#define CapTouch_TST_NOT_EXECUTED                               (0x00000001uL << 31u)

    /* Defines external capacitor ID */
    #define CapTouch_TST_CMOD_MAP                               (0u)
    #define CapTouch_TST_CSH_MAP                                (1u)
    #define CapTouch_TST_CINTA_MAP                              (2u)
    #define CapTouch_TST_CINTB_MAP                              (3u)

    #define CapTouch_08_BIT_SHIFT                               (8uL)

    #define CapTouch_BIST_SNS_CAP_MAX_CP                        ((0x00000001uL << 8uL) - 1uL)
    #define CapTouch_BIST_SH_CAP_MAX_CP                         ((0x00000001uL << 16uL) - 1uL)
    #define CapTouch_BIST_SNS_CAP_UNIT_SCALE                    (1000uL)
    #define CapTouch_BIST_10_BIT_MASK                           ((0x00000001uL << 10uL) - 1uL)
    #define CapTouch_BIST_CALIBRATION_TARGET                    ((CapTouch_BIST_10_BIT_MASK * \
                                                                          CapTouch_CSD_RAWCOUNT_CAL_LEVEL) / \
                                                                          CapTouch_PERCENTAGE_100)

    #define CapTouch_BIST_AVG_CYCLES_PER_LOOP                   (5u)
    #define CapTouch_BIST_MEASURE_MAX_TIME_US                   (3000u)
    #define CapTouch_BIST_PRECHARGE_MAX_TIME_US                 (250u)

    #define CapTouch_BIST_MEASURE_WATCHDOG_CYCLES_NUM           (((CYDEV_BCLK__HFCLK__MHZ) * (CapTouch_BIST_MEASURE_MAX_TIME_US)) /\
                                                                        (CapTouch_BIST_AVG_CYCLES_PER_LOOP))
    #define CapTouch_BIST_PRECHARGE_WATCHDOG_CYCLES_NUM         (((CYDEV_BCLK__HFCLK__MHZ) * (CapTouch_BIST_PRECHARGE_MAX_TIME_US)) /\
                                                                        (CapTouch_BIST_AVG_CYCLES_PER_LOOP))

    /* Min idac code at which we can guarantee 10% of raw count step per idac changing for one */
    #define CapTouch_BIST_MIN_IDAC_VALUE                        (12uL)
    #define CapTouch_BIST_MAX_IDAC_VALUE                        ((1uL << CapTouch_IDAC_BITS_USED) - 5uL)
    #define CapTouch_BIST_MAX_MODCLK_DIVIDER                    (0x000000FFuL)

#if (CapTouch_ENABLE == CapTouch_CSDV2)
    #define CapTouch_TST_EXT_CAP_LOW_RANGE                  (5uL)
    #define CapTouch_TST_EXT_CAP_RESOLUTION                 (0x00000001uL << 10uL)
    #define CapTouch_TST_EXT_CAP_DURATION                   (1uL)
    #define CapTouch_TST_EXT_CAP_MODCLK_MHZ                 (4uL)
    #define CapTouch_SENSE_DUTY_TST_EXT_CAP_WIDTH           (10uL)
    #define CapTouch_SEQ_INIT_CNT_FINE_INIT_SKIP            (0x00000000uL)
    #define CapTouch_IDAC_BITS_USED                         (7u)
#else /* (CapTouch_ENABLE == CapTouch_CSDV2) */
    #define CapTouch_TST_EXT_CAP_LOW_RANGE                  (5uL << 3uL)
    #define CapTouch_TST_EXT_CAP_RESOLUTION                 (0xFFuL)
    #define CapTouch_TST_EXT_CAP_SNSCLK_DIVIDER             (0xFFuL)
    #define CapTouch_TST_EXT_CAP_DURATION                   (CapTouch_TST_EXT_CAP_RESOLUTION << \
                                                                     CapTouch_RESOLUTION_OFFSET)
    #define CapTouch_TST_EXT_CAP_MODCLK_MHZ                 (2uL)
    #define CapTouch_IDAC_BITS_USED                         (8u)
#endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

#define CapTouch_BIST_CAL_MIDDLE_BIT                        (1uL << (CapTouch_IDAC_BITS_USED - 1u))
#define CapTouch_TST_EXT_CAP_RESOLUTION_75                  ((CapTouch_TST_EXT_CAP_RESOLUTION >> 1uL) +\
                                                                     (CapTouch_TST_EXT_CAP_RESOLUTION >> 2uL))
#define CapTouch_TST_EXT_CAP_MLTPLR                         (2u)
#define CapTouch_CSDCMP_TST_CAP_MEAS                        (0x00000201uL)

/***********************************************************************************************************************
* HW CSD Block Config
***********************************************************************************************************************/
#if (CapTouch_ENABLE == CapTouch_CSDV2)

    #define CapTouch_BIST_INTR_SET_CFG                          (0x00000000uL)
    #define CapTouch_BIST_SW_FW_TANK_SEL_CFG                    (0x00000000uL)
    #define CapTouch_BIST_CAP_SENSE_DUTY_SEL                    (0x00010000uL)
    #define CapTouch_BIST_AMBUF_PWR_MODE_OFF                    (CapTouch_AMBUF_PWR_MODE_OFF)
    #define CapTouch_BIST_AUTO_ZERO_TIME                        (CapTouch_CSD_AUTO_ZERO_TIME)

    #if (CapTouch_ENABLE == CapTouch_CSD_EN)
        #if (CapTouch_CSD__CMOD_PAD == CapTouch_CMOD_CONNECTION)
            #define CapTouch_BIST_SW_DSI_SEL_CMODPAD                (CapTouch_SW_DSI_CMOD)
        #elif (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CMOD_CONNECTION)
            #define CapTouch_BIST_SW_DSI_SEL_CMODPAD                (CapTouch_SW_DSI_CTANK)
        #else
            #define CapTouch_BIST_SW_DSI_SEL_CMODPAD                (0x00000000uL)
        #endif

        #if (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CTANK_CONNECTION)
            #define CapTouch_BIST_SW_DSI_SEL_TANKPAD                (CapTouch_SW_DSI_CTANK)
        #elif (CapTouch_CSD__CMOD_PAD == CapTouch_CTANK_CONNECTION)
            #define CapTouch_BIST_SW_DSI_SEL_TANKPAD                (CapTouch_SW_DSI_CMOD)
        #else
            #define CapTouch_BIST_SW_DSI_SEL_TANKPAD                (0x00000000uL)
        #endif
    #else
        #define CapTouch_BIST_SW_DSI_SEL_CMODPAD                (CapTouch_SW_DSI_CMOD)
        #define CapTouch_BIST_SW_DSI_SEL_TANKPAD                (CapTouch_SW_DSI_CTANK)
    #endif

    #define CapTouch_BIST_DEFAULT_SW_DSI_SEL                    (CapTouch_BIST_SW_DSI_SEL_CMODPAD | \
                                                                         CapTouch_BIST_SW_DSI_SEL_TANKPAD)
    #define CapTouch_BIST_ADC_CTL_CFG                           (0x00000000uL)
    /* Shield switch default config */
    #define CapTouch_BIST_SW_SHIELD_SEL_INIT                 (CapTouch_SW_SHIELD_SEL_SW_HCAV_HSCMP)
    #if ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
         (CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN))
        #if (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG)
            #define CapTouch_BIST_SW_SHIELD_SEL_CFG          (CapTouch_SW_SHIELD_SEL_SW_HCBG_HSCMP)
        #else
            #define CapTouch_BIST_SW_SHIELD_SEL_CFG          (CapTouch_SW_SHIELD_SEL_SW_HCBV_HSCMP)
        #endif /* (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG) */
    #elif(CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN)
        #if (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG)
            #define CapTouch_BIST_SW_SHIELD_SEL_CFG          (CapTouch_SW_SHIELD_SEL_SW_HCBV_PHI1 | \
                                                                      CapTouch_SW_SHIELD_SEL_SW_HCBG_PHI2_HSCMP)
        #else
            #define CapTouch_BIST_SW_SHIELD_SEL_CFG          (CapTouch_SW_SHIELD_SEL_SW_HCBG_PHI1 | \
                                                                      CapTouch_SW_SHIELD_SEL_SW_HCBV_PHI2_HSCMP)
        #endif /* (CapTouch_IDAC_SINKING == CapTouch_CSD_IDAC_CONFIG) */
    #else
        #define CapTouch_BIST_SW_SHIELD_SEL_CFG              (0x00000000uL)
    #endif /* ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
               (CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN)) */

    /* SW_HS_P_SEL switches state for Coarse initialization of CMOD (sense path) */
    #if (CapTouch_ENABLE == CapTouch_CSD_EN)
        #if (CapTouch_CSD__CMOD_PAD == CapTouch_CMOD_CONNECTION)
            #define CapTouch_BIST_HS_P_SEL_COARSE_CMOD               (CapTouch_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
        #elif (CapTouch_CSD__CSHIELD_PAD == CapTouch_CMOD_CONNECTION)
            #define CapTouch_BIST_HS_P_SEL_COARSE_CMOD               (CapTouch_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
        #else
            #define CapTouch_BIST_HS_P_SEL_COARSE_CMOD               (CapTouch_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
        #endif /* (CapTouch_CSD__CMOD_PAD == CapTouch_CMOD_CONNECTION) */
    #else
        #define CapTouch_BIST_HS_P_SEL_COARSE_CMOD                   (CapTouch_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
    #endif /* ((CapTouch_ENABLE == CapTouch_CSD_CSX_EN) && (CapTouch_ENABLE == CapTouch_CSD_EN)) */

    #if ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN) && \
        (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN))
        /* SW_HS_P_SEL switches state for Coarse initialization of CTANK (sense path) */
        #if (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CTANK_CONNECTION)
            #define CapTouch_BIST_HS_P_SEL_COARSE_TANK               (CapTouch_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
            #define CapTouch_BIST_HS_P_SEL_SCAN_TANK                (CapTouch_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
        #elif (CapTouch_CSD__CSHIELD_PAD == CapTouch_CTANK_CONNECTION)
            #define CapTouch_BIST_HS_P_SEL_COARSE_TANK               (CapTouch_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
            #define CapTouch_BIST_HS_P_SEL_SCAN_TANK                (CapTouch_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
        #elif (CapTouch_CSD__CMOD_PAD == CapTouch_CTANK_CONNECTION)
            #define CapTouch_BIST_HS_P_SEL_COARSE_TANK               (CapTouch_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
            #define CapTouch_BIST_HS_P_SEL_SCAN_TANK                (CapTouch_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
        #else
            #define CapTouch_BIST_HS_P_SEL_COARSE_TANK               (CapTouch_SW_HS_P_SEL_SW_HMMA_STATIC_CLOSE)
            #define CapTouch_BIST_HS_P_SEL_SCAN_TANK                (CapTouch_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE)
        #endif /* (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CTANK_CONNECTION) */
    #else
        #define CapTouch_BIST_HS_P_SEL_COARSE_TANK                   (CapTouch_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
        #define CapTouch_BIST_HS_P_SEL_SCAN_TANK                     (CapTouch_STATIC_OPEN)
    #endif /* ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN) && \
               (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN)) */

    #if ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN) && \
        (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN))
        #define CapTouch_BIST_HS_P_SEL_SCAN                       (CapTouch_BIST_HS_P_SEL_SCAN_TANK)
    #elif(CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN)
        #define CapTouch_BIST_HS_P_SEL_SCAN                       (CapTouch_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE)
    #else
        #define CapTouch_BIST_HS_P_SEL_SCAN                       (CapTouch_STATIC_OPEN)
    #endif /* ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN) && \
               (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN)) */

    #define CapTouch_BIST_SW_HS_P_SEL_COARSE                    (CapTouch_BIST_HS_P_SEL_COARSE_CMOD | CapTouch_BIST_HS_P_SEL_COARSE_TANK)
    #define CapTouch_BIST_SW_HS_P_SEL_CFG                       (0x00000000uL)
    #define CapTouch_BIST_SW_HS_N_SEL_CFG                       (0x00000000uL)

    #if (CapTouch_ENABLE == CapTouch_CSD_AUTO_ZERO_EN)
        #define CapTouch_BIST_AZ_ENABLE_CFG                  (CapTouch_CSD_AZ_EN_MASK)
    #else
        #define CapTouch_BIST_AZ_ENABLE_CFG                  (0uL)
    #endif /* (CapTouch_ENABLE == CapTouch_CSD_AUTO_ZERO_EN) */

    #define CapTouch_BIST_HSCMP_CFG                             (CapTouch_HSCMP_EN_MASK | CapTouch_BIST_AZ_ENABLE_CFG)
    #define CapTouch_BIST_CSDCMP_INIT                           (CapTouch_CSDCMP_CSDCMP_DISABLED)

    #define CapTouch_BIST_BLOCK_ON_DELAY                        (3uL * CYDEV_BCLK__HFCLK__MHZ)
    #define CapTouch_BIST_HSCMP_ON_DELAY                        (1uL * CYDEV_BCLK__HFCLK__MHZ)

    #if (CapTouch_ENABLE == CapTouch_CSD_EN)
        #if (CapTouch_CSD__CMOD_PAD == CapTouch_CMOD_CONNECTION)
            #define CapTouch_BIST_SW_FW_MOD_SEL_INIT    (CapTouch_SW_FW_MOD_SEL_SW_F1PM_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_MOD_SEL_SW_F1MA_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_MOD_SEL_SW_F1CA_STATIC_CLOSE)
        #else
            #define CapTouch_BIST_SW_FW_MOD_SEL_INIT    (0x00000000uL)
        #endif /* (CapTouch_CSD__CMOD_PAD == CapTouch_CMOD_CONNECTION) */

        #if (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CTANK_CONNECTION)
            #define CapTouch_BIST_SW_FW_TANK_SEL_INIT   (CapTouch_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_TANK_SEL_SW_F2MA_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_TANK_SEL_SW_F2CA_STATIC_CLOSE)
        #else
            #define CapTouch_BIST_SW_FW_TANK_SEL_INIT   (0x00000000uL)
        #endif /* (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CTANK_CONNECTION) */

    #elif (CapTouch_ENABLE == CapTouch_CSX_EN)
        #define CapTouch_BIST_SW_FW_MOD_SEL_INIT        (CapTouch_SW_FW_MOD_SEL_SW_F1PM_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_MOD_SEL_SW_F1MA_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_MOD_SEL_SW_F1CA_STATIC_CLOSE)

        #define CapTouch_BIST_SW_FW_TANK_SEL_INIT       (CapTouch_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_TANK_SEL_SW_F2MA_STATIC_CLOSE |\
                                                                 CapTouch_SW_FW_TANK_SEL_SW_F2CA_STATIC_CLOSE)
    #else
        #define CapTouch_BIST_SW_FW_MOD_SEL_INIT        (0x00000000uL)
        #define CapTouch_BIST_SW_FW_TANK_SEL_INIT       (0x00000000uL)
    #endif /* (CapTouch_ENABLE == CapTouch_CSD_EN) */


    #define CapTouch_BIST_SW_RES_INIT                           (CapTouch_CSD_INIT_SWITCH_RES << CYFLD_CSD_RES_HCAV__OFFSET)
    #define CapTouch_BIST_SW_FW_MOD_SEL_SCAN                    (0x00000000uL)
    #if ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
         (CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN) && \
         (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CTANK_CONNECTION))
        #define CapTouch_BIST_SW_FW_TANK_SEL_SCAN               (CapTouch_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                         CapTouch_SW_FW_TANK_SEL_SW_F2CB_PHI2)
    #else
        #define CapTouch_BIST_SW_FW_TANK_SEL_SCAN               (0x00000000uL)
    #endif

    #if ((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
         (CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN) && \
         (CapTouch_CSD__CSH_TANK_PAD == CapTouch_CTANK_CONNECTION))
        #define CapTouch_BIST_SW_SHIELD_SEL                     (CapTouch_SW_SHIELD_SEL_SW_HCBV_HSCMP)
    #elif((CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) && \
          (CapTouch_ENABLE == CapTouch_CSD_SHIELD_TANK_EN))
        #define CapTouch_BIST_SW_SHIELD_SEL                     (CapTouch_SW_SHIELD_SEL_SW_HCBV_HSCMP)
    #elif(CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN)
        #define CapTouch_BIST_SW_SHIELD_SEL                     (CapTouch_SW_SHIELD_SEL_SW_HCBG_PHI1 | \
                                                                         CapTouch_SW_SHIELD_SEL_SW_HCBV_PHI2_HSCMP)
    #else
        #define CapTouch_BIST_SW_SHIELD_SEL                     (0x00000000uL)
    #endif
    #define CapTouch_BIST_SW_RES_SCAN                           ((CapTouch_CSD_SHIELD_SWITCH_RES << CYFLD_CSD_RES_HCBV__OFFSET) |\
                                                                         (CapTouch_CSD_SHIELD_SWITCH_RES << CYFLD_CSD_RES_HCBG__OFFSET))
    #define CapTouch_BIST_HSCMP_SCAN_MASK                       (CapTouch_HSCMP_EN_MASK)
    #define CapTouch_BIST_IDACB_CFG                             (0x00000000uL)
    #define CapTouch_BIST_IDACA_CFG                             (0x01830000uL)

    /* IDAC Gain in nA */
    #define CapTouch_TST_SNS_CAP_IDAC_GAIN                      (2400uL)

    #if ((CYDEV_BCLK__HFCLK__HZ / CapTouch_TST_MOD_CLK_DIVIDER) <= CapTouch_MOD_CSD_CLK_12MHZ)
        #define CapTouch_FILTER_DELAY_CFG                       (CapTouch_CONFIG_FILTER_DELAY_12MHZ)
    #elif ((CYDEV_BCLK__HFCLK__HZ / CapTouch_TST_MOD_CLK_DIVIDER) <= CapTouch_MOD_CSD_CLK_24MHZ)
        #define CapTouch_FILTER_DELAY_CFG                       (CapTouch_CONFIG_FILTER_DELAY_24MHZ)
    #else
        /* ((CYDEV_BCLK__HFCLK__HZ / CapTouch_TST_MOD_CLK_DIVIDER) <= CapTouch_MOD_CSD_CLK_48MHZ) */
        #define CapTouch_FILTER_DELAY_CFG                       (CapTouch_CONFIG_FILTER_DELAY_48MHZ)
    #endif

    #define CapTouch_BIST_CSD_CONFIG                            (CapTouch_CONFIG_FILTER_DELAY_2_CYCLES)
    #define CapTouch_BIST_SW_REFGEN_SEL_CFG                     (CapTouch_SW_REFGEN_SEL_SW_SGR_MASK)

    #define CapTouch_TST_MEASMODE_VREF                              (0x1uL << CYFLD_CSD_ADC_MODE__OFFSET)
    #define CapTouch_TST_MEASMODE_VREFBY2                           (0x2uL << CYFLD_CSD_ADC_MODE__OFFSET)
    #define CapTouch_TST_MEASMODE_VIN                               (0x3uL << CYFLD_CSD_ADC_MODE__OFFSET)

    /* Clock defines */
    #define CapTouch_TST_VDDA_MODCLK_DIV_DEFAULT                    (CapTouch_TST_MOD_CLK_DIVIDER)
    #define CapTouch_TST_VDDA_SENSE_DIV_DEFAULT                     (0x4uL)
    #define CapTouch_TST_VDDA_TOTAL_CLOCK_DIV                       (CapTouch_TST_VDDA_MODCLK_DIV_DEFAULT * \
                                                                             CapTouch_TST_VDDA_SENSE_DIV_DEFAULT)

    /* Acquisition time definitions: ADC_CTL */
    #define CapTouch_VDDA_ACQUISITION_TIME_US                       (10uL)
    #define CapTouch_VDDA_ACQUISITION_BASE                          ((CapTouch_VDDA_ACQUISITION_TIME_US * \
                                                                            (CYDEV_BCLK__HFCLK__MHZ)) / \
                                                                            CapTouch_TST_VDDA_TOTAL_CLOCK_DIV)

    #define CapTouch_TST_VDDA_ADC_AZ_TIME                           (5uL)
    #define CapTouch_TST_VDDA_SEQ_TIME_BASE                         (((CYDEV_BCLK__HFCLK__HZ * CapTouch_TST_VDDA_ADC_AZ_TIME) / \
                                                                               CapTouch_TST_VDDA_TOTAL_CLOCK_DIV) / 1000000uL)

    #if (0 == CapTouch_TST_VDDA_SEQ_TIME_BASE)
        #define CapTouch_TST_VDDA_SEQ_TIME_DEFAULT                  (1u)
    #else
        #define CapTouch_TST_VDDA_SEQ_TIME_DEFAULT                  (CapTouch_TST_VDDA_SEQ_TIME_BASE)
    #endif




    #define CapTouch_TST_VDDA_FILTER_DELAY                          (2uL)
    #define CapTouch_TST_VDDA_FINE_INIT_TIME                        (CapTouch_TST_FINE_INIT_TIME)
    #define CapTouch_TST_VDDA_SCAN_DURATION                         (2uL)

    #define CapTouch_TST_VDDA_VREF_TRIM_MAX_DEVIATION               (20uL)

    /* The reference voltage macros */
    #define CapTouch_TST_VDDA_VREF_CALIB_USED                       (2400uL)

    #define CapTouch_TST_VDDA_BAD_RESULT                            (0xFFFFu)


    #define CapTouch_TST_VDDA_CONFIG_DEFAULT_CFG                    (CapTouch_CONFIG_ENABLE_MASK |\
                                                                             CapTouch_CONFIG_SAMPLE_SYNC_MASK |\
                                                                             CapTouch_CONFIG_SENSE_EN_MASK |\
                                                                             CapTouch_CONFIG_DSI_COUNT_SEL_MASK)

    #define CapTouch_TST_VDDA_IDACA_DEFAULT_CFG                     (0x00000000uL)
    #define CapTouch_TST_VDDA_IDACB_DEFAULT_CFG                     (CapTouch_TST_VDDA_IDAC_DEFAULT |\
                                                                            (CapTouch_IDAC_COMP_POL_DYN_DYNAMIC << \
                                                                              CYFLD_CSD_POL_DYN__OFFSET) |\
                                                                              CapTouch_IDAC_COMP_LEG3_EN_MASK)

    #define CapTouch_TST_CSDCMP_DEFAULT_CFG                         (0x00000000uL)
    #define CapTouch_TST_SW_DSI_SEL_DEFAULT_CFG                     (0x00000000uL)
    #define CapTouch_TST_SENSE_DUTY_VDDA_CFG                        (0x00000000uL)
    #define CapTouch_TST_SEQ_INIT_CNT_DEFAULT_CFG                   (1u)
    #define CapTouch_TST_SEQ_NORM_CNT_DEFAULT_CFG                   (CapTouch_TST_VDDA_SCAN_DURATION)
    #define CapTouch_TST_SW_HS_P_SEL_DEFAULT_CFG                    (CapTouch_SW_HS_P_SEL_SW_HMRH_STATIC_CLOSE)
    #define CapTouch_TST_SW_HS_N_SEL_DEFAULT_CFG                    (CapTouch_SW_HS_N_SEL_SW_HCCD_STATIC_CLOSE)
    #define CapTouch_TST_SW_SHIELD_SEL_DEFAULT_CFG                  (0x00000000uL)
    #define CapTouch_TST_SW_BYP_SEL_DEFAULT_CFG                     (CapTouch_SW_BYP_SEL_SW_BYA_MASK |\
                                                                             CapTouch_SW_BYP_SEL_SW_BYB_MASK)
    #define CapTouch_TST_SW_CMP_P_SEL_DEFAULT_CFG                   (0x00000000uL)
    #define CapTouch_TST_SW_CMP_N_SEL_DEFAULT_CFG                   (0x00000000uL)
    #define CapTouch_TST_SW_FW_MOD_SEL_DEFAULT_CFG                  (CapTouch_SW_FW_MOD_SEL_SW_C1CC_STATIC_CLOSE |\
                                                                             CapTouch_SW_FW_MOD_SEL_SW_C1CD_STATIC_CLOSE)
    #define CapTouch_TST_SW_FW_TANK_SEL_DEFAULT_CFG                 (CapTouch_SW_FW_TANK_SEL_SW_C2CC_STATIC_CLOSE |\
                                                                             CapTouch_SW_FW_TANK_SEL_SW_C2CD_STATIC_CLOSE)
    #define CapTouch_TST_SW_REFGEN_SEL_DEFAULT_CFG                  (CapTouch_SW_REFGEN_SEL_SW_SGR_MASK)
    #define CapTouch_TST_REFGEN_DEFAULT_CFG                         (CapTouch_REFGEN_REFGEN_EN_MASK |\
                                                                             CapTouch_REFGEN_RES_EN_MASK |\
                                                                            ((uint32)CapTouch_TST_VDDA_VREF_GAIN << \
                                                                             CYFLD_CSD_GAIN__OFFSET))
    #define CapTouch_TST_SW_AMUXBUF_SEL_DEFAULT_CFG                 (0x00000000uL)
    #define CapTouch_TST_HSCMP_DEFAULT_CFG                          (CapTouch_HSCMP_EN_MASK |\
                                                                             CapTouch_CSD_AZ_EN_MASK)

    #define CapTouch_TST_VDDA_SEQ_START_MEASURE_CFG                 (CapTouch_SEQ_START_AZ0_SKIP_MASK |\
                                                                            CapTouch_SEQ_START_START_MASK)
#else /* (CapTouch_ENABLE == CapTouch_CSDV2) */

#define CapTouch_BIST_CSD_CONFIG                            (CapTouch_CONFIG_SENSE_COMP_BW_MASK | \
                                                                     CapTouch_CONFIG_SENSE_INSEL_MASK | \
                                                                     CapTouch_CONFIG_REFBUF_DRV_MASK)

#define CapTouch_BIST_CMOD_PRECHARGE_CONFIG                 (CapTouch_BIST_CSD_CONFIG | \
                                                                     CapTouch_CONFIG_REFBUF_EN_MASK | \
                                                                     CapTouch_CONFIG_COMP_PIN_MASK)

#define CapTouch_BIST_CMOD_PRECHARGE_CONFIG_CSD_EN          (CapTouch_BIST_CMOD_PRECHARGE_CONFIG | \
                                                                     CapTouch_CSD_ENABLE_MASK)

/* Third-generation HW block Ctank pre-charge mode configuration */
#if(CapTouch_CSD_CSH_PRECHARGE_SRC == CapTouch_CSH_PRECHARGE_VREF)
    #if (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN)
        #define  CapTouch_BIST_CTANK_PRECHARGE_CONFIG       (CapTouch_BIST_CSD_CONFIG | \
                                                                     CapTouch_CONFIG_REFBUF_EN_MASK | \
                                                                     CapTouch_CONFIG_PRS_CLEAR_MASK | \
                                                                     CapTouch_CONFIG_REFBUF_OUTSEL_MASK)
    #else
        #define  CapTouch_BIST_CTANK_PRECHARGE_CONFIG       (CapTouch_BIST_CSD_CONFIG | \
                                                                     CapTouch_CONFIG_REFBUF_OUTSEL_MASK | \
                                                                     CapTouch_CONFIG_PRS_CLEAR_MASK)
    #endif /* (CapTouch_ENABLE == CapTouch_CSD_SHIELD_EN) */
#else
    #define  CapTouch_BIST_CTANK_PRECHARGE_CONFIG           (CapTouch_BIST_CSD_CONFIG |\
                                                                     CapTouch_CONFIG_REFBUF_OUTSEL_MASK |\
                                                                     CapTouch_CONFIG_REFBUF_EN_MASK |\
                                                                     CapTouch_CONFIG_COMP_MODE_MASK |\
                                                                     CapTouch_CONFIG_PRS_CLEAR_MASK |\
                                                                     CapTouch_CONFIG_COMP_PIN_MASK)

#endif /* (CapTouch_CSD_CSH_PRECHARGE_SRC == CapTouch__CSH_PRECHARGE_IO_BUF) */

#define  CapTouch_BIST_CTANK_PRECHARGE_CONFIG_CSD_EN        (CapTouch_BIST_CTANK_PRECHARGE_CONFIG | \
                                                                     CapTouch_CONFIG_ENABLE_MASK | \
                                                                     CapTouch_CONFIG_SENSE_COMP_EN_MASK)

#define CapTouch_BIST_IDAC_CFG                              (0x00000200uL)
#define CapTouch_BIST_IDAC_MOD_MASK                         (0x000003FFuL)
#define CapTouch_BIST_IDAC_MOD_VAL_MASK                     (0x000000FFuL)

/* IDAC Gain = 8x that corresponds to 1LSB = 1200 nA */
#define CapTouch_TST_SNS_CAP_IDAC_GAIN                      (1200uL)

#define CapTouch_BIST_SCAN_DURATION                         (CapTouch_BIST_10_BIT_MASK << CapTouch_RESOLUTION_OFFSET)
#define CapTouch_BIST_SCAN_DURATION_255                     (0x1FFuL << CapTouch_RESOLUTION_OFFSET)
#endif /* (CapTouch_ENABLE == CapTouch_CSDV2) */

#endif /* #if (CapTouch_ENABLE == CapTouch_SELF_TEST_EN) */

#endif /* End CY_SENSE_CapTouch_SELFTEST_H */


/* [] END OF FILE */
