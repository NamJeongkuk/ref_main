/*!
 * @file
 * @brief Initialize Gea communication
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GEASTACK_H
#define GEASTACK_H

#include "Gea2Configurator.h"
#include "Gea2MessageEndpoint_Gea2PacketEndpointAdapter.h"
#include "SimpleDataSourceMessageEndpointConnector.h"
#include "DataSourcePacketSubscriptionManager_Simple.h"
#include "Gea2CommonCommands.h"
#include "I_DataModel.h"
#include "I_DataSource.h"
#include "Gea2PacketEndpoint_PacketRestrictor.h"
#include "Input_ErdGea2UnlockState.h"
#include "Validator_RestrictedRangeErd.h"
#include "ErdClient_ApiRevision2.h"
#include "ErdGea2ReadWriteApiRevision2.h"
#include "ErdGea2SubscriptionApiRevision2.h"
#include "ConstArrayMap_BinarySearch.h"
#include "ErdStreamReceiver.h"
#include "ErdXMacroUtils.h"
#include "SystemErds.h"

enum
{
   SendBufferSize = 251,
   ReceiveBufferSize = 251,
   SendReceiveBufferSize = Gea2Interface_SingleWireMaxSendReceiveBufferSize,
   PacketQueueStorageSize = 300,
   DynamicRoutingTableBufferSize = 8,
   ErdApiV2SubscriptionClients = 5,
   QueueBufferElements = 1024
};

// clang-format off

#define EXPAND_AS_PUBLIC_AND_SERVICE_ERD_COUNT_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_PUBLIC_AND_SERVICE_, Number)(uint8_t Name;)

// clang-format on

typedef struct
{
   ERD_TABLE(EXPAND_AS_PUBLIC_AND_SERVICE_ERD_COUNT_STRUCT_MEMBER)
} GeaStackPublicAndServiceErdCount_t;

typedef struct
{
   struct
   {
      I_Gea2PacketEndpoint_t *packetEndpoint;
      Gea2MessageEndpoint_Gea2PacketEndpointAdapter_t messageEndpointAdapter;
      Gea2CommonCommands_t commonCommands;
      ErdGea2ReadWriteApiRevision2_t erdApiRevision2ReadWrite;
      ErdGea2SubscriptionApiRevision2_t erdApiRevision2Subscription;
      ErdGea2SubscriptionApiRevision2SubscriptionResources_t subscriptionResources[ErdApiV2SubscriptionClients];
      uint8_t subscriptionBuffers[ErdApiV2SubscriptionClients][sizeof(GeaStackPublicAndServiceErdCount_t) / 4 + 1];
      ConstArrayMap_BinarySearch_t publicAndServiceErdMap;
      ErdGea2ReadWriteApiRevision2_t erdApiRevision2;

      Gea2Configurator_t configurator;
      Gea2ConfiguratorNode_t applicationNode;
      Gea2ConfiguratorPacketEndpointNodeResources_t applicationPacketEndpointResources;

      ErdClient_ApiRevision2_t erdClient;
      uint8_t queueBuffer[QueueBufferElements];

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorDefaultBackgroundSingleWireUartInterfaceNodeResources_t nodeResources;
         Gea2ConfiguratorDynamicRoutingTableWithReplacementResources_t dynamicRoutingResources;
         uint8_t dynamicRoutingTable[DynamicRoutingTableBufferSize];
      } gea2Bus;

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorCustomBufferedFullDuplexUartInterfaceNodeResources_t nodeResources;
         Gea2ConfiguratorDynamicRoutingTableWithReplacementResources_t dynamicRoutingResources;
         uint8_t dynamicRoutingTable[DynamicRoutingTableBufferSize];

         struct
         {
            uint8_t sendBuffer[SendBufferSize];
            uint8_t receiveBuffer[ReceiveBufferSize];
            uint8_t packetQueueStorage[PacketQueueStorageSize];
         } buffers;
      } wifi;

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorCustomBufferedFullDuplexUartInterfaceNodeResources_t nodeResources;
         union
         {
            Gea2ConfiguratorDynamicRoutingTableWithReplacementResources_t dynamicRoutingResources;
            Gea2ConfiguratorFullRoutingTableResources_t fullRoutingResources;
         } resources;
         uint8_t dynamicRoutingTable[DynamicRoutingTableBufferSize];

         struct
         {
            uint8_t sendBuffer[SendBufferSize];
            uint8_t receiveBuffer[ReceiveBufferSize];
            uint8_t packetQueueStorage[PacketQueueStorageSize];
         } buffers;
      } factory;

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorCustomBufferedFullDuplexUartInterfaceNodeResources_t nodeResources;
         Gea2ConfiguratorDynamicRoutingTableWithReplacementResources_t dynamicRoutingResources;
         uint8_t dynamicRoutingTable[DynamicRoutingTableBufferSize];

         struct
         {
            uint8_t sendBuffer[SendBufferSize];
            uint8_t receiveBuffer[ReceiveBufferSize];
            uint8_t packetQueueStorage[PacketQueueStorageSize];
         } buffers;
      } door;

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorCustomBufferedFullDuplexUartInterfaceNodeResources_t nodeResources;
         Gea2ConfiguratorDynamicRoutingTableWithReplacementResources_t dynamicRoutingResources;
         uint8_t dynamicRoutingTable[DynamicRoutingTableBufferSize];

         struct
         {
            uint8_t sendBuffer[SendBufferSize];
            uint8_t receiveBuffer[ReceiveBufferSize];
            uint8_t packetQueueStorage[PacketQueueStorageSize];
         } buffers;
      } caseBus;

      struct
      {
         Gea2PacketEndpoint_PacketRestrictor_t restrictedErdPacketRestrictor;
         Input_ErdGea2UnlockState_t erdGea2UnlockStateInput;
         Validator_RestrictedRangeErd_t restrictedRangeErdValidator;
      } erdSecurity;
   } _private;
} GeaStack_t;

/*!
 * @param instance
 * @param dataModel
 * @param externalDataSource
 */
void GeaStack_Init(
   GeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource,
   uint8_t geaAddress);

/*!
 * @param instance
 * @return
 */
I_Gea2PacketEndpoint_t *GeaStack_GetGea2PacketEndpoint(GeaStack_t *instance);

/*!
 * @param instance
 */
void GeaStack_Run(GeaStack_t *instance);

#endif
