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
#include "ErdGea2ReadWriteApiRevision2.h"

enum
{
   MaxNumberOfSubscriptions = 5,
   SendBufferSize = 251,
   ReceiveBufferSize = 251,
   PacketQueueStorageSize = 300,
   DynamicRoutingTableBufferSize = 8,
};

typedef struct
{
   struct
   {
      I_Gea2PacketEndpoint_t *packetEndpoint;
      Gea2MessageEndpoint_Gea2PacketEndpointAdapter_t messageEndpointAdapter;
      SimpleDataSourceMessageEndpointConnector_t endpointConnector;
      DataSourcePacketReadWriteManager_Simple_t dataSourceReadWriteManager;
      DataSourcePacketGea2MessageEndpointConnector_t dataSourceEndpointConnector;
      DataSourcePacketSubscriptionFrontEnd_Simple_t dataSourceSubscriptionFrontEnd;
      DataSourcePacketSubscriptionManager_Simple_SubscriptionListItem_t subscriptionList[MaxNumberOfSubscriptions];
      DataSourcePacketSubscriptionManager_Simple_t dataSourceSubscriptionManager;
      Gea2CommonCommands_t commonCommands;
      ErdGea2ReadWriteApiRevision2_t erdApiRevision2;

      Gea2Configurator_t configurator;
      Gea2ConfiguratorNode_t applicationNode;
      Gea2ConfiguratorPacketEndpointNodeResources_t applicationPacketEndpointResources;

      struct
      {
         Gea2ConfiguratorNode_t node;
         Gea2ConfiguratorCustomBufferedFullDuplexUartInterfaceNodeResources_t nodeResources;
         Gea2ConfiguratorStaticRoutingTableResources_t staticRoutingResources;
         struct
         {
            uint8_t sendBuffer[SendBufferSize];
            uint8_t receiveBuffer[ReceiveBufferSize];
            uint8_t packetQueueStorage[PacketQueueStorageSize];
         } buffers;
      } internal;

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
      } external;

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
   I_DataSource_t *externalDataSource);

/*!
 * @param instance
 * @return Gea2 Packet Endpoint
 */
I_Gea2PacketEndpoint_t * GeaStack_GetGea2PacketEndpoint(GeaStack_t *instance);

/*!
 * @param instance
 */
void GeaStack_Run(GeaStack_t *instance);

#endif
