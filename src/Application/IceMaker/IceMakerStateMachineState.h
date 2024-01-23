/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERSTATEMACHINESTATE_H
#define ICEMAKERSTATEMACHINESTATE_H

#include <stdint.h>

enum
{
   IceMakerStateMachineState_Global,
   IceMakerStateMachineState_Freeze,
   IceMakerStateMachineState_Harvest,
   IceMakerStateMachineState_HarvestFix,
   IceMakerStateMachineState_HarvestFault,
   IceMakerStateMachineState_Fill,
   IceMakerStateMachineState_IdleFill,
   IceMakerStateMachineState_ThermistorFault,
   IceMakerStateMachineState_Homing,
   IceMakerStateMachineState_BucketIsFull,
   IceMakerStateMachineState_MotorError
};
typedef uint8_t IceMakerStateMachineState_t;

#endif
