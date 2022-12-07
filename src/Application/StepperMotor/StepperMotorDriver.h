/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef STEPPERMOTORDRIVER_H
#define STEPPERMOTORDRIVER_H

#include "Erd.h"
#include "I_DataModel.h"
#include "I_GpioGroup.h"
#include "SingleDamperData.h"

typedef struct
{
   Erd_t motorDriveA;
   Erd_t motorDriveB;
   Erd_t motorDriveABar;
   Erd_t motorDriveBBar;
   Erd_t motorDriveEnable;
} StepperMotorPins_t;

typedef struct
{
   Erd_t stepperMotorPositionRequestErd;
   const StepperMotorPins_t *pins;
} StepperMotorDriverConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const StepperMotorDriverConfiguration_t *config;
      I_GpioGroup_t *gpioGroup;
      EventSubscription_t erdChangeSubscription;
      EventSubscription_t stepEventSubscription;
      I_Event_t *stepEvent;
      uint8_t currentStepPosition;
      uint16_t stepsToRun;
      bool directionToTurn;
      uint8_t countBetweenSteps;
      const SingleDamperData_t *freshFoodDamperParametricData;
   } _private;
} StepperMotorDriver_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param gpioGroup
 * @param stepEvent
 */
void StepperMotorDriver_Init(
   StepperMotorDriver_t *instance,
   I_DataModel_t *dataModel,
   const StepperMotorDriverConfiguration_t *config,
   I_GpioGroup_t *gpioGroup,
   I_Event_t *stepEvent);

#endif
