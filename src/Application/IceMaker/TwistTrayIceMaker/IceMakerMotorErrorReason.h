/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOTORERRORREASON_H
#define ICEMAKERMOTORERRORREASON_H

#include <stdint.h>

enum
{
   IceMakerMotorErrorReason_None,
   IceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome,
   IceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome,
   IceMakerMotorErrorReason_MotorStallsInStateHomingReadyToLandInHomePosition,
   IceMakerMotorErrorReason_MotorStallsInStateHarvestStoppingAtFullTwistPosition,
   IceMakerMotorErrorReason_MotorStallsInStateHarvestUntwisting,
   IceMakerMotorErrorReason_MotorStallsInStateHarvestReadyToLandInHomePosition,
};
typedef uint8_t IceMakerMotorErrorReason_t;

#endif
