/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "HardwareEeprom_DelayedPageOperationWrapper.h"
#include "utils.h"

#define Eeprom_I2c_Size_In_Bytes (16384)
#define ALIGN_UP_SHIFT(n, s) (((n >> s) + 1) << s)

enum
{
   EepromEraseSize = Eeprom_I2c_Size_In_Bytes,
   EepromSize = Eeprom_I2c_Size_In_Bytes,
   EepromPageShift = 6,
   EepromMaxRetryCount = 3,
};

enum
{
   Signal_ClientReadRequested = Fsm_UserSignalStart,
   Signal_ClientWriteRequested,
   Signal_ClientEraseRequested,
   Signal_EepromReadCompleted,
   Signal_EepromWriteCompleted,
   Signal_EepromEraseCompleted,
};

typedef HardwareEeprom_DelayedPageOperationWrapper_t Instance_t;

static void State_Idle(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Read(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Write(Fsm_t *fsm, FsmSignal_t signal, const void *data);
static void State_Erase(Fsm_t *fsm, FsmSignal_t signal, const void *data);

static uint16_t GetNextPageStartAddress(uint16_t offset)
{
   return ALIGN_UP_SHIFT(offset, EepromPageShift);
}

static uint16_t GetSlicedNumberOfBytes(uint16_t offset, uint16_t endOffset)
{
   return MIN(GetNextPageStartAddress(offset), endOffset) - offset;
}

static void ReadASlice(void *context)
{
   REINTERPRET(instance, context, Instance_t *);

   instance->_private.tries++;

   uint16_t sliceSizeInBytes = GetSlicedNumberOfBytes(
      instance->_private.operationInfo.offset,
      instance->_private.endOffset);

   HardwareEeprom_Read(
      instance->_private.eeprom,
      instance->_private.operationInfo.offset,
      sliceSizeInBytes,
      instance->_private.operationInfo.readBuffer);
}

static void WriteASlice(void *context)
{
   REINTERPRET(instance, context, Instance_t *);

   instance->_private.tries++;

   uint16_t sliceSizeInBytes = GetSlicedNumberOfBytes(
      instance->_private.operationInfo.offset,
      instance->_private.endOffset);

   HardwareEeprom_Write(
      instance->_private.eeprom,
      instance->_private.operationInfo.offset,
      sliceSizeInBytes,
      instance->_private.operationInfo.writeBuffer);
}

static void StartDelayTimer(Instance_t *instance, TimerTicks_t ticks, TimerCallback_t callback)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.delayTimer,
      ticks,
      callback,
      instance);
}

static void EventPublishCallback(Event_SafeSynchronous_t event, HardwareEepromMode_t mode, HardwareEepromErrorSource_t errorSource)
{
   HardwareEepromEventArgs_t callbackArgs = {
      .mode = mode,
      .source = errorSource
   };
   Event_SafeSynchronous_Publish(&event, &callbackArgs);
}

static void EventReadPublishCallback(void *context)
{
   REINTERPRET(instance, context, Instance_t *);
   EventPublishCallback(instance->_private.readCompleteEvent, HardwareEepromMode_Read, instance->_private.lastErrorSource);
}

static void StartReadPublishDelayTimer(Instance_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.delayTimer,
      EEPROM_READ_ACCESS_DELAY_MSEC,
      EventReadPublishCallback,
      instance);
}

static void EventWrittenPublishCallback(void *context)
{
   REINTERPRET(instance, context, Instance_t *);
   EventPublishCallback(instance->_private.writeCompleteEvent, HardwareEepromMode_Write, instance->_private.lastErrorSource);
}

static void StartWrittenPublishDelayTimer(Instance_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.delayTimer,
      EEPROM_WRITE_ACCESS_DELAY_MSEC,
      EventWrittenPublishCallback,
      instance);
}

static void EventErasePublishCallback(void *context)
{
   REINTERPRET(instance, context, Instance_t *);
   EventPublishCallback(instance->_private.eraseCompleteEvent, HardwareEepromMode_Erase, instance->_private.lastErrorSource);
}

static void StartErasePublishDelayTimer(Instance_t *instance)
{
   TimerModule_StartOneShot(
      instance->_private.timerModule,
      &instance->_private.delayTimer,
      EEPROM_WRITE_ACCESS_DELAY_MSEC,
      EventErasePublishCallback,
      instance);
}

