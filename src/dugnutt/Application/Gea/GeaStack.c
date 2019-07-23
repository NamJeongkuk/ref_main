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
   GeaAddress = DugnuttGeaAddress,
   Gea2CrcSeed = 0x1021,
   ParametricHackRequired = false,
   UnlockErdSecurityPeriodMsec = 30 * MSEC_PER_MIN,
   MissedAckLimit = 31,
   RestrictedFullRangeErdStart = 0xF000,
   RestrictedFullRangeErdEnd = 0xFFFF,
   RetryCount = 3
};

static const uint8_t staticRoutingTable[] =
   {
      TrukGeaAddress
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
   Input_ErdGea2UnlockState_Init(
      &instance->_private.erdSecurity.erdGea2UnlockStateInput,
      instance->_private.packetEndpoint,
      UnlockErdSecurityPeriodMsec,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule));

   Validator_RestrictedRangeErd_Init(
      &instance->_private.erdSecurity.restrictedRangeErdValidator,
      &instance->_private.erdSecurity.erdGea2UnlockStateInput.interface,
      &restrictedRangeConfiguration);

   Gea2MessageEndpoint_MessageRestrictor_Init(
      &instance->_private.erdSecurity.restrictedErdMessageRestrictor,
      &instance->_private.messageEndpointAdapter.interface,
      &instance->_private.erdSecurity.restrictedRangeErdValidator.interface);

   DataModelErdPointerAccess_Write(
      dataModel,
      Erd_Gea2MessageEndpoint,
      &instance->_private.erdSecurity.restrictedErdMessageRestrictor.interface);
}

static void ConnectGea2MessageEndpointToDataSource(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource)
{
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
      instance->_private.packetEndpoint);

   InitializeErdSecurityComponents(instance, dataModel);

   DataSourcePacketGea2MessageEndpointConnector_Init(
      &instance->_private.dataSourceEndpointConnector,
      &instance->_private.erdSecurity.restrictedErdMessageRestrictor.interface,
      &instance->_private.dataSourceReadWriteManager.interface,
      &instance->_private.dataSourceSubscriptionFrontEnd.interface,
      GEA2MESSAGE_MAXPAYLOAD,
      RetryCount,
      RetryCount);
}

static void CreatePacketAndMessageEndpoints(
   GeaStack_t *instance,
   I_DataModel_t *dataModel)
{
   instance->_private.packetEndpoint = Gea2Configurator_CreatePacketEndpointNode(
      &instance->_private.configurator,
      &instance->_private.applicationNode,
      &instance->_private.applicationNodeResources,
      GeaAddress);

   Gea2MessageEndpoint_Gea2PacketEndpointAdapter_Init(
      &instance->_private.messageEndpointAdapter,
      instance->_private.packetEndpoint);

   DataModelErdPointerAccess_Write(
      dataModel,
      Erd_Gea2MessageEndpoint,
      &instance->_private.messageEndpointAdapter.interface);
}

static void InitializeInternalNode(GeaStack_t *instance, I_DataModel_t *dataModel)
{
   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.internal.node,
      &instance->_private.internal.nodeResources,
      DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_InternalBufferedUart),
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      GeaAddress,
      RetryCount,
      instance->_private.internal.buffers.sendBuffer,
      sizeof(instance->_private.internal.buffers.sendBuffer),
      instance->_private.internal.buffers.receiveBuffer,
      sizeof(instance->_private.internal.buffers.receiveBuffer),
      instance->_private.internal.buffers.packetQueueStorage,
      sizeof(instance->_private.internal.buffers.packetQueueStorage));

   uassert(GEA2PACKET_MAXPAYLOAD == Gea2Interface_MaxPayloadLength(&instance->_private.internal.nodeResources._private.gea2Interface.interface));

   Gea2Configurator_AddStaticRoutingTableToNode(
      &instance->_private.configurator,
      &instance->_private.internal.node,
      &instance->_private.internal.staticRoutingResources,
      staticRoutingTable,
      ELEMENT_COUNT(staticRoutingTable));
}

static void InitializeExternalNode(GeaStack_t *instance, I_DataModel_t *dataModel)
{
   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.external.node,
      &instance->_private.external.nodeResources,
      DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_ExternalBufferedUart),
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      GeaAddress,
      RetryCount,
      instance->_private.external.buffers.sendBuffer,
      sizeof(instance->_private.external.buffers.sendBuffer),
      instance->_private.external.buffers.receiveBuffer,
      sizeof(instance->_private.external.buffers.receiveBuffer),
      instance->_private.external.buffers.packetQueueStorage,
      sizeof(instance->_private.external.buffers.packetQueueStorage));

   uassert(GEA2PACKET_MAXPAYLOAD == Gea2Interface_MaxPayloadLength(&instance->_private.external.nodeResources._private.gea2Interface.interface));

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
   I_DataSource_t *externalDataSource)
{
   Gea2Configurator_Init(&instance->_private.configurator);
   InitializeInternalNode(instance, dataModel);
   InitializeExternalNode(instance, dataModel);
   CreatePacketAndMessageEndpoints(instance, dataModel);

   ConnectGea2MessageEndpointToDataSource(instance, dataModel, externalDataSource);

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

I_Gea2PacketEndpoint_t * GeaStack_GetGea2PacketEndpoint(GeaStack_t *instance)
{
   return instance->_private.packetEndpoint;
}

void GeaStack_Run(GeaStack_t *instance)
{
   Gea2Configurator_Run(&instance->_private.configurator);
}
