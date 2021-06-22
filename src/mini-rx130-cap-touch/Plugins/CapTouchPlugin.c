/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapTouchPlugin.h"
#include "CapTouchPluginConfiguration.h"
#include "DataSourceErdPointerAccess.h"
#include "r_smc_entry.h"
#include "qe_common.h"
#include "r_touch_qe_if.h"
#include "r_ctsu_qe_pinset.h"
#include "r_qetouch_diagnostics_if.h"
#include "KeyState.h"
#include "utils.h"

enum
{
   DiagnosticsScanInProgressLimit = CapTouchDiagnosticsScanMaximumPeriodMsec / CapTouchPollPeriodMsec
};

#define COMMA ,

#define EXPAND_AS_KEY_MASKS(keyId) \
   CONCAT(CONCAT(CapTouchConfigName, _MASK_), keyId),

static const uint64_t keyMasks[] = {
   CAP_TOUCH_KEYS(EXPAND_AS_KEY_MASKS)
};

static Timer_t pollTimer;
static uint16_t diagnosticsFailureCount;
static uint16_t diagnosticsScanInProgressCount;

static void RunDiagnostics(void)
{
   e_diagerr_t result = R_CTSU_DiagAll(CONCAT(QE_METHOD_, CapTouchConfigName));

   if(result == DIAG_CTSU_SCAN_IN_PROGRESS)
   {
      diagnosticsScanInProgressCount++;

      if(diagnosticsScanInProgressCount > DiagnosticsScanInProgressLimit)
      {
         CapTouchSystemReset();
      }

      return;
   }
   else
   {
      diagnosticsScanInProgressCount = 0;
   }

   if(result == DIAG_ALL_CHECK_SUCCESS)
   {
      diagnosticsFailureCount = 0;
   }
   else
   {
      diagnosticsFailureCount++;

      if(diagnosticsFailureCount > CapTouchDiagnosticsConsecutiveFailureLimit)
      {
         CapTouchSystemReset();
      }
   }
}

static inline void StartScan(void)
{
   R_TOUCH_UpdateDataAndStartScan();
}

static void CaptureScanResults(I_DataSource_t *dataSource)
{
   uint64_t rawState;
   R_TOUCH_GetAllBtnStates(CONCAT(QE_METHOD_, CapTouchConfigName), &rawState);

   CapTouchState_t state = 0;

   for(uint8_t i = NUM_ELEMENTS(keyMasks); i > 0; i--)
   {
      state <<= 1;
      state |= (rawState & keyMasks[i - 1]) == keyMasks[i - 1];
   }

   DataSource_Write(dataSource, CapTouchResultErd, &state);
}

static void Poll(void *context)
{
   IGNORE(context);
   REINTERPRET(dataSource, context, I_DataSource_t *);

   RunDiagnostics();
   CaptureScanResults(dataSource);
   StartScan();
}

void CapTouchPlugin_Init(I_DataSource_t *dataSource)
{
   R_CTSU_PinSetInit();

   if(R_TOUCH_Open(gp_ctsu_cfgs, gp_touch_cfgs, QE_NUM_METHODS, QE_TRIG_SOFTWARE) != QE_SUCCESS)
   {
      CapTouchSystemReset();
   }

   StartScan();

   TimerModule_StartPeriodic(
      DataSourceErdPointerAccess_GetTimerModule(dataSource, CapTouchTimerModuleErd),
      &pollTimer,
      CapTouchPollPeriodMsec,
      Poll,
      dataSource);
}
