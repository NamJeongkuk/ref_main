/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERMOTORACTION_H
#define TWISTTRAYICEMAKERMOTORACTION_H

#include <stdint.h>
#include "Vote.h"

enum
{
   TwistTrayIceMakerMotorAction_Idle,
   TwistTrayIceMakerMotorAction_RunHomingRoutine,
   TwistTrayIceMakerMotorAction_RunCycle
};
typedef uint8_t TwistTrayIceMakerMotorAction_t;

typedef struct
{
   TwistTrayIceMakerMotorAction_t action;
   Vote_t care;
} TwistTrayIceMakerMotorVotedAction_t;

#endif
