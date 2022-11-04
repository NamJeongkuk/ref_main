/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTREQUEST_H
#define DEFROSTREQUEST_H

#include "Signal.h"

enum
{
   DefrostEnableRequest_Disable,
   DefrostEnableRequest_Enable,
};
typedef uint8_t DefrostEnableRequest_t;

typedef struct
{
   DefrostEnableRequest_t request;
   Signal_t requestId;
} DefrostRequest_t;

#endif
