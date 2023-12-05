/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef I_SERVICETEST_H
#define I_SERVICETEST_H

#include "ServiceModeTestStatus.h"
#include "ServiceModeTestNumber.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "EventSubscription.h"

typedef struct
{
   ServiceModeTestStatus_t status;
} ServiceTestResultArgs_t;

typedef struct
{
   const struct I_ServiceTest_Api_t *api;
   ServiceModeTestNumber_t testNumber;
} I_ServiceTest_t;

typedef void (*ServiceTestCallback_t)(void *context, const ServiceTestResultArgs_t *args);

typedef struct
{
   I_DataModel_t *dataModel;
   Timer_t timer;
   void *context;
   ServiceTestCallback_t callback;
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
