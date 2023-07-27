/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAMPINGPWMDUTYCYCLEPERCENTAGE_H
#define RAMPINGPWMDUTYCYCLEPERCENTAGE_H

#include <stdint.h>
#include "PercentageDutyCycle.h"

typedef struct
{
   PercentageDutyCycle_t pwmDutyCyclePercentage;
   uint8_t rampingUpCountInMsec;
   uint8_t rampingDownCountInMsec;
} RampingPwmDutyCyclePercentage_t;

#endif
