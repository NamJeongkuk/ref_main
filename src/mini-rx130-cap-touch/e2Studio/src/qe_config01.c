/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : qe_config01.c
* Description  : This module implements
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 27.09.2018 1.00    First Release
*              : 02.04.2019 1.01    Add Some Comments
*              : 02.04.2019 1.02    Add Section information for VDE Safety Protocols
*              : 18.04.2019 1.03    Change the Minor Format
*              : 18.04.2019 1.04    Correspond to GCC Compiler
*              : 25.04.2019 1.05    Fix the Macros for Section information
*              : 07.05.2019 1.06    Add the Macro for Section information
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"
#include "qe_config01.h"
#include "qe_common.h"


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section _QE_TOUCH_SAFETY_CONSTANT
#else
R_BSP_ATTRIB_SECTION_CHANGE(C, _QE_TOUCH_SAFETY_CONSTANT, 1)
#endif
#endif
/* Element arrays must be in hardware scan order */
const elem_ch_t g_config01_elem_ch[] =
{
    { .rx_chan=11, .tx_chan=QE_UNUSED_CHAN },
    { .rx_chan=12, .tx_chan=QE_UNUSED_CHAN },
};

#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section _QE_TOUCH_SAFETY_CONSTANT
#else
R_BSP_ATTRIB_SECTION_CHANGE(C, _QE_TOUCH_SAFETY_CONSTANT, 4)
#endif
#endif
const port_cfg_t g_config01_port_cfg[] =
{
    { .p_pmr=&PORT5.PMR.BYTE, .ts_board_mask=0x30, .ts_method_mask=0x30 },
};

#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section _QE_TOUCH_SAFETY_RAMINIT
#else
R_BSP_ATTRIB_SECTION_CHANGE(D, _QE_TOUCH_SAFETY_RAMINIT, 2)
#endif
#endif
elem_regs_t g_config01_elem_regs[] =
{
    { .ctsussc=0x0000, .ctsuso0=0x1CC4, .ctsuso1=0x2300 },
    { .ctsussc=0x0000, .ctsuso0=0x1CCD, .ctsuso1=0x2300 },
};

#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section _QE_TOUCH_SAFETY_RAMINIT
#else
R_BSP_ATTRIB_SECTION_CHANGE(D, _QE_TOUCH_SAFETY_RAMINIT, 4)
#endif
#endif
ctsu_cfg_t g_ctsu_cfg_config01 =
{
    .ctsuchac0 = 0x00,   /* ch0-ch7 enable mask */
    .ctsuchac1 = 0x18,   /* ch8-ch15 enable mask */
    .ctsuchac2 = 0x00,   /* ch16-ch23 enable mask */
    .ctsuchac3 = 0x00,   /* ch24-ch31 enable mask */
    .ctsuchac4 = 0x00,   /* ch32-ch39 enable mask */

    .ctsuchtrc0 = 0x00,   /* ch0-ch7 mutual tx mask */
    .ctsuchtrc1 = 0x00,   /* ch8-ch15 mutual tx mask */
    .ctsuchtrc2 = 0x00,   /* ch16-ch23 mutual tx mask */
    .ctsuchtrc3 = 0x00,   /* ch24-ch31 mutual tx mask */
    .ctsuchtrc4 = 0x00,   /* ch32-ch39 mutual tx mask */

    .ctsucr1 = 0x43,
    .ctsusdprs = 0x23,

    .p_elem_ch = g_config01_elem_ch,
    .p_elem_regs = g_config01_elem_regs,
    .p_scan_buf = (uint16_t *)g_config01_ico_data,
    .p_port_cfg = g_config01_port_cfg,
    .num_elements = CONFIG01_NUM_ELEMENTS,
    .num_ports = 1,
    .method = QE_METHOD_CONFIG01,
    .txvsel = 0,
    .ctsu_status = 0
};

#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section
#else
R_BSP_ATTRIB_SECTION_CHANGE_END
#endif
#endif


#if (CONFIG01_NUM_BUTTONS != 0)
btn_ctrl_t g_buttons_config01[] =
{
    {
        .elem_index = 0,
        .thr_offset = CONFIG01_KEY1_THR_OFFSET,
        .hys_offset = CONFIG01_KEY1_HYS_OFFSET,
    },
    {
        .elem_index = 1,
        .thr_offset = CONFIG01_KEY2_THR_OFFSET,
        .hys_offset = CONFIG01_KEY2_HYS_OFFSET,
    },
};
#endif


#if (CONFIG01_NUM_SLIDERS != 0)
sldr_ctrl_t g_sliders_config01[] =
{
    {
        .elem_index[0] = 0xFF,
        .elem_index[1] = 0xFF,
        .elem_index[2] = 0xFF,
        .elem_index[3] = 0xFF,
        .elem_index[4] = 0xFF,
        .elem_index[5] = 0xFF,
        .elem_index[6] = 0xFF,
        .elem_index[7] = 0xFF,
        .elem_index[8] = 0xFF,
        .elem_index[9] = 0xFF,
        .num_elements = 0,
        .threshold = CONFIG01_NONE_SLIDER_THRESHOLD,   /* Position calculation start threshold value */
        .thr_offset = CONFIG01_NONE_SLIDER_THR_OFFSET,   /* Threshold offset value */
        .resolution = CONFIG01_NONE_SLIDER_RESOLUTION,   /* Position resolution */
        .value = 0xFFFF,
    },
};
#endif


