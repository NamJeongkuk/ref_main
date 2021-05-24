/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "FlashBlockGroup_Psoc6.h"
#include "Event_Synchronous.h"
#include "uassert.h"
#include "utils.h"
#include "cy_pdl.h"

#define RowStartAddress(row) DataFlashStartAddress + (row * RowSize)
#define RowNumber(address) (((uint32_t)(address - DataFlashStartAddress)) / RowSize)
#define RowOffset(address) ((address) % RowSize)

enum
{
   DataFlashStartAddress = 0x14000000,
   MinimumProgramSize = 1,
   RowSize = CY_FLASH_SIZEOF_ROW,
   BlockSize = RowSize,
   BlockCount = 64,

   EmptyFlashByte = 0x00,
   MaximumBytesToCheckForBlankness = 50
};

typedef struct
{
   I_FlashBlockGroup_t interface;

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
      TimerModule_t *timerModule;
      Timer_t flashOperationTimer;
      FlashBlockOnWrittenArgs_t writeArgs;
      FlashBlock_t eraseBlock;
      bool blankCheckResult;
      bool operationInProgress;

      const uint8_t *currentSourceAddress;
      uint32_t currentDestinationAddress;
      size_t remainingBytes;
   } flashOperation;
} FlashBlockGroup_Psoc6_t;

static FlashBlockGroup_Psoc6_t instance;

static void WriteFlash(void);

static bool ErrorOccurred(cy_en_flashdrv_status_t status)
{
   switch(status)
   {
      case CY_FLASH_DRV_INV_PROT:
      case CY_FLASH_DRV_INVALID_FM_PL:
      case CY_FLASH_DRV_INVALID_FLASH_ADDR:
      case CY_FLASH_DRV_ROW_PROTECTED:
      case CY_FLASH_DRV_IPC_BUSY:
      case CY_FLASH_DRV_INVALID_INPUT_PARAMETERS:
      case CY_FLASH_DRV_PL_ROW_COMP_FA:
      case CY_FLASH_DRV_ERR_UNC:
         return true;
         break;

      default:
         return false;
         break;
   }
}

static void CheckForEraseComplete(void *context)
{
   IGNORE(context);

   cy_en_flashdrv_status_t status = Cy_Flash_IsOperationComplete();

   if(ErrorOccurred(status))
   {
      Action_SafeInvoke(instance.actions.error);
   }
   else if(status == CY_FLASH_DRV_OPERATION_STARTED || status == CY_FLASH_DRV_PROGRESS_NO_ERROR)
   {
      return;
   }
   else if(status == CY_FLASH_DRV_SUCCESS)
   {
      if(instance.flashOperation.operationInProgress)
      {
         instance.flashOperation.operationInProgress = false;

         Cy_SysLib_ClearFlashCacheAndBuffer();

         TimerModule_Stop(
            instance.flashOperation.timerModule,
            &instance.flashOperation.flashOperationTimer);
         Event_Synchronous_Publish(
            &instance.events.onErased,
            &instance.flashOperation.eraseBlock);
      }
   }
}

static void CheckForWriteComplete(void *context)
{
   IGNORE(context);

   cy_en_flashdrv_status_t status = Cy_Flash_IsOperationComplete();

   if(ErrorOccurred(status))
   {
      Action_SafeInvoke(instance.actions.error);
   }
   else if(status == CY_FLASH_DRV_OPERATION_STARTED || status == CY_FLASH_DRV_PROGRESS_NO_ERROR)
   {
      return;
   }
   else if(status == CY_FLASH_DRV_SUCCESS)
   {
      if(instance.flashOperation.remainingBytes)
      {
         WriteFlash();
      }
      else
      {
         instance.flashOperation.operationInProgress = false;

         Cy_SysLib_ClearFlashCacheAndBuffer();

         TimerModule_Stop(
            instance.flashOperation.timerModule,
            &instance.flashOperation.flashOperationTimer);
         Event_Synchronous_Publish(
            &instance.events.onWritten,
            &instance.flashOperation.writeArgs);
      }
   }
}

static void AssertOperationIsNotInProgress(void)
{
   uassert(
      !instance.flashOperation.operationInProgress &&
      !TimerModule_IsRunning(instance.flashOperation.timerModule, &instance.flashOperation.flashOperationTimer));
}

