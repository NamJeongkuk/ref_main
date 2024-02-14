/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOTORREQUESTEDSTATE_H
#define ICEMAKERMOTORREQUESTEDSTATE_H

#include "Signal.h"
#include "IceMakerMotorVotedState.h"

typedef struct
{
   IceMakerMotorState_t state;
   Signal_t signal;
} IceMakerMotorRequestedState_t;

#endif
