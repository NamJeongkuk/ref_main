/*!
 * @file
 * @brief Load(s) are voted off when a door from an associated list
 * is opened for a parametrically defined time. After the time expires,
 * the loads will be voted don't care.
 * When the associated list of doors are closed before the time expires, the
 * loads will be voted don't care after a parametrically defined time.
 * If a new door opens from the associated list before the time expires, it
 * does not restart the time.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LOADOFFDOOROPEN_H
#define LOADOFFDOOROPEN_H

#include "ErdList.h"
#include "I_DataModel.h"
#include "LoadOffDoorOpenData.h"
#include "Fsm.h"
#include "Timer.h"

typedef struct
{
   Erd_t loadVoteErd;
   uint32_t offValue;
} LoadVotePair_t;

typedef struct
{
   const LoadVotePair_t *pairs;
   uint8_t numberOfPairs;
} CompartmentVoteErdList_t;

typedef struct
{
   CompartmentVoteErdList_t compartmentVoteErdList;
   ErdList_t doorStatusErdList;
   Erd_t timerModuleErd;
} LoadOffDoorOpenConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t timeoutTimer;
      EventSubscription_t onDataModelChangeSubscription;
      Fsm_t fsm;
      const LoadOffDoorOpenConfiguration_t *config;
      const LoadOffDoorOpenCompartmentData_t *data;
   } _private;
} LoadOffDoorOpen_t;

void LoadOffDoorOpen_Init(
   LoadOffDoorOpen_t *instance,
   I_DataModel_t *dataModel,
   const LoadOffDoorOpenCompartmentData_t *loadOffDoorOpenCompartmentData,
   const LoadOffDoorOpenConfiguration_t *config);

#endif
