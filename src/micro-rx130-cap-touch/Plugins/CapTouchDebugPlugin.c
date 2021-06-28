/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapTouchDebugPlugin.h"
#include "CapTouchPluginConfiguration.h"
#include "TinyTimer.h"
#include "r_touch_qe_if.h"
#include "CapTouchVersionCheck.h"
#include "QeDebug.h"

#define EXPAND_AS_KEY_COUNT_FIELDS(keyId) \
   uint8_t CONCAT(key, keyId);

typedef struct
{
   CAP_TOUCH_KEYS(EXPAND_AS_KEY_COUNT_FIELDS)
} KeyCount_t;

static TinyTimer_t pollTimer;

static void ArmPollTimer(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule);

static void Poll(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);
   REINTERPRET(dataSource, context, I_TinyDataSource_t *);

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
      TinyDataSource_Write(dataSource, CapTouchDebugRawCountsErd, rawCounts);
   }

   if(CapTouchDebugCountsErd != (Erd_t)Erd_Invalid)
   {
      TinyDataSource_Write(dataSource, CapTouchDebugCountsErd, counts);
   }

   if(CapTouchDebugDeltasErd != (Erd_t)Erd_Invalid)
   {
      TinyDataSource_Write(dataSource, CapTouchDebugDeltasErd, deltas);
   }

   ArmPollTimer(dataSource, timerModule);
}

static void ArmPollTimer(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(
      timerModule,
      &pollTimer,
      CapTouchDebugPollPeriodMsec,
      Poll,
      dataSource);
}

void CapTouchDebugPlugin_Init(I_TinyDataSource_t *dataSource)
{
   ArmPollTimer(dataSource, TinyDataSource_ReadPointer(dataSource, CapTouchTimerModuleErd));
}
