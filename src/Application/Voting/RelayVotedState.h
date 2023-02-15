/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RELAYVOTEDSTATE_H
#define RELAYVOTEDSTATE_H
#include "Vote.h"

enum
{
   RelayState_Off,
   RelayState_On
};
typedef uint8_t RelayState_t;

typedef struct
{
   RelayState_t state;
   Vote_t care;
} RelayVotedState_t;

#endif
