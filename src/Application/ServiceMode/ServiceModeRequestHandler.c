/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "ServiceModeRequestHandler.h"
#include "ServiceModeTestRequest.h"
#include "ServiceModeTestStatus.h"
#include "SystemErds.h"

static void ClearServiceTestRequest(ServiceModeRequestHandler_t *instance)
{
   ServiceModeTestRequest_t clearRequest;
   clearRequest.testRequestCommand = ServiceModeTestRequestCommand_Clear;
   DataModel_Write(instance->_private.resources.dataModel, Erd_ServiceModeTestNumberRequest, &clearRequest);
}

static void ServiceTestIsInvalid(ServiceModeRequestHandler_t *instance, ServiceModeTestNumber_t testNumber)
{
   ServiceModeTestStatus_t status;
   status.testNumber = testNumber;
   status.testResponse = ServiceModeTestStatusResponse_NotSupported;
   status.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   memset(&status.diagnosticData, 0, sizeof(status.diagnosticData));

   DataModel_Write(instance->_private.resources.dataModel, Erd_ServiceModeTestNumberStatus, &status);
}

static void TestsResultCallback(void *context, const ServiceTestResultArgs_t *args)
{
   ServiceModeRequestHandler_t *instance = context;

   DataModel_Write(instance->_private.resources.dataModel, Erd_ServiceModeTestNumberStatus, &args->status);
}

static I_ServiceTest_t *FindTestComponentWithTestNumber(ServiceModeRequestHandler_t *instance, ServiceModeTestNumber_t testNumber)
{
   LinkedList_ForEach(&instance->_private.components, ServiceModeTestComponent_t, component, {
      for(uint8_t index = 0; index < component->test->testNumbersMappingTable->numberOfItems; index++)
      {
         if(component->test->testNumbersMappingTable->testNumberEntries[index] == testNumber)
         {
            instance->_private.resources.itemIndex = index;
            instance->_private.resources.testNumber = testNumber;

            return component->test;
         }
      }
   });

   return NULL;
}

static void ProcessTestRequest(ServiceModeRequestHandler_t *instance, ServiceModeTestRequest_t request)
{
   I_ServiceTest_t *test = FindTestComponentWithTestNumber(instance, request.testNumber);

   if(test)
   {
      switch(request.testRequestCommand)
      {
         case ServiceModeTestRequestCommand_Start:
            ServiceTest_Start(
               test,
               instance,
               &instance->_private.resources,
               TestsResultCallback);
            break;

         case ServiceModeTestRequestCommand_Stop:
            ServiceTest_Stop(
               test,
               instance,
               &instance->_private.resources,
               TestsResultCallback);
            break;
      }
   }
   else
   {
      ServiceTestIsInvalid(instance, request.testNumber);
   }
}

static void RequestChanged(void *context, const void *args)
{
   ServiceModeRequestHandler_t *instance = context;
   const ServiceModeTestRequest_t *request = args;

   if(request->testRequestCommand != ServiceModeTestRequestCommand_Clear)
   {
      ProcessTestRequest(instance, *request);
      ClearServiceTestRequest(instance);
   }
}

void ServiceModeRequestHandler_Init(
   ServiceModeRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   ServiceModeTestNumber_t maxTestNumber)
{
   instance->_private.resources.dataModel = dataModel;

   DataModel_Write(dataModel, Erd_ServiceModeTestNumberMaximum, &maxTestNumber);
   ClearServiceTestRequest(instance);

   LinkedList_Init(&instance->_private.components);

   EventSubscription_Init(&instance->_private.serviceTestRequestChanged, instance, RequestChanged);
   DataModel_Subscribe(dataModel, Erd_ServiceModeTestNumberRequest, &instance->_private.serviceTestRequestChanged);
}

void ServiceModeRequestHandler_AddTest(
   ServiceModeRequestHandler_t *instance,
   I_ServiceTest_t *test,
   ServiceModeTestComponent_t *component)
{
   component->test = test;
   LinkedList_PushBack(&instance->_private.components, &component->node);
}
