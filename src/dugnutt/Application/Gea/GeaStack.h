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
#include "Gea2MessageEndpoint_MessageRestrictor.h"
#include "Input_ErdGea2UnlockState.h"
#include "Validator_RestrictedRangeErd.h"
#include "ErdGea2ReadWriteApiRevision2.h"

#define NUMBER_OF_SUBSCRIPTIONS  (5)

enum
{
   FullDuplexMaxSendBufferSize = 251,
   InterfaceReceiveBufferSize = 251,
   PacketQueueStorageSize = 300
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
      DataSourcePacketSubscriptionManager_Simple_SubscriptionListItem_t subscriptionList[NUMBER_OF_SUBSCRIPTIONS];
      DataSourcePacketSubscriptionManager_Simple_t dataSourceSubscriptionManager;
      Gea2CommonCommands_t commonCommands;
      ErdGea2ReadWriteApiRevision2_t erdApiRevision2;

      Gea2Configurator_t configurator;
      Gea2ConfiguratorNode_t node;
      Gea2ConfiguratorPacketEndpointNodeResources_t packetEndpointResources;
      Gea2ConfiguratorCustomBufferedFullDuplexUartInterfaceNodeResources_t customBackgroundResources;
      uint8_t interfaceSendBuffer[FullDuplexMaxSendBufferSize];
      uint8_t interfaceReceiveBuffer[InterfaceReceiveBufferSize];
      uint8_t packetQueueStorage[PacketQueueStorageSize];

      Gea2ConfiguratorSimpleDynamicRoutingTableResources_t dynamicRoutingTableResources;
      Gea2ConfiguratorStaticRoutingTableResources_t staticRoutingResources;
      uint8_t dynamicRoutingTable[16];
      struct
      {
         Gea2MessageEndpoint_MessageRestrictor_t restrictedErdMessageRestrictor;
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
void GeaStack_RunGea(GeaStack_t *instance);

#endif
