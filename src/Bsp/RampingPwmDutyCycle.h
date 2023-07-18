/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAMPINGPWMDUTYCYCLE_H
#define RAMPINGPWMDUTYCYCLE_H

#include <stdint.h>
#include "PwmDutyCycle.h"

typedef struct
{
   PwmDutyCycle_t pwmDutyCycle;
   uint16_t rampingUpCountPerMsec;
   uint16_t rampingDownCountPerMsec;
} RampingPwmDutyCycle_t;

#define SWAP_DEFINITION_RampingPwmDutyCycle_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, pwmDutyCycle),               \
      SWAPPED_FIELD(_name, _type, rampingUpCountPerMsec),   \
      SWAPPED_FIELD(_name, _type, rampingDownCountPerMsec),

#endif
