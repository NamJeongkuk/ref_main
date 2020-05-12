/***************************************************************************//**
* \file CapTouch_RegisterMap.h
* \version 6.0
*
* \brief
*   This file provides the definitions for the Component data structure register.
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

#if !defined(CY_SENSE_CapTouch_REGISTER_MAP_H)
#define CY_SENSE_CapTouch_REGISTER_MAP_H

#include <cytypes.h>
#include "CapTouch_Configuration.h"
#include "CapTouch_Structure.h"

/*****************************************************************************/
/* RAM Data structure register definitions                                   */
/*****************************************************************************/
#define CapTouch_CONFIG_ID_VALUE                            (CapTouch_dsRam.configId)
#define CapTouch_CONFIG_ID_OFFSET                           (0u)
#define CapTouch_CONFIG_ID_SIZE                             (2u)
#define CapTouch_CONFIG_ID_PARAM_ID                         (0x87000000u)

#define CapTouch_DEVICE_ID_VALUE                            (CapTouch_dsRam.deviceId)
#define CapTouch_DEVICE_ID_OFFSET                           (2u)
#define CapTouch_DEVICE_ID_SIZE                             (2u)
#define CapTouch_DEVICE_ID_PARAM_ID                         (0x8B000002u)

#define CapTouch_HW_CLOCK_VALUE                             (CapTouch_dsRam.hwClock)
#define CapTouch_HW_CLOCK_OFFSET                            (4u)
#define CapTouch_HW_CLOCK_SIZE                              (2u)
#define CapTouch_HW_CLOCK_PARAM_ID                          (0x86000004u)

#define CapTouch_TUNER_CMD_VALUE                            (CapTouch_dsRam.tunerCmd)
#define CapTouch_TUNER_CMD_OFFSET                           (6u)
#define CapTouch_TUNER_CMD_SIZE                             (2u)
#define CapTouch_TUNER_CMD_PARAM_ID                         (0xA1000006u)

#define CapTouch_SCAN_COUNTER_VALUE                         (CapTouch_dsRam.scanCounter)
#define CapTouch_SCAN_COUNTER_OFFSET                        (8u)
#define CapTouch_SCAN_COUNTER_SIZE                          (2u)
#define CapTouch_SCAN_COUNTER_PARAM_ID                      (0x85000008u)

#define CapTouch_STATUS_VALUE                               (CapTouch_dsRam.status)
#define CapTouch_STATUS_OFFSET                              (12u)
#define CapTouch_STATUS_SIZE                                (4u)
#define CapTouch_STATUS_PARAM_ID                            (0xCB00000Cu)

#define CapTouch_WDGT_ENABLE0_VALUE                         (CapTouch_dsRam.wdgtEnable[0u])
#define CapTouch_WDGT_ENABLE0_OFFSET                        (16u)
#define CapTouch_WDGT_ENABLE0_SIZE                          (4u)
#define CapTouch_WDGT_ENABLE0_PARAM_ID                      (0xE6000010u)

#define CapTouch_WDGT_WORKING0_VALUE                        (CapTouch_dsRam.wdgtWorking[0u])
#define CapTouch_WDGT_WORKING0_OFFSET                       (20u)
#define CapTouch_WDGT_WORKING0_SIZE                         (4u)
#define CapTouch_WDGT_WORKING0_PARAM_ID                     (0xCC000014u)

#define CapTouch_WDGT_STATUS0_VALUE                         (CapTouch_dsRam.wdgtStatus[0u])
#define CapTouch_WDGT_STATUS0_OFFSET                        (24u)
#define CapTouch_WDGT_STATUS0_SIZE                          (4u)
#define CapTouch_WDGT_STATUS0_PARAM_ID                      (0xCF000018u)

#define CapTouch_SNS_STATUS0_VALUE                          (CapTouch_dsRam.snsStatus[0u])
#define CapTouch_SNS_STATUS0_OFFSET                         (28u)
#define CapTouch_SNS_STATUS0_SIZE                           (1u)
#define CapTouch_SNS_STATUS0_PARAM_ID                       (0x4900001Cu)

#define CapTouch_SNS_STATUS1_VALUE                          (CapTouch_dsRam.snsStatus[1u])
#define CapTouch_SNS_STATUS1_OFFSET                         (29u)
#define CapTouch_SNS_STATUS1_SIZE                           (1u)
#define CapTouch_SNS_STATUS1_PARAM_ID                       (0x4F00001Du)

#define CapTouch_SNS_STATUS2_VALUE                          (CapTouch_dsRam.snsStatus[2u])
#define CapTouch_SNS_STATUS2_OFFSET                         (30u)
#define CapTouch_SNS_STATUS2_SIZE                           (1u)
#define CapTouch_SNS_STATUS2_PARAM_ID                       (0x4500001Eu)

#define CapTouch_SNS_STATUS3_VALUE                          (CapTouch_dsRam.snsStatus[3u])
#define CapTouch_SNS_STATUS3_OFFSET                         (31u)
#define CapTouch_SNS_STATUS3_SIZE                           (1u)
#define CapTouch_SNS_STATUS3_PARAM_ID                       (0x4300001Fu)

#define CapTouch_SNS_STATUS4_VALUE                          (CapTouch_dsRam.snsStatus[4u])
#define CapTouch_SNS_STATUS4_OFFSET                         (32u)
#define CapTouch_SNS_STATUS4_SIZE                           (1u)
#define CapTouch_SNS_STATUS4_PARAM_ID                       (0x45000020u)

#define CapTouch_SNS_STATUS5_VALUE                          (CapTouch_dsRam.snsStatus[5u])
#define CapTouch_SNS_STATUS5_OFFSET                         (33u)
#define CapTouch_SNS_STATUS5_SIZE                           (1u)
#define CapTouch_SNS_STATUS5_PARAM_ID                       (0x43000021u)

#define CapTouch_SNS_STATUS6_VALUE                          (CapTouch_dsRam.snsStatus[6u])
#define CapTouch_SNS_STATUS6_OFFSET                         (34u)
#define CapTouch_SNS_STATUS6_SIZE                           (1u)
#define CapTouch_SNS_STATUS6_PARAM_ID                       (0x49000022u)

#define CapTouch_SNS_STATUS7_VALUE                          (CapTouch_dsRam.snsStatus[7u])
#define CapTouch_SNS_STATUS7_OFFSET                         (35u)
#define CapTouch_SNS_STATUS7_SIZE                           (1u)
#define CapTouch_SNS_STATUS7_PARAM_ID                       (0x4F000023u)

#define CapTouch_SNS_STATUS8_VALUE                          (CapTouch_dsRam.snsStatus[8u])
#define CapTouch_SNS_STATUS8_OFFSET                         (36u)
#define CapTouch_SNS_STATUS8_SIZE                           (1u)
#define CapTouch_SNS_STATUS8_PARAM_ID                       (0x44000024u)

#define CapTouch_SNS_STATUS9_VALUE                          (CapTouch_dsRam.snsStatus[9u])
#define CapTouch_SNS_STATUS9_OFFSET                         (37u)
#define CapTouch_SNS_STATUS9_SIZE                           (1u)
#define CapTouch_SNS_STATUS9_PARAM_ID                       (0x42000025u)

#define CapTouch_SNS_STATUS10_VALUE                         (CapTouch_dsRam.snsStatus[10u])
#define CapTouch_SNS_STATUS10_OFFSET                        (38u)
#define CapTouch_SNS_STATUS10_SIZE                          (1u)
#define CapTouch_SNS_STATUS10_PARAM_ID                      (0x48000026u)

#define CapTouch_SNS_STATUS11_VALUE                         (CapTouch_dsRam.snsStatus[11u])
#define CapTouch_SNS_STATUS11_OFFSET                        (39u)
#define CapTouch_SNS_STATUS11_SIZE                          (1u)
#define CapTouch_SNS_STATUS11_PARAM_ID                      (0x4E000027u)

#define CapTouch_SNS_STATUS12_VALUE                         (CapTouch_dsRam.snsStatus[12u])
#define CapTouch_SNS_STATUS12_OFFSET                        (40u)
#define CapTouch_SNS_STATUS12_SIZE                          (1u)
#define CapTouch_SNS_STATUS12_PARAM_ID                      (0x47000028u)

#define CapTouch_CSD0_CONFIG_VALUE                          (CapTouch_dsRam.csd0Config)
#define CapTouch_CSD0_CONFIG_OFFSET                         (42u)
#define CapTouch_CSD0_CONFIG_SIZE                           (2u)
#define CapTouch_CSD0_CONFIG_PARAM_ID                       (0xA580002Au)

#define CapTouch_MOD_CSX_CLK_VALUE                          (CapTouch_dsRam.modCsxClk)
#define CapTouch_MOD_CSX_CLK_OFFSET                         (44u)
#define CapTouch_MOD_CSX_CLK_SIZE                           (1u)
#define CapTouch_MOD_CSX_CLK_PARAM_ID                       (0x6080002Cu)

#define CapTouch_GLB_CRC_VALUE                              (CapTouch_dsRam.glbCrc)
#define CapTouch_GLB_CRC_OFFSET                             (46u)
#define CapTouch_GLB_CRC_SIZE                               (2u)
#define CapTouch_GLB_CRC_PARAM_ID                           (0x8200002Eu)

#define CapTouch_U102_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u102.crc)
#define CapTouch_U102_CRC_OFFSET                            (48u)
#define CapTouch_U102_CRC_SIZE                              (2u)
#define CapTouch_U102_CRC_PARAM_ID                          (0x88000030u)

#define CapTouch_U102_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u102.resolution)
#define CapTouch_U102_RESOLUTION_OFFSET                     (50u)
#define CapTouch_U102_RESOLUTION_SIZE                       (2u)
#define CapTouch_U102_RESOLUTION_PARAM_ID                   (0xA2800032u)

#define CapTouch_U102_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u102.fingerTh)
#define CapTouch_U102_FINGER_TH_OFFSET                      (52u)
#define CapTouch_U102_FINGER_TH_SIZE                        (2u)
#define CapTouch_U102_FINGER_TH_PARAM_ID                    (0xAF800034u)

#define CapTouch_U102_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u102.noiseTh)
#define CapTouch_U102_NOISE_TH_OFFSET                       (54u)
#define CapTouch_U102_NOISE_TH_SIZE                         (1u)
#define CapTouch_U102_NOISE_TH_PARAM_ID                     (0x6B800036u)

#define CapTouch_U102_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u102.nNoiseTh)
#define CapTouch_U102_NNOISE_TH_OFFSET                      (55u)
#define CapTouch_U102_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U102_NNOISE_TH_PARAM_ID                    (0x6D800037u)

#define CapTouch_U102_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u102.hysteresis)
#define CapTouch_U102_HYSTERESIS_OFFSET                     (56u)
#define CapTouch_U102_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U102_HYSTERESIS_PARAM_ID                   (0x64800038u)

#define CapTouch_U102_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u102.onDebounce)
#define CapTouch_U102_ON_DEBOUNCE_OFFSET                    (57u)
#define CapTouch_U102_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U102_ON_DEBOUNCE_PARAM_ID                  (0x62800039u)

#define CapTouch_U102_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u102.lowBslnRst)
#define CapTouch_U102_LOW_BSLN_RST_OFFSET                   (58u)
#define CapTouch_U102_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U102_LOW_BSLN_RST_PARAM_ID                 (0x6880003Au)

#define CapTouch_U102_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u102.snsClk)
#define CapTouch_U102_SNS_CLK_OFFSET                        (60u)
#define CapTouch_U102_SNS_CLK_SIZE                          (2u)
#define CapTouch_U102_SNS_CLK_PARAM_ID                      (0xAD80003Cu)

#define CapTouch_U102_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u102.snsClkSource)
#define CapTouch_U102_SNS_CLK_SOURCE_OFFSET                 (62u)
#define CapTouch_U102_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U102_SNS_CLK_SOURCE_PARAM_ID               (0x4280003Eu)

#define CapTouch_U103_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u103.crc)
#define CapTouch_U103_CRC_OFFSET                            (64u)
#define CapTouch_U103_CRC_SIZE                              (2u)
#define CapTouch_U103_CRC_PARAM_ID                          (0x89010040u)

#define CapTouch_U103_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u103.resolution)
#define CapTouch_U103_RESOLUTION_OFFSET                     (66u)
#define CapTouch_U103_RESOLUTION_SIZE                       (2u)
#define CapTouch_U103_RESOLUTION_PARAM_ID                   (0xA3810042u)

#define CapTouch_U103_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u103.fingerTh)
#define CapTouch_U103_FINGER_TH_OFFSET                      (68u)
#define CapTouch_U103_FINGER_TH_SIZE                        (2u)
#define CapTouch_U103_FINGER_TH_PARAM_ID                    (0xAE810044u)

#define CapTouch_U103_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u103.noiseTh)
#define CapTouch_U103_NOISE_TH_OFFSET                       (70u)
#define CapTouch_U103_NOISE_TH_SIZE                         (1u)
#define CapTouch_U103_NOISE_TH_PARAM_ID                     (0x6A810046u)

#define CapTouch_U103_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u103.nNoiseTh)
#define CapTouch_U103_NNOISE_TH_OFFSET                      (71u)
#define CapTouch_U103_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U103_NNOISE_TH_PARAM_ID                    (0x6C810047u)

#define CapTouch_U103_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u103.hysteresis)
#define CapTouch_U103_HYSTERESIS_OFFSET                     (72u)
#define CapTouch_U103_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U103_HYSTERESIS_PARAM_ID                   (0x65810048u)

#define CapTouch_U103_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u103.onDebounce)
#define CapTouch_U103_ON_DEBOUNCE_OFFSET                    (73u)
#define CapTouch_U103_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U103_ON_DEBOUNCE_PARAM_ID                  (0x63810049u)

#define CapTouch_U103_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u103.lowBslnRst)
#define CapTouch_U103_LOW_BSLN_RST_OFFSET                   (74u)
#define CapTouch_U103_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U103_LOW_BSLN_RST_PARAM_ID                 (0x6981004Au)

#define CapTouch_U103_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u103.snsClk)
#define CapTouch_U103_SNS_CLK_OFFSET                        (76u)
#define CapTouch_U103_SNS_CLK_SIZE                          (2u)
#define CapTouch_U103_SNS_CLK_PARAM_ID                      (0xAC81004Cu)

#define CapTouch_U103_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u103.snsClkSource)
#define CapTouch_U103_SNS_CLK_SOURCE_OFFSET                 (78u)
#define CapTouch_U103_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U103_SNS_CLK_SOURCE_PARAM_ID               (0x4381004Eu)

#define CapTouch_U104_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u104.crc)
#define CapTouch_U104_CRC_OFFSET                            (80u)
#define CapTouch_U104_CRC_SIZE                              (2u)
#define CapTouch_U104_CRC_PARAM_ID                          (0x89020050u)

#define CapTouch_U104_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u104.resolution)
#define CapTouch_U104_RESOLUTION_OFFSET                     (82u)
#define CapTouch_U104_RESOLUTION_SIZE                       (2u)
#define CapTouch_U104_RESOLUTION_PARAM_ID                   (0xA3820052u)

#define CapTouch_U104_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u104.fingerTh)
#define CapTouch_U104_FINGER_TH_OFFSET                      (84u)
#define CapTouch_U104_FINGER_TH_SIZE                        (2u)
#define CapTouch_U104_FINGER_TH_PARAM_ID                    (0xAE820054u)

