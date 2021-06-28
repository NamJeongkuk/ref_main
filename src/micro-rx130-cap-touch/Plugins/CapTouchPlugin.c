/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapTouchPlugin.h"
#include "CapTouchPluginConfiguration.h"
#include "r_smc_entry.h"
#include "qe_common.h"
#include "r_touch_qe_if.h"
#include "r_ctsu_qe_pinset.h"
#include "r_qetouch_diagnostics_if.h"
#include "KeyState.h"
#include "utils.h"

enum
{
   DiagnosticsState_Idle,
   DiagnosticsState_InProgress,
   DiagnosticsState_Complete
};
typedef uint8_t DiagnosticsState_t;

#define EXPAND_AS_KEY_MASKS(keyId) \
   CONCAT(CONCAT(CapTouchConfigName, _MASK_), keyId),

static const uint64_t keyMasks[] = {
   CAP_TOUCH_KEYS(EXPAND_AS_KEY_MASKS)
};

#define EXPAND_AS_KEY_INDEXES(keyId) \
   CONCAT(CONCAT(CapTouchConfigName, _INDEX_), keyId),

static const uint8_t safetyCriticalKeyIndexes[] = {
   SAFETY_CRITICAL_CAP_TOUCH_KEYS(EXPAND_AS_KEY_INDEXES)
};

static struct
{
   TinyTimer_t timer;
} poll;

static struct
{
   TinyTimer_t timer;
   DiagnosticsState_t state;
   uint16_t failureCount[ELEMENT_COUNT(safetyCriticalKeyIndexes)];
   uint8_t currentKey;
   uint8_t results[ELEMENT_COUNT(safetyCriticalKeyIndexes)];
} diagnostics;

static void ArmDiagnosticsTimer(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule);
static void ArmPollTimer(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule);

static void RunDiagnostics(I_TinyDataSource_t *dataSource)
{
   if(NUM_ELEMENTS(safetyCriticalKeyIndexes) == 0)
   {
      diagnostics.state = DiagnosticsState_Complete;
      return;
   }

   e_diagerr_t result = R_CTSU_DiagSingle(
      CONCAT(QE_METHOD_, CapTouchConfigName),
      safetyCriticalKeyIndexes[diagnostics.currentKey]);

   if(result == DIAG_CTSU_SCAN_IN_PROGRESS)
   {
      return;
   }
   else if(result == DIAG_ALL_CHECK_SUCCESS)
   {
      diagnostics.failureCount[diagnostics.currentKey] = 0;
   }
   else
   {
      diagnostics.failureCount[diagnostics.currentKey]++;

      if(diagnostics.failureCount[diagnostics.currentKey] > CapTouchDiagnosticsConsecutiveFailureLimit)
      {
         CapTouchSystemReset();
      }
   }

   diagnostics.results[diagnostics.currentKey] = result;

   diagnostics.currentKey++;

   if(diagnostics.currentKey == ELEMENT_COUNT(safetyCriticalKeyIndexes))
   {
      diagnostics.state = DiagnosticsState_Complete;

      if(CapTouchDiagnosticsResultsErd != (Erd_t)Erd_Invalid)
      {
         TinyDataSource_Write(dataSource, CapTouchDiagnosticsResultsErd, diagnostics.results);
      }
   }
}

static inline void StartScan(void)
{
   R_TOUCH_UpdateDataAndStartScan();
}

static void CaptureScanResults(I_TinyDataSource_t *dataSource)
{
   uint64_t rawState;
   R_TOUCH_GetAllBtnStates(CONCAT(QE_METHOD_, CapTouchConfigName), &rawState);

   CapTouchState_t state = 0;

   for(uint8_t i = NUM_ELEMENTS(keyMasks); i > 0; i--)
   {
      state <<= 1;
      state |= (rawState & keyMasks[i - 1]) == keyMasks[i - 1];
   }

   TinyDataSource_Write(dataSource, CapTouchResultErd, &state);
}

static void Poll(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   REINTERPRET(dataSource, context, I_TinyDataSource_t *);

   if(diagnostics.state == DiagnosticsState_InProgress)
   {
      RunDiagnostics(dataSource);
   }

   CaptureScanResults(dataSource);
   StartScan();

   ArmPollTimer(dataSource, timerModule);
}

static void ArmPollTimer(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(
      timerModule,
      &poll.timer,
      CapTouchPollPeriodMsec,
      Poll,
      dataSource);
}

static void StartDiagnostics(void *context, struct TinyTimerModule_t *timerModule)
{
   REINTERPRET(dataSource, context, I_TinyDataSource_t *);

   if(diagnostics.state == DiagnosticsState_InProgress)
   {
      CapTouchSystemReset();
   }

   diagnostics.state = DiagnosticsState_InProgress;
   diagnostics.currentKey = 0;

   ArmDiagnosticsTimer(dataSource, timerModule);
}

static void ArmDiagnosticsTimer(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(
      timerModule,
      &diagnostics.timer,
      CapTouchDiagnosticsPeriodMsec,
      StartDiagnostics,
      dataSource);
}

void CapTouchPlugin_Init(I_TinyDataSource_t *dataSource)
{
   R_CTSU_PinSetInit();

   if(R_TOUCH_Open(gp_ctsu_cfgs, gp_touch_cfgs, QE_NUM_METHODS, QE_TRIG_SOFTWARE) != QE_SUCCESS)
   {
      CapTouchSystemReset();
   }

   StartScan();

   ArmPollTimer(dataSource, TinyDataSource_ReadPointer(dataSource, CapTouchTimerModuleErd));
   ArmDiagnosticsTimer(dataSource, TinyDataSource_ReadPointer(dataSource, CapTouchTimerModuleErd));
}
