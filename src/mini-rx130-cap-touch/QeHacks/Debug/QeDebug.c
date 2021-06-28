/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "QeDebug.h"
#include "r_touch_qe_private.h"

/***********************************************************************************************************************
* Function Name: R_TOUCH_GetBtnBaselines
* Description  : This function gets the long term moving average values (values which touch judgment compares against)
*                for button elements only. The values are loaded into the buffer in their respective element index
*                locations (same as GetData()).
* Arguments    : method -
*                    Method to get data for.
*                p_buf -
*                    Pointer to buffer to load data into.
*                    (Non-button sensor locations are not loaded).
*                p_cnt -
*                    Pointer to variable to load word-size of buffer
* Return Value : QE_SUCCESS -
*                    Command performed successfully.
*                QE_ERR_INVALID_ARG -
*                    "method" is invalid.
*                QE_ERR_NULL_PTR -
*                    "p_buf" or "p_cnt" is NULL.
***********************************************************************************************************************/
qe_err_t R_TOUCH_GetBtnBaselines(uint8_t method, uint16_t *p_buf, uint8_t *p_cnt)
{
   uint8_t i;
   uint8_t j;

   /* load expected size of buffer passed in */
   *p_cnt = g_key_info[method].ena_num;

   /* poke button baseline values into passed-in buffer */
   for(i = 0; i < gp_touch_cfgs[method]->num_buttons; i++)
   {
      j = gp_touch_cfgs[method]->p_buttons[i].elem_index; // get buffer index
      p_buf[j] = g_key_info[method].ref[i]; // poke baseline value
   }

   return QE_SUCCESS;
} /* End of function R_TOUCH_GetBtnBaselines() */
