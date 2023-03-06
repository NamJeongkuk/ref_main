/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GeaStack.h"
#include "DataModelErdPointerAccess.h"
#include "Gea2Message.h"
#include "Gea2RefAddresses.h"
#include "Header.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "XMacroUtils.h"
#include "uassert.h"
#include "ContextProtector_Rx2xx.h"

enum
{
   Gea2CrcSeed = 0x1021,
   ParametricHackRequired = false,
   UnlockErdSecurityPeriodMsec = 30 * MSEC_PER_MIN,
   MissedAckLimit = 31,
   RestrictedFullRangeErdStart = 0xF000,
   RestrictedFullRangeErdEnd = 0xFFFF,
   RetryCount = 3,
   RequestTimeoutInMsec = 1 * MSEC_PER_SEC,
   SubscriptionUpdatePeriodInTicks = 100,
};

// clang-format off

#define EXPAND_AS_PUBLIC_ERDS(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_PUBLIC_, Number)(Public##Name COMMA)

// clang-format on

static const Erd_t publicErds[] = {
   ERD_TABLE(EXPAND_AS_PUBLIC_ERDS)
};

static const ConstArrayMap_BinarySearchConfiguration_t publicErdMapConfiguration = {
   publicErds,
   NUM_ELEMENTS(publicErds),
   ELEMENT_SIZE(publicErds),
   sizeof(Erd_t),
   0,
   false
};

static const uint8_t rangeRestrictedValidatorRestrictedAddressTable[] = {
   Gea2Address_EmbeddedWiFi,
   Gea2Address_ConnectPlusWiFi
};

static const Validator_RestrictedRangeErdConfiguration_t restrictedRangeConfiguration = {
   rangeRestrictedValidatorRestrictedAddressTable,
   NUM_ELEMENTS(rangeRestrictedValidatorRestrictedAddressTable),
   RestrictedFullRangeErdStart,
   RestrictedFullRangeErdEnd
};

static const ErdClient_ApiRevision2Configuration_t erdClientConfiguration = {
   .requestTimeout = RequestTimeoutInMsec,
   .requestRetries = RetryCount,
};

static void InitializeErdSecurityComponents(
   GeaStack_t *instance,
   I_DataModel_t *dataModel)
{
   Input_ErdGea2UnlockState_Init(
      &instance->_private.erdSecurity.erdGea2UnlockStateInput,
      instance->_private.packetEndpoint,
      UnlockErdSecurityPeriodMsec,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));

   Validator_RestrictedRangeErd_Init(
      &instance->_private.erdSecurity.restrictedRangeErdValidator,
      &instance->_private.erdSecurity.erdGea2UnlockStateInput.interface,
      &restrictedRangeConfiguration);

   Gea2PacketEndpoint_PacketRestrictor_Init(
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor,
      instance->_private.packetEndpoint,
      &instance->_private.erdSecurity.restrictedRangeErdValidator.interface);
}

static void ConnectGea2MessageEndpointToDataSource(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource,
   TimerModule_t *timerModule)
{
   PeriodicSignalUpdater_Init(
      &instance->_private.mainboardToUiHeartbeatPeriodicSignalUpdater,
      DataModel_GetInputOutput(dataModel, Erd_SignOfLifeFromMainboardToUi),
      timerModule,
      SubscriptionUpdatePeriodInTicks);

   ErdGea2ReadWriteApiRevision2_Init(
      &instance->_private.erdApiRevision2ReadWrite,
      externalDataSource,
      NULL,
      NULL,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface);

   ConstArrayMap_BinarySearch_Init(
      &instance->_private.publicErdMap,
      &publicErdMapConfiguration);

   ErdGea2SubscriptionApiRevision2_Init(
      &instance->_private.erdApiRevision2Subscription,
      externalDataSource,
      NULL,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface,
      timerModule,
      &instance->_private.publicErdMap.interface,
      instance->_private.subscriptionResources,
      ErdApiV2SubscriptionClients,
      &instance->_private.subscriptionBuffers[0][0],
      sizeof(instance->_private.subscriptionBuffers[0]),
      false,
      0);

   ErdClient_ApiRevision2_Init(
      &instance->_private.erdClient,
      timerModule,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface,
      instance->_private.queueBuffer,
      NUM_ELEMENTS(instance->_private.queueBuffer),
      &erdClientConfiguration);
}

static void CreatePacketAndMessageEndpoints(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   instance->_private.packetEndpoint =
      Gea2Configurator_CreatePacketEndpointNode(
         &instance->_private.configurator,
         &instance->_private.applicationNode,
         &instance->_private.applicationPacketEndpointResources,
         geaAddress);

   InitializeErdSecurityComponents(instance, dataModel);

   Gea2MessageEndpoint_Gea2PacketEndpointAdapter_Init(
      &instance->_private.messageEndpointAdapter,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface);

   DataModelErdPointerAccess_Write(dataModel, Erd_Gea2MessageEndpoint, &instance->_private.messageEndpointAdapter.interface);
}

