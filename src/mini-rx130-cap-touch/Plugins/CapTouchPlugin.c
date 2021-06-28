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
#include "CapTouchVersionCheck.h"
#include "KeyState.h"
#include "utils.h"

enum
{
   RawCountsWatchdogTimeoutMsec = 5 * 60 * 1000,
   RawCountsWatchdogCounterLimit = RawCountsWatchdogTimeoutMsec / CapTouchPollPeriodMsec,
};

enum
{
   DiagnosticsState_Idle,
   DiagnosticsState_InProgress,
   DiagnosticsState_Complete
};
typedef uint8_t DiagnosticsState_t;

typedef struct
{
   uint16_t lastRaw;
   uint16_t counter;
} RawCountsWatchdogState_t;

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

enum
{
   CapTouchKeyCount = ELEMENT_COUNT(keyMasks)
};

static struct
{
   Timer_t timer;
} poll;

static struct
{
   RawCountsWatchdogState_t state[CapTouchKeyCount];
} rawCountsWatchdog;

static struct
{
   Timer_t timer;
   DiagnosticsState_t state;
   uint16_t failureCount[ELEMENT_COUNT(safetyCriticalKeyIndexes)];
   uint8_t currentKey;
   uint8_t results[ELEMENT_COUNT(safetyCriticalKeyIndexes)];
} diagnostics;

static void RunDiagnostics(I_DataSource_t *dataSource)
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
         DataSource_Write(dataSource, CapTouchDiagnosticsResultsErd, diagnostics.results);
      }
   }
}

static void UpdateRawCountsWatchdog(void)
{
   uint8_t count;
   uint16_t rawCounts[CapTouchKeyCount];
   R_TOUCH_GetRawData(CONCAT(QE_METHOD_, CapTouchConfigName), rawCounts, &count);

   for(uint8_t key = 0; key < CapTouchKeyCount; key++)
   {
      if(rawCounts[key] != rawCountsWatchdog.state[key].lastRaw)
      {
         rawCountsWatchdog.state[key].counter = 0;
         rawCountsWatchdog.state[key].lastRaw = rawCounts[key];
      }
      else
      {
         rawCountsWatchdog.state[key].counter++;

         if(rawCountsWatchdog.state[key].counter >= RawCountsWatchdogCounterLimit)
         {
            CapTouchSystemReset();
         }
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

   if(diagnostics.state == DiagnosticsState_InProgress)
   {
      RunDiagnostics(dataSource);
   }

   UpdateRawCountsWatchdog();

   CaptureScanResults(dataSource);
   StartScan();
}

static void ArmPollTimer(I_DataSource_t *dataSource)
{
   TimerModule_StartPeriodic(
      DataSourceErdPointerAccess_GetPointer(dataSource, CapTouchTimerModuleErd),
      &poll.timer,
      CapTouchPollPeriodMsec,
      Poll,
      dataSource);
}

static void StartDiagnostics(void *context)
{
   IGNORE(context);

   if(diagnostics.state == DiagnosticsState_InProgress)
   {
      CapTouchSystemReset();
   }

   diagnostics.state = DiagnosticsState_InProgress;
   diagnostics.currentKey = 0;
}

static void ArmDiagnosticsTimer(I_DataSource_t *dataSource)
{
   TimerModule_StartPeriodic(
      DataSourceErdPointerAccess_GetPointer(dataSource, CapTouchTimerModuleErd),
      &diagnostics.timer,
      CapTouchDiagnosticsPeriodMsec,
      StartDiagnostics,
      dataSource);
}

void CapTouchPlugin_Init(I_DataSource_t *dataSource)
{
   R_CTSU_PinSetInit();

   if(R_TOUCH_Open(gp_ctsu_cfgs, gp_touch_cfgs, QE_NUM_METHODS, QE_TRIG_SOFTWARE) != QE_SUCCESS)
   {
      CapTouchSystemReset();
   }

   StartScan();

   ArmPollTimer(dataSource);
   ArmDiagnosticsTimer(dataSource);
}