#define CapTouch_U104_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u104.noiseTh)
#define CapTouch_U104_NOISE_TH_OFFSET                       (86u)
#define CapTouch_U104_NOISE_TH_SIZE                         (1u)
#define CapTouch_U104_NOISE_TH_PARAM_ID                     (0x6A820056u)

#define CapTouch_U104_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u104.nNoiseTh)
#define CapTouch_U104_NNOISE_TH_OFFSET                      (87u)
#define CapTouch_U104_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U104_NNOISE_TH_PARAM_ID                    (0x6C820057u)

#define CapTouch_U104_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u104.hysteresis)
#define CapTouch_U104_HYSTERESIS_OFFSET                     (88u)
#define CapTouch_U104_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U104_HYSTERESIS_PARAM_ID                   (0x65820058u)

#define CapTouch_U104_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u104.onDebounce)
#define CapTouch_U104_ON_DEBOUNCE_OFFSET                    (89u)
#define CapTouch_U104_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U104_ON_DEBOUNCE_PARAM_ID                  (0x63820059u)

#define CapTouch_U104_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u104.lowBslnRst)
#define CapTouch_U104_LOW_BSLN_RST_OFFSET                   (90u)
#define CapTouch_U104_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U104_LOW_BSLN_RST_PARAM_ID                 (0x6982005Au)

#define CapTouch_U104_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u104.snsClk)
#define CapTouch_U104_SNS_CLK_OFFSET                        (92u)
#define CapTouch_U104_SNS_CLK_SIZE                          (2u)
#define CapTouch_U104_SNS_CLK_PARAM_ID                      (0xAC82005Cu)

#define CapTouch_U104_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u104.snsClkSource)
#define CapTouch_U104_SNS_CLK_SOURCE_OFFSET                 (94u)
#define CapTouch_U104_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U104_SNS_CLK_SOURCE_PARAM_ID               (0x4382005Eu)

#define CapTouch_U105_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u105.crc)
#define CapTouch_U105_CRC_OFFSET                            (96u)
#define CapTouch_U105_CRC_SIZE                              (2u)
#define CapTouch_U105_CRC_PARAM_ID                          (0x85030060u)

#define CapTouch_U105_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u105.resolution)
#define CapTouch_U105_RESOLUTION_OFFSET                     (98u)
#define CapTouch_U105_RESOLUTION_SIZE                       (2u)
#define CapTouch_U105_RESOLUTION_PARAM_ID                   (0xAF830062u)

#define CapTouch_U105_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u105.fingerTh)
#define CapTouch_U105_FINGER_TH_OFFSET                      (100u)
#define CapTouch_U105_FINGER_TH_SIZE                        (2u)
#define CapTouch_U105_FINGER_TH_PARAM_ID                    (0xA2830064u)

#define CapTouch_U105_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u105.noiseTh)
#define CapTouch_U105_NOISE_TH_OFFSET                       (102u)
#define CapTouch_U105_NOISE_TH_SIZE                         (1u)
#define CapTouch_U105_NOISE_TH_PARAM_ID                     (0x66830066u)

#define CapTouch_U105_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u105.nNoiseTh)
#define CapTouch_U105_NNOISE_TH_OFFSET                      (103u)
#define CapTouch_U105_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U105_NNOISE_TH_PARAM_ID                    (0x60830067u)

#define CapTouch_U105_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u105.hysteresis)
#define CapTouch_U105_HYSTERESIS_OFFSET                     (104u)
#define CapTouch_U105_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U105_HYSTERESIS_PARAM_ID                   (0x69830068u)

#define CapTouch_U105_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u105.onDebounce)
#define CapTouch_U105_ON_DEBOUNCE_OFFSET                    (105u)
#define CapTouch_U105_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U105_ON_DEBOUNCE_PARAM_ID                  (0x6F830069u)

#define CapTouch_U105_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u105.lowBslnRst)
#define CapTouch_U105_LOW_BSLN_RST_OFFSET                   (106u)
#define CapTouch_U105_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U105_LOW_BSLN_RST_PARAM_ID                 (0x6583006Au)

#define CapTouch_U105_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u105.snsClk)
#define CapTouch_U105_SNS_CLK_OFFSET                        (108u)
#define CapTouch_U105_SNS_CLK_SIZE                          (2u)
#define CapTouch_U105_SNS_CLK_PARAM_ID                      (0xA083006Cu)

#define CapTouch_U105_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u105.snsClkSource)
#define CapTouch_U105_SNS_CLK_SOURCE_OFFSET                 (110u)
#define CapTouch_U105_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U105_SNS_CLK_SOURCE_PARAM_ID               (0x4F83006Eu)

#define CapTouch_U106_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u106.crc)
#define CapTouch_U106_CRC_OFFSET                            (112u)
#define CapTouch_U106_CRC_SIZE                              (2u)
#define CapTouch_U106_CRC_PARAM_ID                          (0x89040070u)

#define CapTouch_U106_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u106.resolution)
#define CapTouch_U106_RESOLUTION_OFFSET                     (114u)
#define CapTouch_U106_RESOLUTION_SIZE                       (2u)
#define CapTouch_U106_RESOLUTION_PARAM_ID                   (0xA3840072u)

#define CapTouch_U106_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u106.fingerTh)
#define CapTouch_U106_FINGER_TH_OFFSET                      (116u)
#define CapTouch_U106_FINGER_TH_SIZE                        (2u)
#define CapTouch_U106_FINGER_TH_PARAM_ID                    (0xAE840074u)

#define CapTouch_U106_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u106.noiseTh)
#define CapTouch_U106_NOISE_TH_OFFSET                       (118u)
#define CapTouch_U106_NOISE_TH_SIZE                         (1u)
#define CapTouch_U106_NOISE_TH_PARAM_ID                     (0x6A840076u)

#define CapTouch_U106_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u106.nNoiseTh)
#define CapTouch_U106_NNOISE_TH_OFFSET                      (119u)
#define CapTouch_U106_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U106_NNOISE_TH_PARAM_ID                    (0x6C840077u)

#define CapTouch_U106_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u106.hysteresis)
#define CapTouch_U106_HYSTERESIS_OFFSET                     (120u)
#define CapTouch_U106_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U106_HYSTERESIS_PARAM_ID                   (0x65840078u)

#define CapTouch_U106_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u106.onDebounce)
#define CapTouch_U106_ON_DEBOUNCE_OFFSET                    (121u)
#define CapTouch_U106_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U106_ON_DEBOUNCE_PARAM_ID                  (0x63840079u)

#define CapTouch_U106_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u106.lowBslnRst)
#define CapTouch_U106_LOW_BSLN_RST_OFFSET                   (122u)
#define CapTouch_U106_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U106_LOW_BSLN_RST_PARAM_ID                 (0x6984007Au)

#define CapTouch_U106_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u106.snsClk)
#define CapTouch_U106_SNS_CLK_OFFSET                        (124u)
#define CapTouch_U106_SNS_CLK_SIZE                          (2u)
#define CapTouch_U106_SNS_CLK_PARAM_ID                      (0xAC84007Cu)

#define CapTouch_U106_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u106.snsClkSource)
#define CapTouch_U106_SNS_CLK_SOURCE_OFFSET                 (126u)
#define CapTouch_U106_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U106_SNS_CLK_SOURCE_PARAM_ID               (0x4384007Eu)

#define CapTouch_U107_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u107.crc)
#define CapTouch_U107_CRC_OFFSET                            (128u)
#define CapTouch_U107_CRC_SIZE                              (2u)
#define CapTouch_U107_CRC_PARAM_ID                          (0x8B050080u)

#define CapTouch_U107_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u107.resolution)
#define CapTouch_U107_RESOLUTION_OFFSET                     (130u)
#define CapTouch_U107_RESOLUTION_SIZE                       (2u)
#define CapTouch_U107_RESOLUTION_PARAM_ID                   (0xA1850082u)

#define CapTouch_U107_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u107.fingerTh)
#define CapTouch_U107_FINGER_TH_OFFSET                      (132u)
#define CapTouch_U107_FINGER_TH_SIZE                        (2u)
#define CapTouch_U107_FINGER_TH_PARAM_ID                    (0xAC850084u)

#define CapTouch_U107_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u107.noiseTh)
#define CapTouch_U107_NOISE_TH_OFFSET                       (134u)
#define CapTouch_U107_NOISE_TH_SIZE                         (1u)
#define CapTouch_U107_NOISE_TH_PARAM_ID                     (0x68850086u)

#define CapTouch_U107_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u107.nNoiseTh)
#define CapTouch_U107_NNOISE_TH_OFFSET                      (135u)
#define CapTouch_U107_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U107_NNOISE_TH_PARAM_ID                    (0x6E850087u)

#define CapTouch_U107_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u107.hysteresis)
#define CapTouch_U107_HYSTERESIS_OFFSET                     (136u)
#define CapTouch_U107_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U107_HYSTERESIS_PARAM_ID                   (0x67850088u)

#define CapTouch_U107_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u107.onDebounce)
#define CapTouch_U107_ON_DEBOUNCE_OFFSET                    (137u)
#define CapTouch_U107_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U107_ON_DEBOUNCE_PARAM_ID                  (0x61850089u)

#define CapTouch_U107_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u107.lowBslnRst)
#define CapTouch_U107_LOW_BSLN_RST_OFFSET                   (138u)
#define CapTouch_U107_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U107_LOW_BSLN_RST_PARAM_ID                 (0x6B85008Au)

#define CapTouch_U107_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u107.snsClk)
#define CapTouch_U107_SNS_CLK_OFFSET                        (140u)
#define CapTouch_U107_SNS_CLK_SIZE                          (2u)
#define CapTouch_U107_SNS_CLK_PARAM_ID                      (0xAE85008Cu)

#define CapTouch_U107_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u107.snsClkSource)
#define CapTouch_U107_SNS_CLK_SOURCE_OFFSET                 (142u)
#define CapTouch_U107_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U107_SNS_CLK_SOURCE_PARAM_ID               (0x4185008Eu)

#define CapTouch_U108_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u108.crc)
#define CapTouch_U108_CRC_OFFSET                            (144u)
#define CapTouch_U108_CRC_SIZE                              (2u)
#define CapTouch_U108_CRC_PARAM_ID                          (0x8B060090u)

#define CapTouch_U108_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u108.resolution)
#define CapTouch_U108_RESOLUTION_OFFSET                     (146u)
#define CapTouch_U108_RESOLUTION_SIZE                       (2u)
#define CapTouch_U108_RESOLUTION_PARAM_ID                   (0xA1860092u)

#define CapTouch_U108_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u108.fingerTh)
#define CapTouch_U108_FINGER_TH_OFFSET                      (148u)
#define CapTouch_U108_FINGER_TH_SIZE                        (2u)
#define CapTouch_U108_FINGER_TH_PARAM_ID                    (0xAC860094u)

#define CapTouch_U108_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u108.noiseTh)
#define CapTouch_U108_NOISE_TH_OFFSET                       (150u)
#define CapTouch_U108_NOISE_TH_SIZE                         (1u)
#define CapTouch_U108_NOISE_TH_PARAM_ID                     (0x68860096u)

#define CapTouch_U108_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u108.nNoiseTh)
#define CapTouch_U108_NNOISE_TH_OFFSET                      (151u)
#define CapTouch_U108_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U108_NNOISE_TH_PARAM_ID                    (0x6E860097u)

#define CapTouch_U108_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u108.hysteresis)
#define CapTouch_U108_HYSTERESIS_OFFSET                     (152u)
#define CapTouch_U108_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U108_HYSTERESIS_PARAM_ID                   (0x67860098u)

#define CapTouch_U108_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u108.onDebounce)
#define CapTouch_U108_ON_DEBOUNCE_OFFSET                    (153u)
#define CapTouch_U108_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U108_ON_DEBOUNCE_PARAM_ID                  (0x61860099u)

#define CapTouch_U108_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u108.lowBslnRst)
#define CapTouch_U108_LOW_BSLN_RST_OFFSET                   (154u)
#define CapTouch_U108_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U108_LOW_BSLN_RST_PARAM_ID                 (0x6B86009Au)

#define CapTouch_U108_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u108.snsClk)
#define CapTouch_U108_SNS_CLK_OFFSET                        (156u)
#define CapTouch_U108_SNS_CLK_SIZE                          (2u)
#define CapTouch_U108_SNS_CLK_PARAM_ID                      (0xAE86009Cu)

#define CapTouch_U108_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u108.snsClkSource)
#define CapTouch_U108_SNS_CLK_SOURCE_OFFSET                 (158u)
#define CapTouch_U108_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U108_SNS_CLK_SOURCE_PARAM_ID               (0x4186009Eu)

#define CapTouch_U109_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u109.crc)
#define CapTouch_U109_CRC_OFFSET                            (160u)
#define CapTouch_U109_CRC_SIZE                              (2u)
#define CapTouch_U109_CRC_PARAM_ID                          (0x870700A0u)

#define CapTouch_U109_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u109.resolution)
#define CapTouch_U109_RESOLUTION_OFFSET                     (162u)
#define CapTouch_U109_RESOLUTION_SIZE                       (2u)
#define CapTouch_U109_RESOLUTION_PARAM_ID                   (0xAD8700A2u)

#define CapTouch_U109_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u109.fingerTh)
#define CapTouch_U109_FINGER_TH_OFFSET                      (164u)
#define CapTouch_U109_FINGER_TH_SIZE                        (2u)
#define CapTouch_U109_FINGER_TH_PARAM_ID                    (0xA08700A4u)

#define CapTouch_U109_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u109.noiseTh)
#define CapTouch_U109_NOISE_TH_OFFSET                       (166u)
#define CapTouch_U109_NOISE_TH_SIZE                         (1u)
#define CapTouch_U109_NOISE_TH_PARAM_ID                     (0x648700A6u)

#define CapTouch_U109_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u109.nNoiseTh)
#define CapTouch_U109_NNOISE_TH_OFFSET                      (167u)
#define CapTouch_U109_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U109_NNOISE_TH_PARAM_ID                    (0x628700A7u)

#define CapTouch_U109_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u109.hysteresis)
#define CapTouch_U109_HYSTERESIS_OFFSET                     (168u)
#define CapTouch_U109_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U109_HYSTERESIS_PARAM_ID                   (0x6B8700A8u)

#define CapTouch_U109_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u109.onDebounce)
#define CapTouch_U109_ON_DEBOUNCE_OFFSET                    (169u)
#define CapTouch_U109_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U109_ON_DEBOUNCE_PARAM_ID                  (0x6D8700A9u)

#define CapTouch_U109_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u109.lowBslnRst)
#define CapTouch_U109_LOW_BSLN_RST_OFFSET                   (170u)
#define CapTouch_U109_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U109_LOW_BSLN_RST_PARAM_ID                 (0x678700AAu)

#define CapTouch_U109_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u109.snsClk)
#define CapTouch_U109_SNS_CLK_OFFSET                        (172u)
#define CapTouch_U109_SNS_CLK_SIZE                          (2u)
#define CapTouch_U109_SNS_CLK_PARAM_ID                      (0xA28700ACu)

#define CapTouch_U109_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u109.snsClkSource)
#define CapTouch_U109_SNS_CLK_SOURCE_OFFSET                 (174u)
#define CapTouch_U109_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U109_SNS_CLK_SOURCE_PARAM_ID               (0x4D8700AEu)

#define CapTouch_U110_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u110.crc)
#define CapTouch_U110_CRC_OFFSET                            (176u)
#define CapTouch_U110_CRC_SIZE                              (2u)
#define CapTouch_U110_CRC_PARAM_ID                          (0x8A0800B0u)

