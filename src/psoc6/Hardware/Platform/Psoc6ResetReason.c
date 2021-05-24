/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Psoc6ResetReason.h"
#include "utils.h"
#include "cy_pdl.h"

ResetReason_t Psoc6ResetReason_GetResetReason(void)
{
   ResetReason_t resetReason;
   uint32_t resetBitfield = Cy_SysLib_GetResetReason();

   resetReason.metadata = resetBitfield;

   if(resetBitfield & CY_SYSLIB_RESET_HWWDT ||
      resetBitfield & CY_SYSLIB_RESET_SWWDT0)
   {
      resetReason.source = ResetSource_Watchdog;
   }
   else if(BITMAP_STATE(resetBitfield, CY_SYSLIB_RESET_SOFT))
   {
      resetReason.source = ResetSource_Software;
   }
   else if(resetBitfield & CY_SYSLIB_RESET_ACT_FAULT ||
      resetBitfield & CY_SYSLIB_RESET_DPSLP_FAULT ||
      resetBitfield & CY_SYSLIB_RESET_CSV_LOSS_WAKEUP ||
      resetBitfield & CY_SYSLIB_RESET_CSV_ERROR_WAKEUP ||
      resetBitfield & CY_SYSLIB_RESET_HIB_WAKEUP)
   {
      resetReason.source = ResetSource_Unknown;
   }
   else
   {
      // There is no way to infer a power-on reset from the reset cause registers,
      // but we can assume it is if none of the other bits are set.
      resetReason.source = ResetSource_PowerOn;
   }

   return resetReason;
}
