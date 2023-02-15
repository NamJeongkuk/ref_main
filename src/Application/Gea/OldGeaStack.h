/*!
 * @file
 * @brief Initialize Gea communication
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef OLDGEASTACK_H
#define OLDGEASTACK_H

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
#include "ErdGea2ReadWriteApiRevision2.h"
#include "ErdGea2SubscriptionApiRevision2.h"
#include "ConstArrayMap_BinarySearch.h"
#include "ErdStreamReceiver.h"
#include "GeaStackXmacroUtils.h"
#include "SystemErds.h"

enum
{
   MaxNumberOfSubscriptions = 5,
   SendBufferSize = 251,
   ReceiveBufferSize = 251,
   SendReceiveBufferSize = Gea2Interface_SingleWireMaxSendReceiveBufferSize,
   PacketQueueStorageSize = 300,
   DynamicRoutingTableBufferSize = 8,
   ErdApiV2SubscriptionClients = 1
};

// clang-format off

#define EXPAND_AS_PUBLIC_ERD_COUNT_STRUCT_MEMBER(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_PUBLIC_, Number)(uint8_t Name;)

// clang-format on

typedef struct
{
   ERD_TABLE(EXPAND_AS_PUBLIC_ERD_COUNT_STRUCT_MEMBER)
} OldGeaStackPublicErdCount_t;

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
      uint8_t subscriptionBuffers[ErdApiV2SubscriptionClients][sizeof(OldGeaStackPublicErdCount_t) / 4 + 1];
      ConstArrayMap_BinarySearch_t publicErdMap;
      ErdStreamReceiver_t erdStreamReceiver;
      DataSourcePacketReadWriteManager_Simple_t dataSourceReadWriteManager;
      DataSourcePacketGea2MessageEndpointConnector_t dataSourceEndpointConnector;
      DataSourcePacketSubscriptionFrontEnd_Simple_t dataSourceSubscriptionFrontEnd;
      DataSourcePacketSubscriptionManager_Simple_SubscriptionListItem_t subscriptionList[MaxNumberOfSubscriptions];
      DataSourcePacketSubscriptionManager_Simple_t dataSourceSubscriptionManager;
      ErdGea2ReadWriteApiRevision2_t erdApiRevision2;

      Gea2Configurator_t configurator;
      Gea2ConfiguratorNode_t applicationNode;
      Gea2ConfiguratorPacketEndpointNodeResources_t applicationPacketEndpointResources;

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorDefaultForegroundSingleWireUartInterfaceNodeResources_t nodeResources;
         Gea2ConfiguratorStaticRoutingTableResources_t staticRoutingResources;
         Gea2ConfiguratorDynamicRoutingTableWithReplacementResources_t dynamicRoutingResources;
         uint8_t dynamicRoutingTable[DynamicRoutingTableBufferSize];
      } internal;

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorDefaultForegroundSingleWireUartInterfaceNodeResources_t nodeResources;
         Gea2ConfiguratorDynamicRoutingTableWithReplacementResources_t dynamicRoutingResources;
         uint8_t dynamicRoutingTable[DynamicRoutingTableBufferSize];
      } external;

      struct
      {
         Gea2PacketEndpoint_PacketRestrictor_t restrictedErdPacketRestrictor;
         Input_ErdGea2UnlockState_t erdGea2UnlockStateInput;
         Validator_RestrictedRangeErd_t restrictedRangeErdValidator;
      } erdSecurity;
   } _private;
} OldGeaStack_t;

/*!
 * @param instance
 * @param dataModel
 * @param externalDataSource
 */
void OldGeaStack_Init(
   OldGeaStack_t *instance,
   I_DataModel_t *dataModel,
   I_DataSource_t *externalDataSource,
   uint8_t geaAddress,
   const uint8_t *staticRoutingTable,
   uint8_t staticRoutingTableEntryCount);

/*!
 * @param instance
 * @return
 */
I_Gea2PacketEndpoint_t *OldGeaStack_GetGea2PacketEndpoint(OldGeaStack_t *instance);

/*!
 * @param instance
 */
void OldGeaStack_Run(OldGeaStack_t *instance);

#endif
