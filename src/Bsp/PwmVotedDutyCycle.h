/*!
 * @file
 * @brief Pwm voted duty cycle definition
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PWMVOTEDDUTYCYCLE_H
#define PWMVOTEDDUTYCYCLE_H

#include "PwmDutyCycle.h"
#include "Vote.h"

typedef struct
{
   PwmDutyCycle_t pwmDutyCycle;
   Vote_t care;
} PwmVotedDutyCycle_t;

#endif
