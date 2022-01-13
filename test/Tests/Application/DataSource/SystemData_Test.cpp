/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

extern "C"
{
#include "SystemData.h"
#include "Action_Null.h"
#include "Action_Context.h"
#include "Crc16Calculator_Table.h"
}

#include <string.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "TimerModule_TestDouble.h"
#include "Action_Mock.h"
#include "Crc16Calculator_TestDouble.h"
#include "Eeprom_Model.h"

#define SIZE_1K 1024

static uint8_t eeprom[SIZE_1K];

#define AllErds                \
   size_t i = 0;               \
   i < NUM_ELEMENTS(erdTable); \
   i++
#define And

enum
{
   DataMatched = 0,

   OutputIsEnabled = 1,
   InputAndOutputAreEnabled = 2,

   WriteTime = 1,
   EraseTime = 1,
   MinAllowableBytes = 1,

   Block0 = 0,
   Block1,
   Block2,
   Block3,
   BlockCount,

   BlockSize = 1024
};

#define Swap_N 0
#define Swap_Y 1

#define Io_None 0
#define Io_O 1
#define Io_All 2

#define Sub_N 0
#define Sub_Y 1

// clang-format off

#define ERD_EXPAND_AS_ENDIANNESS_AWARE_RAM_STORAGE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)({ Name COMMA Number COMMA Swap COMMA Io COMMA Sub } COMMA)

#define ERD_EXPAND_AS_ENDIANNESS_AWARE_NV_STORAGE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_NV_, StorageType)({ Name COMMA Number COMMA Swap COMMA Io COMMA Sub } COMMA)

#define ERD_EXPAND_AS_ENDIANNESS_AWARE_BSP_STORAGE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, FaultId) \
   CONCAT(INCLUDE_BSP_, StorageType)({ Name COMMA Number COMMA Swap COMMA Io COMMA Sub } COMMA)

typedef struct
{
   Erd_t erd;
   Erd_t externalErd;
   bool endiannessNeedsToBeSwapped;
   uint8_t io;
   uint8_t subscription;
} ErdTableElement_t;

static const ErdTableElement_t erdTable[] = {
   ERD_TABLE(ERD_EXPAND_AS_ENDIANNESS_AWARE_RAM_STORAGE)
   ERD_TABLE(ERD_EXPAND_AS_ENDIANNESS_AWARE_NV_STORAGE)
   ERD_TABLE(ERD_EXPAND_AS_ENDIANNESS_AWARE_BSP_STORAGE)
};

// clang-format on

static void RunTimerModule(void *context)
{
   REINTERPRET(timerModule, context, TimerModule_TestDouble_t *);
   TimerModule_TestDouble_ElapseTime(timerModule, 1);
}

