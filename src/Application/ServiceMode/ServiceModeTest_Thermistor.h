/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODETEST_THERMISTOR_H
#define SERVICEMODETEST_THERMISTOR_H

#include "I_ServiceTest.h"

typedef struct
{
   const Erd_t *unfilteredTemperatureErds;
   uint8_t numberOfItems;
} ServiceModeTest_ThermistorMappingConfig_t;

typedef struct
{
   I_ServiceTest_t interface;

   struct
   {
      ServiceTestResources_t *resources;
      const ServiceModeTest_ThermistorMappingConfig_t *thermistorsMappingConfig;

   } _private;
} ServiceModeTest_Thermistor_t;

/*!
 * @param instance
 * @param testNumbersMappingConfig The ServiceTest Group Numbers Mapping Configuration
 * @param thermistorsMappingConfig The thermistors Mapping Configuration
 */
void ServiceModeTest_Thermistor_Init(
   ServiceModeTest_Thermistor_t *instance,
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingConfig,
   const ServiceModeTest_ThermistorMappingConfig_t *thermistorsMappingConfig);

#endif
