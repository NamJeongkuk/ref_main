/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SEALEDSYSTEMVALVEREQUESTMANAGER_H
#define SEALEDSYSTEMVALVEREQUESTMANAGER_H

#include "SealedSystemValveData.h"
#include "I_DataModel.h"
#include "Erd.h"
#include "Fsm.h"
#include "EventSubscription.h"
#include "SealedSystemValvePosition.h"

typedef struct
{
   Erd_t positionRequestResolvedVoteErd; // SealedSystemValveVotedPosition_t
   Erd_t stepperMotorPositionRequestErd; // StepperPositionRequest_t
   Erd_t homingRequestErd; // bool
   Erd_t currentPositionErd; // SealedSystemValvePosition_t
} SealedSystemValveRequestManagerConfiguration_t;

typedef struct
{
   struct
   {
      Fsm_t fsm;
      EventSubscription_t dataModelSubscription;
      const SealedSystemValveRequestManagerConfiguration_t *config;
      const SealedSystemValveData_t *data;
      I_DataModel_t *dataModel;
      SealedSystemValvePosition_t activePositionRequest;
      bool homingRequired;
   } _private;
} SealedSystemValveRequestManager_t;

void SealedSystemValveRequestManager_Init(
   SealedSystemValveRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const SealedSystemValveRequestManagerConfiguration_t *config,
   const SealedSystemValveData_t *data);

#endif
