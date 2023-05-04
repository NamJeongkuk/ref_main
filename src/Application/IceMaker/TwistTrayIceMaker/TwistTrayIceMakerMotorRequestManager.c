/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorRequestManager.h"
#include "TwistTrayIceMakerMotorController.h"
#include "TwistTrayIceMakerMotorAction.h"
#include "TwistTrayIceMakerMotorActionResult.h"
#include "TwistTrayIceMakerMotorOperationState.h"
#include "Constants_Binary.h"
#include "Timer.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"

enum
{
   PollingFrequencyInMsec = 150
};

static void ClearMotorRequestAndStopTimer(TwistTrayIceMakerMotorRequestManager_t *instance)
{
   DataModel_Write(instance->_private.dataModel, instance->_private.config->motorRequestErd, clear);

   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.motorPollingTimer);
}

static void PollMotorActionResult(void *context)
{
   TwistTrayIceMakerMotorRequestManager_t *instance = context;
   TwistTrayIceMakerMotorActionResult_t result =
      TwistTrayIceMakerMotorController_ActionResult(instance->_private.motorController);
   TwistTrayIceMakerMotorOperationState_t state =
      TwistTrayIceMakerMotorController_OperationState(instance->_private.motorController);

   DataModel_Write(instance->_private.dataModel, instance->_private.config->motorActionResultErd, &result);
   DataModel_Write(instance->_private.dataModel, instance->_private.config->motorOperationStateErd, &state);

   if(result == TwistTrayIceMakerMotorActionResult_MotorError)
   {
      TwistTrayIceMakerMotorOperationState_t reason =
         TwistTrayIceMakerMotorController_MotorErrorReason(instance->_private.motorController);
      DataSource_Write(
         instance->_private.dataModel,
         Erd_TwistTrayIceMaker_MotorErrorReason,
         &reason);

      ClearMotorRequestAndStopTimer(instance);
   }
   else if(result == TwistTrayIceMakerMotorActionResult_Harvested ||
      result == TwistTrayIceMakerMotorActionResult_BucketWasFull ||
      result == TwistTrayIceMakerMotorActionResult_Homed)
   {
      ClearMotorRequestAndStopTimer(instance);
   }
}

static void StartMotorActionResultPolling(TwistTrayIceMakerMotorRequestManager_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.motorPollingTimer,
      PollingFrequencyInMsec,
      PollMotorActionResult,
      instance);
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
         TwistTrayIceMakerMotorVotedAction_t motorVote;
         DataModel_Read(instance->_private.dataModel, instance->_private.config->resolvedVoteErd, &motorVote);
         TwistTrayIceMakerMotorController_DoAction(instance->_private.motorController, motorVote.action);

         StartMotorActionResultPolling(instance);
      }
   }
}

void TwistTrayIceMakerMotorRequestManager_Init(
   TwistTrayIceMakerMotorRequestManager_t *instance,
   I_DataModel_t *dataModel,
   TwistTrayIceMakerMotorController_t *motorController,
   const TwistTrayIceMakerMotorRequestManagerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.motorController = motorController;

   EventSubscription_Init(&instance->_private.dataModelEventSubscription, instance, DataModelChanged);
   Event_Subscribe(dataModel->OnDataChange, &instance->_private.dataModelEventSubscription);
}
