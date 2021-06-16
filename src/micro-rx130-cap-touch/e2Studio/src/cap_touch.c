/***********************************************************************
*
*  FILE        : cap_touch.c
*  DATE        : 2021-06-14
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include "r_smc_entry.h"
#include "qe_common.h"
#include "r_touch_qe_if.h"
#include "qe_config01.h"
#include "r_cmt_rx_if.h"
#include "r_ctsu_qe_pinset.h"

void main(void);

void main(void)
{
   qe_err_t err;
   bool success;
   uint32_t cmt_ch;

   R_CTSU_PinSetInit();

   err = R_TOUCH_Open(gp_ctsu_cfgs, gp_touch_cfgs, QE_NUM_METHODS, QE_TRIG_SOFTWARE);
   if(err != QE_SUCCESS)
   {
      while(1)
         ;
   }

   success = R_CMT_CreatePeriodic(50, qetouch_timer_callback, &cmt_ch);
   if(success == false)
   {
      while(1)
         ;
   }

   while(1U)
   {
      if(g_qetouch_timer_flg == true)
      {
         g_qetouch_timer_flg = false;
         R_TOUCH_UpdateDataAndStartScan();

         R_TOUCH_GetAllBtnStates(QE_METHOD_CONFIG01, &btn_states);
      }
   }
}
