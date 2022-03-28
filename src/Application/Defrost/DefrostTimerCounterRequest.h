/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTIMERCOUNTERREQUEST_H
#define DEFROSTTIMERCOUNTERREQUEST_H

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
} DefrostTimerCounterRequest_t;

#endif
