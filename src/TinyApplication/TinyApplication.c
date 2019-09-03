/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "TinyApplication.h"
#include "TinySystemErds.h"

#define EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)                                                   \
   ({ Number COMMA RemoteErd COMMA CONCAT(INCLUDE_STREAM_EVENT_, Stream)(ErdStreamDataType_Event) CONCAT(INCLUDE_STREAM_LEVEL_, Stream)(ErdStreamDataType_Level) } COMMA)

static const SwitchedLedPluginConfiguration_t ledPluginConfiguration =
   {
      .ledStateErd = Erd_LedState,
      .buttonStateErd = Erd_ButtonState
   };

static const ErdStreamLocalToRemoteErdMap_t streamLocalToRemoteErdMap[] =
   {
      ERD_TABLE(EXPAND_AS_LOCAL_TO_REMOTE_ERD_MAP)
   };

static const TinyErdStreamSenderConfiguration_t erdStreamSenderConfiguration =
   {
      .erdStreamErd = Erd_ErdStream,
      .requestedStateErdFromReceiver = Erd_ErdStreamRequestedState,
      .streamEntryCount = NumberOfStreamedErds,
      .sizeOfLargestStreamedErd = sizeof(StreamedErd_t),
      .mappings = streamLocalToRemoteErdMap,
      .mappingCount = NUM_ELEMENTS(streamLocalToRemoteErdMap)
   };

void TinyApplication_Init(TinyApplication_t *instance, I_TinyDataSource_t *dataSource)
{
   SwitchedLedPlugin_Init(&instance->_private.switchedLedPlugin, dataSource, &ledPluginConfiguration);
   TinyErdStreamSender_Init(&instance->_private.erdStreamSender, dataSource, &erdStreamSenderConfiguration);
}
