/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef STEPPERPOSITIONREQUEST_H
#define STEPPERPOSITIONREQUEST_H

#include <stdint.h>

enum
{
   TurningDirection_CounterClockwise,
   TurningDirection_Clockwise
};
typedef bool TurningDirection_t;

typedef struct
{
   TurningDirection_t direction;
   uint16_t stepsToMove;
   bool resetSubstep;
} StepperPositionRequest_t;

#define SWAP_DEFINITION_StepperPositionRequest_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, stepsToMove),

#endif
