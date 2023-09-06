/*!
 * @file
 * @brief Lighting door vote resolver module which takes in the states of doors and votes for a PwmVotedDutyCycle for a single
 * light whenever one of the doors opens and then when all doors are closed.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LIGHTINGDOORVOTERESOLVER_H
#define LIGHTINGDOORVOTERESOLVER_H

#include "I_DataModel.h"
#include "Timer.h"
#include "DoorLightingData.h"
#include "Fsm.h"

typedef struct
{
   Erd_t timerModuleErd; // TimerModule *
   Erd_t userAllowableLightStateErd; // UserAllowableInteriorLightState_t
   Erd_t rampingPwmDutyCyclePercentageErd; // RampingPwmDutyCyclePercentageVote_t
   const Erd_t *doorIsOpenErds; // bool[]
   uint8_t numberOfDoorErds;
} LightingDoorVoteResolverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const LightingDoorVoteResolverConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
      Timer_t maxDoorOpenTimer;
      Fsm_t fsm;
      const DoorLightingData_t *doorLightingData;
   } _private;
} LightingDoorVoteResolver_t;

void LightingDoorVoteResolver_Init(
   LightingDoorVoteResolver_t *instance,
   I_DataModel_t *dataModel,
   const LightingDoorVoteResolverConfig_t *config,
   const DoorLightingData_t *doorLightingData);

#endif
