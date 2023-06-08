/*!
 * @file
 * @brief One wire device test double header file
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ONEWIRE_TESTDOUBLE_H
#define ONEWIRE_TESTDOUBLE_H

#include "I_OneWire.h"

typedef struct
{
   I_OneWire_t interface;

   struct
   {
      bool present;
      bool fail;
      uint8_t transfer;
      uint8_t data[256];
   } _private;
} OneWire_TestDouble_t;

void OneWire_TestDouble_Init(OneWire_TestDouble_t *instance);

void OneWire_TestDouble_SetDevicePresent(OneWire_TestDouble_t *instance, bool present);

void OneWire_TestDouble_SetMaxTransferSize(OneWire_TestDouble_t *instance, uint8_t transfer);

uint8_t *OneWire_TestDouble_GetDataLocation(OneWire_TestDouble_t *instance);

void OneWire_TestDouble_GenerateFailure(OneWire_TestDouble_t *instance);

#endif
