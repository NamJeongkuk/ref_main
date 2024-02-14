/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOTORVOTEDSTATE_H
#define ICEMAKERMOTORVOTEDSTATE_H

#include <stdint.h>
#include "Vote.h"

enum
{
   IceMakerMotorState_Off,
   IceMakerMotorState_Run,
   IceMakerMotorState_Homing
};
typedef uint8_t IceMakerMotorState_t;

typedef struct
{
   IceMakerMotorState_t state;
   Vote_t care;
} IceMakerMotorVotedState_t;

#endif
