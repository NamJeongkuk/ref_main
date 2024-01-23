/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERREQUESTMANAGER_H
#define DAMPERREQUESTMANAGER_H

#include "I_DataModel.h"
#include "Fsm.h"
#include "DamperVotedPosition.h"
#include "EventSubscription.h"
#include "StepperPositionRequest.h"
#include "SingleDamperData.h"

typedef struct
{
   Erd_t damperPositionRequestResolvedVoteErd;
   Erd_t damperStepperMotorPositionRequestErd;
   Erd_t damperHomingRequestErd;
   Erd_t damperCurrentPositionErd;
} DamperRequestManagerConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      bool homingRequired;
      EventSubscription_t dataModelSubscription;
      const DamperRequestManagerConfiguration_t *configuration;
      const SingleDamperData_t *damperData;
   } _private;
} DamperRequestManager_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param singleDamperData
 */
void DamperRequestManager_Init(
   DamperRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const DamperRequestManagerConfiguration_t *config,
   const SingleDamperData_t *singleDamperData);

#endif
