/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GeaStack.h"
#include "Gea2Addresses.h"
#include "SystemErds.h"
#include "MemoryMap.h"
#include "Reset.h"
#include "ReadyToEnterBootLoader.h"
#include "TinyCommonCommands.h"
#include "TinyErdHeartbeat.h"
#include "TinyErdStreamSender.h"
#include "TinyErdGea2OpenLoopWriteApiRevision2.h"
#include "TinyGea2Interface_FullDuplex.h"
#include "utils.h"

enum
{
   Gea2SendBufferSize = 100,
   Gea2ReceiveBufferSize = 100,

   HeartbeatDestinationAddress = 0xC0,
   HeartbeatPeriod = 200,

   RemoteErdStreamErd = 0xF100,
   RemoteHeartbeatTickErd = 0xF101
};

static TinyGea2Interface_FullDuplex_t gea2Interface;
static uint8_t sendBuffer[Gea2SendBufferSize];
static uint8_t receiveBuffer[Gea2ReceiveBufferSize];

static TinyCommonCommands_t commonCommands;
static TinyErdHeartbeat_t erdHeartbeat;
static TinyErdStreamSender_t erdStreamSender;
static TinyErdGea2OpenLoopWriteApiRevision2_t openLoopWriteApi;

static const TinyCommonCommandsConfiguration_t commonCommandsConfiguration = {
   .bootLoaderHeader = BootLoaderHeaderAddress,
   .applicationHeader = ApplicationHeaderAddress,
   .parametricHeader = NULL,
   .reset = Reset,
   .readyToEnterBootLoader = ReadyToEnterBootLoader,
};

static const TinyErdHeartbeatErdPair_t erdHeartbeatPairs[] = {
   { .local = Erd_ErdStream, .remote = RemoteErdStreamErd },
   { .local = Erd_HeartbeatTick, .remote = RemoteHeartbeatTickErd },
};

static const TinyErdHeartbeatConfiguration_t erdHeartbeatConfiguration = {
   .destination = HeartbeatDestinationAddress,
   .period = HeartbeatPeriod,
   .pairs = erdHeartbeatPairs,
   .pairCount = NUM_ELEMENTS(erdHeartbeatPairs),
   .enableHeartbeat = Erd_Invalid
};

#define EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP(Name, Number, DataType, Stream, StreamRemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)                                                         \
   ({ Number COMMA StreamRemoteErd COMMA CONCAT(INCLUDE_STREAM_EVENT_, Stream)(ErdStreamDataType_Event) CONCAT(INCLUDE_STREAM_LEVEL_, Stream)(ErdStreamDataType_Level) } COMMA)

static const ErdStreamLocalToRemoteErdMap_t streamLocalToRemoteErdMap[] = {
   ERD_TABLE(EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP)
};

static const TinyErdStreamSenderConfiguration_t erdStreamSenderConfiguration = {
   .erdStreamErd = Erd_ErdStream,
   .requestedStateErdFromReceiver = Erd_ErdStreamRequestedState,
   .streamEntryCount = StreamEntryCount,
   .sizeOfLargestStreamedErd = sizeof(StreamedErd_t),
   .mappings = streamLocalToRemoteErdMap,
   .mappingCount = NUM_ELEMENTS(streamLocalToRemoteErdMap)
};

void GeaStack_Init(
   TinyTimerModule_t *timerModule,
   I_TinyDataSource_t *externalDataSource,
   I_TinyUart_t *uart,
   uint8_t geaAddress)
{
   TinyGea2Interface_FullDuplex_Init(
      &gea2Interface,
      uart,
      geaAddress,
      sendBuffer,
      sizeof(sendBuffer),
      receiveBuffer,
      sizeof(receiveBuffer),
      false);

   TinyCommonCommands_Init(
      &commonCommands,
      &gea2Interface.interface,
      timerModule,
      &commonCommandsConfiguration);

   TinyErdHeartbeat_Init(
      &erdHeartbeat,
      &gea2Interface.interface,
      externalDataSource,
      timerModule,
      &erdHeartbeatConfiguration);

   TinyErdStreamSender_Init(
      &erdStreamSender,
      externalDataSource,
      &erdStreamSenderConfiguration);

   TinyErdGea2OpenLoopWriteApiRevision2_Init(
      &openLoopWriteApi,
      externalDataSource,
      &gea2Interface.interface);
}

I_TinyGea2Interface_t *GeaStack_GetGea2Interface(void)
{
   return &gea2Interface.interface;
}

void GeaStack_Run(void)
{
   TinyGea2Interface_FullDuplex_Run(&gea2Interface);
}
