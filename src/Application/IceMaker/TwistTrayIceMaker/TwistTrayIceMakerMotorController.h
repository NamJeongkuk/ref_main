/*!
 * @file This Module controls the Motor For The Twist Tray Ice Maker
 * The motor must run in the interrupt context
 * so that the motor movements can be precisely timed to the millisecond
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORCONTROLLER_H
#define TWISTTRAYICEMAKERMOTORCONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "I_Output.h"
#include "Fsm.h"
#include "TwistTrayIceMakerData.h"
#include "TwistTrayIceMakerMotorActionResult.h"
#include "TwistTrayIceMakerMotorAction.h"
#include "TwistTrayIceMakerMotorErrorReason.h"

enum
{
   Timer_MotorError = 0,
   Timer_GeneralPurpose,
   Timer_Max
};

typedef struct
{
   struct
   {
      I_Output_t *motorStateOutput;
      const TwistTrayIceMakerData_t *data;

      uint32_t timeCount[Timer_Max];
      uint32_t timerDuration[Timer_Max];
      FsmSignal_t timerSignal[Timer_Max];

      bool lastSwitchState : 1;
      bool initialized : 1;
      uint8_t lastMotorState : 6;

      uint8_t actionResult : 4;
      uint8_t harvestActionResult : 4;

      uint8_t operationState;
      TwistTrayIceMakerMotorErrorReason_t motorErrorReason;

      Fsm_t fsm;
   } _private;
} TwistTrayIceMakerMotorController_t;

/*!
 * Initializes the Twist Tray Ice Maker Motor Controller
 * @param instance
 * @param data
 * @param motorStateOutput to drive the motor
 */
void TwistTrayIceMakerMotorController_Init(
   TwistTrayIceMakerMotorController_t *instance,
   const TwistTrayIceMakerData_t *data,
   I_Output_t *motorStateOutput);

/*!
 * Runs the twist tray ice maker motor
 * This function is designed to be run in the interrupt context
 * @param instance
 */
void TwistTrayIceMakerMotorController_Run(TwistTrayIceMakerMotorController_t *instance);

/*!
 * Gives the state of the switch to the motor controller
 * It will decide if the data has changed
 * You can pass it whatever the switch is reading, regardless of
 *    whether or not the switch has just changed
 * @param instance
 * @param newSwitchState
 */
void TwistTrayIceMakerMotorController_UpdateSwitchState(
   TwistTrayIceMakerMotorController_t *instance,
   bool newSwitchState);

/*!
 * Tells the motor to do an action
 * @param instance
 * @param action the action that will be taken (either Home or Run Cycle)
 */
void TwistTrayIceMakerMotorController_DoAction(
   TwistTrayIceMakerMotorController_t *instance,
   TwistTrayIceMakerMotorAction_t action);

/*!
 * A client can call this function if they want the result of the latest action
 * it will give you `no result` if no action has been started
 * @param instance
 * @return the result of the action
 */
TwistTrayIceMakerMotorActionResult_t TwistTrayIceMakerMotorController_ActionResult(
   TwistTrayIceMakerMotorController_t *instance);

/*!
 * Returns the operation state of the motor, which is an enum defined in the source file
 * @param instance
 * @return the operation state of the motor
 */
uint8_t TwistTrayIceMakerMotorController_OperationState(
   TwistTrayIceMakerMotorController_t *instance);

/*!
 * Returns the motor error reason, containing the state in which the error occurred
 * @param instance
 * @return the error reason of the motor
 */
TwistTrayIceMakerMotorErrorReason_t TwistTrayIceMakerMotorController_MotorErrorReason(
   TwistTrayIceMakerMotorController_t *instance);

#endif
