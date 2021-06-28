/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapTouchDebugPlugin.h"
#include "CapTouchPluginConfiguration.h"
#include "Timer.h"
#include "DataSourceErdPointerAccess.h"
#include "r_touch_qe_if.h"
#include "CapTouchVersionCheck.h"
#include "QeDebug.h"

#define EXPAND_AS_KEY_COUNT_FIELDS(keyId) \
   uint8_t CONCAT(key, keyId);

typedef struct
{
   CAP_TOUCH_KEYS(EXPAND_AS_KEY_COUNT_FIELDS)
} KeyCount_t;

static Timer_t pollTimer;

static void Poll(void *context)
{
   IGNORE(context);
   REINTERPRET(dataSource, context, I_DataSource_t *);

   uint16_t rawCounts[sizeof(KeyCount_t)];
   uint16_t counts[sizeof(KeyCount_t)];
   uint16_t deltas[sizeof(KeyCount_t)];
   uint8_t count;

   R_TOUCH_GetRawData(CONCAT(QE_METHOD_, CapTouchConfigName), rawCounts, &count);
   R_TOUCH_GetData(CONCAT(QE_METHOD_, CapTouchConfigName), counts, &count);
   R_TOUCH_GetBtnBaselines(CONCAT(QE_METHOD_, CapTouchConfigName), deltas, &count);

   for(uint8_t i = 0; i < sizeof(KeyCount_t); i++)
   {
      deltas[i] = counts[i] - deltas[i];
   }

   if(CapTouchDebugRawCountsErd != (Erd_t)Erd_Invalid)
   {
      DataSource_Write(dataSource, CapTouchDebugRawCountsErd, rawCounts);
   }

   if(CapTouchDebugCountsErd != (Erd_t)Erd_Invalid)
   {
      DataSource_Write(dataSource, CapTouchDebugCountsErd, counts);
   }

   if(CapTouchDebugDeltasErd != (Erd_t)Erd_Invalid)
   {
      DataSource_Write(dataSource, CapTouchDebugDeltasErd, deltas);
   }
}

static void ArmPollTimer(I_DataSource_t *dataSource)
{
   TimerModule_StartPeriodic(
      DataSourceErdPointerAccess_GetPointer(dataSource, CapTouchTimerModuleErd),
      &pollTimer,
      CapTouchDebugPollPeriodMsec,
      Poll,
      dataSource);
}

void CapTouchDebugPlugin_Init(I_DataSource_t *dataSource)
{
   ArmPollTimer(dataSource);
}
