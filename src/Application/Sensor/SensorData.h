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

typedef struct
{
   Mapper_LookupTableConfiguration_t *lookupTable;
   int16_t fallbackValueDegFx100;
   uint8_t alphaNum;
   uint8_t alphaDenom;
   uint8_t windowSize;
   int16_t slewRateDegFx100PerSecond;
   uint16_t goodReadingCounterMax;
   uint16_t badReadingCounterMax;
} SensorType_t;

typedef struct
{
   SensorType_t *fzCabinetThermistor;
   SensorType_t *ffCabinetThermistor;
   SensorType_t *fzEvapThermistor;
   SensorType_t *ffEvapThermistor;
   SensorType_t *ccThermistor;
   SensorType_t *ambientThermistor;
   SensorType_t *ccEvapThermistor;
   uint16_t periodicUpdateRate;
} SensorData_t;

#endif
