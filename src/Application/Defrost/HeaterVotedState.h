/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HEATERVOTEDSTATE_H
#define HEATERVOTEDSTATE_H

#include <stdint.h>
#include "Vote.h"

enum
{
   HeaterState_Off,
   HeaterState_On
};
typedef uint8_t HeaterState_t;

typedef struct
{
   HeaterState_t state;
   Vote_t care;
} HeaterVotedState_t;

#endif
