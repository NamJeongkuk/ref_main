/*!
 * @file
 * @brief Fan speed definition.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef FANSPEED_H
#define FANSPEED_H

#include <stdint.h>
#include "Vote.h"

enum
{
   FanSpeed_Off,
   FanSpeed_SuperLow,
   FanSpeed_Low,
   FanSpeed_On = FanSpeed_Low,
   FanSpeed_Medium,
   FanSpeed_High,
   FanSpeed_SuperHigh,
   FanSpeed_NumberOfSpeeds
};
typedef uint8_t FanSpeed_t;

typedef struct
{
   FanSpeed_t speed;
   Vote_t care;
} FanVotedSpeed_t;

#endif
