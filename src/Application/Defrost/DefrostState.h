/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTSTATE_H
#define DEFROSTSTATE_H

#include <stdint.h>

enum
{
   DefrostState_Idle,
   DefrostState_Prechill,
   DefrostState_HeaterOn,
   DefrostState_Dwell,
   DefrostState_PostDwell,
   DefrostState_Disabled
};
typedef uint8_t DefrostState_t;

#endif
