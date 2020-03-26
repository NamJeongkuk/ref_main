/*!
 * @file
 * @brief Foreground full duplex GEA communication stack
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GEASTACK_H
#define GEASTACK_H

#include "I_TinyUart.h"
#include "I_TinyDataSource.h"
#include "TinyGea2Interface_FullDuplex.h"

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
      uint8_t sendBuffer[SendBufferSize];
      uint8_t receiveBuffer[ReceiveBufferSize];
   } _private;
} GeaStack_t;

/*!
 * @param instance
 * @param uart
 * @param geaAddress
 */
void GeaStack_Init(
   GeaStack_t *instance,
   I_TinyUart_t *uart,
   uint8_t geaAddress);

/*!
 * @param instance
 * @return I_Gea2Interface_t
 */
I_TinyGea2Interface_t *GeaStack_GetGea2Interface(GeaStack_t *instance);

/*!
 * @param instance
 */
void GeaStack_Run(GeaStack_t *instance);

#endif
