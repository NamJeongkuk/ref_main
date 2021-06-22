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
* File Name    : qe_config01.h
* Description  : This file includes definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 27.09.2018 1.00    First Release
*              : 02.04.2019 1.01    Add Some Comments
*              : 18.04.2019 1.02    Change the Minor Format
***********************************************************************************************************************/

#ifndef QE_CONFIG01_H
#define QE_CONFIG01_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_touch_qe_private.h"
#include "qe_common.h"


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define CONFIG01_NUM_BUTTONS        (2)
#define CONFIG01_INDEX_KEY1     (0)   /* btn_ctrl_t array index */
#define CONFIG01_ID_KEY1        ((QE_METHOD_CONFIG01<<8) | CONFIG01_INDEX_KEY1)   /* button ID */
#define CONFIG01_MASK_KEY1      (1ULL << QE_TS11)   /* button state mask */
#define CONFIG01_INDEX_KEY2     (1)   /* btn_ctrl_t array index */
#define CONFIG01_ID_KEY2        ((QE_METHOD_CONFIG01<<8) | CONFIG01_INDEX_KEY2)   /* button ID */
#define CONFIG01_MASK_KEY2      (1ULL << QE_TS12)   /* button state mask */

#define CONFIG01_NUM_SLIDERS        (0)

#define CONFIG01_NUM_WHEELS         (0)


#define CONFIG01_NUM_ELEMENTS       (2)   /* num TS for SELF; RX x TX for MUTUAL */
#define CONFIG01_BTN_MASKS_SIZE     (1)   /* necessary array size for widgets.button_masks[] */


/* Button configurations */
#define CONFIG01_KEY1_THR_OFFSET    (2352)
#define CONFIG01_KEY1_HYS_OFFSET    (117)
#define CONFIG01_KEY2_THR_OFFSET    (2524)
#define CONFIG01_KEY2_HYS_OFFSET    (126)

/* Slider configurations */

/* Wheel configurations */

/* Touch configurations */
#define CONFIG01_TOUCH_ON           (3)
#define CONFIG01_TOUCH_OFF          (3)
#define CONFIG01_MAX_ON_TIME        (0)
#define CONFIG01_DRIFT_ENABLE       (1)
#define CONFIG01_DRIFT_FREQ         (255)


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

#define QE_METHOD_CONFIG01_CAPACITANCE_TYPE (TOUCH_SELF_MODE)

extern ctsu_cfg_t   g_ctsu_cfg_config01;
extern btn_ctrl_t   g_buttons_config01[];
extern sldr_ctrl_t  g_sliders_config01[];
extern wheel_ctrl_t g_wheels_config01[];
extern touch_cfg_t  g_touch_cfg_config01;

extern uint16_t     g_config01_write[];
#if (QE_METHOD_CONFIG01_CAPACITANCE_TYPE == TOUCH_SELF_MODE)
extern self_ico_t   g_config01_ico_data[];
extern self_ico_t   g_config01_raw_data[];
extern uint16_t     g_config01_add_sensor[];
extern uint16_t     g_config01_sensor_cnt[];
#else
extern mutual_ico_t g_config01_ico_data[];
extern mutual_ico_t g_config01_raw_data[];
extern uint16_t     g_config01_add_pri_sensor[];
extern uint16_t     g_config01_add_snd_sensor[];
extern uint16_t     g_config01_sensor_diff[];
#endif
extern int8_t       g_config01_current_sign[];
#if (CONFIG01_NUM_BUTTONS != 0)
extern uint16_t     g_config01_ref[];
extern uint16_t     g_config01_thr[];
extern uint16_t     g_config01_user_thr[];
extern uint16_t     g_config01_hys[];
extern uint16_t     g_config01_delta[];
extern uint16_t     g_config01_touch_cnt[];
extern uint16_t     g_config01_non_touch_cnt[];
extern uint16_t     g_config01_in_touch[];
extern uint16_t     g_config01_out_touch[];
extern uint16_t     g_config01_touch_result[];
extern uint16_t     g_config01_drift_permission[];
extern uint32_t     g_config01_drift_ref[];
extern uint16_t     g_config01_drift_cnt[];
extern uint16_t     g_config01_key_uesd_info[];
#endif
extern uint8_t      g_config01_sensor_index[];
extern int8_t       g_config01_counter_magni[];
extern uint16_t     g_config01_tune_ctsuso[];
extern volatile uint8_t    g_config01_tune_result[];


/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/


#endif /* QE_CONFIG01_H */
