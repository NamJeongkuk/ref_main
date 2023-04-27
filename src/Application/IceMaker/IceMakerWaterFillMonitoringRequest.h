/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERWATERFILLMONITORINGREQUEST_H
#define ICEMAKERWATERFILLMONITORINGREQUEST_H

#include <stdint.h>

enum
{
   IceMakerWaterFillMonitoringRequest_Stop,
   IceMakerWaterFillMonitoringRequest_Start,
   IceMakerWaterFillMonitoringRequest_Pause,
   IceMakerWaterFillMonitoringRequest_Resume
};
typedef uint8_t IceMakerWaterFillMonitoringRequest_t;

#endif
