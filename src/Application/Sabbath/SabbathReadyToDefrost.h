/*!
 * @file
 * @brief The module checks defrosting is ready in sabbath mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SABBATHREADYTODEFROST_H
#define SABBATHREADYTODEFROST_H

#include "Fsm.h"
#include "I_DataModel.h"
#include "SabbathData.h"
#include "Timer.h"
#include "EventSubscription.h"

typedef struct
{
   Erd_t timerModuleErd; // TimerModule_t *
   Erd_t waitingToDefrostErd; // bool
   Erd_t sabbathIsReadyToDefrostErd; // bool
   Erd_t sabbathTimeBetweenDefrostsInMinutesErd; // uint16_t
   Erd_t sabbathWaitingForDefrostTimeInMinutesErd; // uint16_t
} SabbathReadyToDefrostConfig_t;

typedef struct
{
   struct
   {
      Fsm_t fsm;
      Timer_t timer;
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;
      const SabbathData_t *sabbathData;
      const SabbathReadyToDefrostConfig_t *config;
   } _private;
} SabbathReadyToDefrost_t;

void SabbathReadyToDefrost_Init(
   SabbathReadyToDefrost_t *instance,
   I_DataModel_t *dataModel,
   const SabbathReadyToDefrostConfig_t *config,
   const SabbathData_t *sabbathData);

#endif
