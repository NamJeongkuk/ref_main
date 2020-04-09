/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NANOSYSTEMERDS_H
#define NANOSYSTEMERDS_H

#include "ErdStream.h"
#include "XMacroUtils.h"
#include "utils.h"
#include <stdint.h>

enum
{
   StreamedItemCount = 5
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

// Name, Number, DataType, Stream, RemoteErd
#define ERD_TABLE(ENTRY) \
   ENTRY(Erd_ErdStream,                0xF000, ErdStreamErd_t,          StreamNone,    NotMapped) \
   ENTRY(Erd_ErdStreamRequestedState,  0xF001, ErdStreamStateRequest_t, StreamNone,    NotMapped) \
   ENTRY(Erd_ButtonState,              0xF002, bool,                    StreamEvent,   0xA000) \
   ENTRY(Erd_LedState,                 0xF003, bool,                    StreamEvent,   0xA001) \

#define EXPAND_AS_STREAMED_ITEM_UNION(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)(DataType item##Name;)

typedef union
{
   ERD_TABLE(EXPAND_AS_STREAMED_ITEM_UNION)
} StreamedErd_t;

typedef struct
{
   uint8_t erd[sizeof(Erd_t)];
   uint8_t item[sizeof(StreamedErd_t)];
} StreamedItem_t;

#define EXPAND_AS_STREAM_COUNT_ENUM(Name, Number, DataType, Stream, RemoteErd) \
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

#define EXPAND_AS_ENUM(Name, Number, DataType, Stream, RemoteErd) \
   Name = Number COMMA

#define EXPAND_AS_LARGEST_STREAMED_ERD_SIZE_CHECK(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)(((sizeof(DataType) <= LARGEST_STREAMED_ERD_SIZE) ? true : false) &&)

enum
{
   ERD_TABLE(EXPAND_AS_ENUM)
};

// clang-format on

#endif
