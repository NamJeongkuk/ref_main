/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORACCELERATIONCOUNTERFSMSTATE_H
#define DOORACCELERATIONCOUNTERFSMSTATE_H

#include <stdint.h>

enum
{
   DoorAccelerationCounterFsmState_Stop,
   DoorAccelerationCounterFsmState_Pause,
   DoorAccelerationCounterFsmState_Run
};
typedef uint8_t DoorAccelerationCounterFsmState_t;

#endif
