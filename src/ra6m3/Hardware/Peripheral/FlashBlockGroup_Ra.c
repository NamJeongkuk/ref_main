/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include "FlashBlockGroup_Ra.h"
#include "Event_Synchronous.h"
#include "uassert.h"
#include "utils.h"
#include "r_flash_hp.h"
#include <string.h>

#define BlockStartAddress(block) DataFlashStartAddress + (block * DataFlashBlockSize)

enum
{
   DataFlashStartAddress = 0x40100000,

   DataFlashMinimumProgramSize = 4,
   DataFlashBlockSize = 64,
   DataFlashBlockCount = 1024
};

typedef struct
{
   I_FlashBlockGroup_t interface;

   struct
   {
      I_Action_t *blankCheck;
      I_Action_t *error;
   } actions;

   struct
   {
      Event_Synchronous_t OnWritten;
      Event_Synchronous_t OnErased;
   } events;

   struct
   {
      TimerModule_t *timerModule_1ms;
      Timer_t flashOperationTimer;
      FlashBlockOnWrittenArgs_t writeArgs;
      FlashBlock_t eraseBlock;
      bool blankCheckResult;
      bool operationInProgress;
      bool flashErrorOccurred;
   } flashOperation;
} FlashBlockGroup_Ra_t;

static struct
{
   I_FlashBlockGroup_t interface;

   struct
   {
      I_Action_t *blankCheck;
      I_Action_t *error;
   } actions;

   struct
   {
      Event_Synchronous_t onWritten;
      Event_Synchronous_t onErased;
   } events;

   struct
   {
      TimerModule_t *timerModule_1ms;
      Timer_t flashOperationTimer;
      FlashBlockOnWrittenArgs_t writeArgs;
      FlashBlock_t eraseBlock;
      bool blankCheckResult;
      bool operationInProgress;
      bool flashErrorOccurred;
   } flashOperation;
} instance;

static flash_hp_instance_ctrl_t flashControl;

static void CheckForFlashError(void)
{
   if(instance.flashOperation.flashErrorOccurred)
   {
      Action_SafeInvoke(instance.actions.error);
   }
}

static void CheckForEraseComplete(void *context)
{
   IGNORE_ARG(context);

   CheckForFlashError();

   unless(instance.flashOperation.operationInProgress)
   {
      TimerModule_Stop(
         instance.flashOperation.timerModule_1ms,
         &instance.flashOperation.flashOperationTimer);
      Event_Synchronous_Publish(
         &instance.events.onErased,
         &instance.flashOperation.eraseBlock);
   }
}

static void CheckForWriteComplete(void *context)
{
   IGNORE_ARG(context);

   CheckForFlashError();

   unless(instance.flashOperation.operationInProgress)
   {
      TimerModule_Stop(
         instance.flashOperation.timerModule_1ms,
         &instance.flashOperation.flashOperationTimer);
      Event_Synchronous_Publish(
         &instance.events.onWritten,
         &instance.flashOperation.writeArgs);
   }
}

static void AssertOperationIsNotInProgress(void)
{
   uassert(!instance.flashOperation.operationInProgress && !TimerModule_IsRunning(instance.flashOperation.timerModule_1ms, &instance.flashOperation.flashOperationTimer));
}

static void Erase(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Ra_t *);

   AssertOperationIsNotInProgress();

   instance->flashOperation.operationInProgress = true;
   instance->flashOperation.eraseBlock = block;

   TimerModule_StartPeriodic(
      instance->flashOperation.timerModule_1ms,
      &instance->flashOperation.flashOperationTimer,
      1,
      CheckForEraseComplete,
      NULL);

   fsp_err_t result = R_FLASH_HP_Erase(&flashControl, BlockStartAddress(block), 1);
   uassert(result == FSP_SUCCESS);
}

static void Read(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes,
   void *readBuffer)
{
   IGNORE_ARG(_instance);
   memcpy(readBuffer, (void *)(BlockStartAddress(block) + offset), numBytes);
}

