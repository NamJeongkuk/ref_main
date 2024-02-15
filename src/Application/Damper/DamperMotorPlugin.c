/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataModelErdPointerAccess.h"
#include "DataSource_Gpio.h"
#include "DamperMotorPlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "EventQueueInterruptSafePlugin.h"

static const uint16_t noExcitationDelay = 0;

static const StepperMotorPins_t damperPins = {
   .motorDriveA = Erd_BspGpio_MTR_DRV_00,
   .motorDriveB = Erd_BspGpio_MTR_DRV_02,
   .motorDriveABar = Erd_BspGpio_MTR_DRV_01,
   .motorDriveBBar = Erd_BspGpio_MTR_DRV_03
};

static const StepperMotorSubStep_t subSteps[] = {
   { ON, ON, OFF, OFF },
   { OFF, ON, ON, OFF },
   { OFF, OFF, ON, ON },
   { ON, OFF, OFF, ON }
};

static const StepperMotorSubStepConfiguration_t motorSubStepConfig = {
   .subSteps = subSteps,
   .numberOfSubSteps = NUM_ELEMENTS(subSteps)
};

static const StepperMotorDriverConfiguration_t driverConfig = {
   .stepperMotorPositionRequestErd = Erd_DamperStepperMotorPositionRequest,
   .motorControlRequestErd = Erd_DamperStepperMotorControlRequest,
   .motorEnableErd = Erd_DamperStepperMotorDriveEnable,
   .pins = &damperPins,
   .subStepConfig = &motorSubStepConfig
};

void DamperMotorPlugin_Init(DamperMotorPlugin_t *instance, I_DataModel_t *dataModel)
{
   StepperMotorDriver_Init(
      &instance->_private.stepperMotorDriver,
      dataModel,
      &driverConfig,
      DataModelErdPointerAccess_GetGpioGroup(dataModel, Erd_GpioGroupInterface),
      (DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_FastTickInterrupt))->OnInterrupt,
      EventQueueInterruptSafePlugin_GetEventQueueInterface(),
      &PersonalityParametricData_Get(dataModel)->damperData->delayBetweenStepEventsInHundredsOfMicroseconds,
      &noExcitationDelay);
}
