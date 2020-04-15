/*!
 * @file
 * @brief Foreground full duplex GEA communication stack
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GEASTACK_H
#define GEASTACK_H

#include "I_TinyUart.h"
#include "TinyTimer.h"
#include "TinyGea2Interface_FullDuplex.h"
#include "TinyCommonCommands.h"
#include "TinyErdGea2PublicApiRevision2.h"
#include "GeaStackXmacroUtils.h"
#include "MicroSystemErds.h"

enum
{
   SendBufferSize = 80,
   ReceiveBufferSize = 80
};

#define EXPAND_AS_PUBLIC_ERD_COUNT_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   CONCAT(INCLUDE_PUBLIC_, Number)(uint8_t Name;)

typedef struct
{
   ERD_TABLE(EXPAND_AS_PUBLIC_ERD_COUNT_STRUCT_MEMBER)
} GeaStackPublicErdCount_t;

typedef struct
{
   struct
   {
      TinyGea2Interface_FullDuplex_t gea2Interface;
      TinyCommonCommands_t commonCommands;
      TinyErdGea2PublicApiRevision2_t erdApi;
      uint8_t subscriptionBuffer[sizeof(GeaStackPublicErdCount_t) / 4 + 1];
      uint8_t sendBuffer[SendBufferSize];
      uint8_t receiveBuffer[ReceiveBufferSize];
   } _private;
} GeaStack_t;

/*!
 * @param instance
 * @param timerModule
 * @param dataSource
 * @param uart
 * @param geaAddress
 */
void GeaStack_Init(
   GeaStack_t *instance,
   TinyTimerModule_t *timerModule,
   I_TinyDataSource_t *dataSource,
   I_TinyUart_t *uart,
   uint8_t geaAddress);

/*!
 * @param instance
 * @return
 */
I_TinyGea2Interface_t *GeaStack_GetGea2Interface(GeaStack_t *instance);

/*!
 * @param instance
 */
void GeaStack_Run(GeaStack_t *instance);

#endif
