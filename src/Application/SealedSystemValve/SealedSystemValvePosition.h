/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SEALEDSYSTEMVALVEPOSITION_H
#define SEALEDSYSTEMVALVEPOSITION_H

#include <stdint.h>
#include "Vote.h"

enum
{
   SealedSystemValvePosition_Home,
   SealedSystemValvePosition_A,
   SealedSystemValvePosition_B,
   SealedSystemValvePosition_C,
   SealedSystemValvePosition_D
};
typedef uint8_t SealedSystemValvePosition_t;

typedef struct
{
   SealedSystemValvePosition_t position;
   Vote_t care;
} SealedSystemValveVotedPosition_t;

#endif
