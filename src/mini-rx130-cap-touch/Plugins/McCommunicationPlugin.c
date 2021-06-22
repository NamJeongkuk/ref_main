/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "McCommunicationPlugin.h"
#include "SystemErds.h"
#include "ErdHeartbeat.h"
#include "ErdStreamSender.h"
#include "Gea2Addresses.h"
#include "Gea2MessageEndpoint_Gea2PacketEndpointAdapter.h"
#include "ConstArrayMap_LinearSearch.h"
#include "utils.h"

enum
{
   HeartbeatPeriodMsec = 200,
   HeartbeatThrottleMsec = 15
};

static ErdHeartbeat_t erdHeartbeat;
static ErdStreamSender_t erdStreamSender;
static Gea2MessageEndpoint_Gea2PacketEndpointAdapter_t gea2MessageEndpoint;
static uint8_t erdStreamBuffer[sizeof(StreamEntry_t) * StreamEntryCount];
static ConstArrayMap_LinearSearch_t erdStreamStreamedErdMap;

static const ErdHeartbeatErdPairs_t erdHeartbeatPairs[] = {
   { .local = Erd_ErdStream, .remote = RemoteErdStreamErd },
   { .local = Erd_HeartbeatTick, .remote = RemoteHeartbeatTickErd }
};

static const ErdHeartbeatConfiguration_t erdHeartbeatConfiguration = {
   .erdPairs = erdHeartbeatPairs,
   .erdPairCount = NUM_ELEMENTS(erdHeartbeatPairs),
   .heartbeatRateInTicks = HeartbeatPeriodMsec,
   .throttledOnChangeRateInTicks = HeartbeatThrottleMsec,
   .enableHeartbeat = Erd_Invalid,
   .destinationAddress = PcToolAddress,
   .transmitOnChange = true,
   .erdApiRevision = ErdHeartbeatErdApi_Revision2
};

#define EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP(Name, Number, DataType, Stream, StreamRemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)                                                         \
   ({ Number COMMA StreamRemoteErd COMMA CONCAT(INCLUDE_STREAM_EVENT_, Stream)(ErdStreamDataType_Event) CONCAT(INCLUDE_STREAM_LEVEL_, Stream)(ErdStreamDataType_State) } COMMA)

static const ErdStreamLocalToRemoteErdMap_t erdStreamLocalToRemoteErdMap[] = {
   ERD_TABLE(EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP)
};

static const ErdStreamSenderConfiguration_t erdStreamSenderConfiguration = {
   .erdStreamErd = Erd_ErdStream,
   .sequenceNumberErd = Erd_ErdStreamSequenceNumber,
   .requestedStateErdFromReceiver = Erd_ErdStreamRequestedState,
   .maxNumberOfEntriesInBuffer = StreamEntryCount,
   .numberOfBytesInLargestErdInBuffer = sizeof(StreamedErd_t)
};

static const ConstArrayMap_LinearSearchConfiguration_t erdStreamStreamedErdMapConfiguration = {
   .array = erdStreamLocalToRemoteErdMap,
   .elementCount = ELEMENT_COUNT(erdStreamLocalToRemoteErdMap),
   .elementSize = ELEMENT_SIZE(erdStreamLocalToRemoteErdMap),
   .keySize = MEMBER_SIZE(ErdStreamLocalToRemoteErdMap_t, localErdToMonitor),
   .keyOffset = OFFSET_OF(ErdStreamLocalToRemoteErdMap_t, localErdToMonitor)
};

void McCommunicationPlugin_Init(
   I_DataSource_t *dataSource,
   I_DataSource_t *externalDataSource,
   I_Gea2PacketEndpoint_t *gea2PacketEndpoint)
{
   TimerModule_t *timerModule;
   DataSource_Read(dataSource, Erd_TimerModule, &timerModule);

   Gea2MessageEndpoint_Gea2PacketEndpointAdapter_Init(
      &gea2MessageEndpoint,
      gea2PacketEndpoint);

   ErdHeartbeat_Init(
      &erdHeartbeat,
      externalDataSource,
      timerModule,
      &gea2MessageEndpoint.interface,
      &erdHeartbeatConfiguration);

   ConstArrayMap_LinearSearch_Init(
      &erdStreamStreamedErdMap,
      &erdStreamStreamedErdMapConfiguration);

   ErdStreamSender_Init(
      &erdStreamSender,
      externalDataSource,
      &erdStreamSenderConfiguration,
      erdStreamBuffer,
      sizeof(erdStreamBuffer),
      &erdStreamStreamedErdMap.interface);
}
