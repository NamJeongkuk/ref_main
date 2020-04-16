/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GeaStack.h"
#include "Gea2Addresses.h"
#include "MemoryMap.h"
#include "Reset.h"
#include "ReadyToEnterBootLoader.h"
#include "TinyCommonCommands.h"
#include "TinyGea2Interface_FullDuplex.h"
#include "utils.h"

enum
{
   SendBufferSize = 100,
   ReceiveBufferSize = 100
};

static TinyGea2Interface_FullDuplex_t gea2Interface;
static TinyCommonCommands_t commonCommands;
static uint8_t sendBuffer[SendBufferSize];
static uint8_t receiveBuffer[ReceiveBufferSize];

static const TinyCommonCommandsConfiguration_t commonCommandsConfiguration = {
   .bootLoaderHeader = BootLoaderHeaderAddress,
   .applicationHeader = ApplicationHeaderAddress,
   .parametricHeader = NULL,
   .reset = Reset,
   .readyToEnterBootLoader = ReadyToEnterBootLoader,
};

void GeaStack_Init(
   TinyTimerModule_t *timerModule,
   I_TinyDataSource_t *dataSource,
   I_TinyUart_t *uart,
   uint8_t geaAddress)
{
   TinyGea2Interface_FullDuplex_Init(
      &gea2Interface,
      uart,
      geaAddress,
      sendBuffer,
      sizeof(sendBuffer),
      receiveBuffer,
      sizeof(receiveBuffer),
      false);

   TinyCommonCommands_Init(
      &commonCommands,
      &gea2Interface.interface,
      timerModule,
      &commonCommandsConfiguration);
}

I_TinyGea2Interface_t *GeaStack_GetGea2Interface(void)
{
   return &gea2Interface.interface;
}

void GeaStack_Run(void)
{
   TinyGea2Interface_FullDuplex_Run(&gea2Interface);
}
