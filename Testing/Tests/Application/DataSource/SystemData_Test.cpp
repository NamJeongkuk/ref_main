/*!
 * @file
 * @brief Tests for System Data
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

extern "C"
{
#include "SystemData.h"
#include "Action_Null.h"
#include "Action_Context.h"
}

#include <string.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "TimerModule_TestDouble.h"
#include "Action_Mock.h"
#include "Crc16Calculator_TestDouble.h"
#include "FlashBlocks.h"
#include "FlashBlockGroup_Model.h"

enum
{
   DataMatched = 0,

   OutputIsEnabled = 1,
   InputAndOutputIsEnabled = 2,

   WriteTime = 1,
   EraseTime = 1,
   MinAllowableBytes = 1,

   Block0 = 0,
   Block1,
   Block2,
   Block3,
   BlockCount,

   BlockSize2K = 2048
};

#define ERD_EXPAND_AS_ENDIANNESS_AWARE_RAM_STORAGE(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)                                                                             \
   ({ Name COMMA Number COMMA Swap } COMMA)

#define ERD_EXPAND_AS_ENDIANNESS_AWARE_NV_STORAGE(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)                                                                             \
   ({ Name COMMA Number COMMA Swap } COMMA)

typedef struct
{
   Erd_t erd;
   Erd_t externalErd;
   bool endiannessNeedsToBeSwapped;
} EndianessCheckTableElement_t;

static const EndianessCheckTableElement_t endiannessAwareTable[] =
   {
      ERD_TABLE(ERD_EXPAND_AS_ENDIANNESS_AWARE_RAM_STORAGE)
      ERD_TABLE(ERD_EXPAND_AS_ENDIANNESS_AWARE_NV_STORAGE)
   };

#define ERD_EXPAND_AS_IO_CONFIGURATION(Name, Number, DataType, Swap, Io, StorageType, NvDefaultData, FaultId) \
   { Name, Io },

typedef struct
{
   Erd_t erd;
   uint8_t io;
} IoCheckTableElement_t;

static const IoCheckTableElement_t ioConfigurationTable[] =
   {
      ERD_TABLE(ERD_EXPAND_AS_IO_CONFIGURATION)
   };

static const FlashBlockItem_t flashBlockTable[] =
   {
      { Block0, BlockSize2K },
      { Block1, BlockSize2K },
      { Block2, BlockSize2K },
      { Block3, BlockSize2K }
   };

static uint8_t block0Data[BlockSize2K];
static uint8_t block1Data[BlockSize2K];
static uint8_t block2Data[BlockSize2K];
static uint8_t block3Data[BlockSize2K];

static uint8_t *blockList[] =
   { block0Data, block1Data, block2Data, block3Data };

static uint8_t block0BlankStatus[BlockSize2K];
static uint8_t block1BlankStatus[BlockSize2K];
static uint8_t block2BlankStatus[BlockSize2K];
static uint8_t block3BlankStatus[BlockSize2K];

static uint8_t *blockBlankStatusList[] =
   { block0BlankStatus, block1BlankStatus, block2BlankStatus, block3BlankStatus };

static void RunTimerModule(void *context)
{
   REINTERPRET(timerModule, context, TimerModule_TestDouble_t *);
   TimerModule_TestDouble_ElapseTime(timerModule, 1);
}

TEST_GROUP(SystemData)
{
   SystemData_t instance;
   uint8_t blockOfRandomData[200];
   uint8_t dataFromExternalDataSource[200];
   uint8_t dataFromInternalDataSource[200];
   I_DataModel_t *dataModel;
   I_DataSource_t *externalDataSource;

   TimerModule_TestDouble_t timerModuleDouble;
   Crc16Calculator_TestDouble_t crc16CalculatorDouble;
   FlashBlockGroup_Model_t flashBlockGroupModel;
   Action_Context_t runTimerModuleAction;

   void setup()
   {
      TimerModule_TestDouble_Init(&timerModuleDouble);
      Crc16Calculator_TestDouble_Init(&crc16CalculatorDouble);

      Action_Context_Init(&runTimerModuleAction, &timerModuleDouble.timerModule, RunTimerModule);

      FlashBlockGroup_Model_Init(
         &flashBlockGroupModel,
         flashBlockTable,
         blockList,
         blockBlankStatusList,
         BlockCount,
         &timerModuleDouble.timerModule,
         WriteTime,
         EraseTime,
         MinAllowableBytes);

      memset(dataFromExternalDataSource, 0, sizeof(dataFromExternalDataSource));
      FillBlockOfRandomData();
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleDouble, ticks);
   }

   void FillBlockOfRandomData()
   {
      for(uint8_t index = 0; index < sizeof(blockOfRandomData); index++)
      {
         blockOfRandomData[index] = index + 5;
      }
   }

   void GivenThatSystemDataIsInitialized()
   {
      SystemData_Init(
         &instance,
         &timerModuleDouble.timerModule,
         &flashBlockGroupModel.interface,
         &crc16CalculatorDouble.interface,
         &runTimerModuleAction.interface,
         Action_Null_GetInstance());

      dataModel = SystemData_DataModel(&instance);
      externalDataSource = SystemData_ExternalDataSource(&instance);
   }

   void WhenFlashBlockGroupWriteCompletes()
   {
      After(1);
   }

   void WhenSystemDataIsReset()
   {
      GivenThatSystemDataIsInitialized();
   }
};

TEST(SystemData, ShouldSupportReadAndWriteToInternalDataModel)
{
   GivenThatSystemDataIsInitialized();

   for(uint8_t i = 0; i < NUM_ELEMENTS(endiannessAwareTable); i++)
   {
      Erd_t erd = endiannessAwareTable[i].erd;

      DataModel_Write(dataModel, erd, blockOfRandomData);
      DataModel_Read(dataModel, erd, dataFromInternalDataSource);
      CHECK_EQUAL(DataMatched, memcmp(blockOfRandomData, dataFromInternalDataSource, DataModel_SizeOf(dataModel, erd)));
   }
}

TEST(SystemData, ShouldSwapEndiannessOfSpecifiedErds)
{
   GivenThatSystemDataIsInitialized();

   for(uint8_t i = 0; i < NUM_ELEMENTS(endiannessAwareTable); i++)
   {
      Erd_t erd = endiannessAwareTable[i].erd;
      bool endiannessNeedsToBeSwapped = endiannessAwareTable[i].endiannessNeedsToBeSwapped;

      DataModel_Write(dataModel, erd, blockOfRandomData);
      DataModel_Read(dataModel, erd, dataFromInternalDataSource);
      DataSource_Read(externalDataSource, erd, dataFromExternalDataSource);

      if(endiannessNeedsToBeSwapped)
      {
         if(DataMatched == memcmp(dataFromInternalDataSource, dataFromExternalDataSource, DataSource_SizeOf(externalDataSource, erd)))
         {
            char error[50];
            sprintf(error, "ERD 0x%04X was not swapped but should be", endiannessAwareTable[i].externalErd);
            STRCMP_EQUAL("", error);
         }
      }
      else
      {
         if(DataMatched != memcmp(dataFromInternalDataSource, dataFromExternalDataSource, DataSource_SizeOf(externalDataSource, erd)))
         {
            char error[50];
            sprintf(error, "ERD 0x%04X was swapped but shouldn't be", endiannessAwareTable[i].externalErd);
            STRCMP_EQUAL("", error);
         }
      }
   }
}

TEST(SystemData, ShouldSupportInputsAndInputOutputsForSpecifiedErds)
{
   GivenThatSystemDataIsInitialized();

   for(uint8_t i = 0; i < NUM_ELEMENTS(ioConfigurationTable); i++)
   {
      Erd_t erd = ioConfigurationTable[i].erd;
      uint8_t ioConfiguration = ioConfigurationTable[i].io;

      if(ioConfiguration == InputAndOutputIsEnabled)
      {
         I_Input_t *input = DataModel_GetInput(dataModel, erd);
         I_InputOutput_t *inputOutput = DataModel_GetInputOutput(dataModel, erd);
         CHECK(input != NULL);
         CHECK(inputOutput != NULL);
      }
      else
      {
         ShouldFailAssertionWhen(DataModel_GetInput(dataModel, erd))
         ShouldFailAssertionWhen(DataModel_GetInputOutput(dataModel, erd))
      }
   }
}

TEST(SystemData, ShouldSupportOutputsForSpecifiedErds)
{
   GivenThatSystemDataIsInitialized();

   for(uint8_t i = 0; i < NUM_ELEMENTS(ioConfigurationTable); i++)
   {
      Erd_t erd = ioConfigurationTable[i].erd;
      uint8_t ioConfiguration = ioConfigurationTable[i].io;

      if(ioConfiguration == OutputIsEnabled || ioConfiguration == InputAndOutputIsEnabled)
      {
         I_Output_t *output = DataModel_GetOutput(dataModel, erd);
         CHECK(output != NULL);
      }
      else
      {
         ShouldFailAssertionWhen(DataModel_GetOutput(dataModel, erd))
      }
   }
}

TEST(SystemData, ShouldSupportReadAndWriteToAnErdViaAnInputAndOutput)
{
   GivenThatSystemDataIsInitialized();

   I_Output_t *output = DataModel_GetOutput(dataModel, Erd_TimerModuleDiagnosticsEnable);
   I_Input_t *input = DataModel_GetInput(dataModel, Erd_TimerModuleDiagnosticsEnable);
   bool expected = true;
   bool actual;

   Output_Write(output, &expected);
   Input_Read(input, &actual);
   CHECK_EQUAL(expected, actual);
}

TEST(SystemData, ShouldSupportReadAndWriteToBspErdsViaExternalDataSource)
{
   GivenThatSystemDataIsInitialized();

   AdcCounts_t expected = 0x1234;
   AdcCounts_t actual;

   DataSource_Write(externalDataSource, Erd_SomeAnalogInput, &expected);
   DataSource_Read(externalDataSource, Erd_SomeAnalogInput, &actual);

   CHECK_EQUAL(expected, actual);
}

TEST(SystemData, ShouldSupportReadAndWriteToBspErdsViaInternalDataSource)
{
   GivenThatSystemDataIsInitialized();

   AdcCounts_t expected = 0x4321;
   AdcCounts_t actual;

   DataModel_Write(dataModel, Erd_SomeAnalogInput, &expected);
   DataModel_Read(dataModel, Erd_SomeAnalogInput, &actual);

   CHECK_EQUAL(expected, actual);
}

TEST(SystemData, ShouldSetNvErdDataToDefaultValues)
{
   GivenThatSystemDataIsInitialized();

   uint32_t expected = 0xC0DE;
   uint32_t actual;

   DataModel_Read(dataModel, Erd_SomeData, &actual);

   CHECK_EQUAL(expected, actual);
}

TEST(SystemData, NvErdsShouldPersistAfterReset)
{
   GivenThatSystemDataIsInitialized();

   uint32_t expected = 0xABCD;
   uint32_t actual;

   DataModel_Write(dataModel, Erd_SomeData, &expected);
   DataModel_Read(dataModel, Erd_SomeData, &actual);

   WhenFlashBlockGroupWriteCompletes();

   WhenSystemDataIsReset();

   DataModel_Read(dataModel, Erd_SomeData, &actual);

   CHECK_EQUAL(expected, actual);
}
