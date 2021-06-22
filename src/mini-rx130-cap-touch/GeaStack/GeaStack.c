/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GeaStack.h"
#include "DataSourceErdPointerAccess.h"
#include "Gea2Message.h"
#include "Gea2Addresses.h"
#include "MemoryMap.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "Crc16Calculator_Table.h"
#include "Input_True.h"
#include "uassert.h"

enum
{
   Gea2CrcSeed = 0x1021,
   RetryCount = 3,
   ParametricHackNotRequired = false
};

static void ConnectGea2MessageEndpointToDataSource(
   GeaStack_t *instance,
   I_DataSource_t *externalDataSource)
{
   ErdGea2ReadWriteApiRevision2_Init(
      &instance->_private.erdApiRevision2ReadWrite,
      externalDataSource,
      NULL,
      NULL,
      instance->_private.packetEndpoint);
}

static void CreatePacketEndpoint(
   GeaStack_t *instance,
   uint8_t geaAddress)
{
   instance->_private.packetEndpoint =
      Gea2Configurator_CreatePacketEndpointNode(
         &instance->_private.configurator,
         &instance->_private.applicationNode,
         &instance->_private.applicationPacketEndpointResources,
         geaAddress);
}

static void CreateUartNode(
   GeaStack_t *instance,
   I_BufferedUart_t *uart,
   uint8_t geaAddress)
{
   Gea2Configurator_CreateCustomBufferedFullDuplexUartInterfaceNode(
      &instance->_private.configurator,
      &instance->_private.node,
      &instance->_private.nodeResources,
      uart,
      Crc16Calculator_Table,
      geaAddress,
      RetryCount,
      instance->_private.buffers.sendBuffer,
      sizeof(instance->_private.buffers.sendBuffer),
      instance->_private.buffers.receiveBuffer,
      sizeof(instance->_private.buffers.receiveBuffer),
      instance->_private.buffers.packetQueueStorage,
      sizeof(instance->_private.buffers.packetQueueStorage));

   Gea2Configurator_AddFullRoutingTableToNode(
      &instance->_private.configurator,
      &instance->_private.node,
      &instance->_private.fullRoutingResources);
}

void GeaStack_Init(
   GeaStack_t *instance,
   I_DataSource_t *dataSource,
   I_DataSource_t *externalDataSource,
   I_BufferedUart_t *uart,
   I_Action_t *jumpToBootLoaderAction,
   I_Action_t *resetAction,
   uint8_t geaAddress)
{
   Gea2Configurator_Init(&instance->_private.configurator);

   CreateUartNode(instance, uart, geaAddress);

   CreatePacketEndpoint(instance, geaAddress);

   ConnectGea2MessageEndpointToDataSource(instance, externalDataSource);

   Gea2CommonCommands_Init(
      &instance->_private.commonCommands,
      instance->_private.packetEndpoint,
      Crc16Calculator_Table,
      Gea2CrcSeed,
      BootLoaderImageHeader,
      ApplicationImageHeader,
      NULL,
      NULL,
      jumpToBootLoaderAction,
      resetAction,
      DataSourceErdPointerAccess_GetTimerModule(dataSource, Erd_TimerModule),
      NULL,
      0,
      ParametricHackNotRequired,
      0,
      Input_True_GetInstance());
}

I_Gea2PacketEndpoint_t *GeaStack_Gea2PacketEndpoint(GeaStack_t *instance)
{
   return instance->_private.packetEndpoint;
}

void GeaStack_Run(GeaStack_t *instance)
{
   Gea2Configurator_Run(&instance->_private.configurator);
}
