/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODNONHEATEDCYCLEDEFROSTDATA_H
#define FRESHFOODNONHEATEDCYCLEDEFROSTDATA_H

#include "FanSpeed.h"

typedef struct
{
   bool fanSpeedCompressorOffEnable;
   bool valveChangeEnable;
   uint8_t defaultTimeValveChangeInMinutes;
   uint8_t valveChangeSlopeInMinutesX10PerDegF;
   uint8_t maxTimeValveChangeInMinutes;
   uint8_t minTimeValveChangeInMinutes;
   FanSpeed_t fanSpeedCompressorOff;
   FanSpeed_t fanSpeedValveChange;
} FreshFoodNonHeatedCycleDefrostData_t;

#endif
