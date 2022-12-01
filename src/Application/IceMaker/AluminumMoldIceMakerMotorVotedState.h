/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERMOTORVOTEDSTATE_H
#define ALUMINUMMOLDICEMAKERMOTORVOTEDSTATE_H

#include <stdint.h>

enum
{
   MotorState_Off,
   MotorState_On
};
typedef uint8_t MotorState_t;

typedef struct
{
   MotorState_t state;
   bool care;
} AluminumMoldIceMakerMotorVotedState_t;

#endif
