/*!
 * @file
 * @brief Parametric data structure for Variable sweat heater
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VARIABLESWEATHEATERDATA_H
#define VARIABLESWEATHEATERDATA_H

#include <stdint.h>
#include "PercentageDutyCycle.h"
#include "HeaterControlType.h"

typedef struct
{
   PercentageDutyCycle_t variableHeaterFallbackDutyCycleInPercent;
   HeaterControlType_t heaterControlType;
   int32_t temperatureCoefficient;
   int32_t humidityCoefficient;
   int32_t freshFoodCoefficient;
   int32_t freezerCoefficient;
   int16_t temperatureSquaredCoefficient;
   int16_t humiditySquaredCoefficient;
   int16_t freshFoodSquaredCoefficient;
   int16_t freezerSquaredCoefficient;
   int16_t temperatureHumidityCoefficient;
   int16_t temperatureFreshFoodCoefficient;
   int16_t temperatureFreezerCoefficient;
   int16_t humidityFreshFoodCoefficient;
   int16_t humidityFreezerCoefficient;
   int16_t freshFoodFreezerCoefficient;
   int32_t interceptCoefficient;
} VariableSweatHeaterData_t;

#endif
