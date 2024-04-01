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
#include "TddPersonality.h"
#include "DataModelErdPointerAccess.h"
#include "ParametricDataErds.h"
#include <string.h>
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "TimerModule_TestDouble.h"
#include "Action_Mock.h"
#include "Crc16Calculator_TestDouble.h"
#include "AsyncDataSource_Eeprom_TestDouble.h"
#include "PersonalityParametricDataTestDouble.h"
#include <sstream>

#define AllErds                \
   size_t i = 0;               \
   i < NUM_ELEMENTS(erdTable); \
   i++

#define AllBspErds                    \
   size_t i = 0;                      \
   i < NUM_ELEMENTS(someBspErdTable); \
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

#define Swap_No 0
#define Swap_Yes 1
#define Swap_Range 1
#define Swap_Range_S 1
#define Swap_Range_E 1

#define Io_None 0
#define Io_O 1
#define Io_All 2

#define Sub_N 0
#define Sub_Y 1

// clang-format off

#define ERD_EXPAND_AS_ENDIANNESS_AWARE_RAM_STORAGE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_RAM_, StorageType)({ Name COMMA Number COMMA Swap COMMA Io COMMA Sub } COMMA)

#define ERD_EXPAND_AS_ENDIANNESS_AWARE_NV_STORAGE(Name, Number, DataType, Swap, Io, Sub, StorageType, NvDefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)({ Name COMMA Number COMMA Swap COMMA Io COMMA Sub } COMMA)

#define ERD_EXPAND_NUMBER_SIZE_NV_STORAGE(Name, Number, DataType, Swap, Io, Sub, StorageType, DefaultData, YearlyWrites, FaultId) \
   CONCAT(INCLUDE_NVALL_, StorageType)({ Number COMMA sizeof(DataType) } COMMA)

typedef struct
{
   Erd_t erd;
   Erd_t externalErd;
   bool endiannessNeedsToBeSwapped;
   uint8_t io;
   uint8_t subscription;
} ErdTableElement_t;

static const ErdTableElement_t erdTable[] = {
   ERD_TABLE(ERD_EXPAND_AS_ENDIANNESS_AWARE_NV_STORAGE)
   ERD_TABLE(ERD_EXPAND_AS_ENDIANNESS_AWARE_RAM_STORAGE)
};

static const ErdTableElement_t someBspErdTable[] = {
   { Erd_HeartbeatLed, 0xF500, Swap_No, Io_O, Sub_N },
   { Erd_RelayWatchdog, 0xF502, Swap_No, Io_O, Sub_N },
   { Erd_CompressorInverterDriver, 0xF502, Swap_No, Io_None, Sub_N },
   { Erd_AugerMotorDirection, 0xF50A, Swap_No, Io_None, Sub_N },
   { Erd_AugerMotorPower, 0xF50B, Swap_No, Io_None, Sub_N },
   { Erd_Pwm_PWM_25K_00, 0xF56F, Swap_Range_S, Io_None, Sub_N },
   { Erd_InputCapture_CAPTURE_00, 0xF58F, Swap_Yes, Io_None, Sub_N },
   { Erd_Adc_ADC_0, 0xF59A, Swap_Range_S, Io_None, Sub_N }
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
   AsyncDataSource_Eeprom_TestDouble_t nvAsyncDataSource;
   Action_Context_t runTimerModuleAction;

   PersonalityParametricData_t *personalityParametricData;

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

      AsyncDataSource_Eeprom_TestDouble_Init(
         &nvAsyncDataSource,
         &timerModuleDouble.timerModule,
         Crc16Calculator_Table,
         true);

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
         AsyncDataSource_Eeprom_TestDouble_GetAsyncDataSource(&nvAsyncDataSource),
         Crc16Calculator_Table,
         &runTimerModuleAction.interface,
         Action_Null_GetInstance());

      dataModel = SystemData_DataModel(&instance);
      externalDataSource = SystemData_ExternalDataSource(&instance);

      personalityParametricData = (PersonalityParametricData_t *)GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(TddPersonality_DevelopmentSingleEvaporator);
      DataModelErdPointerAccess_Write(
         dataModel,
         Erd_PersonalityParametricData,
         personalityParametricData);

      SystemData_AddBspDataSource(
         &instance,
         dataModel,
         NULL,
         NULL);
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
         snprintf(error,
            sizeof(error),
            "ERD 0x%04X was not swapped but should be",
            externalErd);
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
         snprintf(error,
            sizeof(error),
            "ERD 0x%04X was swapped but shouldn't be",
            externalErd);
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

