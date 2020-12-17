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
#include "TinyWiFiService.h"
#include "Signal.h"
#include "utils.h"

enum
{
   StreamEntryCount = 5
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
   ENTRY(Erd_ModelNumber,                       0x0001, ModelNumber_t,              StreamNone,    NotMapped) \
   ENTRY(Erd_SerialNumber,                      0x0002, SerialNumber_t,             StreamNone,    NotMapped) \
   ENTRY(Erd_ApplianceType,                     0x0008, ApplianceType_t,            StreamNone,    NotMapped) \
   ENTRY(Erd_ReadyToEnterBootLoader,            0x0030, bool,                       StreamNone,    NotMapped) \
   ENTRY(Erd_Reset,                             0x0032, uint8_t,                    StreamNone,    NotMapped) \
   ENTRY(Erd_Personality,                       0x0035, AppliancePersonality_t,     StreamNone,    NotMapped) \
   ENTRY(Erd_SupportedImageTypes,               0x0038, uint8_t,                    StreamNone,    NotMapped) \
   ENTRY(Erd_BootLoaderVersion,                 0x0039, Version_t,                  StreamNone,    NotMapped) \
   ENTRY(Erd_ApplicationVersion,                0x003A, Version_t,                  StreamNone,    NotMapped) \
   ENTRY(Erd_ParametricVersion,                 0x003B, Version_t,                  StreamNone,    NotMapped) \
   ENTRY(Erd_AuxiliaryVersion,                  0x003C, Version_t,                  StreamNone,    NotMapped) \
   ENTRY(Erd_ApplianceApiManifest,              0x0092, ApplianceApiManifest_t,     StreamNone,    NotMapped) \
   ENTRY(Erd_TimerModule,                       0xF000, TinyTimerModule_t *,        StreamNone,    NotMapped) \
   ENTRY(Erd_WiFiServiceRequest,                0xF001, TinyWiFiServiceRequest_t,   StreamNone,    NotMapped) \
   ENTRY(Erd_WiFiServiceState,                  0xF002, TinyWiFiServiceState_t,     StreamNone,    NotMapped) \
   ENTRY(Erd_WiFiServiceConnectionEstablished,  0xF003, Signal_t,                   StreamNone,    NotMapped) \

#define SWAPPED_FIELDS \
   SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, revision), \
   SWAPPED_FIELD(Erd_ApplianceApiManifest, ApplianceApiManifest_t, features), \

#define EXPAND_AS_STREAMED_ITEM_UNION(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_STREAM_, Stream)(DataType item##Name;)

typedef union
{
   uint8_t dummy;
   ERD_TABLE(EXPAND_AS_STREAMED_ITEM_UNION)
} StreamedErd_t;

typedef struct
{
   uint8_t erd[sizeof(Erd_t)];
   uint8_t data[sizeof(StreamedErd_t)];
} StreamEntry_t;

typedef struct
{
   ErdStreamHeader_t header;
   StreamEntry_t items[StreamEntryCount];
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
