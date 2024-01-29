/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOTORACTION_H
#define ICEMAKERMOTORACTION_H

#include <stdint.h>
#include "Vote.h"

enum
{
   IceMakerMotorAction_Idle,
   IceMakerMotorAction_RunHomingRoutine,
   IceMakerMotorAction_RunCycle
};
typedef uint8_t IceMakerMotorAction_t;

typedef struct
{
   IceMakerMotorAction_t action;
   Vote_t care;
} IceMakerTwistMotorVotedAction_t;

#endif
