/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorRequestManager.h"
#include "TwistTrayIceMakerMotorController.h"
#include "IceMakerMotorAction.h"
#include "IceMakerMotorDoAction.h"
#include "IceMakerMotorActionResult.h"
#include "IceMakerMotorOperationState.h"
#include "Constants_Binary.h"
#include "Timer.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

static void ClearMotorRequest(TwistTrayIceMakerMotorRequestManager_t *instance)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->motorRequestErd, clear);
}

static void UpdateMotorDoActionToResolvedVoteMotorAction(TwistTrayIceMakerMotorRequestManager_t *instance)
{
   TwistTrayIceMakerMotorVotedAction_t motorVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedVoteErd,
      &motorVote);

   IceMakerMotorDoAction_t doAction;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->motorDoActionErd,
      &doAction);

   doAction.action = motorVote.action;
   doAction.signal++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->motorDoActionErd,
      &doAction);
}

static void DataModelChanged(void *context, const void *_args)
{
   TwistTrayIceMakerMotorRequestManager_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->resolvedVoteErd)
   {
      const TwistTrayIceMakerMotorVotedAction_t *newVote = args->data;
      if(newVote->action != IceMakerMotorAction_Idle)
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->motorRequestErd,
            set);
      }
      else
      {
         // This allows the motor controller module to get the idle request and clear the motor action
         UpdateMotorDoActionToResolvedVoteMotorAction(instance);
      }
   }
   else if(args->erd == instance->_private.config->motorEnableErd)
   {
      const bool *enabled = args->data;
      if(*enabled)
      {
         UpdateMotorDoActionToResolvedVoteMotorAction(instance);
      }
   }
   else if(args->erd == instance->_private.config->motorActionResultErd)
   {
      const IceMakerMotorActionResult_t *result = args->data;
      if((*result == IceMakerMotorActionResult_MotorError) ||
         (*result == IceMakerMotorActionResult_Harvested) ||
         (*result == IceMakerMotorActionResult_BucketWasFull) ||
         (*result == IceMakerMotorActionResult_Homed))
      {
         ClearMotorRequest(instance);
      }
   }
}

void TwistTrayIceMakerMotorRequestManager_Init(
   TwistTrayIceMakerMotorRequestManager_t *instance,
   I_DataModel_t *dataModel,
   const TwistTrayIceMakerMotorRequestManagerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.dataModelEventSubscription,
      instance,
      DataModelChanged);
   Event_Subscribe(
      dataModel->OnDataChange,
      &instance->_private.dataModelEventSubscription);
}
