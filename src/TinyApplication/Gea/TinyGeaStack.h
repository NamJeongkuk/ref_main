/*!
 * @file
 * @brief Foreground full duplex GEA communication stack
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYGEASTACK_H
#define TINYGEASTACK_H

#include "I_TinyUart.h"
#include "I_TinyDataSource.h"
#include "TinyGea2Interface_FullDuplex.h"
#include "TinyEventSubscription.h"
#include "TinyErdGea2OpenLoopWriteApiRevision2.h"

enum
{
   SendBufferSize = 80,
   ReceiveBufferSize = 80
};

typedef struct
{
   struct
   {
      TinyGea2Interface_FullDuplex_t gea2Interface;
      TinyErdGea2OpenLoopWriteApiRevision2_t erdGea2OpenLoopWriteApi;
      TinyEventSubscription_t geaMessageSubscription;
      uint8_t sendBuffer[SendBufferSize];
      uint8_t receiveBuffer[ReceiveBufferSize];
      uint8_t geaAddress;
   } _private;
} TinyGeaStack_t;

/*!
 * Initialize background full duplex GEA stack
 * @param instance
 * @param uart
 * @param dataSource
 * @param geaAddress
 */
void TinyGeaStack_Init(
   TinyGeaStack_t *instance,
   I_TinyUart_t *uart,
   I_TinyDataSource_t *dataSource,
   uint8_t geaAddress);

/*!
 * Get Gea2Interface
 * @param instance
 * @return I_Gea2Interface_t
 */
I_TinyGea2Interface_t *TinyGeaStack_GetGea2Interface(TinyGeaStack_t *instance);

/*!
 * Run Gea2Interface_FullDuplex & Uart_IsrSafe
 * @param instance
 */
void TinyGeaStack_Run(TinyGeaStack_t *instance);

#endif