#define CapTouch_U110_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u110.resolution)
#define CapTouch_U110_RESOLUTION_OFFSET                     (178u)
#define CapTouch_U110_RESOLUTION_SIZE                       (2u)
#define CapTouch_U110_RESOLUTION_PARAM_ID                   (0xA08800B2u)

#define CapTouch_U110_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u110.fingerTh)
#define CapTouch_U110_FINGER_TH_OFFSET                      (180u)
#define CapTouch_U110_FINGER_TH_SIZE                        (2u)
#define CapTouch_U110_FINGER_TH_PARAM_ID                    (0xAD8800B4u)

#define CapTouch_U110_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u110.noiseTh)
#define CapTouch_U110_NOISE_TH_OFFSET                       (182u)
#define CapTouch_U110_NOISE_TH_SIZE                         (1u)
#define CapTouch_U110_NOISE_TH_PARAM_ID                     (0x698800B6u)

#define CapTouch_U110_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u110.nNoiseTh)
#define CapTouch_U110_NNOISE_TH_OFFSET                      (183u)
#define CapTouch_U110_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U110_NNOISE_TH_PARAM_ID                    (0x6F8800B7u)

#define CapTouch_U110_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u110.hysteresis)
#define CapTouch_U110_HYSTERESIS_OFFSET                     (184u)
#define CapTouch_U110_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U110_HYSTERESIS_PARAM_ID                   (0x668800B8u)

#define CapTouch_U110_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u110.onDebounce)
#define CapTouch_U110_ON_DEBOUNCE_OFFSET                    (185u)
#define CapTouch_U110_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U110_ON_DEBOUNCE_PARAM_ID                  (0x608800B9u)

#define CapTouch_U110_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u110.lowBslnRst)
#define CapTouch_U110_LOW_BSLN_RST_OFFSET                   (186u)
#define CapTouch_U110_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U110_LOW_BSLN_RST_PARAM_ID                 (0x6A8800BAu)

#define CapTouch_U110_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u110.snsClk)
#define CapTouch_U110_SNS_CLK_OFFSET                        (188u)
#define CapTouch_U110_SNS_CLK_SIZE                          (2u)
#define CapTouch_U110_SNS_CLK_PARAM_ID                      (0xAF8800BCu)

#define CapTouch_U110_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u110.snsClkSource)
#define CapTouch_U110_SNS_CLK_SOURCE_OFFSET                 (190u)
#define CapTouch_U110_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U110_SNS_CLK_SOURCE_PARAM_ID               (0x408800BEu)

#define CapTouch_U111_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u111.crc)
#define CapTouch_U111_CRC_OFFSET                            (192u)
#define CapTouch_U111_CRC_SIZE                              (2u)
#define CapTouch_U111_CRC_PARAM_ID                          (0x8B0900C0u)

#define CapTouch_U111_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u111.resolution)
#define CapTouch_U111_RESOLUTION_OFFSET                     (194u)
#define CapTouch_U111_RESOLUTION_SIZE                       (2u)
#define CapTouch_U111_RESOLUTION_PARAM_ID                   (0xA18900C2u)

#define CapTouch_U111_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u111.fingerTh)
#define CapTouch_U111_FINGER_TH_OFFSET                      (196u)
#define CapTouch_U111_FINGER_TH_SIZE                        (2u)
#define CapTouch_U111_FINGER_TH_PARAM_ID                    (0xAC8900C4u)

#define CapTouch_U111_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u111.noiseTh)
#define CapTouch_U111_NOISE_TH_OFFSET                       (198u)
#define CapTouch_U111_NOISE_TH_SIZE                         (1u)
#define CapTouch_U111_NOISE_TH_PARAM_ID                     (0x688900C6u)

#define CapTouch_U111_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u111.nNoiseTh)
#define CapTouch_U111_NNOISE_TH_OFFSET                      (199u)
#define CapTouch_U111_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U111_NNOISE_TH_PARAM_ID                    (0x6E8900C7u)

#define CapTouch_U111_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u111.hysteresis)
#define CapTouch_U111_HYSTERESIS_OFFSET                     (200u)
#define CapTouch_U111_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U111_HYSTERESIS_PARAM_ID                   (0x678900C8u)

#define CapTouch_U111_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u111.onDebounce)
#define CapTouch_U111_ON_DEBOUNCE_OFFSET                    (201u)
#define CapTouch_U111_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U111_ON_DEBOUNCE_PARAM_ID                  (0x618900C9u)

#define CapTouch_U111_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u111.lowBslnRst)
#define CapTouch_U111_LOW_BSLN_RST_OFFSET                   (202u)
#define CapTouch_U111_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U111_LOW_BSLN_RST_PARAM_ID                 (0x6B8900CAu)

#define CapTouch_U111_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u111.snsClk)
#define CapTouch_U111_SNS_CLK_OFFSET                        (204u)
#define CapTouch_U111_SNS_CLK_SIZE                          (2u)
#define CapTouch_U111_SNS_CLK_PARAM_ID                      (0xAE8900CCu)

#define CapTouch_U111_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u111.snsClkSource)
#define CapTouch_U111_SNS_CLK_SOURCE_OFFSET                 (206u)
#define CapTouch_U111_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U111_SNS_CLK_SOURCE_PARAM_ID               (0x418900CEu)

#define CapTouch_U112_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u112.crc)
#define CapTouch_U112_CRC_OFFSET                            (208u)
#define CapTouch_U112_CRC_SIZE                              (2u)
#define CapTouch_U112_CRC_PARAM_ID                          (0x8B0A00D0u)

#define CapTouch_U112_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u112.resolution)
#define CapTouch_U112_RESOLUTION_OFFSET                     (210u)
#define CapTouch_U112_RESOLUTION_SIZE                       (2u)
#define CapTouch_U112_RESOLUTION_PARAM_ID                   (0xA18A00D2u)

#define CapTouch_U112_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u112.fingerTh)
#define CapTouch_U112_FINGER_TH_OFFSET                      (212u)
#define CapTouch_U112_FINGER_TH_SIZE                        (2u)
#define CapTouch_U112_FINGER_TH_PARAM_ID                    (0xAC8A00D4u)

#define CapTouch_U112_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u112.noiseTh)
#define CapTouch_U112_NOISE_TH_OFFSET                       (214u)
#define CapTouch_U112_NOISE_TH_SIZE                         (1u)
#define CapTouch_U112_NOISE_TH_PARAM_ID                     (0x688A00D6u)

#define CapTouch_U112_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u112.nNoiseTh)
#define CapTouch_U112_NNOISE_TH_OFFSET                      (215u)
#define CapTouch_U112_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U112_NNOISE_TH_PARAM_ID                    (0x6E8A00D7u)

#define CapTouch_U112_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u112.hysteresis)
#define CapTouch_U112_HYSTERESIS_OFFSET                     (216u)
#define CapTouch_U112_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U112_HYSTERESIS_PARAM_ID                   (0x678A00D8u)

#define CapTouch_U112_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u112.onDebounce)
#define CapTouch_U112_ON_DEBOUNCE_OFFSET                    (217u)
#define CapTouch_U112_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U112_ON_DEBOUNCE_PARAM_ID                  (0x618A00D9u)

#define CapTouch_U112_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u112.lowBslnRst)
#define CapTouch_U112_LOW_BSLN_RST_OFFSET                   (218u)
#define CapTouch_U112_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U112_LOW_BSLN_RST_PARAM_ID                 (0x6B8A00DAu)

#define CapTouch_U112_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u112.snsClk)
#define CapTouch_U112_SNS_CLK_OFFSET                        (220u)
#define CapTouch_U112_SNS_CLK_SIZE                          (2u)
#define CapTouch_U112_SNS_CLK_PARAM_ID                      (0xAE8A00DCu)

#define CapTouch_U112_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u112.snsClkSource)
#define CapTouch_U112_SNS_CLK_SOURCE_OFFSET                 (222u)
#define CapTouch_U112_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U112_SNS_CLK_SOURCE_PARAM_ID               (0x418A00DEu)

#define CapTouch_U113_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u113.crc)
#define CapTouch_U113_CRC_OFFSET                            (224u)
#define CapTouch_U113_CRC_SIZE                              (2u)
#define CapTouch_U113_CRC_PARAM_ID                          (0x870B00E0u)

#define CapTouch_U113_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u113.resolution)
#define CapTouch_U113_RESOLUTION_OFFSET                     (226u)
#define CapTouch_U113_RESOLUTION_SIZE                       (2u)
#define CapTouch_U113_RESOLUTION_PARAM_ID                   (0xAD8B00E2u)

#define CapTouch_U113_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u113.fingerTh)
#define CapTouch_U113_FINGER_TH_OFFSET                      (228u)
#define CapTouch_U113_FINGER_TH_SIZE                        (2u)
#define CapTouch_U113_FINGER_TH_PARAM_ID                    (0xA08B00E4u)

#define CapTouch_U113_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u113.noiseTh)
#define CapTouch_U113_NOISE_TH_OFFSET                       (230u)
#define CapTouch_U113_NOISE_TH_SIZE                         (1u)
#define CapTouch_U113_NOISE_TH_PARAM_ID                     (0x648B00E6u)

#define CapTouch_U113_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u113.nNoiseTh)
#define CapTouch_U113_NNOISE_TH_OFFSET                      (231u)
#define CapTouch_U113_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U113_NNOISE_TH_PARAM_ID                    (0x628B00E7u)

#define CapTouch_U113_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u113.hysteresis)
#define CapTouch_U113_HYSTERESIS_OFFSET                     (232u)
#define CapTouch_U113_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U113_HYSTERESIS_PARAM_ID                   (0x6B8B00E8u)

#define CapTouch_U113_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u113.onDebounce)
#define CapTouch_U113_ON_DEBOUNCE_OFFSET                    (233u)
#define CapTouch_U113_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U113_ON_DEBOUNCE_PARAM_ID                  (0x6D8B00E9u)

#define CapTouch_U113_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u113.lowBslnRst)
#define CapTouch_U113_LOW_BSLN_RST_OFFSET                   (234u)
#define CapTouch_U113_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U113_LOW_BSLN_RST_PARAM_ID                 (0x678B00EAu)

#define CapTouch_U113_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u113.snsClk)
#define CapTouch_U113_SNS_CLK_OFFSET                        (236u)
#define CapTouch_U113_SNS_CLK_SIZE                          (2u)
#define CapTouch_U113_SNS_CLK_PARAM_ID                      (0xA28B00ECu)

#define CapTouch_U113_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u113.snsClkSource)
#define CapTouch_U113_SNS_CLK_SOURCE_OFFSET                 (238u)
#define CapTouch_U113_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U113_SNS_CLK_SOURCE_PARAM_ID               (0x4D8B00EEu)

#define CapTouch_U114_CRC_VALUE                             (CapTouch_dsRam.wdgtList.u114.crc)
#define CapTouch_U114_CRC_OFFSET                            (240u)
#define CapTouch_U114_CRC_SIZE                              (2u)
#define CapTouch_U114_CRC_PARAM_ID                          (0x8B0C00F0u)

#define CapTouch_U114_RESOLUTION_VALUE                      (CapTouch_dsRam.wdgtList.u114.resolution)
#define CapTouch_U114_RESOLUTION_OFFSET                     (242u)
#define CapTouch_U114_RESOLUTION_SIZE                       (2u)
#define CapTouch_U114_RESOLUTION_PARAM_ID                   (0xA18C00F2u)

#define CapTouch_U114_FINGER_TH_VALUE                       (CapTouch_dsRam.wdgtList.u114.fingerTh)
#define CapTouch_U114_FINGER_TH_OFFSET                      (244u)
#define CapTouch_U114_FINGER_TH_SIZE                        (2u)
#define CapTouch_U114_FINGER_TH_PARAM_ID                    (0xAC8C00F4u)

#define CapTouch_U114_NOISE_TH_VALUE                        (CapTouch_dsRam.wdgtList.u114.noiseTh)
#define CapTouch_U114_NOISE_TH_OFFSET                       (246u)
#define CapTouch_U114_NOISE_TH_SIZE                         (1u)
#define CapTouch_U114_NOISE_TH_PARAM_ID                     (0x688C00F6u)

#define CapTouch_U114_NNOISE_TH_VALUE                       (CapTouch_dsRam.wdgtList.u114.nNoiseTh)
#define CapTouch_U114_NNOISE_TH_OFFSET                      (247u)
#define CapTouch_U114_NNOISE_TH_SIZE                        (1u)
#define CapTouch_U114_NNOISE_TH_PARAM_ID                    (0x6E8C00F7u)

#define CapTouch_U114_HYSTERESIS_VALUE                      (CapTouch_dsRam.wdgtList.u114.hysteresis)
#define CapTouch_U114_HYSTERESIS_OFFSET                     (248u)
#define CapTouch_U114_HYSTERESIS_SIZE                       (1u)
#define CapTouch_U114_HYSTERESIS_PARAM_ID                   (0x678C00F8u)

#define CapTouch_U114_ON_DEBOUNCE_VALUE                     (CapTouch_dsRam.wdgtList.u114.onDebounce)
#define CapTouch_U114_ON_DEBOUNCE_OFFSET                    (249u)
#define CapTouch_U114_ON_DEBOUNCE_SIZE                      (1u)
#define CapTouch_U114_ON_DEBOUNCE_PARAM_ID                  (0x618C00F9u)

#define CapTouch_U114_LOW_BSLN_RST_VALUE                    (CapTouch_dsRam.wdgtList.u114.lowBslnRst)
#define CapTouch_U114_LOW_BSLN_RST_OFFSET                   (250u)
#define CapTouch_U114_LOW_BSLN_RST_SIZE                     (1u)
#define CapTouch_U114_LOW_BSLN_RST_PARAM_ID                 (0x6B8C00FAu)

#define CapTouch_U114_SNS_CLK_VALUE                         (CapTouch_dsRam.wdgtList.u114.snsClk)
#define CapTouch_U114_SNS_CLK_OFFSET                        (252u)
#define CapTouch_U114_SNS_CLK_SIZE                          (2u)
#define CapTouch_U114_SNS_CLK_PARAM_ID                      (0xAE8C00FCu)

#define CapTouch_U114_SNS_CLK_SOURCE_VALUE                  (CapTouch_dsRam.wdgtList.u114.snsClkSource)
#define CapTouch_U114_SNS_CLK_SOURCE_OFFSET                 (254u)
#define CapTouch_U114_SNS_CLK_SOURCE_SIZE                   (1u)
#define CapTouch_U114_SNS_CLK_SOURCE_PARAM_ID               (0x418C00FEu)

#define CapTouch_U102_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u102[0u].raw[0u])
#define CapTouch_U102_RX0_RAW0_OFFSET                       (256u)
#define CapTouch_U102_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U102_RX0_RAW0_PARAM_ID                     (0x8C000100u)

#define CapTouch_U102_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u102[0u].bsln[0u])
#define CapTouch_U102_RX0_BSLN0_OFFSET                      (258u)
#define CapTouch_U102_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U102_RX0_BSLN0_PARAM_ID                    (0x80000102u)

#define CapTouch_U102_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u102[0u].bslnInv[0u])
#define CapTouch_U102_RX0_BSLN_INV0_OFFSET                  (260u)
#define CapTouch_U102_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U102_RX0_BSLN_INV0_PARAM_ID                (0x8D000104u)

#define CapTouch_U102_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u102[0u].bslnExt[0u])
#define CapTouch_U102_RX0_BSLN_EXT0_OFFSET                  (262u)
#define CapTouch_U102_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U102_RX0_BSLN_EXT0_PARAM_ID                (0x49000106u)

