/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ServiceModeTest_BoardVersion.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "Gea2Message.h"
#include "Gea2CommonCommands.h"
#include "SystemErds.h"
#include "utils.h"
#include "Version.h"

enum
{
   NumberOfGea2Retries = 2,
   VersionRequestTimeoutInMsec = 1 * MSEC_PER_SEC
};

static void SendVersionRequest(ServiceModeTest_BoardVersion_t *instance)
{
   STACK_ALLOC_GEA2MESSAGE(message, 0);

   Gea2Message_SetDestination(
      message,
      instance->_private.boardVersionsMappingConfig->destinationAddresses[instance->_private.resources->itemIndex]);
   Gea2Message_SetCommand(message, Gea2CommonCommand_Version);
   Gea2MessageEndpoint_Send(
      DataModelErdPointerAccess_GetGea2MessageEndpoint(
         instance->_private.resources->dataModel,
         Erd_Gea2MessageEndpoint),
      message,
      NumberOfGea2Retries);
}

static void VersionRequestTimeout(void *context)
{
   ServiceModeTest_BoardVersion_t *instance = context;

   ServiceTestResultArgs_t testResultArgs;
   testResultArgs.status.testResponse = ServiceModeTestStatusResponse_Running;
   testResultArgs.status.dataFormat = ServiceModeTestStatusDataFormat_NoVersionInfo;
   testResultArgs.status.testNumber = instance->_private.resources->testNumber;
   testResultArgs.status.diagnosticData[0] = 0;
   testResultArgs.status.diagnosticData[1] = 0;
   testResultArgs.status.diagnosticData[2] = 0;
   testResultArgs.status.diagnosticData[3] = 0;

   instance->_private.resources->callback(instance->_private.resources->context, &testResultArgs);

   SendVersionRequest(instance);
}

static void SendVersionRequestAndStartPeriodicTimeoutTimer(ServiceModeTest_BoardVersion_t *instance)
{
   SendVersionRequest(instance);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.resources->dataModel,
         Erd_TimerModule),
      &instance->_private.resources->timer,
      VersionRequestTimeoutInMsec,
      VersionRequestTimeout,
      instance);
}

static void OnGeaMessageReceived(void *context, const void *args)
{
   ServiceModeTest_BoardVersion_t *instance = context;
   const Gea2MessageEndpointOnReceiveArgs_t *received = args;

   if((Gea2Message_GetSource(received->message) == instance->_private.boardVersionsMappingConfig->destinationAddresses[instance->_private.resources->itemIndex]) &&
      (Gea2Message_GetCommand(received->message) == Gea2CommonCommand_Version) &&
      (Gea2Message_GetPayloadLength(received->message) >= sizeof(Version_t)))
   {
      const uint8_t *payload = Gea2Message_GetConstPayload(received->message);

      ServiceTestResultArgs_t testResultArgs;
      testResultArgs.status.testResponse = ServiceModeTestStatusResponse_Running;
      testResultArgs.status.dataFormat = ServiceModeTestStatusDataFormat_VersionInfo;
      testResultArgs.status.testNumber = instance->_private.resources->testNumber;
      testResultArgs.status.diagnosticData[0] = payload[2];
      testResultArgs.status.diagnosticData[1] = payload[3];
      testResultArgs.status.diagnosticData[2] = 0;
      testResultArgs.status.diagnosticData[3] = 0;

      instance->_private.resources->callback(instance->_private.resources->context, &testResultArgs);

      SendVersionRequestAndStartPeriodicTimeoutTimer(instance);
   }
}

static void Start(I_ServiceTest_t *_instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   REINTERPRET(instance, _instance, ServiceModeTest_BoardVersion_t *);

   resources->context = context;
   resources->callback = callback;
   instance->_private.resources = resources;

   EventSubscription_Init(
      &resources->eventSubscription,
      instance,
      OnGeaMessageReceived);
   Event_Subscribe(
      Gea2MessageEndpoint_GetOnReceiveEvent(
         DataModelErdPointerAccess_GetGea2MessageEndpoint(
            resources->dataModel,
            Erd_Gea2MessageEndpoint)),
      &resources->eventSubscription);

   SendVersionRequestAndStartPeriodicTimeoutTimer(instance);
}

static void Stop(I_ServiceTest_t *_instance, void *context, ServiceTestResources_t *resources, ServiceTestCallback_t callback)
{
   REINTERPRET(instance, _instance, ServiceModeTest_BoardVersion_t *);

   resources->context = context;
   resources->callback = callback;
   instance->_private.resources = resources;

   ServiceTestResultArgs_t testResultArgs;
   testResultArgs.status.testResponse = ServiceModeTestStatusResponse_Stopped;
   testResultArgs.status.dataFormat = ServiceModeTestStatusDataFormat_Unused;
   testResultArgs.status.testNumber = instance->_private.resources->testNumber;
   testResultArgs.status.diagnosticData[0] = 0;
   testResultArgs.status.diagnosticData[1] = 0;
   testResultArgs.status.diagnosticData[2] = 0;
   testResultArgs.status.diagnosticData[3] = 0;

   callback(context, &testResultArgs);

   Event_Unsubscribe(
      Gea2MessageEndpoint_GetOnReceiveEvent(
         DataModelErdPointerAccess_GetGea2MessageEndpoint(
            resources->dataModel,
            Erd_Gea2MessageEndpoint)),
      &resources->eventSubscription);

   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         resources->dataModel,
         Erd_TimerModule),
      &resources->timer);
}

static const I_ServiceTest_Api_t api = {
   .Start = Start,
   .Stop = Stop
};

void ServiceModeTest_BoardVersion_Init(
   ServiceModeTest_BoardVersion_t *instance,
   const ServiceModeTest_TestNumbersMappingTable_t *testNumbersMappingConfig,
   const ServiceModeTest_BoardVersionMappingConfig_t *boardVersionsMappingConfig)
{
   instance->interface.api = &api;
   instance->interface.testNumbersMappingTable = testNumbersMappingConfig;

   instance->_private.boardVersionsMappingConfig = boardVersionsMappingConfig;
}
