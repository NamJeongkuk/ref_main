/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTOROPERATIONSTATE_H
#define TWISTTRAYICEMAKERMOTOROPERATIONSTATE_H

#include <stdint.h>

enum
{
   TwistTrayIceMakerMotorOperationState_Idle,
   TwistTrayIceMakerMotorOperationState_HomingMakingSureTheTrayIsHome,
   TwistTrayIceMakerMotorOperationState_HomingJumpingOutOfHome,
   TwistTrayIceMakerMotorOperationState_HomingReadyToLandInHomePosition,
   TwistTrayIceMakerMotorOperationState_HarvestCheckingIfBucketIsFull,
   TwistTrayIceMakerMotorOperationState_HarvestBucketWasFoundToBeFull,
   TwistTrayIceMakerMotorOperationState_HarvestStoppingAtFullTwistPosition,
   TwistTrayIceMakerMotorOperationStateHarvest_Untwisting,
   TwistTrayIceMakerMotorOperationStateHarvest_ReadyToLandInHomePosition
};
typedef uint8_t TwistTrayIceMakerMotorOperationState_t;

#endif
