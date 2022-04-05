/*!
 * @file
 * @brief definition for the valve position
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VALVEPOSITION_H
#define VALVEPOSITION_H

#include <stdint.h>

enum
{
   ValvePosition_A,
   ValvePosition_B,
   ValvePosition_C,
   ValvePosition_D,
   ValvePosition_E,
   ValvePosition_Home
};
typedef uint8_t ValvePosition_t;

typedef struct
{
   ValvePosition_t position;
   bool care;
} ValveVotedPosition_t;

#endif
