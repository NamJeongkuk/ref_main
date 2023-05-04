/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORERRORREASON_H
#define TWISTTRAYICEMAKERMOTORERRORREASON_H

#include <stdint.h>

enum
{
   TwistTrayIceMakerMotorErrorReason_None,
   TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome,
   TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome,
   TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingReadyToLandInHomePosition,
   TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHarvestStoppingAtFullTwistPosition,
   TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHarvestUntwisting,
   TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHarvestReadyToLandInHomePosition,
};
typedef uint8_t TwistTrayIceMakerMotorErrorReason_t;

#endif
