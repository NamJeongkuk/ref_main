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
   Erd_t versionErd; // Version_t
} ServiceModeTest_VersionConfig_t;

typedef struct
{
   I_ServiceTest_t interface;

   struct
   {
      const ServiceModeTest_VersionConfig_t *config;
   } _private;
} ServiceModeTest_Version_t;

void ServiceModeTest_Version_Init(
   ServiceModeTest_Version_t *instance,
   ServiceModeTestNumber_t testNumber,
   const ServiceModeTest_VersionConfig_t *config);

#endif
