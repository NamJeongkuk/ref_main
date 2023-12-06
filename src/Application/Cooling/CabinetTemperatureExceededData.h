/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CABINETTEMPERATUREEXCEEDEDDATA_H
#define CABINETTEMPERATUREEXCEEDEDDATA_H

#include "TemperatureDegFx100.h"
#include <stdint.h>

typedef struct
{
   TemperatureDegFx100_t temperatureUpperHysteresisLimitAboveSetpointInDegFX100;
   TemperatureDegFx100_t temperatureLowerHysteresisLimitAboveSetpointInDegFX100;
   uint16_t minimumTimeSincePowerOnInMinutes;
   uint8_t minimumTimeDoorsMustBeClosedInMinutes;
   uint8_t minimumTimeSinceDefrostHasCompletedInMinutes;
} CabinetTemperatureExceededData_t;

#endif
