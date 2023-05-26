/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorRequestManager.h"
#include "TwistTrayIceMakerMotorController.h"
#include "TwistTrayIceMakerMotorAction.h"
#include "TwistTrayIceMakerMotorDoAction.h"
#include "TwistTrayIceMakerMotorActionResult.h"
#include "TwistTrayIceMakerMotorOperationState.h"
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

   TwistTrayIceMakerMotorDoAction_t doAction;
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

static bool MotorIsEnabled(TwistTrayIceMakerMotorRequestManager_t *instance)
{
   bool motorIsEnabled;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->motorEnableErd,
      &motorIsEnabled);

   return motorIsEnabled;
}

static void DataModelChanged(void *context, const void *_args)
{
   TwistTrayIceMakerMotorRequestManager_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->resolvedVoteErd)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->motorRequestErd,
         set);
   }
   else if(args->erd == instance->_private.config->motorEnableErd)
   {
      const bool *enabled = args->data;
      if(*enabled)
      {
         UpdateMotorDoActionToResolvedVoteMotorAction(instance);
      }
   }
   else if(args->erd == instance->_private.config->motorActionResultErd && MotorIsEnabled(instance))
   {
      const TwistTrayIceMakerMotorActionResult_t *result = args->data;
      if((*result == TwistTrayIceMakerMotorActionResult_MotorError) ||
         (*result == TwistTrayIceMakerMotorActionResult_Harvested) ||
         (*result == TwistTrayIceMakerMotorActionResult_BucketWasFull) ||
         (*result == TwistTrayIceMakerMotorActionResult_Homed))
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
