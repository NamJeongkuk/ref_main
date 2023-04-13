/*!
 * @file
 * @brief Event that publishes every 1 ms when subscribed to
 * and stops publishing when unsubscribed from.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EVENT_ONEMILLISECONDTIMER_H
#define EVENT_ONEMILLISECONDTIMER_H

#include "I_Event.h"
#include "EventSubscription.h"
#include "Timer.h"

typedef struct
{
   I_Event_t interface;

   struct
   {
      EventSubscription_t *subscription;
      Timer_t periodicTimer;
      TimerModule_t *timerModule;
   } _private;
} Event_OneMillisecondTimer_t;

void Event_OneMillisecondTimer_Init(Event_OneMillisecondTimer_t *instance, TimerModule_t *timerModule);

#endif
