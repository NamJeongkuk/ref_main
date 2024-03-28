/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "RfidCommunicator.h"
#include "Gea2Addresses.h"
#include "DataSourceErdPointerAccess.h"

enum
{
   RfidMessageCommand = 0xEA,
   NumberOfRetries = 3
};

static void ConstructRfidMessage(RfidCommunicator_t *instance, ReadWriteRequest_t readWriteRequest, Gea2Message_t *message)
{
   Gea2Message_SetDestination(message, Gea2Address_RfidBoard);
   Gea2Message_SetSource(message, Gea2Address_Mainboard);
   Gea2Message_SetCommand(message, RfidMessageCommand);
   Gea2Message_SetPayloadLength(message, sizeof(RfidMessagePayload_t));

   REINTERPRET(payload, Gea2Message_GetPayload(message), RfidMessagePayload_t *);
   memset(payload, 0, sizeof(RfidMessagePayload_t));

   payload->readWriteRequest = readWriteRequest;

   if(readWriteRequest == ReadWriteRequest_Write)
   {
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->uid, &payload->uid);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->volumeWaterUsageInOuncesX100, &payload->volumeUsageInOuncesX100);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->calendarUsageInSeconds, &payload->calendarUsageInSeconds);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->filterStatus, &payload->filterStatus);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->lockByte, &payload->lock);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->numberOfUnitsFilterHasBeenOn, &payload->numberOfUnitsFilterHasBeenOn);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->unitSerialNumber, &payload->unitSerialNumber);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->previousUnitSerialNumber, &payload->previousUnitSerialNumber);
      DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidWriteErds->filterWaterUsageInGallons, &payload->lastTwelveMonthsOfWaterUsageInGallons);
   }
}

static void StoreAllRfidDataOnMainboard(RfidCommunicator_t *instance, const Gea2Message_t *message)
{
   REINTERPRET(receivedPayload, Gea2Message_GetConstPayload(message), const RfidReceivedPayload_t *);

   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->filterUid, &receivedPayload->uid);
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->filterIdentifier, &receivedPayload->filterIdentifier);
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->volumeWaterUsageInOuncesX100, &receivedPayload->volumeUsageInOuncesX100);
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->calendarUsageInSeconds, &receivedPayload->calendarUsageInSeconds);
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->leakDetectedTimeInSeconds, &receivedPayload->leakDetectedTimeInSeconds);
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->numberOfWriteOperations, &receivedPayload->numberOfWriteOperations);
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->filterStatus, &receivedPayload->filterStatus);
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->initializationErrorBitfield, &receivedPayload->initErrorBitfield);

   if(Gea2Message_GetPayloadLength(message) >= sizeof(RfidReceivedPayload_t))
   {
      DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->lockByte, &receivedPayload->lock);
      DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->unitSerialNumber, &receivedPayload->unitSerialNumber);
      DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->previousUnitSerialNumber, &receivedPayload->previousUnitSerialNumber);
      DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->numberOfUnitsFilterHasBeenOn, &receivedPayload->numberOfUnitsFilterHasBeenOn);
      DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->filterWaterUsageInGallons, &receivedPayload->lastTwelveMonthsOfWaterUsageInGallons);
   }

   RfidFilterReadWriteResult_t readWriteResult;
   DataSource_Read(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->readWriteResult, &readWriteResult);
   readWriteResult.result = receivedPayload->readWriteResult;
   readWriteResult.signal++;
   DataSource_Write(instance->_private.externalDataSource, instance->_private.config->rfidReadErds->readWriteResult, &readWriteResult);
}

static void SetReadWriteRequestToIdle(RfidCommunicator_t *instance)
{
   ReadWriteRequest_t idleRequest = ReadWriteRequest_Idle;
   DataSource_Write(
      instance->_private.externalDataSource,
      instance->_private.config->dataRequestErd,
      &idleRequest);
}

static void OnMessageReceived(void *context, const void *_args)
{
   RfidCommunicator_t *instance = context;
   const Gea2MessageEndpointOnReceiveArgs_t *args = _args;
   const Gea2Message_t *message = args->message;

   if((Gea2Message_GetSource(message) == Gea2Address_RfidBoard) &&
      (Gea2Message_GetCommand(message) == RfidMessageCommand))
   {
      StoreAllRfidDataOnMainboard(instance, message);
   }
}

static void OnDataSourceChange(void *context, const void *_args)
{
   RfidCommunicator_t *instance = context;
   const DataSourceOnDataChangeArgs_t *args = _args;
   Erd_t erd = args->erd;
   if(erd == instance->_private.config->dataRequestErd)
   {
      const ReadWriteRequest_t *readWriteRequest = args->data;
      if(*readWriteRequest == ReadWriteRequest_Read || *readWriteRequest == ReadWriteRequest_Write)
      {
         STACK_ALLOC_GEA2MESSAGE(message, sizeof(RfidMessagePayload_t));
         ConstructRfidMessage(instance, *readWriteRequest, message);

         Gea2MessageEndpoint_Send(
            DataSourceErdPointerAccess_GetGea2MessageEndpoint(
               instance->_private.externalDataSource,
               instance->_private.config->geaMessageEndpointErd),
            message,
            NumberOfRetries);

         SetReadWriteRequestToIdle(instance);
      }
   }
}

void RfidCommunicator_Init(
   RfidCommunicator_t *instance,
   I_DataSource_t *externalDataSource,
   const RfidCommunicatorConfiguration_t *config)
{
   instance->_private.externalDataSource = externalDataSource;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.messageReceived,
      instance,
      OnMessageReceived);
   Event_Subscribe(
      Gea2MessageEndpoint_GetOnReceiveEvent(
         DataSourceErdPointerAccess_GetGea2MessageEndpoint(
            instance->_private.externalDataSource,
            instance->_private.config->geaMessageEndpointErd)),
      &instance->_private.messageReceived);

   EventSubscription_Init(
      &instance->_private.dataSourceOnChangeSubscription,
      instance,
      OnDataSourceChange);
   Event_Subscribe(
      instance->_private.externalDataSource->OnDataChange,
      &instance->_private.dataSourceOnChangeSubscription);

   SetReadWriteRequestToIdle(instance);
}
