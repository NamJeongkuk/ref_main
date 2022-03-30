/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTIMERISSATISFIEDMONITORREQUEST_H
#define DEFROSTTIMERISSATISFIEDMONITORREQUEST_H

#include "Signal.h"
#include <stdint.h>

enum
{
   DefrostTimerIsSatisfied_Reset,
   DefrostTimerIsSatisfied_Enable,
   DefrostTimerIsSatisfied_Disable
};
typedef uint8_t DefrostTimerIsSatisfied_t;

typedef struct
{
   DefrostTimerIsSatisfied_t request;
   Signal_t requestId;
} DefrostTimerIsSatisfiedMonitorRequest_t;

#endif
