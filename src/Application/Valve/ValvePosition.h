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
   ValvePos_A,
   ValvePos_B,
   ValvePos_C,
   ValvePos_D,
   ValvePos_E,
   ValvePos_Home
};
typedef uint8_t ValvePosition_t;

typedef struct
{
   ValvePosition_t position;
   bool care;
} ValveVotedPosition_t;

#endif
