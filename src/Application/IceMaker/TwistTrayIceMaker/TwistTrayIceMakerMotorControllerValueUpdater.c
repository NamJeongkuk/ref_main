/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayIceMakerMotorControllerValueUpdater.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"

enum
{
   PollingPeriodInMsec = 150
};

static void PollMotorActionResult(void *context)
{
   TwistTrayIceMakerMotorControllerValueUpdater_t *instance = context;

   IceMakerMotorActionResult_t result =
      TwistTrayIceMakerMotorController_MotorActionResult(instance->_private.twistTrayMotorController);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->motorActionResultErd,
      &result);

   IceMakerMotorOperationState_t operationState =
      TwistTrayIceMakerMotorController_MotorOperationState(instance->_private.twistTrayMotorController);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->motorOperationStateErd,
      &operationState);

   IceMakerMotorErrorReason_t motorErrorReason =
      TwistTrayIceMakerMotorController_MotorErrorReason(instance->_private.twistTrayMotorController);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->motorErrorReasonErd,
      &motorErrorReason);
}

void TwistTrayIceMakerMotorControllerValueUpdater_Init(
   TwistTrayIceMakerMotorControllerValueUpdater_t *instance,
   I_DataModel_t *dataModel,
   TwistTrayIceMakerMotorController_t *twistTrayMotorController,
   const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t *config)
{
   instance->_private.twistTrayMotorController = twistTrayMotorController;
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, Erd_TimerModule),
      &instance->_private.periodicUpdateTimer,
      PollingPeriodInMsec,
      PollMotorActionResult,
      instance);
}
