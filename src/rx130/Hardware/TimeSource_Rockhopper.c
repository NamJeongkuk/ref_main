/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TimeSource_Rockhopper.h"
#include "utils.h"
#include "TimeSource_Interrupt.h"

static struct
{
   I_TimeSource_t interface;

   struct
   {
      TimeSource_Interrupt_t timeSourceInterrupt;
   } _private;
} instance;

static TimeSourceTickCount_t GetTicks()
{
   return TimeSource_GetTicks(&instance._private.timeSourceInterrupt.interface);
}

static const I_TimeSource_Api_t api = { GetTicks };

TimeSource_Interrupt_t *TimeSource_Rockhopper_Init(I_Interrupt_t *interrupt)
{
   instance.interface.api = &api;
   TimeSource_Interrupt_Init(&instance._private.timeSourceInterrupt, interrupt);

   return &instance._private.timeSourceInterrupt;
}
