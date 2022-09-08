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
   Clockwise = true,
   CounterClockwise = false
};
typedef bool TurningDirection_t;

typedef struct
{
   TurningDirection_t direction;
   uint16_t stepsToMove;
} StepperPositionRequest_t;

#endif
