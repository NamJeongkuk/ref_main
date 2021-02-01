/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GeaStack.h"
#include "TinyGea2Interface_FullDuplex.h"
#include "TinyCommonCommands.h"
#include "TinyErdGea2PublicApiRevision2.h"
#include "GeaStackXmacroUtils.h"
#include "MicroSystemErds.h"
#include "Gea2Addresses.h"
#include "MemoryMap.h"
#include "Reset.h"
#include "ReadyToEnterBootLoader.h"
#include "utils.h"

enum
{
   SendBufferSize = 255,
   ReceiveBufferSize = 255
};

#define EXPAND_AS_PUBLIC_ERD_COUNT_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_PUBLIC_, Number)                                                          \
   (uint8_t Name;)

typedef struct
{
   ERD_TABLE(EXPAND_AS_PUBLIC_ERD_COUNT_STRUCT_MEMBER)
} PublicErdCount_t;

static struct
{
   TinyGea2Interface_FullDuplex_t gea2Interface;
   TinyCommonCommands_t commonCommands;
   TinyErdGea2PublicApiRevision2_t erdApi;
   uint8_t subscriptionBuffer[sizeof(PublicErdCount_t) / 4 + 1];
   uint8_t sendBuffer[SendBufferSize];
   uint8_t receiveBuffer[ReceiveBufferSize];
} instance;

static const TinyCommonCommandsConfiguration_t commonCommandsConfiguration = {
   .bootLoaderHeader = BootLoaderImageHeader,
   .applicationHeader = ApplicationImageHeader,
   .parametricHeader = NULL,
   .reset = Reset,
   .readyToEnterBootLoader = ReadyToEnterBootLoader,
};

#define EXPAND_AS_PUBLIC_ERDS(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_PUBLIC_, Number)                                       \
   (Name COMMA)

static const Erd_t publicErds[] = {
   ERD_TABLE(EXPAND_AS_PUBLIC_ERDS)
};

void GeaStack_Init(
   TinyTimerModule_t *timerModule,
   I_TinyDataSource_t *dataSource,
   I_TinyUart_t *uart,
   uint8_t geaAddress)
{
   TinyGea2Interface_FullDuplex_Init(
      &instance.gea2Interface,
      uart,
      geaAddress,
      instance.sendBuffer,
      sizeof(instance.sendBuffer),
      instance.receiveBuffer,
      sizeof(instance.receiveBuffer),
      false);

   TinyCommonCommands_Init(
      &instance.commonCommands,
      &instance.gea2Interface.interface,
      timerModule,
      &commonCommandsConfiguration);

   TinyErdGea2PublicApiRevision2_Init(
      &instance.erdApi,
      dataSource,
      &instance.gea2Interface.interface,
      timerModule,
      publicErds,
      NUM_ELEMENTS(publicErds),
      instance.subscriptionBuffer);
}

I_TinyGea2Interface_t *GeaStack_GetGea2Interface(void)
{
   return &instance.gea2Interface.interface;
}

void GeaStack_Run(void)
{
   TinyGea2Interface_FullDuplex_Run(&instance.gea2Interface);
}
