/*!
 * @file
 * @brief Fan speed definition.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef FANSPEED_H
#define FANSPEED_H

#include <stdint.h>

enum
{
   FanSpeed_Off,
   FanSpeed_Min,
   FanSpeed_On,
   FanSpeed_Low = FanSpeed_On,
   FanSpeed_Medium,
   FanSpeed_High,
   FanSpeed_Max,
   FanSpeed_NumberOfSpeeds
};
typedef uint8_t FanSpeed_t;

typedef struct
{
   FanSpeed_t speed;
   bool care;
} FanVotedSpeed_t;

#endif
