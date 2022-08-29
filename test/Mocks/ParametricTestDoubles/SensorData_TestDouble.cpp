/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SensorData_TestDouble.h"
#include <string.h>
#include "utils.h"

static const Mapper_LookupTableMapping_t mappingsForFilter[] = {
   { 0, 30000 },
   { 5375, 30000 },
   { 5376, -3981 },
   { 7360, -3032 },
   { 9728, -2117 },
   { 12480, -1232 },
   { 15552, -376 },
   { 19072, 496 },
   { 22656, 1314 },
   { 22780, 1320 },
   { 26432, 2137 },
   { 30336, 2977 },
   { 34176, 3812 },
   { 38016, 4681 },
   { 41664, 5564 },
   { 42560, 5787 },
   { 43456, 6023 },
   { 44352, 6270 },
   { 45248, 6518 },
   { 46144, 6765 },
   { 47040, 7041 },
   { 47936, 7320 },
   { 48512, 7500 },
   { 49280, 7746 },
   { 50048, 8022 },
   { 50816, 8297 },
   { 51520, 8550 },
   { 52288, 8862 },
   { 53056, 9182 },
   { 53824, 9503 },
   { 54272, 9723 },
   { 55424, 10289 },
   { 56576, 10938 },
   { 58432, 12150 },
   { 59968, 13468 },
   { 60928, 14496 },
   { 60929, 30000 },
   { 65535, 30000 }
};

static Mapper_LookupTableConfiguration_t configurationForMappings = {
   mappingsForFilter,
   NUM_ELEMENTS(mappingsForFilter)
};

static const SensorDataSlewRateData_t slewRateFilterEnabled = {
   .slewRateFilterEnabled = true,
   .slewRatePerSecondx100 = 10
};

static const SensorDataSlewRateData_t slewRateDisabled = {
   .slewRateFilterEnabled = false,
   .slewRatePerSecondx100 = 0
};

static SensorDataSensorType_t freezerCabinetData = {
   .lookupTable = &configurationForMappings,
   .fallbackValueDegFx100 = 300,
   .alphaNum = 1,
   .alphaDenom = 10,
   .windowSize = 10,
   .clampData = slewRateFilterEnabled,
   .goodReadingCounterMax = 100,
   .badReadingCounterMax = 100
};

static SensorDataSensorType_t freshFoodCabinetData = {
   .lookupTable = &configurationForMappings,
   .fallbackValueDegFx100 = -500,
   .alphaNum = 1,
   .alphaDenom = 10,
   .windowSize = 10,
   .clampData = slewRateFilterEnabled,
   .goodReadingCounterMax = 100,
   .badReadingCounterMax = 100
};

static SensorDataSensorType_t freezerEvapData = {
   .lookupTable = &configurationForMappings,
   .fallbackValueDegFx100 = -4100,
   .alphaNum = 1,
   .alphaDenom = 10,
   .windowSize = 10,
   .clampData = slewRateDisabled,
   .goodReadingCounterMax = 100,
   .badReadingCounterMax = 100
};

static SensorDataSensorType_t freshFoodEvapData = {
   .lookupTable = &configurationForMappings,
   .fallbackValueDegFx100 = -4100,
   .alphaNum = 1,
   .alphaDenom = 10,
   .windowSize = 10,
   .clampData = slewRateDisabled,
   .goodReadingCounterMax = 100,
   .badReadingCounterMax = 100
};

static SensorDataConvertibleCompartmentSensorType_t convertibleCompartmentCabinetData = {
   .lookupTable = &configurationForMappings,
   .freshFoodFallbackValueDegFx100 = -500,
   .freezerFallbackValueDegFx100 = 300,
   .alphaNum = 1,
   .alphaDenom = 10,
   .windowSize = 10,
   .clampData = slewRateFilterEnabled,
   .goodReadingCounterMax = 100,
   .badReadingCounterMax = 100
};

static SensorDataSensorType_t ambientData = {
   .lookupTable = &configurationForMappings,
   .fallbackValueDegFx100 = 7500,
   .alphaNum = 1,
   .alphaDenom = 10,
   .windowSize = 10,
   .clampData = slewRateFilterEnabled,
   .goodReadingCounterMax = 100,
   .badReadingCounterMax = 100
};

static SensorDataSensorType_t convertibleCompartmentEvapData = {
   .lookupTable = &configurationForMappings,
   .fallbackValueDegFx100 = -4100,
   .alphaNum = 1,
   .alphaDenom = 10,
   .windowSize = 10,
   .clampData = slewRateDisabled,
   .goodReadingCounterMax = 100,
   .badReadingCounterMax = 100
};

void SensorData_TestDouble_Init(SensorData_t *sensorData)
{
   memset(sensorData, 0, sizeof(SensorData_t));

   sensorData->freezerCabinetThermistor = &freezerCabinetData;
   sensorData->freshFoodCabinetThermistor = &freshFoodCabinetData;
   sensorData->freezerEvapThermistor = &freezerEvapData;
   sensorData->freshFoodEvapThermistor = &freshFoodEvapData;
   sensorData->convertibleCompartmentCabinetThermistor = &convertibleCompartmentCabinetData;
   sensorData->ambientThermistor = &ambientData;
   sensorData->convertibleCompartmentEvapThermistor = &convertibleCompartmentEvapData;
   sensorData->periodicUpdateRateInMs = 1000;
}
