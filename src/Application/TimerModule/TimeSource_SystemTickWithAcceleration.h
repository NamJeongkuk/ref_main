/*!
 * @file
 * @brief This module allows the user to request to accelerate the time source if enabled.
 *
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TIMESOURCE_SYSTEMTICKWITHACCELERATION_H
#define TIMESOURCE_SYSTEMTICKWITHACCELERATION_H

#include "EventSubscription.h"
#include "I_DataModel.h"
#include "I_TimeSource.h"
#include "I_Interrupt.h"

typedef struct
{
   Erd_t accelerationEnabledErd; // bool
   Erd_t ticksRemainingInAccelerationErd; // uint32_t
   Erd_t completeSignalErd; // Signal_t
   TimeSourceTickCount_t maxTicksPerRtc;
} TimeSource_SystemTickWithAcceleration_Config_t;

typedef struct
{
   I_TimeSource_t interface;

   struct
   {
      const TimeSource_SystemTickWithAcceleration_Config_t *config;
      I_Interrupt_t *sysTickInterrupt;
      I_DataModel_t *dataModel;
      EventSubscription_t sysTickSubscription;
      volatile TimeSourceTickCount_t ticks;
   } _private;
} TimeSource_SystemTickWithAcceleration_t;

/*!
 * @param instance
 * @param config
 * @param sysTickInterrupt
 */
void TimeSource_SystemTickWithAcceleration_Init(
   TimeSource_SystemTickWithAcceleration_t *instance,
   const TimeSource_SystemTickWithAcceleration_Config_t *config,
   I_Interrupt_t *sysTickInterrupt);

/*!
 * @param instance
 * @param dataModel
 */
void TimeSource_SystemTickWithAcceleration_SetDataModel(
   TimeSource_SystemTickWithAcceleration_t *instance,
   I_DataModel_t *dataModel);

/*!
 * @param instance
 */
void TimeSource_SystemTickWithAcceleration_Run(
   TimeSource_SystemTickWithAcceleration_t *instance);

#endif
