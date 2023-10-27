/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BoardsInSystem.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "Gea2CommonCommands.h"
#include "Version.h"
#include "Constants_Time.h"
#include "uassert.h"

#define FOR_EACH_GEA_ADDRESS_PAIR_INDEX(_indexName) \
   for(uint8_t _indexName = 0; _indexName < instance->_private.config->allBoardsInSystemNumberEntries; _indexName++)

enum
{
   NumberOfGea2Retries = 2,
};

static bool NvErdIsSet(BoardsInSystem_t *instance, uint8_t geaAddressListIndex)
{
   bool nvErdIsSet;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->boardsInSystemData[geaAddressListIndex].nvErd,
      &nvErdIsSet);
   return nvErdIsSet;
}

static bool SourceAddressIsInAddressErdMap(BoardsInSystem_t *instance, uint8_t gea2SourceAddress, uint8_t *foundIndex)
{
   FOR_EACH_GEA_ADDRESS_PAIR_INDEX(addressIndex)
   {
      if(instance->_private.config->boardsInSystemData[addressIndex].geaAddress == gea2SourceAddress)
      {
         *foundIndex = addressIndex;
         return true;
      }
   }
   return false;
}

static bool CommandIsVersionRequest(Gea2CommonCommand_t sourceCommand)
{
   return Gea2CommonCommand_Version == sourceCommand;
}

static void RequestVersionFromGeaAddressWithIndex(BoardsInSystem_t *instance, uint8_t geaAddressListIndex)
{
   STACK_ALLOC_GEA2MESSAGE(message, 0);

   Gea2Message_SetDestination(
      message,
      instance->_private.config->boardsInSystemData[geaAddressListIndex].geaAddress);
   Gea2Message_SetCommand(message, Gea2CommonCommand_Version);

   Gea2MessageEndpoint_Send(
      DataModelErdPointerAccess_GetGea2MessageEndpoint(
         instance->_private.dataModel,
         instance->_private.config->geaMessageEndpointErd),
      message,
      NumberOfGea2Retries);
}

static void RequestVersionFromAccountedForBoards(BoardsInSystem_t *instance)
{
   FOR_EACH_GEA_ADDRESS_PAIR_INDEX(addressIndex)
   {
      if(NvErdIsSet(instance, addressIndex))
      {
         RequestVersionFromGeaAddressWithIndex(instance, addressIndex);
      }
   }
}

static void RequestVersionFromUnaccountedForBoards(BoardsInSystem_t *instance)
{
   FOR_EACH_GEA_ADDRESS_PAIR_INDEX(addressIndex)
   {
      if(!NvErdIsSet(instance, addressIndex))
      {
         RequestVersionFromGeaAddressWithIndex(instance, addressIndex);
      }
   }
   instance->_private.numberOfRetries++;
}

static void IncrementBoardsInSystemBuffer(BoardsInSystem_t *instance)
{
   FOR_EACH_GEA_ADDRESS_PAIR_INDEX(addressIndex)
   {
      if(NvErdIsSet(instance, addressIndex))
      {
         instance->_private.boardMissedResponseCountBuffer[addressIndex] = TRUNCATE_UNSIGNED_ADDITION(instance->_private.boardMissedResponseCountBuffer[addressIndex], 1, UINT8_MAX);
      }
   }
}

static void SetNvErdFromIndex(BoardsInSystem_t *instance, uint8_t addressIndex)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->boardsInSystemData[addressIndex].nvErd,
      set);
}

static void SetFaultErdFromIndex(BoardsInSystem_t *instance, uint8_t addressIndex)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->boardsInSystemData[addressIndex].faultErd,
      set);
}

static void ClearFaultErdFromIndex(BoardsInSystem_t *instance, uint8_t addressIndex)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->boardsInSystemData[addressIndex].faultErd,
      clear);
}

