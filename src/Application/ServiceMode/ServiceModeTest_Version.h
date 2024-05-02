/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODETEST_VERSION_H
#define SERVICEMODETEST_VERSION_H

#include "I_ServiceTest.h"

typedef struct
{
   const Erd_t *versionErds;
   uint8_t numberOfItems;
} ServiceModeTest_VersionMappingConfig_t;

typedef struct
{
   I_ServiceTest_t interface;

   struct
   {
      const ServiceModeTest_VersionMappingConfig_t *versionsMappingConfig;

   } _private;
} ServiceModeTest_Version_t;

/*!
 * @param instance
 * @param testNumbersMappingConfig The ServiceTest Group Numbers Mapping Configuration
 * @param versionsMappingConfig The Versions Mapping Configuration
 */
void ServiceModeTest_Version_Init(
   ServiceModeTest_Version_t *instance,
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingConfig,
   const ServiceModeTest_VersionMappingConfig_t *versionsMappingConfig);

#endif