TEST(SystemData, ShouldHaveUniqueErdEnumValues)
{
   GivenThatSystemDataIsInitialized();

   for(size_t i = 0; i < NUM_ELEMENTS(erdTable); i++)
   {
      for(size_t j = 0; j < NUM_ELEMENTS(erdTable); j++)
      {
         if(i != j)
         {
            if(erdTable[i].erd == erdTable[j].erd)
            {
               std::stringstream ss;
               ss << "ERD 0x" << std::uppercase << std::hex << erdTable[i].externalErd;
               ss << " shadows ERD 0x" << std::uppercase << std::hex << erdTable[j].externalErd;
               FAIL(ss.str().c_str());
            }
         }
      }
   }
}

TEST(SystemData, ShouldSupportReadsAndWritesToInternalDataModel)
{
   GivenThatSystemDataIsInitialized();

   for(AllErds)
   {
      WhenDataIsWrittenViaInternalDataModel(erdTable[i].erd, blockOfRandomData);
      InternalDataModelShouldReturnDataEqualTo(erdTable[i].erd, blockOfRandomData);
   }
}

TEST(SystemData, ShouldSupportReadsAndWritesToSomeBspErdsInInternalDataModel)
{
   GivenThatSystemDataIsInitialized();

   for(AllBspErds)
   {
      WhenDataIsWrittenViaInternalDataModel(erdTable[i].erd, blockOfRandomData);
      InternalDataModelShouldReturnDataEqualTo(erdTable[i].erd, blockOfRandomData);
   }
}

TEST(SystemData, ShouldSwapEndiannessOfSpecifiedErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllErds)
   {
      Erd_t internalErd = erdTable[i].erd;
      Erd_t externalErd = erdTable[i].externalErd;
      bool endiannessNeedsToBeSwapped = erdTable[i].endiannessNeedsToBeSwapped;

      WhenDataIsWrittenViaInternalDataModel(internalErd, blockOfRandomData);

      if(endiannessNeedsToBeSwapped)
      {
         EndiannessShouldBeSwapped(internalErd, externalErd);
      }
      else
      {
         EndiannessShouldNotBeSwapped(internalErd, externalErd);
      }
   }
}

TEST(SystemData, ShouldSwapEndiannessOfSomeBspErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllBspErds)
   {
      Erd_t internalErd = erdTable[i].erd;
      Erd_t externalErd = erdTable[i].externalErd;
      bool endiannessNeedsToBeSwapped = erdTable[i].endiannessNeedsToBeSwapped;

      WhenDataIsWrittenViaInternalDataModel(internalErd, blockOfRandomData);

      if(endiannessNeedsToBeSwapped)
      {
         EndiannessShouldBeSwapped(internalErd, externalErd);
      }
      else
      {
         EndiannessShouldNotBeSwapped(internalErd, externalErd);
      }
   }
}

TEST(SystemData, ShouldSupportInputsAndInputOutputsForSpecifiedErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllErds)
   {
      Erd_t erd = erdTable[i].erd;
      uint8_t ioConfiguration = erdTable[i].io;

      if(ioConfiguration == InputAndOutputAreEnabled)
      {
         DataModelShouldReturnInputForErd(erd);
         And DataModelShouldReturnInputOutputForErd(erd);
      }
      else
      {
         AssertionShouldFailWhenInputIsRequestedForErd(erd);
         And AssertionShouldFailWhenInputOutputIsRequestedForErd(erd);
      }
   }
}

