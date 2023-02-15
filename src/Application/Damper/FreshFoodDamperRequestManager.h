/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODDAMPERREQUESTMANAGER_H
#define FRESHFOODDAMPERREQUESTMANAGER_H

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
} FreshFoodDamperRequestManagerConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      bool homingRequired;
      EventSubscription_t dataModelSubscription;
      const FreshFoodDamperRequestManagerConfiguration_t *configuration;
      const SingleDamperData_t *freshFoodDamperParametricData;
   } _private;
} FreshFoodDamperRequestManager_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void FreshFoodDamperRequestManager_Init(
   FreshFoodDamperRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const FreshFoodDamperRequestManagerConfiguration_t *config);

#endif
