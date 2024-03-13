/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYMOTOROUTPUT_TESTDOUBLE_H
#define TWISTTRAYMOTOROUTPUT_TESTDOUBLE_H

extern "C"
{
#include "TwistTrayIceMakerMotorState.h"
#include "I_Output.h"
}

typedef struct
{
   I_Output_t interface;
   TwistTrayIceMakerMotorState_t twistTrayMotorState;
} TwistTrayMotorOutput_TestDouble_t;

void TwistTrayMotorOutput_TestDouble_Init(TwistTrayMotorOutput_TestDouble_t *instance);

TwistTrayIceMakerMotorState_t TwistTrayMotorOutput_TestDouble_GetTwistTrayMotorOutputState(TwistTrayMotorOutput_TestDouble_t *instance);

#endif