#if (CONFIG01_NUM_WHEELS != 0)
wheel_ctrl_t g_wheels_config01[] =
{
    {
        .elem_index[0] = 0xFF,
        .elem_index[1] = 0xFF,
        .elem_index[2] = 0xFF,
        .elem_index[3] = 0xFF,
        .elem_index[4] = 0xFF,
        .elem_index[5] = 0xFF,
        .elem_index[6] = 0xFF,
        .elem_index[7] = 0xFF,
        .num_elements = 0,
        .threshold = CONFIG01_NONE_WHEEL_THRESHOLD,   /* Position calculation start threshold value */
        .thr_offset = CONFIG01_NONE_WHEEL_THR_OFFSET,   /* Threshold offset value */
        .resolution = CONFIG01_NONE_WHEEL_RESOLUTION,   /* Position resolution */
        .value = 0xffff
    },
};
#endif


touch_cfg_t g_touch_cfg_config01 =
{
    .p_buttons = &g_buttons_config01[0],
    .p_sliders = NULL,
    .p_wheels = NULL,
    .button_masks = {0x1800, 0x0000, 0x0000, 0x0000},
    .button_masks_size = CONFIG01_BTN_MASKS_SIZE,
    .num_buttons = CONFIG01_NUM_BUTTONS,
    .num_sliders = CONFIG01_NUM_SLIDERS,
    .num_wheels = CONFIG01_NUM_WHEELS,
    .touch_on = CONFIG01_TOUCH_ON,   /* 0-255 The cumulative number of determinations of [ON]; SELF_TOUCH_ON */
    .touch_off = CONFIG01_TOUCH_OFF,   /* 0-255 The cumulative number of determinations of [OFF]; SELF_TOUCH_OFF */
    .max_on_time = CONFIG01_MAX_ON_TIME,   /* 0-65535 Maximum continuous [ON], 0:no use; SELF_MSA */
    .drift_enable = CONFIG01_DRIFT_ENABLE,   /* Drift function enable; SELF_DRIFT_ENABLE */
    .drift_freq = CONFIG01_DRIFT_FREQ,   /* 0-65535; SELF_DRIFT_FREQUENCY */
};


/***********************************************************************************************************************
External global variables and functions
***********************************************************************************************************************/


/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

uint16_t    g_config01_write[sizeof(elem_regs_t)*CONFIG01_NUM_ELEMENTS];   /* DTC transmit write data */
#if (QE_METHOD_CONFIG01_CAPACITANCE_TYPE == TOUCH_SELF_MODE)
self_ico_t  g_config01_ico_data[CONFIG01_NUM_ELEMENTS];   /* ICO counter data */
self_ico_t  g_config01_raw_data[CONFIG01_NUM_ELEMENTS];   /* ICO counter data copy for API Get() */
uint16_t    g_config01_add_sensor[CONFIG01_NUM_ELEMENTS];   /* Sensor counter data adding buffer */
uint16_t    g_config01_sensor_cnt[CONFIG01_NUM_ELEMENTS];   /* Secondary - Primary (Sensor) */
#else
mutual_ico_t g_config01_ico_data[CONFIG01_NUM_ELEMENTS];   /* ICO counter data */
mutual_ico_t g_config01_raw_data[CONFIG01_NUM_ELEMENTS];   /* ICO counter data copy for API Get() */
uint16_t    g_config01_add_pri_sensor[CONFIG01_NUM_ELEMENTS];   /* Primary Sensor counter data adding buffer */
uint16_t    g_config01_add_snd_sensor[CONFIG01_NUM_ELEMENTS];   /* Secondary Sensor counter data adding buffer */
uint16_t    g_config01_sensor_diff[CONFIG01_NUM_ELEMENTS];   /* Secondary - Primary (Sensor) */
#endif
int8_t      g_config01_current_sign[CONFIG01_NUM_ELEMENTS];   /* Current offset plus minus sign */
#if (CONFIG01_NUM_BUTTONS != 0)
uint16_t    g_config01_ref[CONFIG01_NUM_BUTTONS];   /* Reference */
uint16_t    g_config01_thr[CONFIG01_NUM_BUTTONS];   /* Threshold = Reference + User threshold */
uint16_t    g_config01_user_thr[CONFIG01_NUM_BUTTONS];   /* User threshold */
uint16_t    g_config01_hys[CONFIG01_NUM_BUTTONS];   /* Hysteresis */
uint16_t    g_config01_delta[CONFIG01_NUM_BUTTONS];   /* Delta = Reference - Sensor counter */
uint16_t    g_config01_touch_cnt[CONFIG01_NUM_BUTTONS];   /* Touch count */
uint16_t    g_config01_non_touch_cnt[CONFIG01_NUM_BUTTONS];   /* Non-touch count */
uint16_t    g_config01_in_touch[CONFIG01_BTN_MASKS_SIZE];   /* Inside touch/non-touch data */
uint16_t    g_config01_out_touch[CONFIG01_BTN_MASKS_SIZE];   /* Outside touch/non-touch data */
uint16_t    g_config01_touch_result[CONFIG01_BTN_MASKS_SIZE];   /* Touch/non-touch result data */
uint16_t    g_config01_drift_permission[CONFIG01_BTN_MASKS_SIZE];   /* Drift permission */
uint32_t    g_config01_drift_ref[CONFIG01_NUM_BUTTONS];   /* Drift addition reference */
uint16_t    g_config01_drift_cnt[CONFIG01_NUM_BUTTONS];   /* Drift counter */

uint16_t    g_config01_key_uesd_info[(MCU_MAX_SENSORS/16) + 1];
#endif
uint8_t     g_config01_sensor_index[MCU_MAX_SENSORS];   /* Key sensor index */
int8_t      g_config01_counter_magni[CONFIG01_NUM_ELEMENTS];   /* Measurement time */
uint16_t    g_config01_tune_ctsuso[CONFIG01_NUM_ELEMENTS];   /* Offset tuning ctsuso data */
volatile uint8_t    g_config01_tune_result[CONFIG01_NUM_ELEMENTS];   /* Offset tuning result data */