static void CreateGea2Node(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_Uart_t *uart = DataModelErdPointerAccess_GetUart(dataModel, Erd_Gea2Uart);

   if(uart)
   {
      Gea2Configurator_CreateDefaultBackgroundSingleWireUartInterfaceNode(
         &instance->_private.configurator,
         &instance->_private.gea2Bus.node,
         &instance->_private.gea2Bus.nodeResources,
         uart,
         DataModelErdPointerAccess_GetTimeSource(dataModel, Erd_TimeSource),
         DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
         geaAddress);

      Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
         &instance->_private.configurator,
         &instance->_private.gea2Bus.node,
         &instance->_private.gea2Bus.dynamicRoutingResources,
         instance->_private.gea2Bus.dynamicRoutingTable,
         ELEMENT_COUNT(instance->_private.gea2Bus.dynamicRoutingTable));
   }
}

static void CreateWifiNode(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_WifiBufferedUart);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.wifi.node,
      &instance->_private.wifi.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.wifi.buffers.sendBuffer,
      sizeof(instance->_private.wifi.buffers.sendBuffer),
      instance->_private.wifi.buffers.receiveBuffer,
      sizeof(instance->_private.wifi.buffers.receiveBuffer),
      instance->_private.wifi.buffers.packetQueueStorage,
      sizeof(instance->_private.wifi.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.wifi.node,
      &instance->_private.wifi.dynamicRoutingResources,
      instance->_private.wifi.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.wifi.dynamicRoutingTable));
}

static void CreateFactoryNode(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_FactoryBufferedUart);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.factory.node,
      &instance->_private.factory.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.factory.buffers.sendBuffer,
      sizeof(instance->_private.factory.buffers.sendBuffer),
      instance->_private.factory.buffers.receiveBuffer,
      sizeof(instance->_private.factory.buffers.receiveBuffer),
      instance->_private.factory.buffers.packetQueueStorage,
      sizeof(instance->_private.factory.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.factory.node,
      &instance->_private.factory.dynamicRoutingResources,
      instance->_private.factory.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.factory.dynamicRoutingTable));
}

static void CreateDoorNode(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_DoorBufferedUart);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.door.node,
      &instance->_private.door.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.door.buffers.sendBuffer,
      sizeof(instance->_private.door.buffers.sendBuffer),
      instance->_private.door.buffers.receiveBuffer,
      sizeof(instance->_private.door.buffers.receiveBuffer),
      instance->_private.door.buffers.packetQueueStorage,
      sizeof(instance->_private.door.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.door.node,
      &instance->_private.door.dynamicRoutingResources,
      instance->_private.door.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.door.dynamicRoutingTable));
}

static void CreateCaseNode(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   I_BufferedUart_t *bufferedUart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_CaseBufferedUart);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.caseBus.node,
      &instance->_private.caseBus.nodeResources,
      bufferedUart,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.caseBus.buffers.sendBuffer,
      sizeof(instance->_private.caseBus.buffers.sendBuffer),
      instance->_private.caseBus.buffers.receiveBuffer,
      sizeof(instance->_private.caseBus.buffers.receiveBuffer),
      instance->_private.caseBus.buffers.packetQueueStorage,
      sizeof(instance->_private.caseBus.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.caseBus.node,
      &instance->_private.caseBus.dynamicRoutingResources,
      instance->_private.caseBus.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.caseBus.dynamicRoutingTable));
}

void GeaStack_Init(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource,
   uint8_t geaAddress)
{
   Gea2Configurator_Init(&instance->_private.configurator);
   Gea2Configurator_EnablePromiscuousRouting(&instance->_private.configurator);

   CreateGea2Node(instance, dataModel, geaAddress);
   CreateWifiNode(instance, dataModel, geaAddress);
   CreateFactoryNode(instance, dataModel, geaAddress);
   CreateDoorNode(instance, dataModel, geaAddress);
   CreateCaseNode(instance, dataModel, geaAddress);

   CreatePacketAndMessageEndpoints(instance, dataModel, geaAddress);

   ConnectGea2MessageEndpointToDataSource(
      instance,
      dataModel,
      externalDataSource,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));

   Gea2CommonCommands_Init(
      &instance->_private.commonCommands,
      instance->_private.packetEndpoint,
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      Gea2CrcSeed,
      Header_GetImageHeader(ImageType_BootLoader),
      Header_GetImageHeader(ImageType_Application),
      Header_GetImageHeader(ImageType_Parametric),
      Header_GetImageHeader(ImageType_Auxiliary),
      DataModelErdPointerAccess_GetAction(dataModel, Erd_JumpToBootLoaderAction),
      DataModelErdPointerAccess_GetAction(dataModel, Erd_SystemResetAction),
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      NULL,
      0,
      ParametricHackRequired,
      0,
      DataModel_GetInput(dataModel, Erd_ReadyToEnterBootLoader));
}

I_Gea2PacketEndpoint_t *GeaStack_GetGea2PacketEndpoint(GeaStack_t *instance)
{
   return &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface;
}

void GeaStack_Run(GeaStack_t *instance)
{
   Gea2Configurator_Run(&instance->_private.configurator);
}
