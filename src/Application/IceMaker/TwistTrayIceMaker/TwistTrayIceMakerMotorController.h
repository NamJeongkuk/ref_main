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
#include "I_DataModel.h"
#include "I_Output.h"
#include "Fsm.h"
#include "TwistTrayIceMakerData.h"
#include "IceMakerMotorActionResult.h"
#include "IceMakerMotorErrorReason.h"
#include "IceMakerMotorOperationState.h"

enum
{
   Timer_MotorError = 0,
   Timer_GeneralPurpose,
   Timer_Max
};

typedef struct
{
   Erd_t motorRequestedStateErd; // IceMakerMotorRequestedState_t
} TwistTrayIceMakerMotorControllerConfig_t;

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

      volatile IceMakerMotorActionResult_t motorActionResult;
      volatile IceMakerMotorActionResult_t harvestActionResult;
      volatile IceMakerMotorOperationState_t motorOperationState;
      volatile IceMakerMotorErrorReason_t motorErrorReason;

      Fsm_t fsm;
      I_DataModel_t *dataModel;
      EventSubscription_t motorRequestedStateSubscription;
      const TwistTrayIceMakerMotorControllerConfig_t *config;
   } _private;
} TwistTrayIceMakerMotorController_t;

/*!
 * Initializes the Twist Tray Ice Maker Motor Controller
 * @param instance
 * @param data
 * @param motorStateOutput to drive the motor
 * @param dataModel
 * @param config
 */
void TwistTrayIceMakerMotorController_Init(
   TwistTrayIceMakerMotorController_t *instance,
   const TwistTrayIceMakerData_t *data,
   I_Output_t *motorStateOutput,
   I_DataModel_t *dataModel,
   const TwistTrayIceMakerMotorControllerConfig_t *config);

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
 * A client can call this function if they want the result of the latest action
 * it will give you `no result` if no action has been started
 * @param instance
 * @return the result of the motor action
 */
IceMakerMotorActionResult_t TwistTrayIceMakerMotorController_MotorActionResult(
   TwistTrayIceMakerMotorController_t *instance);

/*!
 * Returns the operation state of the motor, which is an enum defined in the source file
 * @param instance
 * @return the operation state of the motor
 */
IceMakerMotorOperationState_t TwistTrayIceMakerMotorController_MotorOperationState(
   TwistTrayIceMakerMotorController_t *instance);

/*!
 * Returns the motor error reason, containing the state in which the error occurred
 * @param instance
 * @return the error reason of the motor
 */
IceMakerMotorErrorReason_t TwistTrayIceMakerMotorController_MotorErrorReason(
   TwistTrayIceMakerMotorController_t *instance);

#endif
