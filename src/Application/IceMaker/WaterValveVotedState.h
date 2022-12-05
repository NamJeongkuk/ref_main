/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERVALVEVOTEDSTATE_H
#define WATERVALVEVOTEDSTATE_H

#include <stdint.h>
#include "Vote.h"

enum
{
   WaterValveState_Off,
   WaterValveState_On
};
typedef uint8_t WaterValveState_t;

typedef struct
{
   WaterValveState_t state;
   Vote_t care;
} WaterValveVotedState_t;

#endif
