/*!
 * @file
 * @brief Parametric data for the sensors
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SENSORDATA_H
#define SENSORDATA_H

#include "Mapper_LookupTable.h"
#include "Mapper_UnsignedSignedLookupTable.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
   bool slewRateFilterEnabled;
   int16_t slewRatePerSecondx100;
} SensorDataSlewRateData_t;

typedef struct
{
   Mapper_UnsignedSignedLookupTableConfiguration_t *lookupTable;
   int16_t fallbackValueDegFx100;
   uint8_t alphaNum;
   uint8_t alphaDenom;
   uint8_t windowSize;
   SensorDataSlewRateData_t clampData;
   uint16_t goodReadingCounterMax;
   uint16_t badReadingCounterMax;
   bool enableExternalSensorCheck;
   bool discoverable;
} SensorDataSensorType_t;

typedef struct
{
   SensorDataSensorType_t *freezerCabinetThermistor;
   SensorDataSensorType_t *freshFoodCabinetThermistor;
   SensorDataSensorType_t *iceCabinetThermistor;
   SensorDataSensorType_t *convertibleCompartmentAsFreezer;
   SensorDataSensorType_t *convertibleCompartmentAsFreshFood;
   SensorDataSensorType_t *freezerEvapThermistor;
   SensorDataSensorType_t *freshFoodEvapThermistor;
   SensorDataSensorType_t *ambientThermistor;
   SensorDataSensorType_t *ambientHumiditySensor;
   SensorDataSensorType_t *convertibleCompartmentEvapThermistor;
   SensorDataSensorType_t *iceMaker0MoldThermistor;
   SensorDataSensorType_t *iceMaker1MoldThermistor;
   SensorDataSensorType_t *iceMaker2MoldThermistor;
   uint16_t periodicUpdateRateInMs;
} SensorData_t;

#endif