static void Write(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes,
   const void *writeBuffer)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Ra_t *);

   AssertOperationIsNotInProgress();

   instance->flashOperation.operationInProgress = true;
   instance->flashOperation.writeArgs.block = block;
   instance->flashOperation.writeArgs.offset = offset;

   TimerModule_StartPeriodic(
      instance->flashOperation.timerModule_1ms,
      &instance->flashOperation.flashOperationTimer,
      1,
      CheckForWriteComplete,
      NULL);

   fsp_err_t result = R_FLASH_HP_Write(
      &flashControl,
      (uint32_t)writeBuffer,
      BlockStartAddress(block) + offset,
      (uint32_t)numBytes);
   uassert(result == FSP_SUCCESS);
}

static uint16_t GetBlockAlignment(I_FlashBlockGroup_t *_instance)
{
   IGNORE_ARG(_instance);
   return DataFlashMinimumProgramSize;
}

static bool FlashBlockRegionIsBlank(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block,
   FlashBlockOffset_t offset,
   FlashBlockByteCount_t numBytes)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Ra_t *);

   AssertOperationIsNotInProgress();

   instance->flashOperation.operationInProgress = true;

   flash_result_t unused;
   fsp_err_t result = R_FLASH_HP_BlankCheck(
      &flashControl,
      BlockStartAddress(block) + offset,
      numBytes,
      &unused);
   uassert(result == FSP_SUCCESS);

   // Enforce blocking for blank check operations
   while(instance->flashOperation.operationInProgress)
   {
      Action_SafeInvoke(instance->actions.blankCheck);
   }

   CheckForFlashError();

   return instance->flashOperation.blankCheckResult;
}

static FlashBlockCount_t GetNumberOfBlocks(
   I_FlashBlockGroup_t *_instance)
{
   IGNORE_ARG(_instance);
   return DataFlashBlockCount;
}

static FlashBlockByteCount_t GetBlockSize(
   I_FlashBlockGroup_t *_instance,
   FlashBlock_t block)
{
   IGNORE_ARG(_instance);
   IGNORE_ARG(block);
   return DataFlashBlockSize;
}

static void OperationComplete(flash_callback_args_t *args)
{
   switch(args->event)
   {
      case FLASH_EVENT_ERASE_COMPLETE:
      case FLASH_EVENT_WRITE_COMPLETE:
         instance.flashOperation.operationInProgress = false;
         break;

      case FLASH_EVENT_BLANK:
         instance.flashOperation.operationInProgress = false;
         instance.flashOperation.blankCheckResult = true;
         break;

      case FLASH_EVENT_NOT_BLANK:
         instance.flashOperation.operationInProgress = false;
         instance.flashOperation.blankCheckResult = false;
         break;

      default:
         instance.flashOperation.flashErrorOccurred = true;
         break;
   }
}

static I_Event_t *GetOnErasedEvent(I_FlashBlockGroup_t *_instance)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Ra_t *);
   return &instance->events.OnErased.interface;
}

static I_Event_t *GetOnWrittenEvent(I_FlashBlockGroup_t *_instance)
{
   REINTERPRET(instance, _instance, FlashBlockGroup_Ra_t *);
   return &instance->events.OnWritten.interface;
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

static const flash_cfg_t flashConfig = {
   .data_flash_bgo = true,
   .p_callback = OperationComplete,
   .p_context = NULL,
   .irq = VECTOR_NUMBER_FCU_FRDYI,
   .err_irq = VECTOR_NUMBER_FCU_FIFERR,
   .err_ipl = (3),
   .ipl = (3)
};

static void ConfigureFlashDriver(void)
{
   fsp_err_t result = R_FLASH_HP_Open(&flashControl, &flashConfig);
   uassert(result == FSP_SUCCESS);
}

I_FlashBlockGroup_t *FlashBlockGroup_Ra_Init(
   I_Action_t *flashBlockBlankAction,
   I_Action_t *onErrorAction,
   TimerModule_t *timerModule)
{
   instance.interface.api = &api;

   instance.flashOperation.flashErrorOccurred = false;
   instance.flashOperation.operationInProgress = false;

   instance.flashOperation.timerModule_1ms = timerModule;

   instance.actions.error = onErrorAction;
   instance.actions.blankCheck = flashBlockBlankAction;

   Event_Synchronous_Init(&instance.events.onWritten);
   Event_Synchronous_Init(&instance.events.onErased);

   ConfigureFlashDriver();

   return &instance.interface;
}
