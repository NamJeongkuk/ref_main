/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VOTEDPWMDUTYCYCLEPAIR_H
#define VOTEDPWMDUTYCYCLEPAIR_H

#include "Erd.h"

typedef struct
{
   Erd_t pwmDutyCycleResolvedVoteErd;
   Erd_t pwmDutyCycleErd;
} VotedPwmDutyCyclePair_t;

#endif
