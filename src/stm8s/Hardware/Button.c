/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Button.h"
#include "stm8s.h"
#include "utils.h"

enum
{
   Pin5 = (1 << 5),
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

   bool state = GPIOB->IDR & Pin5;
   TinyDataSource_Write(instance.dataSource, instance.buttonErd, &state);

   StartTimer(timerModule);
}

static void StartTimer(TinyTimerModule_t *timerModule)
{
   TinyTimerModule_Start(timerModule, &instance.pollTimer, PollPeriodMsec, PollButtonState, NULL);
}

void Button_Init(I_TinyDataSource_t *dataSource, TinyTimerModule_t *timerModule, Erd_t buttonErd)
{
   instance.dataSource = dataSource;
   instance.buttonErd = buttonErd;

   StartTimer(timerModule);
}
