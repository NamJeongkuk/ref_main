/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERMOTORVOTEDSTATE_H
#define ALUMINUMMOLDICEMAKERMOTORVOTEDSTATE_H

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
} AluminumMoldIceMakerMotorVotedState_t;

#endif