#define CapTouch_U102_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u102[0u].diff)
#define CapTouch_U102_RX0_DIFF_OFFSET                       (264u)
#define CapTouch_U102_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U102_RX0_DIFF_PARAM_ID                     (0x8E000108u)

#define CapTouch_U102_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u102[0u].negBslnRstCnt[0u])
#define CapTouch_U102_RX0_NEG_BSLN_RST_CNT0_OFFSET          (266u)
#define CapTouch_U102_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U102_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4A00010Au)

#define CapTouch_U102_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u102[0u].idacComp[0u])
#define CapTouch_U102_RX0_IDAC_COMP0_OFFSET                 (267u)
#define CapTouch_U102_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U102_RX0_IDAC_COMP0_PARAM_ID               (0x4C00010Bu)

#define CapTouch_U103_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u103[0u].raw[0u])
#define CapTouch_U103_RX0_RAW0_OFFSET                       (268u)
#define CapTouch_U103_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U103_RX0_RAW0_PARAM_ID                     (0x8F00010Cu)

#define CapTouch_U103_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u103[0u].bsln[0u])
#define CapTouch_U103_RX0_BSLN0_OFFSET                      (270u)
#define CapTouch_U103_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U103_RX0_BSLN0_PARAM_ID                    (0x8300010Eu)

#define CapTouch_U103_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u103[0u].bslnInv[0u])
#define CapTouch_U103_RX0_BSLN_INV0_OFFSET                  (272u)
#define CapTouch_U103_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U103_RX0_BSLN_INV0_PARAM_ID                (0x89000110u)

#define CapTouch_U103_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u103[0u].bslnExt[0u])
#define CapTouch_U103_RX0_BSLN_EXT0_OFFSET                  (274u)
#define CapTouch_U103_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U103_RX0_BSLN_EXT0_PARAM_ID                (0x4D000112u)

#define CapTouch_U103_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u103[0u].diff)
#define CapTouch_U103_RX0_DIFF_OFFSET                       (276u)
#define CapTouch_U103_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U103_RX0_DIFF_PARAM_ID                     (0x88000114u)

#define CapTouch_U103_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u103[0u].negBslnRstCnt[0u])
#define CapTouch_U103_RX0_NEG_BSLN_RST_CNT0_OFFSET          (278u)
#define CapTouch_U103_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U103_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4C000116u)

#define CapTouch_U103_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u103[0u].idacComp[0u])
#define CapTouch_U103_RX0_IDAC_COMP0_OFFSET                 (279u)
#define CapTouch_U103_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U103_RX0_IDAC_COMP0_PARAM_ID               (0x4A000117u)

#define CapTouch_U104_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u104[0u].raw[0u])
#define CapTouch_U104_RX0_RAW0_OFFSET                       (280u)
#define CapTouch_U104_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U104_RX0_RAW0_PARAM_ID                     (0x8B000118u)

#define CapTouch_U104_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u104[0u].bsln[0u])
#define CapTouch_U104_RX0_BSLN0_OFFSET                      (282u)
#define CapTouch_U104_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U104_RX0_BSLN0_PARAM_ID                    (0x8700011Au)

#define CapTouch_U104_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u104[0u].bslnInv[0u])
#define CapTouch_U104_RX0_BSLN_INV0_OFFSET                  (284u)
#define CapTouch_U104_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U104_RX0_BSLN_INV0_PARAM_ID                (0x8A00011Cu)

#define CapTouch_U104_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u104[0u].bslnExt[0u])
#define CapTouch_U104_RX0_BSLN_EXT0_OFFSET                  (286u)
#define CapTouch_U104_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U104_RX0_BSLN_EXT0_PARAM_ID                (0x4E00011Eu)

#define CapTouch_U104_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u104[0u].diff)
#define CapTouch_U104_RX0_DIFF_OFFSET                       (288u)
#define CapTouch_U104_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U104_RX0_DIFF_PARAM_ID                     (0x86000120u)

#define CapTouch_U104_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u104[0u].negBslnRstCnt[0u])
#define CapTouch_U104_RX0_NEG_BSLN_RST_CNT0_OFFSET          (290u)
#define CapTouch_U104_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U104_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x42000122u)

#define CapTouch_U104_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u104[0u].idacComp[0u])
#define CapTouch_U104_RX0_IDAC_COMP0_OFFSET                 (291u)
#define CapTouch_U104_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U104_RX0_IDAC_COMP0_PARAM_ID               (0x44000123u)

#define CapTouch_U105_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u105[0u].raw[0u])
#define CapTouch_U105_RX0_RAW0_OFFSET                       (292u)
#define CapTouch_U105_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U105_RX0_RAW0_PARAM_ID                     (0x87000124u)

#define CapTouch_U105_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u105[0u].bsln[0u])
#define CapTouch_U105_RX0_BSLN0_OFFSET                      (294u)
#define CapTouch_U105_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U105_RX0_BSLN0_PARAM_ID                    (0x8B000126u)

#define CapTouch_U105_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u105[0u].bslnInv[0u])
#define CapTouch_U105_RX0_BSLN_INV0_OFFSET                  (296u)
#define CapTouch_U105_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U105_RX0_BSLN_INV0_PARAM_ID                (0x84000128u)

#define CapTouch_U105_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u105[0u].bslnExt[0u])
#define CapTouch_U105_RX0_BSLN_EXT0_OFFSET                  (298u)
#define CapTouch_U105_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U105_RX0_BSLN_EXT0_PARAM_ID                (0x4000012Au)

#define CapTouch_U105_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u105[0u].diff)
#define CapTouch_U105_RX0_DIFF_OFFSET                       (300u)
#define CapTouch_U105_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U105_RX0_DIFF_PARAM_ID                     (0x8500012Cu)

#define CapTouch_U105_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u105[0u].negBslnRstCnt[0u])
#define CapTouch_U105_RX0_NEG_BSLN_RST_CNT0_OFFSET          (302u)
#define CapTouch_U105_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U105_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4100012Eu)

#define CapTouch_U105_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u105[0u].idacComp[0u])
#define CapTouch_U105_RX0_IDAC_COMP0_OFFSET                 (303u)
#define CapTouch_U105_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U105_RX0_IDAC_COMP0_PARAM_ID               (0x4700012Fu)

#define CapTouch_U106_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u106[0u].raw[0u])
#define CapTouch_U106_RX0_RAW0_OFFSET                       (304u)
#define CapTouch_U106_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U106_RX0_RAW0_PARAM_ID                     (0x83000130u)

#define CapTouch_U106_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u106[0u].bsln[0u])
#define CapTouch_U106_RX0_BSLN0_OFFSET                      (306u)
#define CapTouch_U106_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U106_RX0_BSLN0_PARAM_ID                    (0x8F000132u)

#define CapTouch_U106_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u106[0u].bslnInv[0u])
#define CapTouch_U106_RX0_BSLN_INV0_OFFSET                  (308u)
#define CapTouch_U106_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U106_RX0_BSLN_INV0_PARAM_ID                (0x82000134u)

#define CapTouch_U106_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u106[0u].bslnExt[0u])
#define CapTouch_U106_RX0_BSLN_EXT0_OFFSET                  (310u)
#define CapTouch_U106_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U106_RX0_BSLN_EXT0_PARAM_ID                (0x46000136u)

#define CapTouch_U106_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u106[0u].diff)
#define CapTouch_U106_RX0_DIFF_OFFSET                       (312u)
#define CapTouch_U106_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U106_RX0_DIFF_PARAM_ID                     (0x81000138u)

#define CapTouch_U106_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u106[0u].negBslnRstCnt[0u])
#define CapTouch_U106_RX0_NEG_BSLN_RST_CNT0_OFFSET          (314u)
#define CapTouch_U106_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U106_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4500013Au)

#define CapTouch_U106_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u106[0u].idacComp[0u])
#define CapTouch_U106_RX0_IDAC_COMP0_OFFSET                 (315u)
#define CapTouch_U106_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U106_RX0_IDAC_COMP0_PARAM_ID               (0x4300013Bu)

#define CapTouch_U107_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u107[0u].raw[0u])
#define CapTouch_U107_RX0_RAW0_OFFSET                       (316u)
#define CapTouch_U107_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U107_RX0_RAW0_PARAM_ID                     (0x8000013Cu)

#define CapTouch_U107_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u107[0u].bsln[0u])
#define CapTouch_U107_RX0_BSLN0_OFFSET                      (318u)
#define CapTouch_U107_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U107_RX0_BSLN0_PARAM_ID                    (0x8C00013Eu)

#define CapTouch_U107_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u107[0u].bslnInv[0u])
#define CapTouch_U107_RX0_BSLN_INV0_OFFSET                  (320u)
#define CapTouch_U107_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U107_RX0_BSLN_INV0_PARAM_ID                (0x81000140u)

#define CapTouch_U107_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u107[0u].bslnExt[0u])
#define CapTouch_U107_RX0_BSLN_EXT0_OFFSET                  (322u)
#define CapTouch_U107_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U107_RX0_BSLN_EXT0_PARAM_ID                (0x45000142u)

#define CapTouch_U107_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u107[0u].diff)
#define CapTouch_U107_RX0_DIFF_OFFSET                       (324u)
#define CapTouch_U107_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U107_RX0_DIFF_PARAM_ID                     (0x80000144u)

#define CapTouch_U107_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u107[0u].negBslnRstCnt[0u])
#define CapTouch_U107_RX0_NEG_BSLN_RST_CNT0_OFFSET          (326u)
#define CapTouch_U107_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U107_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x44000146u)

#define CapTouch_U107_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u107[0u].idacComp[0u])
#define CapTouch_U107_RX0_IDAC_COMP0_OFFSET                 (327u)
#define CapTouch_U107_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U107_RX0_IDAC_COMP0_PARAM_ID               (0x42000147u)

#define CapTouch_U108_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u108[0u].raw[0u])
#define CapTouch_U108_RX0_RAW0_OFFSET                       (328u)
#define CapTouch_U108_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U108_RX0_RAW0_PARAM_ID                     (0x83000148u)

#define CapTouch_U108_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u108[0u].bsln[0u])
#define CapTouch_U108_RX0_BSLN0_OFFSET                      (330u)
#define CapTouch_U108_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U108_RX0_BSLN0_PARAM_ID                    (0x8F00014Au)

#define CapTouch_U108_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u108[0u].bslnInv[0u])
#define CapTouch_U108_RX0_BSLN_INV0_OFFSET                  (332u)
#define CapTouch_U108_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U108_RX0_BSLN_INV0_PARAM_ID                (0x8200014Cu)

#define CapTouch_U108_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u108[0u].bslnExt[0u])
#define CapTouch_U108_RX0_BSLN_EXT0_OFFSET                  (334u)
#define CapTouch_U108_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U108_RX0_BSLN_EXT0_PARAM_ID                (0x4600014Eu)

#define CapTouch_U108_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u108[0u].diff)
#define CapTouch_U108_RX0_DIFF_OFFSET                       (336u)
#define CapTouch_U108_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U108_RX0_DIFF_PARAM_ID                     (0x84000150u)

#define CapTouch_U108_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u108[0u].negBslnRstCnt[0u])
#define CapTouch_U108_RX0_NEG_BSLN_RST_CNT0_OFFSET          (338u)
#define CapTouch_U108_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U108_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x40000152u)

#define CapTouch_U108_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u108[0u].idacComp[0u])
#define CapTouch_U108_RX0_IDAC_COMP0_OFFSET                 (339u)
#define CapTouch_U108_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U108_RX0_IDAC_COMP0_PARAM_ID               (0x46000153u)

#define CapTouch_U109_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u109[0u].raw[0u])
#define CapTouch_U109_RX0_RAW0_OFFSET                       (340u)
#define CapTouch_U109_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U109_RX0_RAW0_PARAM_ID                     (0x85000154u)

#define CapTouch_U109_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u109[0u].bsln[0u])
#define CapTouch_U109_RX0_BSLN0_OFFSET                      (342u)
#define CapTouch_U109_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U109_RX0_BSLN0_PARAM_ID                    (0x89000156u)

#define CapTouch_U109_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u109[0u].bslnInv[0u])
#define CapTouch_U109_RX0_BSLN_INV0_OFFSET                  (344u)
#define CapTouch_U109_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U109_RX0_BSLN_INV0_PARAM_ID                (0x86000158u)

#define CapTouch_U109_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u109[0u].bslnExt[0u])
#define CapTouch_U109_RX0_BSLN_EXT0_OFFSET                  (346u)
#define CapTouch_U109_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U109_RX0_BSLN_EXT0_PARAM_ID                (0x4200015Au)

#define CapTouch_U109_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u109[0u].diff)
#define CapTouch_U109_RX0_DIFF_OFFSET                       (348u)
#define CapTouch_U109_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U109_RX0_DIFF_PARAM_ID                     (0x8700015Cu)

#define CapTouch_U109_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u109[0u].negBslnRstCnt[0u])
#define CapTouch_U109_RX0_NEG_BSLN_RST_CNT0_OFFSET          (350u)
#define CapTouch_U109_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U109_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4300015Eu)

#define CapTouch_U109_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u109[0u].idacComp[0u])
#define CapTouch_U109_RX0_IDAC_COMP0_OFFSET                 (351u)
#define CapTouch_U109_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U109_RX0_IDAC_COMP0_PARAM_ID               (0x4500015Fu)

#define CapTouch_U110_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u110[0u].raw[0u])
#define CapTouch_U110_RX0_RAW0_OFFSET                       (352u)
#define CapTouch_U110_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U110_RX0_RAW0_PARAM_ID                     (0x8B000160u)

#define CapTouch_U110_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u110[0u].bsln[0u])
#define CapTouch_U110_RX0_BSLN0_OFFSET                      (354u)
#define CapTouch_U110_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U110_RX0_BSLN0_PARAM_ID                    (0x87000162u)

#define CapTouch_U110_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u110[0u].bslnInv[0u])
#define CapTouch_U110_RX0_BSLN_INV0_OFFSET                  (356u)
#define CapTouch_U110_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U110_RX0_BSLN_INV0_PARAM_ID                (0x8A000164u)

#define CapTouch_U110_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u110[0u].bslnExt[0u])
#define CapTouch_U110_RX0_BSLN_EXT0_OFFSET                  (358u)
#define CapTouch_U110_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U110_RX0_BSLN_EXT0_PARAM_ID                (0x4E000166u)

#define CapTouch_U110_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u110[0u].diff)
#define CapTouch_U110_RX0_DIFF_OFFSET                       (360u)
#define CapTouch_U110_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U110_RX0_DIFF_PARAM_ID                     (0x89000168u)

#define CapTouch_U110_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u110[0u].negBslnRstCnt[0u])
#define CapTouch_U110_RX0_NEG_BSLN_RST_CNT0_OFFSET          (362u)
#define CapTouch_U110_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U110_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4D00016Au)

#define CapTouch_U110_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u110[0u].idacComp[0u])
#define CapTouch_U110_RX0_IDAC_COMP0_OFFSET                 (363u)
#define CapTouch_U110_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U110_RX0_IDAC_COMP0_PARAM_ID               (0x4B00016Bu)

#define CapTouch_U111_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u111[0u].raw[0u])
#define CapTouch_U111_RX0_RAW0_OFFSET                       (364u)
#define CapTouch_U111_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U111_RX0_RAW0_PARAM_ID                     (0x8800016Cu)

#define CapTouch_U111_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u111[0u].bsln[0u])
#define CapTouch_U111_RX0_BSLN0_OFFSET                      (366u)
#define CapTouch_U111_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U111_RX0_BSLN0_PARAM_ID                    (0x8400016Eu)

