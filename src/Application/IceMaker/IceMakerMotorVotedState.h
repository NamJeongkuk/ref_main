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
   MotorState_Off,
   MotorState_On
};
typedef uint8_t MotorState_t;

typedef struct
{
   MotorState_t state;
   Vote_t care;
} IceMakerMotorVotedState_t;

#endif
