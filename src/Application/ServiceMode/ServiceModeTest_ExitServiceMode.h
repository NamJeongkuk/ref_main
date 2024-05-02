/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODETEST_EXITSERVICEMODE_H
#define SERVICEMODETEST_EXITSERVICEMODE_H

#include "I_ServiceTest.h"

typedef struct
{
   I_ServiceTest_t interface;
} ServiceModeTest_ExitServiceMode_t;

/*!
 * @param instance
 * @param testNumbersMappingConfig The ServiceTest Group Numbers Mapping Configuration
 */
void ServiceModeTest_ExitServiceMode_Init(
   ServiceModeTest_ExitServiceMode_t *instance,
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingConfig);

#endif
