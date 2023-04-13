/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Event_OneMillisecondTimer.h"
#include "I_Event.h"
#include "uassert.h"
#include "utils.h"
#include "DataModelErdPointerAccess.h"

static void PublishEvent(void *context)
{
   Event_OneMillisecondTimer_t *instance = context;
   EventSubscription_Publish(instance->_private.subscription, NULL);
}

static void Subscribe(I_Event_t *_instance, EventSubscription_t *subscription)
{
   REINTERPRET(instance, _instance, Event_OneMillisecondTimer_t *);
   uassert(instance->_private.subscription == NULL);
   instance->_private.subscription = subscription;

   TimerModule_StartPeriodic(
      instance->_private.timerModule,
      &instance->_private.periodicTimer,
      1,
      PublishEvent,
      instance);
}

static void Unsubscribe(I_Event_t *_instance, EventSubscription_t *subscription)
{
   REINTERPRET(instance, _instance, Event_OneMillisecondTimer_t *);
   IGNORE(subscription);
   instance->_private.subscription = NULL;

   TimerModule_Stop(
      instance->_private.timerModule,
      &instance->_private.periodicTimer);
}

static const I_Event_Api_t api = { Subscribe, Unsubscribe };

void Event_OneMillisecondTimer_Init(Event_OneMillisecondTimer_t *instance, TimerModule_t *timerModule)
{
   instance->interface.api = &api;
   instance->_private.timerModule = timerModule;
   instance->_private.subscription = NULL;
}
