/*!
 * @file
 * @brief Eeprom AT21CS11 driver
 *
 * Memory map of Eeprom is:
 *
 * 0x00 to 0x1F - Zone 0 eeprom (used by dataSource)
 * 0x20 to 0x3F - Zone 1 eeprom (used by dataSource)
 * 0x40 to 0x5F - Zone 2 eeprom (used by dataSource)
 * 0x60 to 0x7F - Zone 3 eeprom
 * 0x80 to 0x87 - 64 bit unique serial number
 * 0x88 to 0x8F - Reserved read-only
 * 0x90 to 0x9F - User programmable data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include <string.h>
#include "Eeprom_AT21CS11.h"
#include "Constants_Binary.h"
#include "utils.h"

enum
{
   AT21CS11_SizeInBytes = 128,
   AT21CS11_ByteAlignment = 1,
   WriteBlockDelayInMs = 5,
   AT21CS11_UniqueIdSize = 8,
   AT21CS11_UniqueIdAddress = 0x80
};

enum
{
   Signal_WaitOver = FSM_USER_SIGNAL_START,
   Signal_Read,
   Signal_Write,
   Signal_Erase
};

static void TimerExpired(void *context)
{
   Eeprom_AT21CS11_t *instance = context;
   Fsm_SendSignal(&instance->_private.fsm, Signal_WaitOver, NULL);
}

static void Wait(Eeprom_AT21CS11_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule_1ms,
      &instance->_private.timer,
      WriteBlockDelayInMs,
      TimerExpired,
      instance);
}

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_WriteBlock(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_ReadBlock(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_Erase(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static Eeprom_AT21CS11_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(Eeprom_AT21CS11_t, _private.fsm, fsm);
}

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   Eeprom_AT21CS11_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);
   switch(signal)
   {
      case Signal_Read:
         if(instance->_private.devicePresent)
         {
            Fsm_Transition(fsm, State_ReadBlock);
         }
         else
         {
            Event_Synchronous_Publish(&instance->_private.OnReadEvent, NULL);
         }
         break;
      case Signal_Write:
         if(instance->_private.devicePresent)
         {
            Fsm_Transition(fsm, State_WriteBlock);
         }
         else
         {
            Event_Synchronous_Publish(&instance->_private.OnWrittenEvent, NULL);
         }
         break;
      case Signal_Erase:
         if(instance->_private.devicePresent)
         {
            Fsm_Transition(fsm, State_Erase);
         }
         else
         {
            Event_Synchronous_Publish(&instance->_private.OnEraseEvent, NULL);
         }
         break;
   }
}

static void WriteBlock(Eeprom_AT21CS11_t *instance)
{
   uint8_t bytesWritten = OneWire_Write(instance->_private.at21cs11, instance->_private.offset, instance->_private.dataSize, instance->_private.writeData);

   if(bytesWritten > 0)
   {
      instance->_private.offset += bytesWritten;
      instance->_private.writeData += bytesWritten;
      instance->_private.dataSize -= bytesWritten;
   }
   else
   {
      instance->_private.dataSize = 0;
   }
}

static void State_WriteBlock(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   Eeprom_AT21CS11_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case FSM_ENTRY:
         WriteBlock(instance);
         Wait(instance);
         break;

      case Signal_WaitOver:
         if(instance->_private.dataSize > 0)
         {
            WriteBlock(instance);
            Wait(instance);
         }
         else
         {
            Fsm_Transition(fsm, State_Idle);
            Event_Synchronous_Publish(&instance->_private.OnWrittenEvent, NULL);
         }
         break;
   }
}

static void ReadBlock(Eeprom_AT21CS11_t *instance)
{
   uint8_t bytesRead = OneWire_Read(instance->_private.at21cs11, instance->_private.offset, instance->_private.dataSize, instance->_private.readData);

   if(bytesRead > 0)
   {
      instance->_private.offset += bytesRead;
      instance->_private.readData += bytesRead;
      instance->_private.dataSize -= bytesRead;
   }
   else
   {
      instance->_private.dataSize = 0;
   }
}

static void State_ReadBlock(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   Eeprom_AT21CS11_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case FSM_ENTRY:
         while(instance->_private.dataSize > 0)
         {
            ReadBlock(instance);
         }
         Fsm_Transition(fsm, State_Idle);
         Event_Synchronous_Publish(&instance->_private.OnReadEvent, NULL);
         break;
   }
}

static void EraseBlock(Eeprom_AT21CS11_t *instance)
{
   uint8_t bytesWritten;
   const uint8_t eraseData[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

   instance->_private.writeData = eraseData;
   bytesWritten =
      OneWire_Write(
         instance->_private.at21cs11,
         instance->_private.offset,
         instance->_private.dataSize,
         instance->_private.writeData);
   if(bytesWritten > 0)
   {
      instance->_private.offset += bytesWritten;
      instance->_private.dataSize -= bytesWritten;
   }
   else
   {
      instance->_private.dataSize = 0;
   }
}

static void State_Erase(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   Eeprom_AT21CS11_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case FSM_ENTRY:
         EraseBlock(instance);
         Wait(instance);
         break;

      case Signal_WaitOver:
         if(instance->_private.dataSize > 0)
         {
            EraseBlock(instance);
            Wait(instance);
         }
         else
         {
            Fsm_Transition(fsm, State_Idle);
            Event_Synchronous_Publish(&instance->_private.OnEraseEvent, NULL);
         }
         break;
   }
}

static void Read(I_HardwareEeprom_t *context, uint16_t offset, uint16_t numBytes, void *readBuffer)
{
   REINTERPRET(instance, context, Eeprom_AT21CS11_t *);

   instance->_private.dataSize = (uint8_t)numBytes;
   instance->_private.offset = (uint8_t)offset;
   instance->_private.readData = (uint8_t *)readBuffer;
   memset(readBuffer, 0, numBytes);
   Fsm_SendSignal(&instance->_private.fsm, Signal_Read, NULL);
}

static void Write(I_HardwareEeprom_t *context, uint16_t offset, uint16_t numBytes, const void *writeBuffer)
{
   REINTERPRET(instance, context, Eeprom_AT21CS11_t *);

   instance->_private.dataSize = (uint8_t)numBytes;
   instance->_private.offset = (uint8_t)offset;
   instance->_private.writeData = (const uint8_t *)writeBuffer;
   Fsm_SendSignal(&instance->_private.fsm, Signal_Write, NULL);
}

static void Erase(I_HardwareEeprom_t *context)
{
   REINTERPRET(instance, context, Eeprom_AT21CS11_t *);
   instance->_private.offset = 0;
   instance->_private.dataSize = AT21CS11_SizeInBytes;
   Fsm_SendSignal(&instance->_private.fsm, Signal_Erase, NULL);
}

static uint16_t GetByteAlignment(I_HardwareEeprom_t *context)
{
   IGNORE(context);
   return AT21CS11_ByteAlignment;
}

static uint16_t GetSize(I_HardwareEeprom_t *context)
{
   IGNORE(context);
   return AT21CS11_SizeInBytes;
}

static I_Event_t *GetOnWrittenEvent(I_HardwareEeprom_t *context)
{
   REINTERPRET(instance, context, Eeprom_AT21CS11_t *);
   return &instance->_private.OnWrittenEvent.interface;
}

static I_Event_t *GetOnReadEvent(I_HardwareEeprom_t *context)
{
   REINTERPRET(instance, context, Eeprom_AT21CS11_t *);
   return &instance->_private.OnReadEvent.interface;
}

static I_Event_t *GetOnErasedEvent(I_HardwareEeprom_t *context)
{
   REINTERPRET(instance, context, Eeprom_AT21CS11_t *);
   return &instance->_private.OnEraseEvent.interface;
}

static const I_HardwareEeprom_Api_t api = {
   Read,
   Write,
   Erase,
   GetByteAlignment,
   GetSize,
   GetOnWrittenEvent,
   GetOnReadEvent,
   GetOnErasedEvent
};

void Eeprom_AT21CS11_Init(Eeprom_AT21CS11_t *instance, TimerModule_t *timerModule, I_OneWire_t *oneWire)
{
   instance->_private.timerModule_1ms = timerModule;
   instance->_private.at21cs11 = oneWire;

   instance->_private.devicePresent = OneWire_DeviceIsPresent(instance->_private.at21cs11);
   instance->interface.api = &api;

   Event_Synchronous_Init(&instance->_private.OnReadEvent);
   Event_Synchronous_Init(&instance->_private.OnWrittenEvent);
   Event_Synchronous_Init(&instance->_private.OnEraseEvent);

   Fsm_Init(&instance->_private.fsm, State_Idle);
}

uint8_t Eeprom_AT21CS11_GetUniqueId(Eeprom_AT21CS11_t *instance)
{
   uint8_t uniqueId;
   OneWire_Read(instance->_private.at21cs11, AT21CS11_UniqueIdAddress, AT21CS11_UniqueIdSize, &uniqueId);
   return uniqueId;
}

I_Eeprom_t *Eeprom_AT21CS11_GetInterface(Eeprom_AT21CS11_t *instance)
{
   return (I_Eeprom_t *)(&instance->interface);
}
