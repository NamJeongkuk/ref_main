/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYSYSTEMERDS_H
#define TINYSYSTEMERDS_H

#include "Version.h"
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
   ENTRY(Erd_ApplicationVersion,       0x003A, Version_t               ) \
   ENTRY(Erd_ErdStream,                0xF000, ErdStreamErd_t          ) \
   ENTRY(Erd_ErdStreamRequestedState,  0xF001, ErdStreamStateRequest_t ) \
   ENTRY(Erd_ButtonState,              0xF002, bool                    ) \
   ENTRY(Erd_LedState,                 0xF003, bool                    )

#define EXPAND_AS_ENUM(Name, Number, DataType) \
   Name = Number COMMA

enum
{
   ERD_TABLE(EXPAND_AS_ENUM)
};

// clang-format on

#endif
