/*!
 * @file
 * @brief Requests a damper full cycle when the damper
 * has been opened/closed for too long.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERIODICDAMPERCYCLING_H
#define PERIODICDAMPERCYCLING_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "Timer.h"
#include "DamperVotedPosition.h"
#include "DamperData.h"

typedef struct
{
   Erd_t damperCurrentPositionErd; // DamperPosition_t
   Erd_t damperFullCycleRequestErd; // bool
} PeriodicDamperCyclingConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t maxTimer;
      EventSubscription_t damperCurrentPositionSubscription;
      const PeriodicDamperCyclingConfiguration_t *configuration;
      const DamperData_t *damperData;
   } _private;
} PeriodicDamperCycling_t;

/*!
 * @param instance
 * @param dataModel
 * @param configuration
 * @param damperData
 */
void PeriodicDamperCycling_Init(
   PeriodicDamperCycling_t *instance,
   I_DataModel_t *dataModel,
   const PeriodicDamperCyclingConfiguration_t *configuration,
   const DamperData_t *damperData);

#endif
