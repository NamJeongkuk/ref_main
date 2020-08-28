/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MICROSYSTEMERDS_H
#define MICROSYSTEMERDS_H

#include <stdint.h>
#include "ErdStream.h"
#include "XMacroUtils.h"
#include "ModelNumber.h"
#include "SerialNumber.h"
#include "ApplianceType.h"
#include "AppliancePersonality.h"
#include "ApplianceApi.h"
#include "Version.h"
#include "TinyTimer.h"
#include "ReadyToEnterBootLoaderState.h"
#include "utils.h"

enum
{
   StreamedItemCount = 10
};

// clang-format off

#define INCLUDE_STREAM_StreamNone(_x)
#define INCLUDE_STREAM_StreamEvent(_x) _x
#define INCLUDE_STREAM_StreamLevel(_x) _x

#define INCLUDE_STREAM_EVENT_StreamNone(_x)
#define INCLUDE_STREAM_EVENT_StreamEvent(_x) _x
#define INCLUDE_STREAM_EVENT_StreamLevel(_x)

#define INCLUDE_STREAM_LEVEL_StreamNone(_x)
#define INCLUDE_STREAM_LEVEL_StreamEvent(_x)
#define INCLUDE_STREAM_LEVEL_StreamLevel(_x) _x

// Name, Number, DataType, Stream, StreamRemoteErd
#define ERD_TABLE(ENTRY) \
   ENTRY(Erd_ReadyToEnterBootLoader,   0x0030, ReadyToEnterBootLoaderState_t,   StreamNone,    NotMapped) \
   ENTRY(Erd_Reset,                    0x0032, uint8_t,                         StreamNone,    NotMapped) \
   ENTRY(Erd_SupportedImageTypes,      0x0038, uint8_t,                         StreamNone,    NotMapped) \
   ENTRY(Erd_BootLoaderVersion,        0x0039, Version_t,                       StreamNone,    NotMapped) \
   ENTRY(Erd_ApplicationVersion,       0x003A, Version_t,                       StreamNone,    NotMapped) \
   ENTRY(Erd_ParametricVersion,        0x003B, Version_t,                       StreamNone,    NotMapped) \
   ENTRY(Erd_AuxiliaryVersion,         0x003C, Version_t,                       StreamNone,    NotMapped) \
   ENTRY(Erd_ApplianceApiManifest,     0x0092, ApplianceApiManifest_t,          StreamNone,    NotMapped) \
   ENTRY(Erd_TimerModule,              0xF000, TinyTimerModule_t *,             StreamNone,    NotMapped) \
   ENTRY(Erd_HeartbeatTick,            0xF001, uint8_t,                         StreamNone,    NotMapped) \
   ENTRY(Erd_ErdStream,                0xF002, ErdStreamErd_t,                  StreamNone,    NotMapped) \
   ENTRY(Erd_ErdStreamRequestedState,  0xF003, ErdStreamStateRequest_t,         StreamNone,    NotMapped) \
   ENTRY(Erd_KeyState,                 0xF004, uint32_t,                        StreamEvent,   0xF123) \

#define SWAPPED_FIELDS \
   SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, revision), \
   SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, features), \
   SWAPPED_ERD(Erd_KeyState, sizeof(uint32_t)), \

#define EXPAND_AS_STREAMED_ITEM_UNION(Name, Number, DataType, Stream, StreamRemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)(DataType item##Name;)

typedef union
{
   uint8_t dummy;
   ERD_TABLE(EXPAND_AS_STREAMED_ITEM_UNION)
} StreamedErd_t;

typedef struct
{
   uint8_t erd[sizeof(Erd_t)];
   uint8_t item[sizeof(StreamedErd_t)];
} StreamedItem_t;

#define EXPAND_AS_STREAM_COUNT_ENUM(Name, Number, DataType, Stream, StreamRemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)(StreamCount##Name COMMA)

enum
{
   ERD_TABLE(EXPAND_AS_STREAM_COUNT_ENUM)
   NumberOfStreamedErds
};

typedef struct
{
   ErdStreamHeader_t header;
   StreamedItem_t items[StreamedItemCount];
} ErdStreamErd_t;

#define EXPAND_AS_ENUM(Name, Number, DataType, Stream, StreamRemoteErd) \
   Name = Number COMMA

#define EXPAND_AS_LARGEST_STREAMED_ERD_SIZE_CHECK(Name, Number, DataType, Stream, StreamRemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)(((sizeof(DataType) <= LARGEST_STREAMED_ERD_SIZE) ? true : false) &&)

enum
{
   ERD_TABLE(EXPAND_AS_ENUM)
};

// clang-format on

#endif
