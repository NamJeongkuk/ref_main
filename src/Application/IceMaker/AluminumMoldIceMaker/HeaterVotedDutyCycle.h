/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef HEATERVOTEDDUTYCYCLE_H
#define HEATERVOTEDDUTYCYCLE_H

#include "PercentageDutyCycle.h"
#include "Vote.h"

typedef struct
{
   PercentageDutyCycle_t dutyCycle;
   Vote_t care;
} HeaterVotedDutyCycle_t;

#endif
