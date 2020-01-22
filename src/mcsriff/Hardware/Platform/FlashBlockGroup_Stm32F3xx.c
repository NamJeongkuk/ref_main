/*!
 * @file
 * @brief Implementation of a FlashBlockGroup for the STM32F3xx
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <string.h>
#include "FlashBlockGroup_Stm32F3xx.h"
#include "Event_Synchronous.h"
#include "uassert.h"
#include "utils.h"
#include "stm32f3xx.h"

enum
{
   BlockAlignment = 2,
   FlashBlockSize = 2048,
   EraseOneBlock = 1,
   EraseCompleteReturnAddress = 0xFFFFFFFF,
   EmptyFlashByte = 0xFF,
   MaximumBytesToCheckForBlankness = 50
};

typedef struct
{
   I_FlashBlockGroup_t interface;

   FlashBlockCount_t blockCount;
   const uint32_t *blockAddresses;
   TimerModule_t *timerModule;
   Timer_t operationCompleteCheckTimer;

   struct
   {
      I_Action_t *error;
   } actions;

   struct
   {
      Event_Synchronous_t onWritten;
      Event_Synchronous_t onErased;
   } events;

   struct
   {
      FlashBlockOnWrittenArgs_t writeArgs;
      FlashBlock_t eraseBlock;
      bool operationInProgress;
      bool flashErrorOccurred;
      uint16_t halfWordsToWrite;
      uint32_t addressBeingProgrammed;
      uint32_t lastOperationAddress;
      const uint16_t *writeBuffer;
   } flashOperation;
} FlashBlockGroup_Stm32F3xx_t;

static void CheckForOperationComplete(void *context);

static FlashBlockGroup_Stm32F3xx_t instance;

static bool CheckForFlashError(void)
{
   if(instance.flashOperation.flashErrorOccurred)
   {
      Action_SafeInvoke(instance.actions.error);
      return true;
   }

   return false;
}

static void EraseComplete(void)
{
   if(CheckForFlashError())
   {
      return;
   }

   Event_Synchronous_Publish(
      &instance.events.onErased,
      &instance.flashOperation.eraseBlock);
}

static void WriteHalfWord(uint32_t address, const uint16_t *data)
{
   instance.flashOperation.operationInProgress = true;

   TimerModule_StartPeriodic(
      instance.timerModule,
      &instance.operationCompleteCheckTimer,
      1,
      CheckForOperationComplete,
      NULL);

   uassert(HAL_OK == HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_HALFWORD, address, *data));
}

static void WriteComplete(void)
{
   if(CheckForFlashError())
   {
      return;
   }

   if(--instance.flashOperation.halfWordsToWrite)
   {
      instance.flashOperation.writeBuffer += BlockAlignment;
      instance.flashOperation.addressBeingProgrammed += BlockAlignment;
      WriteHalfWord(instance.flashOperation.addressBeingProgrammed, instance.flashOperation.writeBuffer);
   }
   else
   {
      Event_Synchronous_Publish(
         &instance.events.onWritten,
         &instance.flashOperation.writeArgs);
   }
}

static void AssertOperationIsNotInProgress(void)
{
   uassert(!instance.flashOperation.operationInProgress);
}

static void CheckForOperationComplete(void *context)
{
   IGNORE(context);

   if(!instance.flashOperation.operationInProgress)
   {
      TimerModule_Stop(
         instance.timerModule,
         &instance.operationCompleteCheckTimer);

      if(instance.flashOperation.lastOperationAddress == EraseCompleteReturnAddress)
      {
         EraseComplete();
      }
      else if(instance.flashOperation.lastOperationAddress == instance.flashOperation.addressBeingProgrammed)
      {
         WriteComplete();
      }
   }
}

static void Erase(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block)
{
   IGNORE(_instance);

   AssertOperationIsNotInProgress();

   instance.flashOperation.operationInProgress = true;
   instance.flashOperation.eraseBlock = block;

   FLASH_EraseInitTypeDef eraseConfig;
   eraseConfig.TypeErase = FLASH_TYPEERASE_PAGES;
   eraseConfig.PageAddress = instance.blockAddresses[block];
   eraseConfig.NbPages = 1;

   TimerModule_StartPeriodic(
      instance.timerModule,
      &instance.operationCompleteCheckTimer,
      1,
      CheckForOperationComplete,
      NULL);

   uassert(HAL_OK == HAL_FLASHEx_Erase_IT(&eraseConfig));
}

static void Read(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes,
   void *readBuffer)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Stm32F3xx_t *);
   memcpy(readBuffer, (void *)(instance->blockAddresses[block] + offset), numBytes);
}

static void Write(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes,
   const void *writeBuffer)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Stm32F3xx_t *);

   // Flash can only be programmed 16-bits at a time
   uassert(!(numBytes % BlockAlignment));

   AssertOperationIsNotInProgress();

   instance->flashOperation.writeArgs.block = block;
   instance->flashOperation.writeArgs.offset = offset;
   instance->flashOperation.writeBuffer = writeBuffer;
   instance->flashOperation.halfWordsToWrite = numBytes / BlockAlignment;
   instance->flashOperation.addressBeingProgrammed = instance->blockAddresses[block] + offset;

   WriteHalfWord(instance->flashOperation.addressBeingProgrammed, instance->flashOperation.writeBuffer);
}

static uint16_t GetBlockAlignment(I_FlashBlockGroup_t *instance)
{
   IGNORE_ARG(instance);
   return BlockAlignment;
}

static bool FlashBlockRegionIsBlank(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Stm32F3xx_t *);
   uint8_t value;

   for(uint16_t i = 0; i < MIN(numBytes, MaximumBytesToCheckForBlankness); i++)
   {
      memcpy(&value, (uint8_t *)(instance->blockAddresses[block] + offset + i), sizeof(uint8_t));
      if(value != EmptyFlashByte)
      {
         return false;
      }
   }
   return true;
}

static FlashBlockCount_t GetNumberOfBlocks(I_FlashBlockGroup_t *_instance)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Stm32F3xx_t *);
   return instance->blockCount;
}

static FlashBlockByteCount_t GetBlockSize(I_FlashBlockGroup_t *instance, FlashBlock_t block)
{
   IGNORE_ARG(instance);
   IGNORE_ARG(block);
   return FlashBlockSize;
}

static I_Event_t *GetOnErasedEvent(I_FlashBlockGroup_t *_instance)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Stm32F3xx_t *);
   return &instance->events.onErased.interface;
}

static I_Event_t *GetOnWrittenEvent(I_FlashBlockGroup_t *_instance)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Stm32F3xx_t *);
   return &instance->events.onWritten.interface;
}

static const I_FlashBlockGroup_Api_t api =
   {
      .Erase = Erase,
      .Read = Read,
      .Write = Write,
      .GetBlockAlignment = GetBlockAlignment,
      .FlashBlockRegionIsBlank = FlashBlockRegionIsBlank,
      .GetNumberOfBlocks = GetNumberOfBlocks,
      .GetBlockSize = GetBlockSize,
      .GetOnErasedEvent = GetOnErasedEvent,
      .GetOnWrittenEvent = GetOnWrittenEvent
   };

static void ConfigureFlashDriver(void)
{
   HAL_FLASH_Unlock();
   HAL_NVIC_SetPriority(FLASH_IRQn, 12, 12);
   HAL_NVIC_EnableIRQ(FLASH_IRQn);
}

I_FlashBlockGroup_t *FlashBlockGroup_Stm32F3xx_Init(
   I_Action_t *onErrorAction,
   TimerModule_t *timerModule,
   FlashBlockCount_t blockCount,
   const uint32_t *blockAddresses)
{
   instance.interface.api = &api;

   instance.flashOperation.flashErrorOccurred = false;
   instance.flashOperation.operationInProgress = false;

   instance.actions.error = onErrorAction;
   instance.timerModule = timerModule;
   instance.blockCount = blockCount;
   instance.blockAddresses = blockAddresses;

   Event_Synchronous_Init(&instance.events.onWritten);
   Event_Synchronous_Init(&instance.events.onErased);

   ConfigureFlashDriver();

   return &instance.interface;
}

void HAL_FLASH_EndOfOperationCallback(uint32_t address)
{
   instance.flashOperation.lastOperationAddress = address;
   instance.flashOperation.operationInProgress = false;
}

void HAL_FLASH_OperationErrorCallback(uint32_t address)
{
   instance.flashOperation.flashErrorOccurred = true;
   instance.flashOperation.lastOperationAddress = address;
   instance.flashOperation.operationInProgress = false;
}

void FLASH_IRQHandler(void)
{
   HAL_FLASH_IRQHandler();
}
