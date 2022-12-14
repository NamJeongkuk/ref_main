/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERCENTAGEDUTYCYCLEVOTE_H
#define PERCENTAGEDUTYCYCLEVOTE_H

#include "Vote.h"
#include "PercentageDutyCycle.h"

typedef struct
{
   PercentageDutyCycle_t percentageDutyCycle;
   Vote_t care;
} PercentageDutyCycleVote_t;

#endif
