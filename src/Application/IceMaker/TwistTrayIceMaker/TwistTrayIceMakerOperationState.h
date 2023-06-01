/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKEROPERATIONSTATE_H
#define TWISTTRAYICEMAKEROPERATIONSTATE_H

#include <stdint.h>

enum
{
   TwistTrayIceMakerOperationState_Homing,
   TwistTrayIceMakerOperationState_Freeze,
   TwistTrayIceMakerOperationState_Harvesting,
   TwistTrayIceMakerOperationState_FillingTrayWithWater,
   TwistTrayIceMakerOperationState_BucketIsFull,
   TwistTrayIceMakerOperationState_MotorError,
   TwistTrayIceMakerOperationState_ThermistorFault,
   TwistTrayIceMakerOperationState_IdleFill
};
typedef uint8_t TwistTrayIceMakerOperationState_t;

#endif
