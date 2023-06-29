/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BOOLEANVOTEDSTATE_H
#define BOOLEANVOTEDSTATE_H

#include "Vote.h"

typedef struct
{
   bool state;
   Vote_t care;
} BooleanVotedState_t;

#endif
