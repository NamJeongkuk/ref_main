/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MotorDriverArbitrator.h"
#include "ParallelMotorControlPlugin.h"
#include "SystemErds.h"

static const MotorDriverArbitratorConfiguration_t motorDriverArbitratorConfig = {
   .motor0ControlRequestErd = Erd_DamperStepperMotorControlRequest,
   .motor1ControlRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motor0DriveEnableErd = Erd_DamperStepperMotorDriveEnable,
   .motor1DriveEnableErd = Erd_TwistIceMakerMotorDriveEnable
};

void ParallelMotorControlPlugin_Init(ParallelMotorControlPlugin_t *instance, I_DataModel_t *dataModel)
{
   MotorDriverArbitrator_Init(
      &instance->_private.motorDriverArbitrator,
      dataModel,
      &motorDriverArbitratorConfig);
}
