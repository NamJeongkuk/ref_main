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
* File Name    : qe_common.c
* Description  : This module implements
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 29.08.2018 1.00    First Release
*              : 02.04.2019 1.01    Add Section information for VDE Safety Protocols
*              : 18.04.2019 1.02    Correspond to GCC Compiler
*              : 25.04.2019 1.03    Fix the Macros for Section information
*              : 07.05.2019 1.04    Add the Macro for Section information
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_touch_qe_private.h"
#include "qe_config01.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/* Configuration arrays must be in method scan order */

ctsu_cfg_t  *gp_ctsu_cfgs[] =
{
    &g_ctsu_cfg_config01,
};

touch_cfg_t *gp_touch_cfgs[] =
{
    &g_touch_cfg_config01,
};

volatile bool g_qetouch_timer_flg = false;


/***********************************************************************************************************************
External global variables and functions
***********************************************************************************************************************/


/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/


#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section _QE_TOUCH_DRIVER
#else
R_BSP_ATTRIB_SECTION_CHANGE(P, _QE_TOUCH_DRIVER)
#endif
#endif
/***********************************************************************************************************************
* Function Name: ctsu_sensor_data_address_set
* Description  : ctsu parameter address setting
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void ctsu_sensor_data_address_set( void )
{
    uint8_t method;

    method = QE_METHOD_CONFIG01;
#if (QE_METHOD_CONFIG01_CAPACITANCE_TYPE == TOUCH_SELF_MODE)
    g_self_ico_data_pt[method]            = &g_config01_ico_data[0];
    g_self_raw_data_pt[method]            = &g_config01_raw_data[0];
    g_self_ico_sensor_pt[method]          = &g_config01_ico_data[0].sen;
    g_self_add_sensor_pt[method]          = &g_config01_add_sensor[0];
    g_self_sensor_cnt_pt[method]          = &g_config01_sensor_cnt[0];
#else
    g_mutual_ico_data_pt[method]          = &g_config01_ico_data[0];
    g_mutual_raw_data_pt[method]          = &g_config01_raw_data[0];
    g_mutual_ico_pri_sensor_pt[method]    = &g_config01_ico_data[0].pri_sen;
    g_mutual_ico_snd_sensor_pt[method]    = &g_config01_ico_data[0].snd_sen;
    g_mutual_add_pri_sensor_pt[method]    = &g_config01_add_pri_sensor[0];
    g_mutual_add_snd_sensor_pt[method]    = &g_config01_add_snd_sensor[0];
    g_mutual_sensor_diff_pt[method]       = &g_config01_sensor_diff[0];
#endif
    g_current_sign_pt[method]             = &g_config01_current_sign[0];

} /* End of function ctsu_sensor_data_address_set() */


#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section _QE_TOUCH_DRIVER
#else
R_BSP_ATTRIB_SECTION_CHANGE(P, _QE_TOUCH_DRIVER)
#endif
#endif
/***********************************************************************************************************************
* Function Name   : touch_parameter_address_set
* Description     : Touch parameter top address setting
* Arguments       : None
* Return value    : None
***********************************************************************************************************************/
void touch_parameter_address_set( void )
{
    uint8_t method;

    method = QE_METHOD_CONFIG01;
    g_key_info[method].mode               = TOUCH_SELF_MODE;
    g_key_info[method].key_num            = CONFIG01_NUM_BUTTONS;
    g_key_info[method].ena_num            = CONFIG01_NUM_ELEMENTS;
    g_key_info[method].key_max_group      = CONFIG01_BTN_MASKS_SIZE;
#if (CONFIG01_NUM_BUTTONS != 0)
    g_key_info[method].ref                = &g_config01_ref[0];
    g_key_info[method].thr                = &g_config01_thr[0];
    g_key_info[method].user_thr           = &g_config01_user_thr[0];
    g_key_info[method].hys                = &g_config01_hys[0];
    g_key_info[method].delta              = &g_config01_delta[0];
    g_key_info[method].touch_cnt          = &g_config01_touch_cnt[0];
    g_key_info[method].non_touch_cnt      = &g_config01_non_touch_cnt[0];
    g_key_info[method].in_touch           = &g_config01_in_touch[0];
    g_key_info[method].out_touch          = &g_config01_out_touch[0];
    g_key_info[method].touch_result       = &g_config01_touch_result[0];
    g_key_info[method].drift_permission   = &g_config01_drift_permission[0];
    g_key_info[method].drift_add_ref      = &g_config01_drift_ref[0];
    g_key_info[method].drift_cnt          = &g_config01_drift_cnt[0];
    g_key_info[method].key_used_info      = &g_config01_key_uesd_info[0];

    g_touch_key_group[method].group       = &g_touch_cfg_config01.button_masks[0];
#endif
    g_key_info[method].sensor_index       = &g_config01_sensor_index[0];
    g_key_info[method].counter_magni      = &g_config01_counter_magni[0];

    g_touch_tuning_info[method].ctsuso    = &g_config01_tune_ctsuso[0];
    g_touch_tuning_info[method].result    = &g_config01_tune_result[0];

} /* End of function touch_parameter_address_set() */


#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section _QE_TOUCH_DRIVER
#else
R_BSP_ATTRIB_SECTION_CHANGE(P, _QE_TOUCH_DRIVER)
#endif
#endif
/***********************************************************************************************************************
* Function Name: qetouch_timer_callback
* Description  : Callback function for periodic timer which indicates to start next scan.
* Arguments    : p_args
*                    (unused)
* Return Value : none
***********************************************************************************************************************/
void qetouch_timer_callback(void *p_args)
{
    g_qetouch_timer_flg = true;
} /* End of function qetouch_timer_callback() */

#if (TOUCH_CFG_SAFETY_LINKAGE_ENABLE)
#if (!defined(R_BSP_VERSION_MAJOR) || (R_BSP_VERSION_MAJOR < 5))
#pragma section
#else
R_BSP_ATTRIB_SECTION_CHANGE_END
#endif
#endif