#define CapTouch_U111_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u111[0u].bslnInv[0u])
#define CapTouch_U111_RX0_BSLN_INV0_OFFSET                  (368u)
#define CapTouch_U111_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U111_RX0_BSLN_INV0_PARAM_ID                (0x8E000170u)

#define CapTouch_U111_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u111[0u].bslnExt[0u])
#define CapTouch_U111_RX0_BSLN_EXT0_OFFSET                  (370u)
#define CapTouch_U111_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U111_RX0_BSLN_EXT0_PARAM_ID                (0x4A000172u)

#define CapTouch_U111_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u111[0u].diff)
#define CapTouch_U111_RX0_DIFF_OFFSET                       (372u)
#define CapTouch_U111_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U111_RX0_DIFF_PARAM_ID                     (0x8F000174u)

#define CapTouch_U111_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u111[0u].negBslnRstCnt[0u])
#define CapTouch_U111_RX0_NEG_BSLN_RST_CNT0_OFFSET          (374u)
#define CapTouch_U111_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U111_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4B000176u)

#define CapTouch_U111_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u111[0u].idacComp[0u])
#define CapTouch_U111_RX0_IDAC_COMP0_OFFSET                 (375u)
#define CapTouch_U111_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U111_RX0_IDAC_COMP0_PARAM_ID               (0x4D000177u)

#define CapTouch_U112_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u112[0u].raw[0u])
#define CapTouch_U112_RX0_RAW0_OFFSET                       (376u)
#define CapTouch_U112_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U112_RX0_RAW0_PARAM_ID                     (0x8C000178u)

#define CapTouch_U112_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u112[0u].bsln[0u])
#define CapTouch_U112_RX0_BSLN0_OFFSET                      (378u)
#define CapTouch_U112_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U112_RX0_BSLN0_PARAM_ID                    (0x8000017Au)

#define CapTouch_U112_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u112[0u].bslnInv[0u])
#define CapTouch_U112_RX0_BSLN_INV0_OFFSET                  (380u)
#define CapTouch_U112_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U112_RX0_BSLN_INV0_PARAM_ID                (0x8D00017Cu)

#define CapTouch_U112_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u112[0u].bslnExt[0u])
#define CapTouch_U112_RX0_BSLN_EXT0_OFFSET                  (382u)
#define CapTouch_U112_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U112_RX0_BSLN_EXT0_PARAM_ID                (0x4900017Eu)

#define CapTouch_U112_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u112[0u].diff)
#define CapTouch_U112_RX0_DIFF_OFFSET                       (384u)
#define CapTouch_U112_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U112_RX0_DIFF_PARAM_ID                     (0x8F000180u)

#define CapTouch_U112_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u112[0u].negBslnRstCnt[0u])
#define CapTouch_U112_RX0_NEG_BSLN_RST_CNT0_OFFSET          (386u)
#define CapTouch_U112_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U112_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4B000182u)

#define CapTouch_U112_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u112[0u].idacComp[0u])
#define CapTouch_U112_RX0_IDAC_COMP0_OFFSET                 (387u)
#define CapTouch_U112_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U112_RX0_IDAC_COMP0_PARAM_ID               (0x4D000183u)

#define CapTouch_U113_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u113[0u].raw[0u])
#define CapTouch_U113_RX0_RAW0_OFFSET                       (388u)
#define CapTouch_U113_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U113_RX0_RAW0_PARAM_ID                     (0x8E000184u)

#define CapTouch_U113_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u113[0u].bsln[0u])
#define CapTouch_U113_RX0_BSLN0_OFFSET                      (390u)
#define CapTouch_U113_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U113_RX0_BSLN0_PARAM_ID                    (0x82000186u)

#define CapTouch_U113_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u113[0u].bslnInv[0u])
#define CapTouch_U113_RX0_BSLN_INV0_OFFSET                  (392u)
#define CapTouch_U113_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U113_RX0_BSLN_INV0_PARAM_ID                (0x8D000188u)

#define CapTouch_U113_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u113[0u].bslnExt[0u])
#define CapTouch_U113_RX0_BSLN_EXT0_OFFSET                  (394u)
#define CapTouch_U113_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U113_RX0_BSLN_EXT0_PARAM_ID                (0x4900018Au)

#define CapTouch_U113_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u113[0u].diff)
#define CapTouch_U113_RX0_DIFF_OFFSET                       (396u)
#define CapTouch_U113_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U113_RX0_DIFF_PARAM_ID                     (0x8C00018Cu)

#define CapTouch_U113_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u113[0u].negBslnRstCnt[0u])
#define CapTouch_U113_RX0_NEG_BSLN_RST_CNT0_OFFSET          (398u)
#define CapTouch_U113_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U113_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4800018Eu)

#define CapTouch_U113_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u113[0u].idacComp[0u])
#define CapTouch_U113_RX0_IDAC_COMP0_OFFSET                 (399u)
#define CapTouch_U113_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U113_RX0_IDAC_COMP0_PARAM_ID               (0x4E00018Fu)

#define CapTouch_U114_RX0_RAW0_VALUE                        (CapTouch_dsRam.snsList.u114[0u].raw[0u])
#define CapTouch_U114_RX0_RAW0_OFFSET                       (400u)
#define CapTouch_U114_RX0_RAW0_SIZE                         (2u)
#define CapTouch_U114_RX0_RAW0_PARAM_ID                     (0x8A000190u)

#define CapTouch_U114_RX0_BSLN0_VALUE                       (CapTouch_dsRam.snsList.u114[0u].bsln[0u])
#define CapTouch_U114_RX0_BSLN0_OFFSET                      (402u)
#define CapTouch_U114_RX0_BSLN0_SIZE                        (2u)
#define CapTouch_U114_RX0_BSLN0_PARAM_ID                    (0x86000192u)

#define CapTouch_U114_RX0_BSLN_INV0_VALUE                   (CapTouch_dsRam.snsList.u114[0u].bslnInv[0u])
#define CapTouch_U114_RX0_BSLN_INV0_OFFSET                  (404u)
#define CapTouch_U114_RX0_BSLN_INV0_SIZE                    (2u)
#define CapTouch_U114_RX0_BSLN_INV0_PARAM_ID                (0x8B000194u)

#define CapTouch_U114_RX0_BSLN_EXT0_VALUE                   (CapTouch_dsRam.snsList.u114[0u].bslnExt[0u])
#define CapTouch_U114_RX0_BSLN_EXT0_OFFSET                  (406u)
#define CapTouch_U114_RX0_BSLN_EXT0_SIZE                    (1u)
#define CapTouch_U114_RX0_BSLN_EXT0_PARAM_ID                (0x4F000196u)

#define CapTouch_U114_RX0_DIFF_VALUE                        (CapTouch_dsRam.snsList.u114[0u].diff)
#define CapTouch_U114_RX0_DIFF_OFFSET                       (408u)
#define CapTouch_U114_RX0_DIFF_SIZE                         (2u)
#define CapTouch_U114_RX0_DIFF_PARAM_ID                     (0x88000198u)

#define CapTouch_U114_RX0_NEG_BSLN_RST_CNT0_VALUE           (CapTouch_dsRam.snsList.u114[0u].negBslnRstCnt[0u])
#define CapTouch_U114_RX0_NEG_BSLN_RST_CNT0_OFFSET          (410u)
#define CapTouch_U114_RX0_NEG_BSLN_RST_CNT0_SIZE            (1u)
#define CapTouch_U114_RX0_NEG_BSLN_RST_CNT0_PARAM_ID        (0x4C00019Au)

#define CapTouch_U114_RX0_IDAC_COMP0_VALUE                  (CapTouch_dsRam.snsList.u114[0u].idacComp[0u])
#define CapTouch_U114_RX0_IDAC_COMP0_OFFSET                 (411u)
#define CapTouch_U114_RX0_IDAC_COMP0_SIZE                   (1u)
#define CapTouch_U114_RX0_IDAC_COMP0_PARAM_ID               (0x4A00019Bu)

#define CapTouch_TEST_RESULT_MASK_VALUE                     (CapTouch_dsRam.selfTest.testResultMask)
#define CapTouch_TEST_RESULT_MASK_OFFSET                    (412u)
#define CapTouch_TEST_RESULT_MASK_SIZE                      (4u)
#define CapTouch_TEST_RESULT_MASK_PARAM_ID                  (0xED00019Cu)

#define CapTouch_EXT_CAP0_VALUE                             (CapTouch_dsRam.selfTest.extCap[0u])
#define CapTouch_EXT_CAP0_OFFSET                            (416u)
#define CapTouch_EXT_CAP0_SIZE                              (2u)
#define CapTouch_EXT_CAP0_PARAM_ID                          (0xAE0001A0u)

#define CapTouch_EXT_CAP1_VALUE                             (CapTouch_dsRam.selfTest.extCap[1u])
#define CapTouch_EXT_CAP1_OFFSET                            (418u)
#define CapTouch_EXT_CAP1_SIZE                              (2u)
#define CapTouch_EXT_CAP1_PARAM_ID                          (0xA20001A2u)

#define CapTouch_VDDA_VOLTAGE_VALUE                         (CapTouch_dsRam.selfTest.vddaVoltage)
#define CapTouch_VDDA_VOLTAGE_OFFSET                        (420u)
#define CapTouch_VDDA_VOLTAGE_SIZE                          (2u)
#define CapTouch_VDDA_VOLTAGE_PARAM_ID                      (0xAF0001A4u)

#define CapTouch_GLB_CRC_CALC_VALUE                         (CapTouch_dsRam.selfTest.glbCrcCalc)
#define CapTouch_GLB_CRC_CALC_OFFSET                        (422u)
#define CapTouch_GLB_CRC_CALC_SIZE                          (2u)
#define CapTouch_GLB_CRC_CALC_PARAM_ID                      (0xA30001A6u)

#define CapTouch_WDGT_CRC_CALC_VALUE                        (CapTouch_dsRam.selfTest.wdgtCrcCalc)
#define CapTouch_WDGT_CRC_CALC_OFFSET                       (424u)
#define CapTouch_WDGT_CRC_CALC_SIZE                         (2u)
#define CapTouch_WDGT_CRC_CALC_PARAM_ID                     (0xAC0001A8u)

#define CapTouch_WDGT_CRC_ID_VALUE                          (CapTouch_dsRam.selfTest.wdgtCrcId)
#define CapTouch_WDGT_CRC_ID_OFFSET                         (426u)
#define CapTouch_WDGT_CRC_ID_SIZE                           (1u)
#define CapTouch_WDGT_CRC_ID_PARAM_ID                       (0x680001AAu)

#define CapTouch_INV_BSLN_WDGT_ID_VALUE                     (CapTouch_dsRam.selfTest.invBslnWdgtId)
#define CapTouch_INV_BSLN_WDGT_ID_OFFSET                    (427u)
#define CapTouch_INV_BSLN_WDGT_ID_SIZE                      (1u)
#define CapTouch_INV_BSLN_WDGT_ID_PARAM_ID                  (0x6E0001ABu)

#define CapTouch_INV_BSLN_SNS_ID_VALUE                      (CapTouch_dsRam.selfTest.invBslnSnsId)
#define CapTouch_INV_BSLN_SNS_ID_OFFSET                     (428u)
#define CapTouch_INV_BSLN_SNS_ID_SIZE                       (1u)
#define CapTouch_INV_BSLN_SNS_ID_PARAM_ID                   (0x650001ACu)

#define CapTouch_SHORTED_WDGT_ID_VALUE                      (CapTouch_dsRam.selfTest.shortedWdgtId)
#define CapTouch_SHORTED_WDGT_ID_OFFSET                     (429u)
#define CapTouch_SHORTED_WDGT_ID_SIZE                       (1u)
#define CapTouch_SHORTED_WDGT_ID_PARAM_ID                   (0x630001ADu)

#define CapTouch_SHORTED_SNS_ID_VALUE                       (CapTouch_dsRam.selfTest.shortedSnsId)
#define CapTouch_SHORTED_SNS_ID_OFFSET                      (430u)
#define CapTouch_SHORTED_SNS_ID_SIZE                        (1u)
#define CapTouch_SHORTED_SNS_ID_PARAM_ID                    (0x690001AEu)

#define CapTouch_P2P_WDGT_ID_VALUE                          (CapTouch_dsRam.selfTest.p2pWdgtId)
#define CapTouch_P2P_WDGT_ID_OFFSET                         (431u)
#define CapTouch_P2P_WDGT_ID_SIZE                           (1u)
#define CapTouch_P2P_WDGT_ID_PARAM_ID                       (0x6F0001AFu)

#define CapTouch_P2P_SNS_ID_VALUE                           (CapTouch_dsRam.selfTest.p2pSnsId)
#define CapTouch_P2P_SNS_ID_OFFSET                          (432u)
#define CapTouch_P2P_SNS_ID_SIZE                            (1u)
#define CapTouch_P2P_SNS_ID_PARAM_ID                        (0x630001B0u)

#define CapTouch_U102_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u102[0u])
#define CapTouch_U102_SNS_CP0_OFFSET                        (436u)
#define CapTouch_U102_SNS_CP0_SIZE                          (1u)
#define CapTouch_U102_SNS_CP0_PARAM_ID                      (0x620001B4u)

#define CapTouch_U102_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u102[1u])
#define CapTouch_U102_SNS_CP1_OFFSET                        (437u)
#define CapTouch_U102_SNS_CP1_SIZE                          (1u)
#define CapTouch_U102_SNS_CP1_PARAM_ID                      (0x640001B5u)

#define CapTouch_U103_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u103[0u])
#define CapTouch_U103_SNS_CP0_OFFSET                        (438u)
#define CapTouch_U103_SNS_CP0_SIZE                          (1u)
#define CapTouch_U103_SNS_CP0_PARAM_ID                      (0x6E0001B6u)

#define CapTouch_U103_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u103[1u])
#define CapTouch_U103_SNS_CP1_OFFSET                        (439u)
#define CapTouch_U103_SNS_CP1_SIZE                          (1u)
#define CapTouch_U103_SNS_CP1_PARAM_ID                      (0x680001B7u)

#define CapTouch_U104_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u104[0u])
#define CapTouch_U104_SNS_CP0_OFFSET                        (440u)
#define CapTouch_U104_SNS_CP0_SIZE                          (1u)
#define CapTouch_U104_SNS_CP0_PARAM_ID                      (0x610001B8u)

#define CapTouch_U104_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u104[1u])
#define CapTouch_U104_SNS_CP1_OFFSET                        (441u)
#define CapTouch_U104_SNS_CP1_SIZE                          (1u)
#define CapTouch_U104_SNS_CP1_PARAM_ID                      (0x670001B9u)

#define CapTouch_U105_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u105[0u])
#define CapTouch_U105_SNS_CP0_OFFSET                        (442u)
#define CapTouch_U105_SNS_CP0_SIZE                          (1u)
#define CapTouch_U105_SNS_CP0_PARAM_ID                      (0x6D0001BAu)

#define CapTouch_U105_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u105[1u])
#define CapTouch_U105_SNS_CP1_OFFSET                        (443u)
#define CapTouch_U105_SNS_CP1_SIZE                          (1u)
#define CapTouch_U105_SNS_CP1_PARAM_ID                      (0x6B0001BBu)

#define CapTouch_U106_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u106[0u])
#define CapTouch_U106_SNS_CP0_OFFSET                        (444u)
#define CapTouch_U106_SNS_CP0_SIZE                          (1u)
#define CapTouch_U106_SNS_CP0_PARAM_ID                      (0x600001BCu)

