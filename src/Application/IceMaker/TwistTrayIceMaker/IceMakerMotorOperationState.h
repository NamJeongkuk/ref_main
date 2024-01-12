/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOTOROPERATIONSTATE_H
#define ICEMAKERMOTOROPERATIONSTATE_H

#include <stdint.h>

enum
{
   IceMakerMotorOperationState_Idle,
   IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome,
   IceMakerMotorOperationState_HomingJumpingOutOfHome,
   IceMakerMotorOperationState_HomingReadyToLandInHomePosition,
   IceMakerMotorOperationState_HarvestCheckingIfBucketIsFull,
   IceMakerMotorOperationState_HarvestBucketWasFoundToBeFull,
   IceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition,
   IceMakerMotorOperationState_HarvestUntwisting,
   IceMakerMotorOperationState_HarvestReadyToLandInHomePosition
};
typedef uint8_t IceMakerMotorOperationState_t;

#endif
