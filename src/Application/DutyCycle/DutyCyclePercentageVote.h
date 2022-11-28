/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DUTYCYCLEPERCENTAGEVOTE_H
#define DUTYCYCLEPERCENTAGEVOTE_H

#include <stdint.h>
#include "Vote.h"

typedef uint8_t DutyCyclePercentage_t;

typedef struct
{
   DutyCyclePercentage_t dutyCyclePercentage;
   Vote_t care;
} DutyCyclePercentageVote_t;

#endif
