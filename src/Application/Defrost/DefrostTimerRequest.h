/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTIMERREQUEST_H
#define DEFROSTTIMERREQUEST_H

#include "Signal.h"
#include <stdint.h>

enum
{
   DefrostTimer_Reset,
   DefrostTimer_Enable,
   DefrostTimer_Disable
};
typedef uint8_t DefrostTimer_t;

typedef struct
{
   DefrostTimer_t request;
   Signal_t requestId;
} DefrostTimerRequest_t;

#endif
