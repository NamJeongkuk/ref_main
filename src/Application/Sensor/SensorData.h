/*!
 * @file
 * @brief Parametric data for the sensors
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "Mapper_LookupTable.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
   bool slewRateEnabled;
   int16_t slewRatePerSecondx100;
} SensorDataSlewRateData_t;

typedef struct
{
   Mapper_LookupTableConfiguration_t *lookupTable;
   int16_t fallbackValueDegFx100;
   uint8_t alphaNum;
   uint8_t alphaDenom;
   uint8_t windowSize;
   SensorDataSlewRateData_t clampData;
   uint16_t goodReadingCounterMax;
   uint16_t badReadingCounterMax;
} SensorDataSensorType_t;

typedef struct
{
   SensorDataSensorType_t *fzCabinetThermistor;
   SensorDataSensorType_t *ffCabinetThermistor;
   SensorDataSensorType_t *fzEvapThermistor;
   SensorDataSensorType_t *ffEvapThermistor;
   SensorDataSensorType_t *ccCabinetThermistor;
   SensorDataSensorType_t *ambientThermistor;
   SensorDataSensorType_t *ccEvapThermistor;
   uint16_t periodicUpdateRate;
} SensorData_t;

#endif
