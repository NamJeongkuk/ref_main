/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EVENTQUEUEINTERRUPTSAFEPLUGIN_H
#define EVENTQUEUEINTERRUPTSAFEPLUGIN_H

#include "EventQueue_InterruptSafe.h"

void EventQueueInterruptSafePlugin_Init(void);

EventQueue_InterruptSafe_t *EventQueueInterruptSafePlugin_GetInterruptSafeEventQueue(void);

I_EventQueue_t *EventQueueInterruptSafePlugin_GetEventQueueInterface(void);

#endif
