/*!
 * @file
 * @brief Test Double implementation of an EEPROM
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

extern "C"
{
}

#include "CppUTestExt/MockSupport.h"
#include "Eeprom_TestDouble.h"
#include "utils.h"

static void Read(I_Eeprom_t *instance, uint32_t offset, uint32_t numBytes, void *readBuffer)
{
   mock().actualCall("Read").onObject(instance).withParameter("offset", offset).withParameter("numBytes", numBytes).withOutputParameter("readBuffer", readBuffer);
}

static void Write(I_Eeprom_t *instance, uint32_t offset, uint32_t numBytes, const void *writeBuffer)
{
   mock().actualCall("Write").onObject(instance).withParameter("offset", offset).withParameter("numBytes", numBytes).withParameter("writeBuffer", (const void *)writeBuffer);
}

static void Erase(I_Eeprom_t *instance)
{
   mock().actualCall("Erase").onObject(instance);
}

static uint16_t GetByteAlignment(I_Eeprom_t *instance)
{
   return mock().actualCall("GetByteAlignment").onObject(instance).returnValue().getIntValue();
}

static uint32_t GetSize(I_Eeprom_t *instance)
{
   return mock().actualCall("GetSize").onObject(instance).returnValue().getIntValue();
}

static I_Event_t *GetOnWrittenEvent(I_Eeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Eeprom_TestDouble_t *);
   return &instance->_private.OnWrittenEvent.interface;
}

static I_Event_t *GetOnReadEvent(I_Eeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Eeprom_TestDouble_t *);
   return &instance->_private.OnReadEvent.interface;
}

static I_Event_t *GetOnErasedEvent(I_Eeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Eeprom_TestDouble_t *);
   return &instance->_private.OnErasedEvent.interface;
}

static const I_Eeprom_Api_t api = { Read, Write, Erase, GetByteAlignment, GetSize, GetOnWrittenEvent, GetOnReadEvent, GetOnErasedEvent };

void Eeprom_TestDouble_Init(Eeprom_TestDouble_t *instance)
{
   instance->interface.api = &api;

   Event_Synchronous_Init(&instance->_private.OnWrittenEvent);
   Event_Synchronous_Init(&instance->_private.OnReadEvent);
   Event_Synchronous_Init(&instance->_private.OnErasedEvent);
}

void Eeprom_TestDouble_TriggerReceiveWritenEvent(Eeprom_TestDouble_t *instance)
{
   Event_Synchronous_Publish(&instance->_private.OnWrittenEvent, NULL);
}

void Eeprom_TestDouble_TriggerReceiveReadEvent(Eeprom_TestDouble_t *instance)
{
   Event_Synchronous_Publish(&instance->_private.OnReadEvent, NULL);
}

void Eeprom_TestDouble_TriggerReceiveErasedEvent(Eeprom_TestDouble_t *instance)
{
   Event_Synchronous_Publish(&instance->_private.OnErasedEvent, NULL);
}
