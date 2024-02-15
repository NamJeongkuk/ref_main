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
#include "Timer.h"
#include "I_EventQueue.h"

typedef struct
{
   bool a;
   bool b;
   bool aBar;
   bool bBar;
} StepperMotorSubStep_t;

typedef struct
{
   const StepperMotorSubStep_t *subSteps;
   uint8_t numberOfSubSteps;
} StepperMotorSubStepConfiguration_t;

typedef struct
{
   Erd_t motorDriveA;
   Erd_t motorDriveB;
   Erd_t motorDriveABar;
   Erd_t motorDriveBBar;
} StepperMotorPins_t;

typedef struct
{
   Erd_t stepperMotorPositionRequestErd; // StepperPositionRequest_t
   Erd_t motorControlRequestErd; // bool
   Erd_t motorEnableErd; // bool

   const StepperMotorPins_t *pins;
   const StepperMotorSubStepConfiguration_t *subStepConfig;
} StepperMotorDriverConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t erdChangeSubscription;
      EventSubscription_t timeSourceEventSubscription;
      EventSubscription_t motorEnableSubscription;
      I_DataModel_t *dataModel;
      const StepperMotorDriverConfiguration_t *config;
      I_GpioGroup_t *gpioGroup;
      I_Event_t *timeSourceEvent;
      I_EventQueue_t *eventQueue;
      const uint8_t *numberOfEventsBetweenSteps;
      const uint16_t *numberOfEventsForExcitationDelay;
      uint16_t currentOverallStep;
      uint16_t stepsToRun;
      uint16_t excitationDelayStepCount;
      uint8_t countBetweenSteps;
      uint8_t currentSubStep;
      bool directionToTurn;
      bool excitationDelayActive;
      bool subStepResetRequested;
   } _private;
} StepperMotorDriver_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param gpioGroup
 * @param timeSourceEvent
 * @param interruptSafeEventQueue
 * @param numberOfEventsBetweenSteps
 * @param numberOfEventsForExcitationDelay
 */
void StepperMotorDriver_Init(
   StepperMotorDriver_t *instance,
   I_DataModel_t *dataModel,
   const StepperMotorDriverConfiguration_t *config,
   I_GpioGroup_t *gpioGroup,
   I_Event_t *timeSourceEvent,
   I_EventQueue_t *interruptSafeEventQueue,
   const uint8_t *numberOfEventsBetweenSteps,
   const uint16_t *numberOfEventsForExcitationDelay);

#endif
