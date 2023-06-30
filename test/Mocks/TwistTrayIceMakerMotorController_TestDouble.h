/*!
 * @file
 * @brief Test double implementation of twist tray ice maker motor controller
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef TWISTTRAYICEMAKERMOTORCONTROLLER_TESTDOUBLE_H
#define TWISTTRAYICEMAKERMOTORCONTROLLER_TESTDOUBLE_H

extern "C"
{
#include "TwistTrayIceMakerMotorController.h"
}

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

      TwistTrayIceMakerMotorActionResult_t motorActionResult;
      TwistTrayIceMakerMotorActionResult_t harvestActionResult;
      TwistTrayIceMakerMotorOperationState_t motorOperationState;
      TwistTrayIceMakerMotorErrorReason_t motorErrorReason;

      Fsm_t fsm;
      I_DataModel_t *dataModel;
      EventSubscription_t doActionSubscription;
      const TwistTrayIceMakerMotorControllerConfig_t *config;
   } _private;
} TwistTrayIceMakerMotorController_TestDouble_t;

/*!
 * Update the motor action result
 * @param instance
 * @param actionResult
 */
void TwistTrayIceMakerMotorController_TestDouble_UpdateMotorActionResult(
   TwistTrayIceMakerMotorController_TestDouble_t *instance,
   TwistTrayIceMakerMotorActionResult_t actionResult);

/*!
 * Update the motor operation state
 * @param instance
 * @param motorOperationState
 */
void TwistTrayIceMakerMotorController_TestDouble_UpdateMotorOperationState(
   TwistTrayIceMakerMotorController_TestDouble_t *instance,
   TwistTrayIceMakerMotorOperationState_t motorOperationState);

/*!
 * Update the motor error reason
 * @param instance
 * @param motorErrorReason
 */
void TwistTrayIceMakerMotorController_TestDouble_UpdateMotorErrorReason(
   TwistTrayIceMakerMotorController_TestDouble_t *instance,
   TwistTrayIceMakerMotorErrorReason_t motorErrorReason);

/*!
 * Initialize the test double
 * @param instance
 */
void TwistTrayIceMakerMotorController_TestDouble_Init(
   TwistTrayIceMakerMotorController_TestDouble_t *instance);

#endif