static void SetFaultDependingOnMissedPings(BoardsInSystem_t *instance)
{
   FOR_EACH_GEA_ADDRESS_PAIR_INDEX(addressIndex)
   {
      if(NvErdIsSet(instance, addressIndex))
      {
         if(instance->_private.boardMissedResponseCountBuffer[addressIndex] >= instance->_private.config->numberOfMonitoringRetryRequestsBeforeSettingFault)
         {
            SetFaultErdFromIndex(instance, addressIndex);
         }
      }
   }
}

static void Gea2PacketReceived(void *context, const void *args)
{
   BoardsInSystem_t *instance = context;
   const Gea2MessageEndpointOnReceiveArgs_t *received = args;

   uint8_t sourceAddress = Gea2Message_GetSource(received->message);
   uint8_t sourceCommand = Gea2Message_GetCommand(received->message);

   uint8_t addressIndex;
   if(SourceAddressIsInAddressErdMap(instance, sourceAddress, &addressIndex) &&
      CommandIsVersionRequest(sourceCommand))
   {
      SetNvErdFromIndex(instance, addressIndex);
      instance->_private.boardMissedResponseCountBuffer[addressIndex] = 0;
      ClearFaultErdFromIndex(instance, addressIndex);
   }
}

static void MonitorBoards(void *context)
{
   BoardsInSystem_t *instance = context;
   SetFaultDependingOnMissedPings(instance);
   IncrementBoardsInSystemBuffer(instance);
   RequestVersionFromAccountedForBoards(instance);
}

static void InitializeAndSubscribeToOnReceiveEvents(BoardsInSystem_t *instance)
{
   EventSubscription_Init(
      &instance->_private.onReceiveEventSubscription,
      instance,
      Gea2PacketReceived);

   Event_Subscribe(
      Gea2MessageEndpoint_GetOnReceiveEvent(
         DataModelErdPointerAccess_GetGea2MessageEndpoint(
            instance->_private.dataModel,
            instance->_private.config->geaMessageEndpointErd)),
      &instance->_private.onReceiveEventSubscription);
}

static void StartMonitoringPeriodicTimer(BoardsInSystem_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.retryTimer,
      instance->_private.config->retryPeriodForMonitoringInMinutes * MSEC_PER_MIN,
      MonitorBoards,
      instance);
}

static void RetryTimerExpired(void *context)
{
   BoardsInSystem_t *instance = context;
   RequestVersionFromUnaccountedForBoards(instance);

   if(instance->_private.numberOfRetries == instance->_private.config->numberOfDiscoveryRetryRequests)
   {
      StartMonitoringPeriodicTimer(instance);
   }
}

static void StartRetryTimer(BoardsInSystem_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.retryTimer,
      instance->_private.config->retryPeriodForDiscoveryInSec * MSEC_PER_SEC,
      RetryTimerExpired,
      instance);
}

static void InitialDelayTimerExpired(void *context)
{
   BoardsInSystem_t *instance = context;

   InitializeAndSubscribeToOnReceiveEvents(instance);

   instance->_private.numberOfRetries = 0;
   RequestVersionFromUnaccountedForBoards(instance);
   StartRetryTimer(instance);
}

static void StartInitialDelayTimer(BoardsInSystem_t *instance)
{
   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.retryTimer,
      instance->_private.config->initialDiscoveryDelayTimeInSec * MSEC_PER_SEC,
      InitialDelayTimerExpired,
      instance);
}

void BoardsInSystem_Init(
   BoardsInSystem_t *instance,
   I_DataModel_t *dataModel,
   const BoardsInSystemConfig_t *config,
   uint8_t *boardMissedResponseCountBuffer,
   uint8_t sizeOfBuffer)
{
   uassert(sizeOfBuffer >= config->allBoardsInSystemNumberEntries);
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.boardMissedResponseCountBuffer = boardMissedResponseCountBuffer;

   StartInitialDelayTimer(instance);
}
