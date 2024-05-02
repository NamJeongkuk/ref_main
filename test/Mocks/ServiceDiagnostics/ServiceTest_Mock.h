/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICETEST_MOCK_H
#define SERVICETEST_MOCK_H

#include "I_ServiceTest.h"
#include "ServiceModeTestNumber.h"

typedef struct
{
   I_ServiceTest_t interface;

   struct
   {
      void *context;
      ServiceTestCallback_t callback;
   } _private;
} ServiceTest_Mock_t;

/*!
 * @param instance
 * @param testNumbersMappingConfig The ServiceTest Group Numbers Mapping Configuration
 */
I_ServiceTest_t *ServiceTest_Mock_Init(
   ServiceTest_Mock_t *instance,
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingTable);

/*!
 * @brief Function to invoke the I_Service_Test callback
 *
 * @param instance
 * @param status
 */
void ServiceTest_Mock_InvokeCallbackForActiveTestWith(
   ServiceTest_Mock_t *instance,
   ServiceModeTestStatus_t status);

#endif