static void PrepareForWrite(Instance_t *instance, const OperationInfo_t *operationInfo)
{
   instance->_private.operationInfo.offset = operationInfo->offset;
   instance->_private.operationInfo.numBytes = operationInfo->numBytes;
   instance->_private.operationInfo.writeBuffer = operationInfo->writeBuffer;
   instance->_private.endOffset = operationInfo->offset + operationInfo->numBytes;
}

static void PrepareForRead(Instance_t *instance, const OperationInfo_t *operationInfo)
{
   instance->_private.operationInfo.offset = operationInfo->offset;
   instance->_private.operationInfo.numBytes = operationInfo->numBytes;
   instance->_private.operationInfo.readBuffer = operationInfo->readBuffer;
   instance->_private.endOffset = operationInfo->offset + operationInfo->numBytes;
}

static void State_Erase(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   Instance_t *instance = CONTAINER_OF(Instance_t, _private.fsm, fsm);
   REINTERPRET(result, data, const HardwareEepromEventArgs_t *);

   switch(signal)
   {
      case Fsm_Entry:
         HardwareEeprom_Erase(instance->_private.eeprom);
         break;

      case Signal_EepromEraseCompleted:
         instance->_private.lastErrorSource = result->source;
         Fsm_Transition(fsm, State_Idle);
         break;

      case Fsm_Exit:
         StartErasePublishDelayTimer(instance);
         break;

      default:
         break;
   }
}

static void State_Idle(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   Instance_t *instance = CONTAINER_OF(Instance_t, _private.fsm, fsm);
   REINTERPRET(operationInfo, data, const OperationInfo_t *);

   switch(signal)
   {
      case Fsm_Entry:
         break;

      case Signal_ClientReadRequested:
         PrepareForRead(instance, operationInfo);
         Fsm_Transition(fsm, State_Read);
         break;

      case Signal_ClientWriteRequested:
         PrepareForWrite(instance, operationInfo);
         Fsm_Transition(fsm, State_Write);
         break;

      case Signal_ClientEraseRequested:
         Fsm_Transition(fsm, State_Erase);
         break;

      case Fsm_Exit:
         break;

      default:
         break;
   }
}

static void UpdateWriteStatus(Instance_t *instance)
{
   uint16_t sliceSizeInBytes = GetSlicedNumberOfBytes(
      instance->_private.operationInfo.offset,
      instance->_private.endOffset);

   instance->_private.operationInfo.offset += sliceSizeInBytes;
   instance->_private.operationInfo.writeBuffer += sliceSizeInBytes;
   instance->_private.tries = 0;
}

static void UpdateReadStatus(Instance_t *instance)
{
   uint16_t sliceSizeInBytes = GetSlicedNumberOfBytes(
      instance->_private.operationInfo.offset,
      instance->_private.endOffset);

   instance->_private.operationInfo.offset += sliceSizeInBytes;
   instance->_private.operationInfo.readBuffer += sliceSizeInBytes;
   instance->_private.tries = 0;
}

static bool OperationIncomplete(Instance_t *instance)
{
   return (instance->_private.operationInfo.offset < instance->_private.endOffset) &&
      (instance->_private.tries < EepromMaxRetryCount);
}

static void State_Write(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   Instance_t *instance = CONTAINER_OF(Instance_t, _private.fsm, fsm);
   REINTERPRET(result, data, const HardwareEepromEventArgs_t *);

   switch(signal)
   {
      case Fsm_Entry:
         instance->_private.tries = 0;
         WriteASlice(instance);
         break;

      case Signal_EepromWriteCompleted:
         instance->_private.lastErrorSource = result->source;
         if(result->source == HardwareEepromErrorSource_None)
         {
            UpdateWriteStatus(instance);
         }

         if(OperationIncomplete(instance))
         {
            StartDelayTimer(instance, EEPROM_WRITE_ACCESS_DELAY_MSEC, WriteASlice);
         }
         else
         {
            Fsm_Transition(fsm, State_Idle);
         }
         break;

      case Fsm_Exit:
         StartWrittenPublishDelayTimer(instance);
         break;

      default:
         break;
   }
}

static void State_Read(Fsm_t *fsm, FsmSignal_t signal, const void *data)
{
   Instance_t *instance = CONTAINER_OF(Instance_t, _private.fsm, fsm);
   REINTERPRET(result, data, const HardwareEepromEventArgs_t *);

   switch(signal)
   {
      case Fsm_Entry:
         instance->_private.tries = 0;
         ReadASlice(instance);
         break;

      case Signal_EepromReadCompleted:
         instance->_private.lastErrorSource = result->source;
         if(result->source == HardwareEepromErrorSource_None)
         {
            UpdateReadStatus(instance);
         }

         if(OperationIncomplete(instance))
         {
            StartDelayTimer(instance, EEPROM_READ_ACCESS_DELAY_MSEC, ReadASlice);
         }
         else
         {
            Fsm_Transition(fsm, State_Idle);
         }
         break;

      case Fsm_Exit:
         StartReadPublishDelayTimer(instance);
         break;

      default:
         break;
   }
}

