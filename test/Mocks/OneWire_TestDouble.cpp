/*!
 * @file
 * @brief Implementation of the One Wire device test double
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "OneWire_TestDouble.h"
#include "utils.h"

enum
{
   DefaultMaximumTransferSize = 8
};

static uint8_t Read(I_OneWire_t *context, uint8_t address, uint8_t numBytes, uint8_t *readBuffer)
{
   REINTERPRET(instance, context, OneWire_TestDouble_t *);
   if(instance->_private.fail)
   {
      instance->_private.fail = false;
      numBytes = 0;
   }
   else
   {
      numBytes = MIN(instance->_private.transfer, numBytes);
      memcpy(readBuffer, &instance->_private.data[address], numBytes);
   }
   return numBytes;
}

static uint8_t Write(I_OneWire_t *context, uint8_t address, uint8_t numBytes, const uint8_t *writeBuffer)
{
   REINTERPRET(instance, context, OneWire_TestDouble_t *);
   if(instance->_private.fail)
   {
      instance->_private.fail = false;
      numBytes = 0;
   }
   else
   {
      numBytes = MIN(instance->_private.transfer, numBytes);
      memcpy(&instance->_private.data[address], writeBuffer, numBytes);
   }
   return numBytes;
}

static bool DeviceIsPresent(I_OneWire_t *context)
{
   REINTERPRET(instance, context, OneWire_TestDouble_t *);
   return instance->_private.present;
}

static const I_OneWire_Api_t api = {
   Read,
   Write,
   DeviceIsPresent
};

void OneWire_TestDouble_Init(OneWire_TestDouble_t *instance)
{
   memset(instance->_private.data, 0, sizeof(instance->_private.data));
   instance->interface.api = &api;
   instance->_private.transfer = DefaultMaximumTransferSize;
   instance->_private.present = true;
   instance->_private.fail = false;
}

void OneWire_TestDouble_SetDevicePresent(OneWire_TestDouble_t *instance, bool present)
{
   instance->_private.present = present;
}

void OneWire_TestDouble_SetMaxTransferSize(OneWire_TestDouble_t *instance, uint8_t transfer)
{
   instance->_private.transfer = transfer;
}

uint8_t *OneWire_TestDouble_GetDataLocation(OneWire_TestDouble_t *instance)
{
   return instance->_private.data;
}

void OneWire_TestDouble_GenerateFailure(OneWire_TestDouble_t *instance)
{
   instance->_private.fail = true;
}
