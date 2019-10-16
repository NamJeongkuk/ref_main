/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GeaStack.h"
#include "DataModelErdPointerAccess.h"
#include "Gea2Message.h"
#include "Gea2Addresses.h"
#include "Header.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "uassert.h"

enum
{
   Gea2CrcSeed = 0x1021,
   ParametricHackRequired = false,
   UnlockErdSecurityPeriodMsec = 30 * MSEC_PER_MIN,
   MissedAckLimit = 31,
   RestrictedFullRangeErdStart = 0xF000,
   RestrictedFullRangeErdEnd = 0xFFFF,
   RetryCount = 3
};

static const uint8_t rangeRestrictedValidatorRestrictedAddressTable[] =
   {
      EmbeddedWiFiGeaAddress,
      ConnectPlusWiFiGeaAddress
   };

static const Validator_RestrictedRangeErdConfiguration_t restrictedRangeConfiguration =
   {
      rangeRestrictedValidatorRestrictedAddressTable,
      NUM_ELEMENTS(rangeRestrictedValidatorRestrictedAddressTable),
      RestrictedFullRangeErdStart,
      RestrictedFullRangeErdEnd
   };

static void InitializeErdSecurityComponents(
   GeaStack_t *instance,
   I_DataModel_t *dataModel)
{
   // Set the ErdGea2Unlock State by an unlock gea2 command or a lock gea2 command
   Input_ErdGea2UnlockState_Init(
      &instance->_private.erdSecurity.erdGea2UnlockStateInput,
      instance->_private.packetEndpoint,
      UnlockErdSecurityPeriodMsec,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));

   // Validate if restricted range erd is valid or not
   Validator_RestrictedRangeErd_Init(
      &instance->_private.erdSecurity.restrictedRangeErdValidator,
      &instance->_private.erdSecurity.erdGea2UnlockStateInput.interface,
      &restrictedRangeConfiguration);

   // Wrap GEA2PacketEndPoint to restrict GEA2Packet using Validator
   Gea2PacketEndpoint_PacketRestrictor_Init(
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor,
      instance->_private.packetEndpoint,
      &instance->_private.erdSecurity.restrictedRangeErdValidator.interface);
}

static void ConnectGea2MessageEndpointToDataSource(
   GeaStack_t *instance,
   I_DataSource_t *externalDataSource)
{
   // Datasource/ERD handling system and GEA interface
   DataSourcePacketReadWriteManager_Init(&instance->_private.dataSourceReadWriteManager, externalDataSource);
   DataSourcePacketSubscriptionManager_Simple_Init(
      &instance->_private.dataSourceSubscriptionManager,
      externalDataSource,
      instance->_private.subscriptionList,
      MaxNumberOfSubscriptions,
      MissedAckLimit);
   DataSourcePacketSubscriptionFrontEnd_Simple_Init(
      &instance->_private.dataSourceSubscriptionFrontEnd,
      &instance->_private.dataSourceSubscriptionManager.interface);

   ErdGea2ReadWriteApiRevision2_Init(
      &instance->_private.erdApiRevision2,
      externalDataSource,
      NULL,
      NULL,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface);

   DataSourcePacketGea2MessageEndpointConnector_Init(
      &instance->_private.dataSourceEndpointConnector,
      &instance->_private.messageEndpointAdapter.interface,
      &instance->_private.dataSourceReadWriteManager.interface,
      &instance->_private.dataSourceSubscriptionFrontEnd.interface,
      GEA2MESSAGE_MAXPAYLOAD,
      RetryCount,
      RetryCount);
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

   // Validate if Erds in the restricted range are restricted
   InitializeErdSecurityComponents(instance, dataModel);

   Gea2MessageEndpoint_Gea2PacketEndpointAdapter_Init(
      &instance->_private.messageEndpointAdapter,
      &instance->_private.erdSecurity.restrictedErdPacketRestrictor.interface);

   DataModelErdPointerAccess_Write(dataModel, Erd_Gea2MessageEndpoint, &instance->_private.messageEndpointAdapter.interface);
}

static void CreateInternalNode(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress,
   const uint8_t *staticRoutingTable,
   uint8_t staticRoutingTableEntryCount)
{
   I_BufferedUart_t *uart = DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_InternalBufferedUart);

   if(uart)
   {
      Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
         &instance->_private.configurator,
         &instance->_private.internal.node,
         &instance->_private.internal.nodeResources,
         uart,
         DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
         geaAddress,
         RetryCount,
         instance->_private.internal.buffers.sendBuffer,
         sizeof(instance->_private.internal.buffers.sendBuffer),
         instance->_private.internal.buffers.receiveBuffer,
         sizeof(instance->_private.internal.buffers.receiveBuffer),
         instance->_private.internal.buffers.packetQueueStorage,
         sizeof(instance->_private.internal.buffers.packetQueueStorage));

      Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
         &instance->_private.configurator,
         &instance->_private.internal.node,
         &instance->_private.internal.dynamicRoutingResources,
         instance->_private.internal.dynamicRoutingTable,
         ELEMENT_COUNT(instance->_private.internal.dynamicRoutingTable));

      Gea2Configurator_AddStaticRoutingTableToNode(
         &instance->_private.configurator,
         &instance->_private.internal.node,
         &instance->_private.internal.staticRoutingResources,
         staticRoutingTable,
         staticRoutingTableEntryCount);
   }
}

static void CreateExternalNode(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   uint8_t geaAddress)
{
   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.external.node,
      &instance->_private.external.nodeResources,
      DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_ExternalBufferedUart),
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      geaAddress,
      RetryCount,
      instance->_private.external.buffers.sendBuffer,
      sizeof(instance->_private.external.buffers.sendBuffer),
      instance->_private.external.buffers.receiveBuffer,
      sizeof(instance->_private.external.buffers.receiveBuffer),
      instance->_private.external.buffers.packetQueueStorage,
      sizeof(instance->_private.external.buffers.packetQueueStorage));

   Gea2Configurator_AddDynamicRoutingTableWithReplacementToNode(
      &instance->_private.configurator,
      &instance->_private.external.node,
      &instance->_private.external.dynamicRoutingResources,
      instance->_private.external.dynamicRoutingTable,
      ELEMENT_COUNT(instance->_private.external.dynamicRoutingTable));
}

void GeaStack_Init(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource,
   uint8_t geaAddress,
   const uint8_t *staticRoutingTable,
   uint8_t staticRoutingTableEntryCount)
{
   Gea2Configurator_Init(&instance->_private.configurator);

   CreateInternalNode(instance, dataModel, geaAddress, staticRoutingTable, staticRoutingTableEntryCount);
   CreateExternalNode(instance, dataModel, geaAddress);

   CreatePacketAndMessageEndpoints(instance, dataModel, geaAddress);

   ConnectGea2MessageEndpointToDataSource(instance, externalDataSource);

   DataModel_Write(dataModel, Erd_ReadyToEnterBootLoader, enabled);

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
