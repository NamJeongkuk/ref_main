/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef I_SERVICETEST_H
#define I_SERVICETEST_H

#include "EventSubscription.h"
#include "I_DataModel.h"
#include "ServiceModeTestNumber.h"
#include "ServiceModeTestStatus.h"
#include "Timer.h"

typedef struct
{
   ServiceModeTestStatus_t status;
} ServiceTestResultArgs_t;

typedef struct
{
   const ServiceModeTestNumber_t *testNumberEntries;
   uint8_t numberOfItems;
} ServiceModeTest_TestNumbersMappingTable_t;

typedef struct
{
   const struct I_ServiceTest_Api_t *api;
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingTable;
} I_ServiceTest_t;

typedef void (*ServiceTestCallback_t)(void *context, const ServiceTestResultArgs_t *args);

typedef struct
{
   I_DataModel_t *dataModel;
   void *context;
   Timer_t timer;
   uint8_t itemIndex;
   ServiceTestCallback_t callback;
   ServiceModeTestNumber_t testNumber;
   EventSubscription_t eventSubscription;
} ServiceTestResources_t;

typedef struct I_ServiceTest_Api_t
{
   /*!
    * Request to start test
    * @param instance
    * @param context
    * @param resources
    * @param callback
    */
   void (*Start)(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback);

   /*!
    * Request to stop test
    * @param instance
    * @param context
    * @param resources
    * @param callback
    */
   void (*Stop)(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback);
} I_ServiceTest_Api_t;

static inline void ServiceTest_Start(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   instance->api->Start(instance, context, resources, callback);
}

static inline void ServiceTest_Stop(I_ServiceTest_t *instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   instance->api->Stop(instance, context, resources, callback);
}

#endif
