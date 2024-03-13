/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorRequestManager.h"
#include "TwistTrayIceMakerMotorController.h"
#include "IceMakerMotorRequestedState.h"
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

static bool MotorEnabled(TwistTrayIceMakerMotorRequestManager_t *instance)
{
   bool enable;
   DataModel_Read(
      instance->_private.dataModel, 
      instance->_private.config->motorEnableErd, 
      &enable);
   return enable;
}

static void UpdateMotorRequestedStateToResolvedVoteMotorState(TwistTrayIceMakerMotorRequestManager_t *instance)
{
   IceMakerMotorVotedState_t motorVote;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->resolvedVoteErd,
      &motorVote);

   IceMakerMotorRequestedState_t motorRequestedState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->motorRequestedStateErd,
      &motorRequestedState);

   motorRequestedState.state = motorVote.state;
   motorRequestedState.signal++;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->motorRequestedStateErd,
      &motorRequestedState);
}

static void DataModelChanged(void *context, const void *_args)
{
   TwistTrayIceMakerMotorRequestManager_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->resolvedVoteErd)
   {
      const IceMakerMotorVotedState_t *newVote = args->data;
      if(newVote->state != IceMakerMotorState_Off)
      {
         if(MotorEnabled(instance))
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->motorRequestErd,
               set);

            UpdateMotorRequestedStateToResolvedVoteMotorState(instance);
         }
         else
         {
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->motorRequestErd,
               set);
         }
      }
      else
      {
         // This allows the motor controller module to get the idle request and clear the motor action
         UpdateMotorRequestedStateToResolvedVoteMotorState(instance);
      }
   }
   else if(args->erd == instance->_private.config->motorEnableErd)
   {
      const bool *enabled = args->data;
      if(*enabled)
      {
         UpdateMotorRequestedStateToResolvedVoteMotorState(instance);
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
