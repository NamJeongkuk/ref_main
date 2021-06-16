/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "McCommunicationPlugin.h"
#include "SystemErds.h"
#include "TinyErdHeartbeat.h"
#include "TinyErdStreamSender.h"
#include "Gea2Addresses.h"

enum
{
   HeartbeatPeriod = 200
};

static TinyErdHeartbeat_t erdHeartbeat;
static TinyErdStreamSender_t erdStreamSender;

static const TinyErdHeartbeatErdPair_t erdHeartbeatPairs[] = {
   { .local = Erd_ErdStream, .remote = RemoteErdStreamErd },
   { .local = Erd_HeartbeatTick, .remote = RemoteHeartbeatTickErd }
};

static const TinyErdHeartbeatConfiguration_t erdHeartbeatConfiguration = {
   .destination = PcToolAddress,
   .period = HeartbeatPeriod,
   .pairs = erdHeartbeatPairs,
   .pairCount = NUM_ELEMENTS(erdHeartbeatPairs)
};

#define EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP(Name, Number, DataType, Stream, StreamRemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)                                                         \
   ({ Number COMMA StreamRemoteErd COMMA CONCAT(INCLUDE_STREAM_EVENT_, Stream)(ErdStreamDataType_Event) CONCAT(INCLUDE_STREAM_LEVEL_, Stream)(ErdStreamDataType_State) } COMMA)

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

void McCommunicationPlugin_Init(
   I_TinyDataSource_t *dataSource,
   I_TinyDataSource_t *externalDataSource,
   I_TinyGea2Interface_t *gea2Interface)
{
   TinyTimerModule_t *timerModule;
   TinyDataSource_Read(dataSource, Erd_TimerModule, &timerModule);

   TinyErdHeartbeat_Init(
      &erdHeartbeat,
      gea2Interface,
      externalDataSource,
      timerModule,
      &erdHeartbeatConfiguration);

   TinyErdStreamSender_Init(
      &erdStreamSender,
      externalDataSource,
      &erdStreamSenderConfiguration);
}
