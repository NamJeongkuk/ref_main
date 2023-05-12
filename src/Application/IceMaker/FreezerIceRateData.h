/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERICERATEDATA_H
#define FREEZERICERATEDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   uint8_t timeInMinutes;
   TemperatureDegFx100_t freezerSetpointInDegFx100;
   FanSpeed_t freezerEvaporatorFanSpeed;
} FreezerIceRateData_t;

#endif
