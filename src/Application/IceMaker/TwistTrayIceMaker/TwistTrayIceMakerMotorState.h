/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORSTATE_H
#define TWISTTRAYICEMAKERMOTORSTATE_H

#include <stdint.h>
#include "Vote.h"

enum
{
   TwistTrayIceMakerMotorState_Coasting, // Idle
   TwistTrayIceMakerMotorState_Untwisting, // Forward
   TwistTrayIceMakerMotorState_Twisting, // Reverse
   TwistTrayIceMakerMotorState_Braking // Brake
};
typedef uint8_t TwistTrayIceMakerMotorState_t;

#endif