static void Read(I_HardwareEeprom_t *_instance, uint16_t offset, uint16_t numBytes, void *readBuffer)
{
   REINTERPRET(instance, _instance, Instance_t *);

   OperationInfo_t readInfo;
   readInfo.offset = offset;
   readInfo.numBytes = numBytes;
   readInfo.readBuffer = readBuffer;

   Fsm_SendSignal(&instance->_private.fsm, Signal_ClientReadRequested, &readInfo);
}

static void Write(I_HardwareEeprom_t *_instance, uint16_t offset, uint16_t numBytes, const void *writeBuffer)
{
   REINTERPRET(instance, _instance, Instance_t *);

   OperationInfo_t writeInfo;
   writeInfo.offset = offset;
   writeInfo.numBytes = numBytes;
   writeInfo.writeBuffer = writeBuffer;

   Fsm_SendSignal(&instance->_private.fsm, Signal_ClientWriteRequested, &writeInfo);
}

static void Erase(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Instance_t *);
   Fsm_SendSignal(&instance->_private.fsm, Signal_ClientEraseRequested, NULL);
}

static uint16_t GetByteAlignment(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Instance_t *);
   return HardwareEeprom_GetByteAlignment(instance->_private.eeprom);
}

static uint16_t GetSize(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Instance_t *);
   return HardwareEeprom_GetSize(instance->_private.eeprom);
}

static I_Event_t *GetOnWrittenEvent(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Instance_t *);
   return &instance->_private.writeCompleteEvent.interface;
}

static I_Event_t *GetOnReadEvent(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Instance_t *);
   return &instance->_private.readCompleteEvent.interface;
}

static I_Event_t *GetOnErasedEvent(I_HardwareEeprom_t *_instance)
{
   REINTERPRET(instance, _instance, Instance_t *);
   return &instance->_private.eraseCompleteEvent.interface;
}

static void EepromWrittenCallback(void *context, const void *_args)
{
   REINTERPRET(instance, context, Instance_t *);
   REINTERPRET(args, _args, const HardwareEepromEventArgs_t *);

   Fsm_SendSignal(&instance->_private.fsm, Signal_EepromWriteCompleted, args);
}

static void EepromReadCallback(void *context, const void *_args)
{
   REINTERPRET(instance, context, Instance_t *);
   REINTERPRET(args, _args, const HardwareEepromEventArgs_t *);

   Fsm_SendSignal(&instance->_private.fsm, Signal_EepromReadCompleted, args);
}

static void EepromErasedCallback(void *context, const void *_args)
{
   REINTERPRET(instance, context, Instance_t *);
   REINTERPRET(args, _args, const HardwareEepromEventArgs_t *);

   Fsm_SendSignal(&instance->_private.fsm, Signal_EepromEraseCompleted, args);
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

void HardwareEeprom_DelayedPageOperationWrapper_Init(
   HardwareEeprom_DelayedPageOperationWrapper_t *instance,
   I_HardwareEeprom_t *eeprom,
   TimerModule_t *timerModule)
{
   instance->interface.api = &api;
   instance->_private.timerModule = timerModule;
   instance->_private.eeprom = eeprom;

   Event_SafeSynchronous_Init(&instance->_private.readCompleteEvent);
   Event_SafeSynchronous_Init(&instance->_private.writeCompleteEvent);
   Event_SafeSynchronous_Init(&instance->_private.eraseCompleteEvent);

   EventSubscription_Init(&instance->_private.subscription.eepromRead, instance, EepromReadCallback);
   EventSubscription_Init(&instance->_private.subscription.eepromWritten, instance, EepromWrittenCallback);
   EventSubscription_Init(&instance->_private.subscription.eepromErased, instance, EepromErasedCallback);
   Event_Subscribe(HardwareEeprom_GetOnReadEvent(eeprom), &instance->_private.subscription.eepromRead);
   Event_Subscribe(HardwareEeprom_GetOnWrittenEvent(eeprom), &instance->_private.subscription.eepromWritten);
   Event_Subscribe(HardwareEeprom_GetOnErasedEvent(eeprom), &instance->_private.subscription.eepromErased);

   Fsm_Init(&instance->_private.fsm, State_Idle);
}
