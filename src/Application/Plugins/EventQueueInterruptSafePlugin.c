/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "EventQueueInterruptSafePlugin.h"
#include "uassert.h"

static struct
{
   EventQueue_InterruptSafe_t eventQueue;
   uint8_t eventQueueBuffer[19]; // Minimum amount of RAM required to queue a callback with data
} instance;

static void UnableToQueueEvent(void)
{
   uassert(!"Event queue full");
}

void EventQueueInterruptSafePlugin_Init(void)
{
   EventQueue_InterruptSafe_Init(
      &instance.eventQueue,
      instance.eventQueueBuffer,
      sizeof(instance.eventQueueBuffer),
      UnableToQueueEvent);
}

EventQueue_InterruptSafe_t *EventQueueInterruptSafePlugin_GetInterruptSafeEventQueue(void)
{
   return &instance.eventQueue;
}

I_EventQueue_t *EventQueueInterruptSafePlugin_GetEventQueueInterface(void)
{
   return &instance.eventQueue.interface;
}
