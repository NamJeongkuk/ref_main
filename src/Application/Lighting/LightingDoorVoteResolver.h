/*!
 * @file
 * @brief Lighting door vote resolver module which takes in the states of doors and votes for a PwmVotedDutyCycle.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LIGHTINGDOORVOTERESOLVER_H
#define LIGHTINGDOORVOTERESOLVER_H

#include "I_DataModel.h"
#include "Timer.h"

typedef struct
{
   Erd_t timerModuleErd; // TimerModule *
   const Erd_t *doorIsOpenErds; // bool[]
   const Erd_t *pwmVotedDutyCycleErds; // PwmVotedDutyCycle_t[]
   uint8_t numberOfDoorErds;
   uint8_t numberOfPwmVotedDutyCycleErds;
} LightingDoorVoteResolverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const LightingDoorVoteResolverConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
      Timer_t maxDoorOpenTimer;
   } _private;
} LightingDoorVoteResolver_t;

void LightingDoorVoteResolver_Init(
   LightingDoorVoteResolver_t *instance,
   I_DataModel_t *dataModel,
   const LightingDoorVoteResolverConfig_t *config);

#endif
