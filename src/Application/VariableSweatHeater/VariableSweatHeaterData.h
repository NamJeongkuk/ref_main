/*!
 * @file
 * @brief Parametric data structure for Variable sweat heater
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VARIABLESWEATHEATERDATA_H
#define VARIABLESWEATHEATERDATA_H

#include "HeaterControlType.h"
#include "PercentageDutyCycle.h"
#include <stdint.h>

typedef struct
{
   int32_t temperatureCoefficient;
   int16_t temperatureSquaredCoefficient;
   int32_t interceptCoefficient;
} VariableSweatHeaterDataType1_t;

typedef struct
{
   int32_t temperatureCoefficient;
   int32_t humidityCoefficient;
   int16_t temperatureSquaredCoefficient;
   int16_t humiditySquaredCoefficient;
   int16_t temperatureHumidityCoefficient;
   int32_t interceptCoefficient;
} VariableSweatHeaterDataType2_t;

typedef struct
{
   int32_t temperatureCoefficient;
   int32_t freshFoodCoefficient;
   int16_t temperatureSquaredCoefficient;
   int16_t freshFoodSquaredCoefficient;
   int16_t temperatureFreshFoodCoefficient;
   int32_t interceptCoefficient;
} VariableSweatHeaterDataType3_t;

typedef struct
{
   int32_t temperatureCoefficient;
   int32_t freezerCoefficient;
   int16_t temperatureSquaredCoefficient;
   int16_t freezerSquaredCoefficient;
   int16_t temperatureFreezerCoefficient;
   int32_t interceptCoefficient;
} VariableSweatHeaterDataType4_t;

typedef struct
{
   int32_t temperatureCoefficient;
   int32_t humidityCoefficient;
   int32_t freshFoodCoefficient;
   int16_t temperatureSquaredCoefficient;
   int16_t humiditySquaredCoefficient;
   int16_t freshFoodSquaredCoefficient;
   int16_t temperatureHumidityCoefficient;
   int16_t temperatureFreshFoodCoefficient;
   int16_t humidityFreshFoodCoefficient;
   int32_t interceptCoefficient;
} VariableSweatHeaterDataType5_t;

typedef struct
{
   int32_t temperatureCoefficient;
   int32_t humidityCoefficient;
   int32_t freezerCoefficient;
   int16_t temperatureSquaredCoefficient;
   int16_t humiditySquaredCoefficient;
   int16_t freezerSquaredCoefficient;
   int16_t temperatureHumidityCoefficient;
   int16_t temperatureFreezerCoefficient;
   int16_t humidityFreezerCoefficient;
   int32_t interceptCoefficient;
} VariableSweatHeaterDataType6_t;

typedef struct
{
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
} VariableSweatHeaterDataType7_t;

typedef struct
{
   HeaterControlType_t heaterControlType;
   PercentageDutyCycle_t variableHeaterFallbackDutyCycleInPercent;

   union
   {
      const VariableSweatHeaterDataType1_t *heaterType1Data;
      const VariableSweatHeaterDataType2_t *heaterType2Data;
      const VariableSweatHeaterDataType3_t *heaterType3Data;
      const VariableSweatHeaterDataType4_t *heaterType4Data;
      const VariableSweatHeaterDataType5_t *heaterType5Data;
      const VariableSweatHeaterDataType6_t *heaterType6Data;
      const VariableSweatHeaterDataType7_t *heaterType7Data;
   };
} VariableSweatHeaterData_t;

#endif
