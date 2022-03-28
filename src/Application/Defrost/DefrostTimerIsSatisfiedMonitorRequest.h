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
   DefrostTimerIsSatisfiedMonitor_Reset,
   DefrostTimerIsSatisfiedMonitor_Enable,
   DefrostTimerIsSatisfiedMonitor_Disable
};
typedef uint8_t DefrostTimerIsSatisfiedMonitor_t;

typedef struct
{
   DefrostTimerIsSatisfiedMonitor_t request;
   Signal_t requestId;
} DefrostTimerIsSatisfiedMonitorRequest_t;

#endif