TEST_GROUP(SystemData)
{
   SystemData_t instance;
   uint8_t blockOfRandomData[UINT8_MAX];
   uint8_t dataFromExternalDataSource[UINT8_MAX];
   uint8_t dataFromInternalDataSource[UINT8_MAX];
   I_DataModel_t *dataModel;
   I_DataSource_t *externalDataSource;

   TimerModule_t *timerModule;
   TimerModule_TestDouble_t timerModuleDouble;
   Eeprom_Model_t eepromModel;
   Action_Context_t runTimerModuleAction;

   TimerTicks_t readTime;
   TimerTicks_t writeTime;
   TimerTicks_t eraseTime;

   uint16_t alignment;
   uint16_t startAddress;

   void setup()
   {
      alignment = 1;
      startAddress = 0;
      readTime = 10;
      writeTime = 5;
      eraseTime = 20;

      TimerModule_TestDouble_Init(&timerModuleDouble);
      timerModule = TimerModule_TestDouble_GetTimerModule(&timerModuleDouble);
      Action_Context_Init(&runTimerModuleAction, &timerModuleDouble.timerModule, RunTimerModule);

      memset(eeprom, 0, sizeof(eeprom));
      Eeprom_Model_Init(
         &eepromModel,
         eeprom,
         alignment,
         startAddress,
         sizeof(eeprom),
         timerModule,
         readTime,
         writeTime,
         eraseTime);

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
         &eepromModel.interface,
         Crc16Calculator_Table,
         &runTimerModuleAction.interface,
         Action_Null_GetInstance());

      dataModel = SystemData_DataModel(&instance);
      externalDataSource = SystemData_ExternalDataSource(&instance);
   }

   void AfterEepromWriteCompletes()
   {
      After(WriteTime * 100);
   }

   void SystemDataIsReset()
   {
      GivenThatSystemDataIsInitialized();
   }

   void WhenDataIsWrittenViaInternalDataModel(Erd_t erd, void *data)
   {
      DataModel_Write(dataModel, erd, data);
   }

   void WhenDataIsWrittenViaExternalDataSource(Erd_t erd, void *data)
   {
      DataSource_Write(externalDataSource, erd, data);
   }

   void WhenOutputForErdIsWrittenWithData(Erd_t erd, void *data)
   {
      Output_Write(DataModel_GetOutput(dataModel, erd), data);
   }

   void InternalDataModelShouldReturnDataEqualTo(Erd_t erd, void *expected)
   {
      uint8_t size = DataModel_SizeOf(dataModel, erd);

      DataModel_Read(dataModel, erd, dataFromInternalDataSource);
      MEMCMP_EQUAL(expected, dataFromInternalDataSource, size);
   }

   void ExternalDataModelShouldReturnDataEqualTo(Erd_t erd, void *expected)
   {
      uint8_t size = DataSource_SizeOf(externalDataSource, erd);

      DataSource_Read(externalDataSource, erd, dataFromExternalDataSource);
      MEMCMP_EQUAL(expected, dataFromExternalDataSource, size);
   }

   void EndiannessShouldBeSwapped(Erd_t internalErd, Erd_t externalErd)
   {
      DataModel_Read(dataModel, internalErd, dataFromInternalDataSource);
      DataSource_Read(externalDataSource, externalErd, dataFromExternalDataSource);

      if(DataMatched == memcmp(dataFromInternalDataSource, dataFromExternalDataSource, DataSource_SizeOf(externalDataSource, externalErd)))
      {
         char error[50];
         sprintf(error, "ERD 0x%04X was not swapped but should be", externalErd);
         FAIL(error);
      }
   }

   void EndiannessShouldNotBeSwapped(Erd_t internalErd, Erd_t externalErd)
   {
      DataModel_Read(dataModel, internalErd, dataFromInternalDataSource);
      DataSource_Read(externalDataSource, externalErd, dataFromExternalDataSource);

      if(DataMatched != memcmp(dataFromInternalDataSource, dataFromExternalDataSource, DataSource_SizeOf(externalDataSource, externalErd)))
      {
         char error[50];
         sprintf(error, "ERD 0x%04X was swapped but shouldn't be", externalErd);
         FAIL(error);
      }
   }

   void DataModelShouldReturnInputForErd(Erd_t erd)
   {
      I_Input_t *input = DataModel_GetInput(dataModel, erd);
      CHECK(input != NULL);
   }

   void DataModelShouldReturnInputOutputForErd(Erd_t erd)
   {
      I_InputOutput_t *inputOutput = DataModel_GetInputOutput(dataModel, erd);
      CHECK(inputOutput != NULL);
   }

   void DataModelShouldReturnOutputForErd(Erd_t erd)
   {
      I_Output_t *output = DataModel_GetOutput(dataModel, erd);
      CHECK(output != NULL);
   }

   void DataModelShouldAllowSubscribeUnsubscribeFor(Erd_t erd)
   {
      EventSubscription_t subscription;
      DataModel_Subscribe(dataModel, erd, &subscription);
      DataModel_Unsubscribe(dataModel, erd, &subscription);
   }

   void AssertionShouldFailWhenInputIsRequestedForErd(Erd_t erd)
   {
      ShouldFailAssertionWhen(DataModel_GetInput(dataModel, erd));
   }

   void AssertionShouldFailWhenInputOutputIsRequestedForErd(Erd_t erd)
   {
      ShouldFailAssertionWhen(DataModel_GetInputOutput(dataModel, erd));
   }

   void AssertionShouldFailWhenOutputIsRequestedForErd(Erd_t erd)
   {
      ShouldFailAssertionWhen(DataModel_GetOutput(dataModel, erd));
   }

   void AssertionShouldFailWhenSubscribingToErd(Erd_t erd)
   {
      EventSubscription_t subscription;
      ShouldFailAssertionWhen(DataModel_Subscribe(dataModel, erd, &subscription));
   }
};

TEST(SystemData, ShouldSupportReadsAndWritesToInternalDataModel)
{
   GivenThatSystemDataIsInitialized();

   // for(AllErds)
   // {
   //    WhenDataIsWrittenViaInternalDataModel(erdTable[i].erd, blockOfRandomData);
   //    InternalDataModelShouldReturnDataEqualTo(erdTable[i].erd, blockOfRandomData);
   // }
}

