/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "GeaStack.h"
#include "Gea2Addresses.h"
#include "utils.h"

void GeaStack_Init(
   GeaStack_t *instance,
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
}

I_TinyGea2Interface_t *GeaStack_GetGea2Interface(GeaStack_t *instance)
{
   return &instance->_private.gea2Interface.interface;
}

void GeaStack_Run(GeaStack_t *instance)
{
   TinyGea2Interface_FullDuplex_Run(&instance->_private.gea2Interface);
}
