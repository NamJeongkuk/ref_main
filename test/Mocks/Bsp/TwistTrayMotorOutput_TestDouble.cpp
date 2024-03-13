/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TwistTrayMotorOutput_TestDouble.h"
#include "utils.h"

static void Write(I_Output_t *_instance, const void *value)
{
   REINTERPRET(instance, _instance, TwistTrayMotorOutput_TestDouble_t *);
   REINTERPRET(newState, value, const TwistTrayIceMakerMotorState_t *);

   instance->twistTrayMotorState = *newState;
}

static const I_Output_Api_t api = { Write };

void TwistTrayMotorOutput_TestDouble_Init(TwistTrayMotorOutput_TestDouble_t *instance)
{
   instance->interface.api = &api;
   instance->twistTrayMotorState = TwistTrayIceMakerMotorState_Coasting;
}

TwistTrayIceMakerMotorState_t TwistTrayMotorOutput_TestDouble_GetTwistTrayMotorOutputState(TwistTrayMotorOutput_TestDouble_t *instance)
{
   return instance->twistTrayMotorState;
}
