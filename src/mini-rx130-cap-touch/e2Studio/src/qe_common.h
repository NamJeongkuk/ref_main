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
* File Name    : qe_common.h
* Description  : This file includes definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 29.08.2018 1.00    First Release
*              : 02.04.2019 1.01    Add Some Comments
*              : 18.04.2019 1.02    Change the Minor Format
***********************************************************************************************************************/

#ifndef QE_COMMON_H
#define QE_COMMON_H

#include "r_typedefs_qe.h"


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define MCU_MAX_SENSORS         (36)    /* highest TS number + 1; legacy MAX_TS from compile line */

#define QE_NUM_METHODS          (1)
#define QE_METHOD_CONFIG01      (0)     /* method ID; array index */

#define QE_MAX_BUTTONS          (2)     /* most buttons present in all methods */
#define QE_MAX_SLIDERS          (0)     /* most sliders present in all methods */
#define QE_MAX_WHEELS           (0)     /* most wheels present in all methods */
#define QE_MAX_SLDR_ELEM_USED   (0)     /* most elements used in a slider in all methods */
#define QE_MAX_WHEEL_ELEM_USED  (0)     /* most elements used in a wheel in all methods */


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
extern ctsu_cfg_t   *gp_ctsu_cfgs[];
extern touch_cfg_t  *gp_touch_cfgs[];
extern volatile bool g_qetouch_timer_flg;


/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
extern void ctsu_sensor_data_address_set(void);
extern void touch_parameter_address_set(void);
extern void qetouch_timer_callback(void *p_args);


#endif /* QE_COMMON_H */