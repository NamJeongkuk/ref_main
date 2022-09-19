/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERPOSITION_H
#define DAMPERPOSITION_H

#include <stdint.h>

enum
{
   DamperPosition_Closed,
   DamperPosition_Open,
   DamperPosition_Home
};
typedef uint8_t DamperPosition_t;

typedef struct
{
   DamperPosition_t position;
   bool care;
} DamperVotedPosition_t;

#endif
