/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORACCELERATIONREQUEST_H
#define DOORACCELERATIONREQUEST_H

#include "Signal.h"
#include <stdint.h>

enum
{
   DoorAcceleration_Reset,
   DoorAcceleration_Enable,
   DoorAcceleration_Disable
};
typedef uint8_t DoorAcceleration_t;

typedef struct
{
   DoorAcceleration_t request;
   Signal_t requestId;
} DoorAccelerationRequest_t;

#endif