#define CapTouch_U106_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u106[1u])
#define CapTouch_U106_SNS_CP1_OFFSET                        (445u)
#define CapTouch_U106_SNS_CP1_SIZE                          (1u)
#define CapTouch_U106_SNS_CP1_PARAM_ID                      (0x660001BDu)

#define CapTouch_U107_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u107[0u])
#define CapTouch_U107_SNS_CP0_OFFSET                        (446u)
#define CapTouch_U107_SNS_CP0_SIZE                          (1u)
#define CapTouch_U107_SNS_CP0_PARAM_ID                      (0x6C0001BEu)

#define CapTouch_U107_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u107[1u])
#define CapTouch_U107_SNS_CP1_OFFSET                        (447u)
#define CapTouch_U107_SNS_CP1_SIZE                          (1u)
#define CapTouch_U107_SNS_CP1_PARAM_ID                      (0x6A0001BFu)

#define CapTouch_U108_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u108[0u])
#define CapTouch_U108_SNS_CP0_OFFSET                        (448u)
#define CapTouch_U108_SNS_CP0_SIZE                          (1u)
#define CapTouch_U108_SNS_CP0_PARAM_ID                      (0x610001C0u)

#define CapTouch_U108_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u108[1u])
#define CapTouch_U108_SNS_CP1_OFFSET                        (449u)
#define CapTouch_U108_SNS_CP1_SIZE                          (1u)
#define CapTouch_U108_SNS_CP1_PARAM_ID                      (0x670001C1u)

#define CapTouch_U109_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u109[0u])
#define CapTouch_U109_SNS_CP0_OFFSET                        (450u)
#define CapTouch_U109_SNS_CP0_SIZE                          (1u)
#define CapTouch_U109_SNS_CP0_PARAM_ID                      (0x6D0001C2u)

#define CapTouch_U109_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u109[1u])
#define CapTouch_U109_SNS_CP1_OFFSET                        (451u)
#define CapTouch_U109_SNS_CP1_SIZE                          (1u)
#define CapTouch_U109_SNS_CP1_PARAM_ID                      (0x6B0001C3u)

#define CapTouch_U110_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u110[0u])
#define CapTouch_U110_SNS_CP0_OFFSET                        (452u)
#define CapTouch_U110_SNS_CP0_SIZE                          (1u)
#define CapTouch_U110_SNS_CP0_PARAM_ID                      (0x600001C4u)

#define CapTouch_U110_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u110[1u])
#define CapTouch_U110_SNS_CP1_OFFSET                        (453u)
#define CapTouch_U110_SNS_CP1_SIZE                          (1u)
#define CapTouch_U110_SNS_CP1_PARAM_ID                      (0x660001C5u)

#define CapTouch_U111_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u111[0u])
#define CapTouch_U111_SNS_CP0_OFFSET                        (454u)
#define CapTouch_U111_SNS_CP0_SIZE                          (1u)
#define CapTouch_U111_SNS_CP0_PARAM_ID                      (0x6C0001C6u)

#define CapTouch_U111_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u111[1u])
#define CapTouch_U111_SNS_CP1_OFFSET                        (455u)
#define CapTouch_U111_SNS_CP1_SIZE                          (1u)
#define CapTouch_U111_SNS_CP1_PARAM_ID                      (0x6A0001C7u)

#define CapTouch_U112_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u112[0u])
#define CapTouch_U112_SNS_CP0_OFFSET                        (456u)
#define CapTouch_U112_SNS_CP0_SIZE                          (1u)
#define CapTouch_U112_SNS_CP0_PARAM_ID                      (0x630001C8u)

#define CapTouch_U112_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u112[1u])
#define CapTouch_U112_SNS_CP1_OFFSET                        (457u)
#define CapTouch_U112_SNS_CP1_SIZE                          (1u)
#define CapTouch_U112_SNS_CP1_PARAM_ID                      (0x650001C9u)

#define CapTouch_U113_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u113[0u])
#define CapTouch_U113_SNS_CP0_OFFSET                        (458u)
#define CapTouch_U113_SNS_CP0_SIZE                          (1u)
#define CapTouch_U113_SNS_CP0_PARAM_ID                      (0x6F0001CAu)

#define CapTouch_U113_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u113[1u])
#define CapTouch_U113_SNS_CP1_OFFSET                        (459u)
#define CapTouch_U113_SNS_CP1_SIZE                          (1u)
#define CapTouch_U113_SNS_CP1_PARAM_ID                      (0x690001CBu)

#define CapTouch_U114_SNS_CP0_VALUE                         (CapTouch_dsRam.snsCp.u114[0u])
#define CapTouch_U114_SNS_CP0_OFFSET                        (460u)
#define CapTouch_U114_SNS_CP0_SIZE                          (1u)
#define CapTouch_U114_SNS_CP0_PARAM_ID                      (0x620001CCu)

#define CapTouch_U114_SNS_CP1_VALUE                         (CapTouch_dsRam.snsCp.u114[1u])
#define CapTouch_U114_SNS_CP1_OFFSET                        (461u)
#define CapTouch_U114_SNS_CP1_SIZE                          (1u)
#define CapTouch_U114_SNS_CP1_PARAM_ID                      (0x640001CDu)

#define CapTouch_SNR_TEST_WIDGET_ID_VALUE                   (CapTouch_dsRam.snrTestWidgetId)
#define CapTouch_SNR_TEST_WIDGET_ID_OFFSET                  (462u)
#define CapTouch_SNR_TEST_WIDGET_ID_SIZE                    (1u)
#define CapTouch_SNR_TEST_WIDGET_ID_PARAM_ID                (0x6E0001CEu)

#define CapTouch_SNR_TEST_SENSOR_ID_VALUE                   (CapTouch_dsRam.snrTestSensorId)
#define CapTouch_SNR_TEST_SENSOR_ID_OFFSET                  (463u)
#define CapTouch_SNR_TEST_SENSOR_ID_SIZE                    (1u)
#define CapTouch_SNR_TEST_SENSOR_ID_PARAM_ID                (0x680001CFu)

#define CapTouch_SNR_TEST_SCAN_COUNTER_VALUE                (CapTouch_dsRam.snrTestScanCounter)
#define CapTouch_SNR_TEST_SCAN_COUNTER_OFFSET               (464u)
#define CapTouch_SNR_TEST_SCAN_COUNTER_SIZE                 (2u)
#define CapTouch_SNR_TEST_SCAN_COUNTER_PARAM_ID             (0x870001D0u)

#define CapTouch_SNR_TEST_RAW_COUNT0_VALUE                  (CapTouch_dsRam.snrTestRawCount[0u])
#define CapTouch_SNR_TEST_RAW_COUNT0_OFFSET                 (466u)
#define CapTouch_SNR_TEST_RAW_COUNT0_SIZE                   (2u)
#define CapTouch_SNR_TEST_RAW_COUNT0_PARAM_ID               (0x8B0001D2u)


/*****************************************************************************/
/* Flash Data structure register definitions                                 */
/*****************************************************************************/
#define CapTouch_U102_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[0].ptr2SnsFlash)
#define CapTouch_U102_PTR2SNS_FLASH_OFFSET                  (0u)
#define CapTouch_U102_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U102_PTR2SNS_FLASH_PARAM_ID                (0xD1000000u)

#define CapTouch_U102_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[0].ptr2WdgtRam)
#define CapTouch_U102_PTR2WD_RAM_OFFSET                     (4u)
#define CapTouch_U102_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U102_PTR2WD_RAM_PARAM_ID                   (0xD0000004u)

#define CapTouch_U102_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[0].ptr2SnsRam)
#define CapTouch_U102_PTR2SNS_RAM_OFFSET                    (8u)
#define CapTouch_U102_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U102_PTR2SNS_RAM_PARAM_ID                  (0xD3000008u)

#define CapTouch_U102_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[0].ptr2FltrHistory)
#define CapTouch_U102_PTR2FLTR_HISTORY_OFFSET               (12u)
#define CapTouch_U102_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U102_PTR2FLTR_HISTORY_PARAM_ID             (0xD200000Cu)

#define CapTouch_U102_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[0].ptr2DebounceArr)
#define CapTouch_U102_PTR2DEBOUNCE_OFFSET                   (16u)
#define CapTouch_U102_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U102_PTR2DEBOUNCE_PARAM_ID                 (0xD4000010u)

#define CapTouch_U102_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[0].staticConfig)
#define CapTouch_U102_STATIC_CONFIG_OFFSET                  (20u)
#define CapTouch_U102_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U102_STATIC_CONFIG_PARAM_ID                (0xD5000014u)

#define CapTouch_U102_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[0].totalNumSns)
#define CapTouch_U102_TOTAL_NUM_SNS_OFFSET                  (24u)
#define CapTouch_U102_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U102_TOTAL_NUM_SNS_PARAM_ID                (0x99000018u)

#define CapTouch_U102_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[0].wdgtType)
#define CapTouch_U102_TYPE_OFFSET                           (26u)
#define CapTouch_U102_TYPE_SIZE                             (1u)
#define CapTouch_U102_TYPE_PARAM_ID                         (0x5D00001Au)

#define CapTouch_U102_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[0].numCols)
#define CapTouch_U102_NUM_COLS_OFFSET                       (27u)
#define CapTouch_U102_NUM_COLS_SIZE                         (1u)
#define CapTouch_U102_NUM_COLS_PARAM_ID                     (0x5B00001Bu)

#define CapTouch_U102_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[0].numRows)
#define CapTouch_U102_NUM_ROWS_OFFSET                       (28u)
#define CapTouch_U102_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U102_NUM_ROWS_PARAM_ID                     (0x5000001Cu)

#define CapTouch_U102_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[0].ptr2SnsCpArr)
#define CapTouch_U102_PTR2SNS_CP_OFFSET                     (32u)
#define CapTouch_U102_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U102_PTR2SNS_CP_PARAM_ID                   (0xDB000020u)

#define CapTouch_U103_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[1].ptr2SnsFlash)
#define CapTouch_U103_PTR2SNS_FLASH_OFFSET                  (36u)
#define CapTouch_U103_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U103_PTR2SNS_FLASH_PARAM_ID                (0xD9010024u)

#define CapTouch_U103_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[1].ptr2WdgtRam)
#define CapTouch_U103_PTR2WD_RAM_OFFSET                     (40u)
#define CapTouch_U103_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U103_PTR2WD_RAM_PARAM_ID                   (0xDA010028u)

#define CapTouch_U103_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[1].ptr2SnsRam)
#define CapTouch_U103_PTR2SNS_RAM_OFFSET                    (44u)
#define CapTouch_U103_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U103_PTR2SNS_RAM_PARAM_ID                  (0xDB01002Cu)

#define CapTouch_U103_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[1].ptr2FltrHistory)
#define CapTouch_U103_PTR2FLTR_HISTORY_OFFSET               (48u)
#define CapTouch_U103_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U103_PTR2FLTR_HISTORY_PARAM_ID             (0xDD010030u)

#define CapTouch_U103_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[1].ptr2DebounceArr)
#define CapTouch_U103_PTR2DEBOUNCE_OFFSET                   (52u)
#define CapTouch_U103_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U103_PTR2DEBOUNCE_PARAM_ID                 (0xDC010034u)

#define CapTouch_U103_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[1].staticConfig)
#define CapTouch_U103_STATIC_CONFIG_OFFSET                  (56u)
#define CapTouch_U103_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U103_STATIC_CONFIG_PARAM_ID                (0xDF010038u)

#define CapTouch_U103_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[1].totalNumSns)
#define CapTouch_U103_TOTAL_NUM_SNS_OFFSET                  (60u)
#define CapTouch_U103_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U103_TOTAL_NUM_SNS_PARAM_ID                (0x9101003Cu)

#define CapTouch_U103_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[1].wdgtType)
#define CapTouch_U103_TYPE_OFFSET                           (62u)
#define CapTouch_U103_TYPE_SIZE                             (1u)
#define CapTouch_U103_TYPE_PARAM_ID                         (0x5501003Eu)

#define CapTouch_U103_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[1].numCols)
#define CapTouch_U103_NUM_COLS_OFFSET                       (63u)
#define CapTouch_U103_NUM_COLS_SIZE                         (1u)
#define CapTouch_U103_NUM_COLS_PARAM_ID                     (0x5301003Fu)

#define CapTouch_U103_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[1].numRows)
#define CapTouch_U103_NUM_ROWS_OFFSET                       (64u)
#define CapTouch_U103_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U103_NUM_ROWS_PARAM_ID                     (0x58010040u)

#define CapTouch_U103_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[1].ptr2SnsCpArr)
#define CapTouch_U103_PTR2SNS_CP_OFFSET                     (68u)
#define CapTouch_U103_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U103_PTR2SNS_CP_PARAM_ID                   (0xDE010044u)

#define CapTouch_U104_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[2].ptr2SnsFlash)
#define CapTouch_U104_PTR2SNS_FLASH_OFFSET                  (72u)
#define CapTouch_U104_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U104_PTR2SNS_FLASH_PARAM_ID                (0xD8020048u)

#define CapTouch_U104_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[2].ptr2WdgtRam)
#define CapTouch_U104_PTR2WD_RAM_OFFSET                     (76u)
#define CapTouch_U104_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U104_PTR2WD_RAM_PARAM_ID                   (0xD902004Cu)

#define CapTouch_U104_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[2].ptr2SnsRam)
#define CapTouch_U104_PTR2SNS_RAM_OFFSET                    (80u)
#define CapTouch_U104_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U104_PTR2SNS_RAM_PARAM_ID                  (0xDF020050u)

#define CapTouch_U104_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[2].ptr2FltrHistory)
#define CapTouch_U104_PTR2FLTR_HISTORY_OFFSET               (84u)
#define CapTouch_U104_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U104_PTR2FLTR_HISTORY_PARAM_ID             (0xDE020054u)

#define CapTouch_U104_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[2].ptr2DebounceArr)
#define CapTouch_U104_PTR2DEBOUNCE_OFFSET                   (88u)
#define CapTouch_U104_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U104_PTR2DEBOUNCE_PARAM_ID                 (0xDD020058u)

#define CapTouch_U104_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[2].staticConfig)
#define CapTouch_U104_STATIC_CONFIG_OFFSET                  (92u)
#define CapTouch_U104_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U104_STATIC_CONFIG_PARAM_ID                (0xDC02005Cu)

#define CapTouch_U104_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[2].totalNumSns)
#define CapTouch_U104_TOTAL_NUM_SNS_OFFSET                  (96u)
#define CapTouch_U104_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U104_TOTAL_NUM_SNS_PARAM_ID                (0x9F020060u)

#define CapTouch_U104_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[2].wdgtType)
#define CapTouch_U104_TYPE_OFFSET                           (98u)
#define CapTouch_U104_TYPE_SIZE                             (1u)
#define CapTouch_U104_TYPE_PARAM_ID                         (0x5B020062u)

#define CapTouch_U104_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[2].numCols)
#define CapTouch_U104_NUM_COLS_OFFSET                       (99u)
#define CapTouch_U104_NUM_COLS_SIZE                         (1u)
#define CapTouch_U104_NUM_COLS_PARAM_ID                     (0x5D020063u)

#define CapTouch_U104_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[2].numRows)
#define CapTouch_U104_NUM_ROWS_OFFSET                       (100u)
#define CapTouch_U104_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U104_NUM_ROWS_PARAM_ID                     (0x56020064u)

#define CapTouch_U104_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[2].ptr2SnsCpArr)
#define CapTouch_U104_PTR2SNS_CP_OFFSET                     (104u)
#define CapTouch_U104_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U104_PTR2SNS_CP_PARAM_ID                   (0xD2020068u)

