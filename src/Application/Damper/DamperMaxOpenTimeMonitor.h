/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERMAXOPENTIMEMONITOR_H
#define DAMPERMAXOPENTIMEMONITOR_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "Timer.h"
#include "DamperVotedPosition.h"
#include "SingleDamperData.h"

typedef struct
{
   Erd_t damperPositionMaxOpenTimeVoteErd; // DamperVotedPosition_t
   Erd_t damperCurrentPositionErd; // DamperPosition_t
} DamperMaxOpenTimeConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t maxOpenTimer;
      TimerTicks_t maxOpenTimerTicks;
      EventSubscription_t damperCurrentPositionSubscription;
      const DamperMaxOpenTimeConfiguration_t *configuration;
   } _private;
} DamperMaxOpenTime_t;

/*!
 * @param instance
 * @param dataModel
 */
void DamperMaxOpenTimeMonitor_Init(
   DamperMaxOpenTime_t *,
   I_DataModel_t *,
   const DamperMaxOpenTimeConfiguration_t *,
   const SingleDamperData_t *);

#endif
