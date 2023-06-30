/*!
 * @file
 * @brief Test Double implementation of a twist tray ice maker
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

extern "C"
{
#include "TwistTrayIceMakerMotorState.h"
#include "utils.h"
}

#include "CppUTestExt/MockSupport.h"
#include "TwistTrayIceMakerMotorController_TestDouble.h"

void TwistTrayIceMakerMotorController_TestDouble_UpdateMotorActionResult(
   TwistTrayIceMakerMotorController_TestDouble_t *instance, TwistTrayIceMakerMotorActionResult_t actionResult)
{
   instance->_private.motorActionResult = actionResult;
}

void TwistTrayIceMakerMotorController_TestDouble_UpdateMotorOperationState(
   TwistTrayIceMakerMotorController_TestDouble_t *instance, TwistTrayIceMakerMotorOperationState_t motorOperationState)
{
   instance->_private.motorOperationState = motorOperationState;
}

void TwistTrayIceMakerMotorController_TestDouble_UpdateMotorErrorReason(
   TwistTrayIceMakerMotorController_TestDouble_t *instance, TwistTrayIceMakerMotorErrorReason_t motorErrorReason)
{
   instance->_private.motorErrorReason = motorErrorReason;
}

void TwistTrayIceMakerMotorController_TestDouble_Init(
   TwistTrayIceMakerMotorController_TestDouble_t *instance)
{
   IGNORE(instance);
}