// TEST(SystemData, ShouldSwapEndiannessOfSpecifiedErds)
// {
//    GivenThatSystemDataIsInitialized();

//    for(AllErds)
//    {
//       Erd_t internalErd = erdTable[i].erd;
//       Erd_t externalErd = erdTable[i].externalErd;
//       bool endiannessNeedsToBeSwapped = erdTable[i].endiannessNeedsToBeSwapped;

//       WhenDataIsWrittenViaInternalDataModel(internalErd, blockOfRandomData);

//       if(endiannessNeedsToBeSwapped)
//       {
//          EndiannessShouldBeSwapped(internalErd, externalErd);
//       }
//       else
//       {
//          EndiannessShouldNotBeSwapped(internalErd, externalErd);
//       }
//    }
// }

// TEST(SystemData, ShouldSupportInputsAndInputOutputsForSpecifiedErds)
// {
//    GivenThatSystemDataIsInitialized();

//    for(AllErds)
//    {
//       Erd_t erd = erdTable[i].erd;
//       uint8_t ioConfiguration = erdTable[i].io;

//       if(ioConfiguration == InputAndOutputAreEnabled)
//       {
//          DataModelShouldReturnInputForErd(erd);
//          And DataModelShouldReturnInputOutputForErd(erd);
//       }
//       else
//       {
//          AssertionShouldFailWhenInputIsRequestedForErd(erd);
//          And AssertionShouldFailWhenInputOutputIsRequestedForErd(erd);
//       }
//    }
// }

// TEST(SystemData, ShouldSupportOutputsForSpecifiedErds)
// {
//    GivenThatSystemDataIsInitialized();

//    for(AllErds)
//    {
//       Erd_t erd = erdTable[i].erd;
//       uint8_t ioConfiguration = erdTable[i].io;

//       if(ioConfiguration == OutputIsEnabled || ioConfiguration == InputAndOutputAreEnabled)
//       {
//          DataModelShouldReturnOutputForErd(erd);
//       }
//       else
//       {
//          AssertionShouldFailWhenOutputIsRequestedForErd(erd);
//       }
//    }
// }

// TEST(SystemData, ShouldSupportSubscribeUnsubscribeForSpecifiedErds)
// {
//    GivenThatSystemDataIsInitialized();

//    for(AllErds)
//    {
//       Erd_t erd = erdTable[i].erd;
//       uint8_t subscriptionConfiguration = erdTable[i].subscription;

//       if(subscriptionConfiguration == Sub_Y)
//       {
//          DataModelShouldAllowSubscribeUnsubscribeFor(erd);
//       }
//       else
//       {
//          AssertionShouldFailWhenSubscribingToErd(erd);
//       }
//    }
// }

// TEST(SystemData, ShouldSupportReadAndWriteToBspErdsViaExternalDataSource)
// {
//    AdcCounts_t expected = 0x1234;
//    GivenThatSystemDataIsInitialized();

//    WhenDataIsWrittenViaExternalDataSource(Erd_SomeAnalogInput, &expected);

//    ExternalDataModelShouldReturnDataEqualTo(Erd_SomeAnalogInput, &expected);
// }

// TEST(SystemData, ShouldSupportReadAndWriteToBspErdsViaInternalDataSource)
// {
//    AdcCounts_t expected = 0x4321;
//    GivenThatSystemDataIsInitialized();

//    WhenDataIsWrittenViaInternalDataModel(Erd_SomeAnalogInput, &expected);

//    InternalDataModelShouldReturnDataEqualTo(Erd_SomeAnalogInput, &expected);
// }

// TEST(SystemData, ShouldSetNvErdDataToDefaultValues)
// {
//    uint32_t expected = 0xC0DECAFE;
//    GivenThatSystemDataIsInitialized();

//    InternalDataModelShouldReturnDataEqualTo(Erd_SomeData, &expected);
// }

// TEST(SystemData, NvErdsShouldPersistAfterReset)
// {
//    uint32_t expected = 0xAAAABBBB;
//    GivenThatSystemDataIsInitialized();

//    WhenDataIsWrittenViaInternalDataModel(Erd_SomeData, &expected);
//    AfterEepromWriteCompletes();
//    SystemDataIsReset();

//    InternalDataModelShouldReturnDataEqualTo(Erd_SomeData, &expected);
// }
