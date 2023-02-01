/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MotorDriverArbitrator.h"
#include "SideBySideParallelMotorControlPlugin.h"
#include "SystemErds.h"

static const MotorDriverArbitratorConfiguration_t motorDriverArbitratorConfig = {
   .motor0ControlRequestErd = Erd_FreshFoodDamperStepperMotorControlRequest,
   .motor1ControlRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motor0DriveEnableErd = Erd_FreshFoodDamperStepperMotorDriveEnable,
   .motor1DriveEnableErd = Erd_TwistIceMakerMotorDriveEnable
};

void SideBySideParallelMotorControlPlugin_Init(SideBySideParallelMotorControlPlugin_t *instance, I_DataModel_t *dataModel)
{
   MotorDriverArbitrator_Init(
      &instance->_private.motorDriverArbitrator,
      dataModel,
      &motorDriverArbitratorConfig);
}
