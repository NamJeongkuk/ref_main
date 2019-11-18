/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapSenseButton.h"
#include "stm8s.h"
#include "utils.h"

enum
{
   PollPeriodMsec = 10
};

typedef struct
{
   I_TinyDataSource_t *dataSource;
   Erd_t buttonErd;
   TinyTimer_t pollTimer;
} Instance_t;

static Instance_t instance;

static void StartTimer(TinyTimerModule_t *timerModule);

static void PollButtonState(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);

   if((TSL_GlobalSetting.b.CHANGED) && (TSLState == TSL_IDLE_STATE))
   {
      TSL_GlobalSetting.b.CHANGED = 0;

      bool state = sSCKeyInfo[0].Setting.b.DETECTED;
      TinyDataSource_Write(instance.dataSource, instance.buttonErd, &state);
   }

   if((TSL_GlobalSetting.b.NOISE) && (TSLState == TSL_IDLE_STATE))
   {
      TSL_GlobalSetting.b.NOISE = 0;
   }

   StartTimer(timerModule);
}

static void StartTimer(TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(timerModule, &instance.pollTimer, PollPeriodMsec, PollButtonState, NULL);
}

void CapSenseButton_Init(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule, Erd_t buttonErd)
{
   instance.dataSource = dataSource;
   instance.buttonErd = buttonErd;

   TSL_Init();

   for(uint8_t i = 0; i < NUMBER_OF_SINGLE_CHANNEL_KEYS; i++)
   {
      sSCKeyInfo[i].Setting.b.IMPLEMENTED = 1;
      sSCKeyInfo[i].Setting.b.ENABLED = 1;
      sSCKeyInfo[i].DxSGroup = 0x01;
   }

   StartTimer(timerModule);
}
