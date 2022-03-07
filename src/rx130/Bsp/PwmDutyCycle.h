/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PWMDUTYCYCLE_H
#define PWMDUTYCYCLE_H

#include <stdint.h>

enum
{
   PwmDutyCycle_Min = 0,
   PwmDutyCycle_Max = UINT16_MAX
};
typedef uint16_t PwmDutyCycle_t;

#endif
