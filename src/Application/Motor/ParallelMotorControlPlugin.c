/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MotorDriverArbitrator.h"
#include "ParallelMotorControlPlugin.h"
#include "SystemErds.h"
#include "utils.h"

static const Erd_t requestErds[] = {
   Erd_IceMaker0_TwistMotorControlRequest,
   Erd_IceMaker1_TwistMotorControlRequest,
   Erd_IceMaker2_TwistMotorControlRequest
};

static const ErdLogicServiceConfigurationEntry_t entries[] = {
   { ErdLogicServiceOperator_Or,
      { requestErds, NUM_ELEMENTS(requestErds) },
      Erd_TwistIceMakerMotorControlRequest }
};

static const ErdLogicServiceConfiguration_t configuration = {
   entries,
   NUM_ELEMENTS(entries)
};

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

   ErdLogicService_Init(
      &instance->_private.erdLogicService,
      &configuration,
      DataModel_AsDataSource(dataModel));
}
