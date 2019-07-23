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

static const uint8_t staticRoutingTable[] =
   {
      McBoardAddress
   };

static const uint8_t blockedSourceAddressTable[] =
   {
      EmbeddedWiFiGeaAddress,
      ConnectPlusWiFiGeaAddress
   };

static const Validator_RestrictedRangeErdConfiguration_t restrictedRangeConfiguration =
   {
      blockedSourceAddressTable,
      NUM_ELEMENTS(blockedSourceAddressTable),
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
   // Datasource/ERD handling system and GEA interface
   DataSourcePacketReadWriteManager_Init(&instance->_private.dataSourceReadWriteManager, externalDataSource);
   DataSourcePacketSubscriptionManager_Simple_Init(
      &instance->_private.dataSourceSubscriptionManager,
      externalDataSource,
      instance->_private.subscriptionList,
      NUMBER_OF_SUBSCRIPTIONS,
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

   // Validate if Erds in the restricted range are restricted
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
   I_Gea2MessageEndpoint_t *messageEndpoint;

   instance->_private.packetEndpoint =
      Gea2Configurator_CreatePacketEndpointNode(
         &instance->_private.configurator,
         &instance->_private.node,
         &instance->_private.packetEndpointResources,
         Gea2Address);

   Gea2MessageEndpoint_Gea2PacketEndpointAdapter_Init(
      &instance->_private.messageEndpointAdapter,
      instance->_private.packetEndpoint);

   messageEndpoint = &instance->_private.messageEndpointAdapter.interface;
   DataModelErdPointerAccess_Write(dataModel, Erd_Gea2MessageEndpoint, &messageEndpoint);
}

void GeaStack_Init(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource)
{
   Gea2Configurator_Init(&instance->_private.configurator);

   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.node,
      &instance->_private.customBackgroundResources,
      DataModelErdPointerAccess_GetBufferedUart(dataModel, Erd_BufferedUart),
      DataModelErdPointerAccess_GetCrc16Calculator(dataModel, Erd_CrcCalcTable),
      DugnuttGeaAddress,
      RetryCount,
      instance->_private.interfaceSendBuffer,
      sizeof(instance->_private.interfaceSendBuffer),
      instance->_private.interfaceReceiveBuffer,
      sizeof(instance->_private.interfaceReceiveBuffer),
      instance->_private.packetQueueStorage,
      sizeof(instance->_private.packetQueueStorage));

   uassert(GEA2PACKET_MAXPAYLOAD == Gea2Interface_MaxPayloadLength(&instance->_private.customBackgroundResources._private.gea2Interface.interface));

   Gea2Configurator_AddStaticRoutingTableToNode(
      &instance->_private.configurator,
      &instance->_private.node,
      &instance->_private.staticRoutingResources,
      staticRoutingTable,
      ELEMENT_COUNT(staticRoutingTable));

   Gea2Configurator_AddSimpleDynamicRoutingTableToNode(
      &instance->_private.configurator,
      &instance->_private.node,
      &instance->_private.dynamicRoutingTableResources,
      instance->_private.dynamicRoutingTable,
      sizeof(instance->_private.dynamicRoutingTable));

   CreatePacketAndMessageEndpoints(instance, dataModel);

   ConnectGea2MessageEndpointToDataSource(instance, dataModel, externalDataSource);

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

I_Gea2PacketEndpoint_t * GeaStack_GetGea2PacketEndpoint(GeaStack_t *instance)
{
   return instance->_private.packetEndpoint;
}

void GeaStack_RunGea(GeaStack_t *instance)
{
   Gea2Configurator_Run(&instance->_private.configurator);
}
