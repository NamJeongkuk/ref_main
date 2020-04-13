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
#include "utils.h"

static const TinyCommonCommandsConfiguration_t commonCommandsConfiguration = {
   .bootLoaderHeader = BootLoaderHeaderAddress,
   .applicationHeader = ApplicationHeaderAddress,
   .parametricHeader = NULL,
   .reset = Reset,
   .readyToEnterBootLoader = ReadyToEnterBootLoader,
};

void GeaStack_Init(
   GeaStack_t *instance,
   TinyTimerModule_t *timerModule,
   I_TinyUart_t *uart,
   uint8_t geaAddress)
{
   TinyGea2Interface_FullDuplex_Init(
      &instance->_private.gea2Interface,
      uart,
      geaAddress,
      instance->_private.sendBuffer,
      sizeof(instance->_private.sendBuffer),
      instance->_private.receiveBuffer,
      sizeof(instance->_private.receiveBuffer),
      false);

   TinyCommonCommands_Init(
      &instance->_private.commonCommands,
      &instance->_private.gea2Interface.interface,
      timerModule,
      &commonCommandsConfiguration);
}

I_TinyGea2Interface_t *GeaStack_GetGea2Interface(GeaStack_t *instance)
{
   return &instance->_private.gea2Interface.interface;
}

void GeaStack_Run(GeaStack_t *instance)
{
   TinyGea2Interface_FullDuplex_Run(&instance->_private.gea2Interface);
}
