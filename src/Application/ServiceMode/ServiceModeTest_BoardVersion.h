/*!
 * @file
 * @brief The module sends version request when test is running and
 *        updates test response with the critical minor version.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODETEST_BOARDVERSION_H
#define SERVICEMODETEST_BOARDVERSION_H

#include "I_ServiceTest.h"

typedef struct
{
   const uint8_t *destinationAddresses;
   uint8_t numberOfItems;
} ServiceModeTest_BoardVersionMappingConfig_t;

typedef struct
{
   I_ServiceTest_t interface;

   struct
   {
      ServiceTestResources_t *resources;
      const ServiceModeTest_BoardVersionMappingConfig_t *boardVersionsMappingConfig;

   } _private;
} ServiceModeTest_BoardVersion_t;

/*!
 * @param instance
 * @param testNumbersMappingConfig The ServiceTest Group Numbers Mapping Configuration
 * @param boardVersionsMappingConfig The ExternalBoard Versions Mapping Configuration
 */
void ServiceModeTest_BoardVersion_Init(
   ServiceModeTest_BoardVersion_t *instance,
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingConfig,
   const ServiceModeTest_BoardVersionMappingConfig_t *boardVersionsMappingConfig);

#endif
