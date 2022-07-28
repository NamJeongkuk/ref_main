/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERSETPOINT_H
#define FREEZERSETPOINT_H

#include <stdint.h>

enum
{
   FreezerSetpoint_Cold,
   FreezerSetpoint_Medium,
   FreezerSetpoint_Warm,
};
typedef uint8_t FreezerSetpoint_t;

#endif
