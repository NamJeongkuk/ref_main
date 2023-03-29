/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PWMFREQUENCY_H
#define PWMFREQUENCY_H

#include <stdint.h>

enum
{
   PwmFrequency_Min = 0,
   PwmFrequency_Max = UINT16_MAX
};
typedef uint16_t PwmFrequency_t;

#endif