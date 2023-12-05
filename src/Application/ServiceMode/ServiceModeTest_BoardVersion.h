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
   uint8_t destinationAddress;
} ServiceModeTest_BoardVersionConfig_t;

typedef struct
{
   I_ServiceTest_t interface;

   struct
   {
      ServiceTestResources_t *resources;
      const ServiceModeTest_BoardVersionConfig_t *config;
   } _private;
} ServiceModeTest_BoardVersion_t;

void ServiceModeTest_BoardVersion_Init(
   ServiceModeTest_BoardVersion_t *instance,
   ServiceModeTestNumber_t testNumber,
   const ServiceModeTest_BoardVersionConfig_t *config);

#endif
