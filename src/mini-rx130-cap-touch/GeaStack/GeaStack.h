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
   SendBufferSize = 251,
   ReceiveBufferSize = 251,
   PacketQueueStorageSize = 300,
};

typedef struct
{
   struct
   {
      I_Gea2PacketEndpoint_t *packetEndpoint;
      Gea2MessageEndpoint_Gea2PacketEndpointAdapter_t messageEndpointAdapter;
      Gea2CommonCommands_t commonCommands;
      ErdGea2ReadWriteApiRevision2_t erdApiRevision2ReadWrite;

      Gea2Configurator_t configurator;
      Gea2ConfiguratorNode_t applicationNode;
      Gea2ConfiguratorPacketEndpointNodeResources_t applicationPacketEndpointResources;

      Gea2ConfiguratorNode_t node;
      Gea2ConfiguratorCustomBufferedFullDuplexUartInterfaceNodeResources_t nodeResources;
      Gea2ConfiguratorFullRoutingTableResources_t fullRoutingResources;
      struct
      {
         uint8_t sendBuffer[SendBufferSize];
         uint8_t receiveBuffer[ReceiveBufferSize];
         uint8_t packetQueueStorage[PacketQueueStorageSize];
      } buffers;
   } _private;
} GeaStack_t;

/*!
 * @param instance
 * @param dataSource
 * @param externalDataSource
 * @param uart
 * @param jumpToBootLoaderAction
 * @param resetAction
 * @param geaAddress
 */
void GeaStack_Init(
   GeaStack_t *instance,
   I_DataSource_t *dataSource,
   I_DataSource_t *externalDataSource,
   I_BufferedUart_t *uart,
   I_Action_t *jumpToBootLoaderAction,
   I_Action_t *resetAction,
   uint8_t geaAddress);

/*!
 * @param instance
 * @return Gea2 Packet Endpoint
 */
I_Gea2PacketEndpoint_t *GeaStack_Gea2PacketEndpoint(GeaStack_t *instance);

/*!
 * @param instance
 */
void GeaStack_Run(GeaStack_t *instance);

#endif
