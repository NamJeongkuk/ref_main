/*!
 * @file
 * @brief Fan state definition.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef FANSTATE_H
#define FANSTATE_H

#include <stdint.h>

enum
{
   FanState_Off,
   FanState_On
};
typedef uint8_t FanState_t;

typedef struct
{
   FanState_t state;
   bool care;
} FanVotedState_t;

#endif