#define CapTouch_U105_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[3].ptr2SnsFlash)
#define CapTouch_U105_PTR2SNS_FLASH_OFFSET                  (108u)
#define CapTouch_U105_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U105_PTR2SNS_FLASH_PARAM_ID                (0xD003006Cu)

#define CapTouch_U105_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[3].ptr2WdgtRam)
#define CapTouch_U105_PTR2WD_RAM_OFFSET                     (112u)
#define CapTouch_U105_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U105_PTR2WD_RAM_PARAM_ID                   (0xD6030070u)

#define CapTouch_U105_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[3].ptr2SnsRam)
#define CapTouch_U105_PTR2SNS_RAM_OFFSET                    (116u)
#define CapTouch_U105_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U105_PTR2SNS_RAM_PARAM_ID                  (0xD7030074u)

#define CapTouch_U105_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[3].ptr2FltrHistory)
#define CapTouch_U105_PTR2FLTR_HISTORY_OFFSET               (120u)
#define CapTouch_U105_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U105_PTR2FLTR_HISTORY_PARAM_ID             (0xD4030078u)

#define CapTouch_U105_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[3].ptr2DebounceArr)
#define CapTouch_U105_PTR2DEBOUNCE_OFFSET                   (124u)
#define CapTouch_U105_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U105_PTR2DEBOUNCE_PARAM_ID                 (0xD503007Cu)

#define CapTouch_U105_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[3].staticConfig)
#define CapTouch_U105_STATIC_CONFIG_OFFSET                  (128u)
#define CapTouch_U105_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U105_STATIC_CONFIG_PARAM_ID                (0xD7030080u)

#define CapTouch_U105_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[3].totalNumSns)
#define CapTouch_U105_TOTAL_NUM_SNS_OFFSET                  (132u)
#define CapTouch_U105_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U105_TOTAL_NUM_SNS_PARAM_ID                (0x99030084u)

#define CapTouch_U105_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[3].wdgtType)
#define CapTouch_U105_TYPE_OFFSET                           (134u)
#define CapTouch_U105_TYPE_SIZE                             (1u)
#define CapTouch_U105_TYPE_PARAM_ID                         (0x5D030086u)

#define CapTouch_U105_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[3].numCols)
#define CapTouch_U105_NUM_COLS_OFFSET                       (135u)
#define CapTouch_U105_NUM_COLS_SIZE                         (1u)
#define CapTouch_U105_NUM_COLS_PARAM_ID                     (0x5B030087u)

#define CapTouch_U105_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[3].numRows)
#define CapTouch_U105_NUM_ROWS_OFFSET                       (136u)
#define CapTouch_U105_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U105_NUM_ROWS_PARAM_ID                     (0x52030088u)

#define CapTouch_U105_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[3].ptr2SnsCpArr)
#define CapTouch_U105_PTR2SNS_CP_OFFSET                     (140u)
#define CapTouch_U105_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U105_PTR2SNS_CP_PARAM_ID                   (0xD403008Cu)

#define CapTouch_U106_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[4].ptr2SnsFlash)
#define CapTouch_U106_PTR2SNS_FLASH_OFFSET                  (144u)
#define CapTouch_U106_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U106_PTR2SNS_FLASH_PARAM_ID                (0xDB040090u)

#define CapTouch_U106_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[4].ptr2WdgtRam)
#define CapTouch_U106_PTR2WD_RAM_OFFSET                     (148u)
#define CapTouch_U106_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U106_PTR2WD_RAM_PARAM_ID                   (0xDA040094u)

#define CapTouch_U106_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[4].ptr2SnsRam)
#define CapTouch_U106_PTR2SNS_RAM_OFFSET                    (152u)
#define CapTouch_U106_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U106_PTR2SNS_RAM_PARAM_ID                  (0xD9040098u)

#define CapTouch_U106_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[4].ptr2FltrHistory)
#define CapTouch_U106_PTR2FLTR_HISTORY_OFFSET               (156u)
#define CapTouch_U106_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U106_PTR2FLTR_HISTORY_PARAM_ID             (0xD804009Cu)

#define CapTouch_U106_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[4].ptr2DebounceArr)
#define CapTouch_U106_PTR2DEBOUNCE_OFFSET                   (160u)
#define CapTouch_U106_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U106_PTR2DEBOUNCE_PARAM_ID                 (0xD40400A0u)

#define CapTouch_U106_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[4].staticConfig)
#define CapTouch_U106_STATIC_CONFIG_OFFSET                  (164u)
#define CapTouch_U106_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U106_STATIC_CONFIG_PARAM_ID                (0xD50400A4u)

#define CapTouch_U106_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[4].totalNumSns)
#define CapTouch_U106_TOTAL_NUM_SNS_OFFSET                  (168u)
#define CapTouch_U106_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U106_TOTAL_NUM_SNS_PARAM_ID                (0x990400A8u)

#define CapTouch_U106_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[4].wdgtType)
#define CapTouch_U106_TYPE_OFFSET                           (170u)
#define CapTouch_U106_TYPE_SIZE                             (1u)
#define CapTouch_U106_TYPE_PARAM_ID                         (0x5D0400AAu)

#define CapTouch_U106_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[4].numCols)
#define CapTouch_U106_NUM_COLS_OFFSET                       (171u)
#define CapTouch_U106_NUM_COLS_SIZE                         (1u)
#define CapTouch_U106_NUM_COLS_PARAM_ID                     (0x5B0400ABu)

#define CapTouch_U106_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[4].numRows)
#define CapTouch_U106_NUM_ROWS_OFFSET                       (172u)
#define CapTouch_U106_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U106_NUM_ROWS_PARAM_ID                     (0x500400ACu)

#define CapTouch_U106_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[4].ptr2SnsCpArr)
#define CapTouch_U106_PTR2SNS_CP_OFFSET                     (176u)
#define CapTouch_U106_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U106_PTR2SNS_CP_PARAM_ID                   (0xD10400B0u)

#define CapTouch_U107_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[5].ptr2SnsFlash)
#define CapTouch_U107_PTR2SNS_FLASH_OFFSET                  (180u)
#define CapTouch_U107_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U107_PTR2SNS_FLASH_PARAM_ID                (0xD30500B4u)

#define CapTouch_U107_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[5].ptr2WdgtRam)
#define CapTouch_U107_PTR2WD_RAM_OFFSET                     (184u)
#define CapTouch_U107_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U107_PTR2WD_RAM_PARAM_ID                   (0xD00500B8u)

#define CapTouch_U107_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[5].ptr2SnsRam)
#define CapTouch_U107_PTR2SNS_RAM_OFFSET                    (188u)
#define CapTouch_U107_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U107_PTR2SNS_RAM_PARAM_ID                  (0xD10500BCu)

#define CapTouch_U107_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[5].ptr2FltrHistory)
#define CapTouch_U107_PTR2FLTR_HISTORY_OFFSET               (192u)
#define CapTouch_U107_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U107_PTR2FLTR_HISTORY_PARAM_ID             (0xD00500C0u)

#define CapTouch_U107_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[5].ptr2DebounceArr)
#define CapTouch_U107_PTR2DEBOUNCE_OFFSET                   (196u)
#define CapTouch_U107_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U107_PTR2DEBOUNCE_PARAM_ID                 (0xD10500C4u)

#define CapTouch_U107_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[5].staticConfig)
#define CapTouch_U107_STATIC_CONFIG_OFFSET                  (200u)
#define CapTouch_U107_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U107_STATIC_CONFIG_PARAM_ID                (0xD20500C8u)

#define CapTouch_U107_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[5].totalNumSns)
#define CapTouch_U107_TOTAL_NUM_SNS_OFFSET                  (204u)
#define CapTouch_U107_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U107_TOTAL_NUM_SNS_PARAM_ID                (0x9C0500CCu)

#define CapTouch_U107_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[5].wdgtType)
#define CapTouch_U107_TYPE_OFFSET                           (206u)
#define CapTouch_U107_TYPE_SIZE                             (1u)
#define CapTouch_U107_TYPE_PARAM_ID                         (0x580500CEu)

#define CapTouch_U107_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[5].numCols)
#define CapTouch_U107_NUM_COLS_OFFSET                       (207u)
#define CapTouch_U107_NUM_COLS_SIZE                         (1u)
#define CapTouch_U107_NUM_COLS_PARAM_ID                     (0x5E0500CFu)

#define CapTouch_U107_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[5].numRows)
#define CapTouch_U107_NUM_ROWS_OFFSET                       (208u)
#define CapTouch_U107_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U107_NUM_ROWS_PARAM_ID                     (0x520500D0u)

#define CapTouch_U107_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[5].ptr2SnsCpArr)
#define CapTouch_U107_PTR2SNS_CP_OFFSET                     (212u)
#define CapTouch_U107_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U107_PTR2SNS_CP_PARAM_ID                   (0xD40500D4u)

#define CapTouch_U108_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[6].ptr2SnsFlash)
#define CapTouch_U108_PTR2SNS_FLASH_OFFSET                  (216u)
#define CapTouch_U108_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U108_PTR2SNS_FLASH_PARAM_ID                (0xD20600D8u)

#define CapTouch_U108_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[6].ptr2WdgtRam)
#define CapTouch_U108_PTR2WD_RAM_OFFSET                     (220u)
#define CapTouch_U108_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U108_PTR2WD_RAM_PARAM_ID                   (0xD30600DCu)

#define CapTouch_U108_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[6].ptr2SnsRam)
#define CapTouch_U108_PTR2SNS_RAM_OFFSET                    (224u)
#define CapTouch_U108_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U108_PTR2SNS_RAM_PARAM_ID                  (0xDF0600E0u)

#define CapTouch_U108_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[6].ptr2FltrHistory)
#define CapTouch_U108_PTR2FLTR_HISTORY_OFFSET               (228u)
#define CapTouch_U108_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U108_PTR2FLTR_HISTORY_PARAM_ID             (0xDE0600E4u)

#define CapTouch_U108_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[6].ptr2DebounceArr)
#define CapTouch_U108_PTR2DEBOUNCE_OFFSET                   (232u)
#define CapTouch_U108_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U108_PTR2DEBOUNCE_PARAM_ID                 (0xDD0600E8u)

#define CapTouch_U108_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[6].staticConfig)
#define CapTouch_U108_STATIC_CONFIG_OFFSET                  (236u)
#define CapTouch_U108_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U108_STATIC_CONFIG_PARAM_ID                (0xDC0600ECu)

#define CapTouch_U108_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[6].totalNumSns)
#define CapTouch_U108_TOTAL_NUM_SNS_OFFSET                  (240u)
#define CapTouch_U108_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U108_TOTAL_NUM_SNS_PARAM_ID                (0x950600F0u)

#define CapTouch_U108_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[6].wdgtType)
#define CapTouch_U108_TYPE_OFFSET                           (242u)
#define CapTouch_U108_TYPE_SIZE                             (1u)
#define CapTouch_U108_TYPE_PARAM_ID                         (0x510600F2u)

#define CapTouch_U108_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[6].numCols)
#define CapTouch_U108_NUM_COLS_OFFSET                       (243u)
#define CapTouch_U108_NUM_COLS_SIZE                         (1u)
#define CapTouch_U108_NUM_COLS_PARAM_ID                     (0x570600F3u)

#define CapTouch_U108_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[6].numRows)
#define CapTouch_U108_NUM_ROWS_OFFSET                       (244u)
#define CapTouch_U108_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U108_NUM_ROWS_PARAM_ID                     (0x5C0600F4u)

#define CapTouch_U108_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[6].ptr2SnsCpArr)
#define CapTouch_U108_PTR2SNS_CP_OFFSET                     (248u)
#define CapTouch_U108_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U108_PTR2SNS_CP_PARAM_ID                   (0xD80600F8u)

#define CapTouch_U109_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[7].ptr2SnsFlash)
#define CapTouch_U109_PTR2SNS_FLASH_OFFSET                  (252u)
#define CapTouch_U109_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U109_PTR2SNS_FLASH_PARAM_ID                (0xDA0700FCu)

#define CapTouch_U109_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[7].ptr2WdgtRam)
#define CapTouch_U109_PTR2WD_RAM_OFFSET                     (256u)
#define CapTouch_U109_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U109_PTR2WD_RAM_PARAM_ID                   (0xD3070100u)

#define CapTouch_U109_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[7].ptr2SnsRam)
#define CapTouch_U109_PTR2SNS_RAM_OFFSET                    (260u)
#define CapTouch_U109_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U109_PTR2SNS_RAM_PARAM_ID                  (0xD2070104u)

#define CapTouch_U109_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[7].ptr2FltrHistory)
#define CapTouch_U109_PTR2FLTR_HISTORY_OFFSET               (264u)
#define CapTouch_U109_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U109_PTR2FLTR_HISTORY_PARAM_ID             (0xD1070108u)

#define CapTouch_U109_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[7].ptr2DebounceArr)
#define CapTouch_U109_PTR2DEBOUNCE_OFFSET                   (268u)
#define CapTouch_U109_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U109_PTR2DEBOUNCE_PARAM_ID                 (0xD007010Cu)

#define CapTouch_U109_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[7].staticConfig)
#define CapTouch_U109_STATIC_CONFIG_OFFSET                  (272u)
#define CapTouch_U109_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U109_STATIC_CONFIG_PARAM_ID                (0xD6070110u)

#define CapTouch_U109_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[7].totalNumSns)
#define CapTouch_U109_TOTAL_NUM_SNS_OFFSET                  (276u)
#define CapTouch_U109_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U109_TOTAL_NUM_SNS_PARAM_ID                (0x98070114u)

#define CapTouch_U109_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[7].wdgtType)
#define CapTouch_U109_TYPE_OFFSET                           (278u)
#define CapTouch_U109_TYPE_SIZE                             (1u)
#define CapTouch_U109_TYPE_PARAM_ID                         (0x5C070116u)

#define CapTouch_U109_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[7].numCols)
#define CapTouch_U109_NUM_COLS_OFFSET                       (279u)
#define CapTouch_U109_NUM_COLS_SIZE                         (1u)
#define CapTouch_U109_NUM_COLS_PARAM_ID                     (0x5A070117u)

#define CapTouch_U109_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[7].numRows)
#define CapTouch_U109_NUM_ROWS_OFFSET                       (280u)
#define CapTouch_U109_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U109_NUM_ROWS_PARAM_ID                     (0x53070118u)

#define CapTouch_U109_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[7].ptr2SnsCpArr)
#define CapTouch_U109_PTR2SNS_CP_OFFSET                     (284u)
#define CapTouch_U109_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U109_PTR2SNS_CP_PARAM_ID                   (0xD507011Cu)

#define CapTouch_U110_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[8].ptr2SnsFlash)
#define CapTouch_U110_PTR2SNS_FLASH_OFFSET                  (288u)
#define CapTouch_U110_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U110_PTR2SNS_FLASH_PARAM_ID                (0xD1080120u)

#define CapTouch_U110_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[8].ptr2WdgtRam)
#define CapTouch_U110_PTR2WD_RAM_OFFSET                     (292u)
#define CapTouch_U110_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U110_PTR2WD_RAM_PARAM_ID                   (0xD0080124u)

#define CapTouch_U110_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[8].ptr2SnsRam)
#define CapTouch_U110_PTR2SNS_RAM_OFFSET                    (296u)
#define CapTouch_U110_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U110_PTR2SNS_RAM_PARAM_ID                  (0xD3080128u)

