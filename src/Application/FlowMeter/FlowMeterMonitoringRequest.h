/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FLOWMETERMONITORINGREQUEST_H
#define FLOWMETERMONITORINGREQUEST_H

#include <stdint.h>

enum
{
   FlowMeterMonitoringRequest_Stop,
   FlowMeterMonitoringRequest_Start,
   FlowMeterMonitoringRequest_Pause,
   FlowMeterMonitoringRequest_Resume
};
typedef uint8_t FlowMeterMonitoringRequest_t;

#endif