static void Erase(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block)
{
   IGNORE(_instance);

   AssertOperationIsNotInProgress();

   instance.flashOperation.operationInProgress = true;
   instance.flashOperation.eraseBlock = block;

   TimerModule_StartPeriodic(
      instance.flashOperation.timerModule,
      &instance.flashOperation.flashOperationTimer,
      1,
      CheckForEraseComplete,
      NULL);

   cy_en_flashdrv_status_t status = Cy_Flash_EraseRow(RowStartAddress(block));
   uassert(status == CY_FLASH_DRV_SUCCESS);
}

static void Read(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes,
   void *readBuffer)
{
   IGNORE(_instance);
   memcpy(readBuffer, (void *)(RowStartAddress(block) + offset), numBytes);
}

static bool PrepareAndWriteRow(
   uint16_t rowOffset,
   uint32_t rowNum,
   uint16_t numberOfBytesForCurrentRow,
   const uint8_t *sourceAddress)
{
   uint8_t rowBuffer[RowSize];

   memcpy(rowBuffer, (const void *)RowStartAddress(rowNum), RowSize);
   memcpy(&rowBuffer[rowOffset], sourceAddress, numberOfBytesForCurrentRow);
   cy_en_flashdrv_status_t status = Cy_Flash_ProgramRow(RowStartAddress(rowNum), (uint32_t *)rowBuffer);
   return (status == CY_FLASH_DRV_SUCCESS);
}

static void WriteFlash(void)
{
   uint16_t row = RowNumber(instance.flashOperation.currentDestinationAddress);
   uint16_t rowOffset = RowOffset(instance.flashOperation.currentDestinationAddress);
   uint16_t count = MIN(instance.flashOperation.remainingBytes, (size_t)(RowSize - rowOffset));

   PrepareAndWriteRow(
      rowOffset,
      row,
      count,
      instance.flashOperation.currentSourceAddress);

   instance.flashOperation.remainingBytes -= count;
   instance.flashOperation.currentDestinationAddress += count;
   instance.flashOperation.currentSourceAddress += count;
}

static void Write(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes,
   const void *writeBuffer)
{
   IGNORE(_instance);

   AssertOperationIsNotInProgress();

   instance.flashOperation.operationInProgress = true;
   instance.flashOperation.writeArgs.block = block;
   instance.flashOperation.writeArgs.offset = offset;

   instance.flashOperation.currentSourceAddress = writeBuffer;
   instance.flashOperation.currentDestinationAddress = RowStartAddress(block) + offset;
   instance.flashOperation.remainingBytes = numBytes;

   TimerModule_StartPeriodic(
      instance.flashOperation.timerModule,
      &instance.flashOperation.flashOperationTimer,
      1,
      CheckForWriteComplete,
      NULL);

   WriteFlash();
}

static uint16_t GetBlockAlignment(I_FlashBlockGroup_t *_instance)
{
   IGNORE(_instance);
   return MinimumProgramSize;
}

static bool FlashBlockRegionIsBlank(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes)
{
   IGNORE(_instance);
   uint32_t startAddress = RowStartAddress(block) + offset;

   for(uint16_t i = 0; i < MIN(numBytes, MaximumBytesToCheckForBlankness); i++)
   {
      uint8_t value = *(uint8_t *)(startAddress + i);
      if(value != EmptyFlashByte)
      {
         return false;
      }
   }

   return true;
}

static FlashBlockCount_t GetNumberOfBlocks(I_FlashBlockGroup_t *_instance)
{
   IGNORE(_instance);
   return BlockCount;
}

static FlashBlockByteCount_t GetBlockSize(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block)
{
   IGNORE(_instance);
   IGNORE(block);
   return BlockSize;
}

static I_Event_t *GetOnErasedEvent(I_FlashBlockGroup_t *_instance)
{
   IGNORE(_instance);
   return &instance.events.onErased.interface;
}

static I_Event_t *GetOnWrittenEvent(I_FlashBlockGroup_t *_instance)
{
   IGNORE(_instance);
   return &instance.events.onWritten.interface;
}

static const I_FlashBlockGroup_Api_t api = {
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

I_FlashBlockGroup_t *FlashBlockGroup_Psoc6_Init(
   I_Action_t *onErrorAction,
   TimerModule_t *timerModule)
{
   instance.interface.api = &api;

   instance.flashOperation.timerModule = timerModule;
   instance.actions.error = onErrorAction;

   instance.flashOperation.operationInProgress = false;

   Event_Synchronous_Init(&instance.events.onWritten);
   Event_Synchronous_Init(&instance.events.onErased);

   return &instance.interface;
}