#define CapTouch_U110_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[8].ptr2FltrHistory)
#define CapTouch_U110_PTR2FLTR_HISTORY_OFFSET               (300u)
#define CapTouch_U110_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U110_PTR2FLTR_HISTORY_PARAM_ID             (0xD208012Cu)

#define CapTouch_U110_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[8].ptr2DebounceArr)
#define CapTouch_U110_PTR2DEBOUNCE_OFFSET                   (304u)
#define CapTouch_U110_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U110_PTR2DEBOUNCE_PARAM_ID                 (0xD4080130u)

#define CapTouch_U110_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[8].staticConfig)
#define CapTouch_U110_STATIC_CONFIG_OFFSET                  (308u)
#define CapTouch_U110_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U110_STATIC_CONFIG_PARAM_ID                (0xD5080134u)

#define CapTouch_U110_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[8].totalNumSns)
#define CapTouch_U110_TOTAL_NUM_SNS_OFFSET                  (312u)
#define CapTouch_U110_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U110_TOTAL_NUM_SNS_PARAM_ID                (0x99080138u)

#define CapTouch_U110_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[8].wdgtType)
#define CapTouch_U110_TYPE_OFFSET                           (314u)
#define CapTouch_U110_TYPE_SIZE                             (1u)
#define CapTouch_U110_TYPE_PARAM_ID                         (0x5D08013Au)

#define CapTouch_U110_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[8].numCols)
#define CapTouch_U110_NUM_COLS_OFFSET                       (315u)
#define CapTouch_U110_NUM_COLS_SIZE                         (1u)
#define CapTouch_U110_NUM_COLS_PARAM_ID                     (0x5B08013Bu)

#define CapTouch_U110_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[8].numRows)
#define CapTouch_U110_NUM_ROWS_OFFSET                       (316u)
#define CapTouch_U110_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U110_NUM_ROWS_PARAM_ID                     (0x5008013Cu)

#define CapTouch_U110_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[8].ptr2SnsCpArr)
#define CapTouch_U110_PTR2SNS_CP_OFFSET                     (320u)
#define CapTouch_U110_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U110_PTR2SNS_CP_PARAM_ID                   (0xD6080140u)

#define CapTouch_U111_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[9].ptr2SnsFlash)
#define CapTouch_U111_PTR2SNS_FLASH_OFFSET                  (324u)
#define CapTouch_U111_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U111_PTR2SNS_FLASH_PARAM_ID                (0xD4090144u)

#define CapTouch_U111_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[9].ptr2WdgtRam)
#define CapTouch_U111_PTR2WD_RAM_OFFSET                     (328u)
#define CapTouch_U111_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U111_PTR2WD_RAM_PARAM_ID                   (0xD7090148u)

#define CapTouch_U111_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[9].ptr2SnsRam)
#define CapTouch_U111_PTR2SNS_RAM_OFFSET                    (332u)
#define CapTouch_U111_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U111_PTR2SNS_RAM_PARAM_ID                  (0xD609014Cu)

#define CapTouch_U111_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[9].ptr2FltrHistory)
#define CapTouch_U111_PTR2FLTR_HISTORY_OFFSET               (336u)
#define CapTouch_U111_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U111_PTR2FLTR_HISTORY_PARAM_ID             (0xD0090150u)

#define CapTouch_U111_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[9].ptr2DebounceArr)
#define CapTouch_U111_PTR2DEBOUNCE_OFFSET                   (340u)
#define CapTouch_U111_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U111_PTR2DEBOUNCE_PARAM_ID                 (0xD1090154u)

#define CapTouch_U111_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[9].staticConfig)
#define CapTouch_U111_STATIC_CONFIG_OFFSET                  (344u)
#define CapTouch_U111_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U111_STATIC_CONFIG_PARAM_ID                (0xD2090158u)

#define CapTouch_U111_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[9].totalNumSns)
#define CapTouch_U111_TOTAL_NUM_SNS_OFFSET                  (348u)
#define CapTouch_U111_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U111_TOTAL_NUM_SNS_PARAM_ID                (0x9C09015Cu)

#define CapTouch_U111_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[9].wdgtType)
#define CapTouch_U111_TYPE_OFFSET                           (350u)
#define CapTouch_U111_TYPE_SIZE                             (1u)
#define CapTouch_U111_TYPE_PARAM_ID                         (0x5809015Eu)

#define CapTouch_U111_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[9].numCols)
#define CapTouch_U111_NUM_COLS_OFFSET                       (351u)
#define CapTouch_U111_NUM_COLS_SIZE                         (1u)
#define CapTouch_U111_NUM_COLS_PARAM_ID                     (0x5E09015Fu)

#define CapTouch_U111_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[9].numRows)
#define CapTouch_U111_NUM_ROWS_OFFSET                       (352u)
#define CapTouch_U111_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U111_NUM_ROWS_PARAM_ID                     (0x58090160u)

#define CapTouch_U111_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[9].ptr2SnsCpArr)
#define CapTouch_U111_PTR2SNS_CP_OFFSET                     (356u)
#define CapTouch_U111_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U111_PTR2SNS_CP_PARAM_ID                   (0xDE090164u)

#define CapTouch_U112_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[10].ptr2SnsFlash)
#define CapTouch_U112_PTR2SNS_FLASH_OFFSET                  (360u)
#define CapTouch_U112_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U112_PTR2SNS_FLASH_PARAM_ID                (0xD80A0168u)

#define CapTouch_U112_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[10].ptr2WdgtRam)
#define CapTouch_U112_PTR2WD_RAM_OFFSET                     (364u)
#define CapTouch_U112_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U112_PTR2WD_RAM_PARAM_ID                   (0xD90A016Cu)

#define CapTouch_U112_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[10].ptr2SnsRam)
#define CapTouch_U112_PTR2SNS_RAM_OFFSET                    (368u)
#define CapTouch_U112_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U112_PTR2SNS_RAM_PARAM_ID                  (0xDF0A0170u)

#define CapTouch_U112_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[10].ptr2FltrHistory)
#define CapTouch_U112_PTR2FLTR_HISTORY_OFFSET               (372u)
#define CapTouch_U112_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U112_PTR2FLTR_HISTORY_PARAM_ID             (0xDE0A0174u)

#define CapTouch_U112_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[10].ptr2DebounceArr)
#define CapTouch_U112_PTR2DEBOUNCE_OFFSET                   (376u)
#define CapTouch_U112_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U112_PTR2DEBOUNCE_PARAM_ID                 (0xDD0A0178u)

#define CapTouch_U112_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[10].staticConfig)
#define CapTouch_U112_STATIC_CONFIG_OFFSET                  (380u)
#define CapTouch_U112_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U112_STATIC_CONFIG_PARAM_ID                (0xDC0A017Cu)

#define CapTouch_U112_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[10].totalNumSns)
#define CapTouch_U112_TOTAL_NUM_SNS_OFFSET                  (384u)
#define CapTouch_U112_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U112_TOTAL_NUM_SNS_PARAM_ID                (0x910A0180u)

#define CapTouch_U112_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[10].wdgtType)
#define CapTouch_U112_TYPE_OFFSET                           (386u)
#define CapTouch_U112_TYPE_SIZE                             (1u)
#define CapTouch_U112_TYPE_PARAM_ID                         (0x550A0182u)

#define CapTouch_U112_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[10].numCols)
#define CapTouch_U112_NUM_COLS_OFFSET                       (387u)
#define CapTouch_U112_NUM_COLS_SIZE                         (1u)
#define CapTouch_U112_NUM_COLS_PARAM_ID                     (0x530A0183u)

#define CapTouch_U112_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[10].numRows)
#define CapTouch_U112_NUM_ROWS_OFFSET                       (388u)
#define CapTouch_U112_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U112_NUM_ROWS_PARAM_ID                     (0x580A0184u)

#define CapTouch_U112_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[10].ptr2SnsCpArr)
#define CapTouch_U112_PTR2SNS_CP_OFFSET                     (392u)
#define CapTouch_U112_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U112_PTR2SNS_CP_PARAM_ID                   (0xDC0A0188u)

#define CapTouch_U113_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[11].ptr2SnsFlash)
#define CapTouch_U113_PTR2SNS_FLASH_OFFSET                  (396u)
#define CapTouch_U113_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U113_PTR2SNS_FLASH_PARAM_ID                (0xDE0B018Cu)

#define CapTouch_U113_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[11].ptr2WdgtRam)
#define CapTouch_U113_PTR2WD_RAM_OFFSET                     (400u)
#define CapTouch_U113_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U113_PTR2WD_RAM_PARAM_ID                   (0xD80B0190u)

#define CapTouch_U113_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[11].ptr2SnsRam)
#define CapTouch_U113_PTR2SNS_RAM_OFFSET                    (404u)
#define CapTouch_U113_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U113_PTR2SNS_RAM_PARAM_ID                  (0xD90B0194u)

#define CapTouch_U113_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[11].ptr2FltrHistory)
#define CapTouch_U113_PTR2FLTR_HISTORY_OFFSET               (408u)
#define CapTouch_U113_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U113_PTR2FLTR_HISTORY_PARAM_ID             (0xDA0B0198u)

#define CapTouch_U113_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[11].ptr2DebounceArr)
#define CapTouch_U113_PTR2DEBOUNCE_OFFSET                   (412u)
#define CapTouch_U113_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U113_PTR2DEBOUNCE_PARAM_ID                 (0xDB0B019Cu)

#define CapTouch_U113_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[11].staticConfig)
#define CapTouch_U113_STATIC_CONFIG_OFFSET                  (416u)
#define CapTouch_U113_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U113_STATIC_CONFIG_PARAM_ID                (0xD70B01A0u)

#define CapTouch_U113_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[11].totalNumSns)
#define CapTouch_U113_TOTAL_NUM_SNS_OFFSET                  (420u)
#define CapTouch_U113_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U113_TOTAL_NUM_SNS_PARAM_ID                (0x990B01A4u)

#define CapTouch_U113_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[11].wdgtType)
#define CapTouch_U113_TYPE_OFFSET                           (422u)
#define CapTouch_U113_TYPE_SIZE                             (1u)
#define CapTouch_U113_TYPE_PARAM_ID                         (0x5D0B01A6u)

#define CapTouch_U113_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[11].numCols)
#define CapTouch_U113_NUM_COLS_OFFSET                       (423u)
#define CapTouch_U113_NUM_COLS_SIZE                         (1u)
#define CapTouch_U113_NUM_COLS_PARAM_ID                     (0x5B0B01A7u)

#define CapTouch_U113_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[11].numRows)
#define CapTouch_U113_NUM_ROWS_OFFSET                       (424u)
#define CapTouch_U113_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U113_NUM_ROWS_PARAM_ID                     (0x520B01A8u)

#define CapTouch_U113_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[11].ptr2SnsCpArr)
#define CapTouch_U113_PTR2SNS_CP_OFFSET                     (428u)
#define CapTouch_U113_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U113_PTR2SNS_CP_PARAM_ID                   (0xD40B01ACu)

#define CapTouch_U114_PTR2SNS_FLASH_VALUE                   (CapTouch_dsFlash.wdgtArray[12].ptr2SnsFlash)
#define CapTouch_U114_PTR2SNS_FLASH_OFFSET                  (432u)
#define CapTouch_U114_PTR2SNS_FLASH_SIZE                    (4u)
#define CapTouch_U114_PTR2SNS_FLASH_PARAM_ID                (0xDB0C01B0u)

#define CapTouch_U114_PTR2WD_RAM_VALUE                      (CapTouch_dsFlash.wdgtArray[12].ptr2WdgtRam)
#define CapTouch_U114_PTR2WD_RAM_OFFSET                     (436u)
#define CapTouch_U114_PTR2WD_RAM_SIZE                       (4u)
#define CapTouch_U114_PTR2WD_RAM_PARAM_ID                   (0xDA0C01B4u)

#define CapTouch_U114_PTR2SNS_RAM_VALUE                     (CapTouch_dsFlash.wdgtArray[12].ptr2SnsRam)
#define CapTouch_U114_PTR2SNS_RAM_OFFSET                    (440u)
#define CapTouch_U114_PTR2SNS_RAM_SIZE                      (4u)
#define CapTouch_U114_PTR2SNS_RAM_PARAM_ID                  (0xD90C01B8u)

#define CapTouch_U114_PTR2FLTR_HISTORY_VALUE                (CapTouch_dsFlash.wdgtArray[12].ptr2FltrHistory)
#define CapTouch_U114_PTR2FLTR_HISTORY_OFFSET               (444u)
#define CapTouch_U114_PTR2FLTR_HISTORY_SIZE                 (4u)
#define CapTouch_U114_PTR2FLTR_HISTORY_PARAM_ID             (0xD80C01BCu)

#define CapTouch_U114_PTR2DEBOUNCE_VALUE                    (CapTouch_dsFlash.wdgtArray[12].ptr2DebounceArr)
#define CapTouch_U114_PTR2DEBOUNCE_OFFSET                   (448u)
#define CapTouch_U114_PTR2DEBOUNCE_SIZE                     (4u)
#define CapTouch_U114_PTR2DEBOUNCE_PARAM_ID                 (0xD90C01C0u)

#define CapTouch_U114_STATIC_CONFIG_VALUE                   (CapTouch_dsFlash.wdgtArray[12].staticConfig)
#define CapTouch_U114_STATIC_CONFIG_OFFSET                  (452u)
#define CapTouch_U114_STATIC_CONFIG_SIZE                    (4u)
#define CapTouch_U114_STATIC_CONFIG_PARAM_ID                (0xD80C01C4u)

#define CapTouch_U114_TOTAL_NUM_SNS_VALUE                   (CapTouch_dsFlash.wdgtArray[12].totalNumSns)
#define CapTouch_U114_TOTAL_NUM_SNS_OFFSET                  (456u)
#define CapTouch_U114_TOTAL_NUM_SNS_SIZE                    (2u)
#define CapTouch_U114_TOTAL_NUM_SNS_PARAM_ID                (0x940C01C8u)

#define CapTouch_U114_TYPE_VALUE                            (CapTouch_dsFlash.wdgtArray[12].wdgtType)
#define CapTouch_U114_TYPE_OFFSET                           (458u)
#define CapTouch_U114_TYPE_SIZE                             (1u)
#define CapTouch_U114_TYPE_PARAM_ID                         (0x500C01CAu)

#define CapTouch_U114_NUM_COLS_VALUE                        (CapTouch_dsFlash.wdgtArray[12].numCols)
#define CapTouch_U114_NUM_COLS_OFFSET                       (459u)
#define CapTouch_U114_NUM_COLS_SIZE                         (1u)
#define CapTouch_U114_NUM_COLS_PARAM_ID                     (0x560C01CBu)

#define CapTouch_U114_NUM_ROWS_VALUE                        (CapTouch_dsFlash.wdgtArray[12].numRows)
#define CapTouch_U114_NUM_ROWS_OFFSET                       (460u)
#define CapTouch_U114_NUM_ROWS_SIZE                         (1u)
#define CapTouch_U114_NUM_ROWS_PARAM_ID                     (0x5D0C01CCu)

#define CapTouch_U114_PTR2SNS_CP_VALUE                      (CapTouch_dsFlash.wdgtArray[12].ptr2SnsCpArr)
#define CapTouch_U114_PTR2SNS_CP_OFFSET                     (464u)
#define CapTouch_U114_PTR2SNS_CP_SIZE                       (4u)
#define CapTouch_U114_PTR2SNS_CP_PARAM_ID                   (0xDC0C01D0u)


#endif /* End CY_SENSE_CapTouch_REGISTER_MAP_H */


/* [] END OF FILE */
