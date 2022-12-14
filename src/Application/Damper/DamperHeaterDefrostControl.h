/*!
 * @file
 * @brief writes to damper heater vote based on defrost heater defrost vote
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERHEATERDEFROSTCONTROL_H
#define DAMPERHEATERDEFROSTCONTROL_H

#include "DamperHeaterData.h"
#include "I_DataModel.h"
#include "Timer.h"

typedef struct
{
   Erd_t defrostHeaterVoteErd; // HeaterVotedState_t
   Erd_t damperHeaterDefrostHeaterSyncVoteErd; // PercentageDutyCycleVote_t
   Erd_t defrostHeaterStateErd; // bool
} DamperHeaterDefrostControlConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const DamperHeaterDefrostControlConfig_t *config;
      EventSubscription_t defrostHeaterVoteSubscription;
      EventSubscription_t defrostHeaterStateSubscription;
      const DamperHeaterData_t *damperHeaterParametricData;
      TimerModule_t *timerModule;
      Timer_t timer;
   } _private;
} DamperHeaterDefrostControl_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param timerModule
 * @param damperHeaterData
 */
void DamperHeaterDefrostControl_Init(
   DamperHeaterDefrostControl_t *instance,
   I_DataModel_t *dataModel,
   const DamperHeaterDefrostControlConfig_t *config,
   TimerModule_t *timerModule,
   const DamperHeaterData_t *damperHeaterData);

#endif
