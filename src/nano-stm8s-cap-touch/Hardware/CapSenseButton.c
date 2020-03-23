/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CapSenseButton.h"
#include "stm8s.h"
#include "utils.h"
#include "TinySingleErdHeartbeatStream.h"

enum
{
   PollPeriodMsec = 0
};

typedef struct
{
   I_TinyDataSource_t *dataSource;
   TinyTimer_t pollTimer;
} Instance_t;

static Instance_t instance;

static void StartTimer(TinyTimerModule_t *timerModule);

static void PollButtonState(void *context, struct TinyTimerModule_t *timerModule)
{
   IGNORE(context);

   if(TSLState == TSL_IDLE_STATE)
   {
      if(TSL_GlobalSetting.b.CHANGED)
      {
         TSL_GlobalSetting.b.CHANGED = 0;

         bool state = sSCKeyInfo[0].Setting.b.DETECTED;

         TinySingleErdHeartbeatStream_UpdateData(state);
      }

      TSL_GlobalSetting.b.NOISE = 0;
   }

   StartTimer(timerModule);
}

static void StartTimer(TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(timerModule, &instance.pollTimer, PollPeriodMsec, PollButtonState, NULL);
}

void CapSenseButton_Init(TinyTimerModule_t *timerModule)
{
   CLK->PCKENR1 |= (CLK_PCKENR1_TIM3 | CLK_PCKENR1_TIM4);

   TSL_Init();

   for(uint8_t i = 0; i < NUMBER_OF_SINGLE_CHANNEL_KEYS; i++)
   {
      sSCKeyInfo[i].Setting.b.IMPLEMENTED = 1;
      sSCKeyInfo[i].Setting.b.ENABLED = 1;
      sSCKeyInfo[i].DxSGroup = 0x01;
   }

   StartTimer(timerModule);
}
