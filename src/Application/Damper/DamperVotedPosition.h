/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERPOSITION_H
#define DAMPERPOSITION_H

#include <stdint.h>
#include "Vote.h"

enum
{
   DamperPosition_Closed,
   DamperPosition_Open
};
typedef uint8_t DamperPosition_t;

typedef struct
{
   DamperPosition_t position;
   Vote_t care;
} DamperVotedPosition_t;

#endif
