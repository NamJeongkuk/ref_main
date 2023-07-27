/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAMPINGPWMDUTYCYCLEPERCENTAGEVOTE_H
#define RAMPINGPWMDUTYCYCLEPERCENTAGEVOTE_H

#include "RampingPwmDutyCyclePercentage.h"
#include "Vote.h"

typedef struct
{
   RampingPwmDutyCyclePercentage_t rampingPwmDutyCyclePercentage;
   Vote_t care;
} RampingPwmDutyCyclePercentageVote_t;

#endif