TEST(SystemData, ShouldSupportInputsAndInputOutputsForSomeBspErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllBspErds)
   {
      Erd_t erd = erdTable[i].erd;
      uint8_t ioConfiguration = erdTable[i].io;

      if(ioConfiguration == InputAndOutputAreEnabled)
      {
         DataModelShouldReturnInputForErd(erd);
         And DataModelShouldReturnInputOutputForErd(erd);
      }
      else
      {
         AssertionShouldFailWhenInputIsRequestedForErd(erd);
         And AssertionShouldFailWhenInputOutputIsRequestedForErd(erd);
      }
   }
}

TEST(SystemData, ShouldSupportOutputsForSpecifiedErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllErds)
   {
      Erd_t erd = erdTable[i].erd;
      uint8_t ioConfiguration = erdTable[i].io;

      if(ioConfiguration == OutputIsEnabled || ioConfiguration == InputAndOutputAreEnabled)
      {
         DataModelShouldReturnOutputForErd(erd);
      }
      else
      {
         AssertionShouldFailWhenOutputIsRequestedForErd(erd);
      }
   }
}

TEST(SystemData, ShouldSupportOutputsForSomeBspErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllBspErds)
   {
      Erd_t erd = erdTable[i].erd;
      uint8_t ioConfiguration = erdTable[i].io;

      if(ioConfiguration == OutputIsEnabled || ioConfiguration == InputAndOutputAreEnabled)
      {
         DataModelShouldReturnOutputForErd(erd);
      }
      else
      {
         AssertionShouldFailWhenOutputIsRequestedForErd(erd);
      }
   }
}

TEST(SystemData, ShouldSupportSubscribeUnsubscribeForSpecifiedErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllErds)
   {
      Erd_t erd = erdTable[i].erd;
      uint8_t subscriptionConfiguration = erdTable[i].subscription;

      if(subscriptionConfiguration == Sub_Y)
      {
         DataModelShouldAllowSubscribeUnsubscribeFor(erd);
      }
      else
      {
         AssertionShouldFailWhenSubscribingToErd(erd);
      }
   }
}

TEST(SystemData, ShouldSupportSubscribeUnsubscribeForSomeBspErds)
{
   GivenThatSystemDataIsInitialized();

   for(AllBspErds)
   {
      Erd_t erd = erdTable[i].erd;
      uint8_t subscriptionConfiguration = erdTable[i].subscription;

      if(subscriptionConfiguration == Sub_Y)
      {
         DataModelShouldAllowSubscribeUnsubscribeFor(erd);
      }
      else
      {
         AssertionShouldFailWhenSubscribingToErd(erd);
      }
   }
}

TEST(SystemData, ShouldSupportReadAndWriteToBspErdsViaExternalDataSource)
{
   TemperatureDegFx100_t expected = 0x1234;
   GivenThatSystemDataIsInitialized();

   WhenDataIsWrittenViaExternalDataSource(Erd_Freezer_FilteredTemperatureInDegFx100, &expected);

   ExternalDataModelShouldReturnDataEqualTo(Erd_Freezer_FilteredTemperatureInDegFx100, &expected);
}

TEST(SystemData, ShouldSupportReadAndWriteToBspErdsViaInternalDataSource)
{
   TemperatureDegFx100_t expected = 0x4321;
   GivenThatSystemDataIsInitialized();

   WhenDataIsWrittenViaInternalDataModel(Erd_Freezer_FilteredTemperatureInDegFx100, &expected);

   InternalDataModelShouldReturnDataEqualTo(Erd_Freezer_FilteredTemperatureInDegFx100, &expected);
}

TEST(SystemData, ShouldSetNvErdDataToDefaultValues)
{
   uint32_t expected = 0xC0DECAFE;
   GivenThatSystemDataIsInitialized();

   InternalDataModelShouldReturnDataEqualTo(Erd_SomeData, &expected);
}

TEST(SystemData, NvErdsShouldPersistAfterReset)
{
   uint32_t expected = 0xAAAABBBB;
   GivenThatSystemDataIsInitialized();

   WhenDataIsWrittenViaInternalDataModel(Erd_SomeData, &expected);
   AfterEepromWriteCompletes();
   SystemDataIsReset();

   InternalDataModelShouldReturnDataEqualTo(Erd_SomeData, &expected);
}
