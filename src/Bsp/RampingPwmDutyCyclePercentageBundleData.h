/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAMPINGPWMDUTYCYCLEPERCENTAGEBUNDLEDATA_H
#define RAMPINGPWMDUTYCYCLEPERCENTAGEBUNDLEDATA_H

#include <stdint.h>
#include "PercentageDutyCycle.h"

typedef struct
{
   PercentageDutyCycle_t maxPwmDutyCyclePercentage;
   PercentageDutyCycle_t minPwmDutyCyclePercentage;
   uint16_t maxPwmRampingUpCountInMsec;
   uint16_t maxPwmRampingDownCountInMsec;
} RampingPwmDutyCyclePercentageBundleData_t;

#endif
