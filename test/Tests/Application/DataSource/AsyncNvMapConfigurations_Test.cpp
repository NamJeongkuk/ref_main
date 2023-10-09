/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AsyncNvMapConfigurations.h"
#include "EepromPartitionIndex.h"
#include "I_ConstArrayMap.h"
#include "SystemErds.h"
#include "uassert.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

#define Given
#define The

enum
{
   ProtectedMapIndex = EepromPartitionIndex_NvProtected,
   UserSettingMapIndex = EepromPartitionIndex_NvUserSetting,
   UnitSettingMapIndex = EepromPartitionIndex_NvUnitSetting,
   RfidMapIndex = EepromPartitionIndex_NvRfid,
   UsageProfileIndex = EepromPartitionIndex_NvUsageProfile,
   FaultSnapshotMapIndex = EepromPartitionIndex_NvFaultSnapshot,
   CycleHistoryMapIndex = EepromPartitionIndex_NvCycleHistory,
   ExpectedMapCount = EepromPartitionIndex_PartitionCount
};

// clang-format off
enum
{
   ERD_TABLE(EXPAND_AS_NVONLY_ERD_ENUM)
   NvErdCount
};
// clang-format on

static const AsyncDataSource_EepromErdInfo_t expectedNvProtectedElements[] = { ERD_TABLE(EXPAND_AS_PROTECTED_ASYNC_MAP_ELEMENTS) };
static const AsyncDataSource_EepromErdInfo_t expectedNvUserSettingElements[] = { ERD_TABLE(EXPAND_AS_USERSETTING_ASYNC_MAP_ELEMENTS) };
static const AsyncDataSource_EepromErdInfo_t expectedNvUnitSettingElements[] = { ERD_TABLE(EXPAND_AS_UNITSETTING_ASYNC_MAP_ELEMENTS) };
static const AsyncDataSource_EepromErdInfo_t expectedNvRfidElements[] = { ERD_TABLE(EXPAND_AS_RFID_ASYNC_MAP_ELEMENTS) };
static const AsyncDataSource_EepromErdInfo_t expectedNvUsageProfileElements[] = { ERD_TABLE(EXPAND_AS_USAGEPROFILE_ASYNC_MAP_ELEMENTS) };
static const AsyncDataSource_EepromErdInfo_t expectedNvFaultSnapshotElements[] = { ERD_TABLE(EXPAND_AS_FAULTSNAPSHOT_ASYNC_MAP_ELEMENTS) };
static const AsyncDataSource_EepromErdInfo_t expectedNvCycleHistoryElements[] = { ERD_TABLE(EXPAND_AS_CYCLEHISTORY_ASYNC_MAP_ELEMENTS) };

TEST_GROUP(AsyncNvMapConfigurations)
{
   const NonVolatileAsyncDataSourceConfiguration_t *configuration;
   NonVolatileAsyncDataSourceResources_t *resources;

   void setup()
   {
   }

   void TheModuleIsInitialized()
   {
      AsyncNvMapConfigurations_Init();
      configuration = AsyncNvMapConfigurations_GetAsyncDataSourceConfiguration();
      resources = AsyncNvMapConfigurations_GetAsyncDataSourceResources();
   }

   void TheMapCountShouldBe(uint8_t expected)
   {
      uassert(configuration->mapCount == expected);
   }

   void TheMetadataErdShouldBe(Erd_t actual, Erd_t expected)
   {
      uassert(actual == expected);
   }

   void NumberOfElementsInTheProtectedMapShouldBe(uint16_t expected)
   {
      uint16_t actual = ConstArrayMap_NumberOfEntries(resources[ProtectedMapIndex].map);
      uassert(actual == expected);
   }

   void NumberOfElementsInTheUnitSettingMapShouldBe(uint16_t expected)
   {
      uint16_t actual = ConstArrayMap_NumberOfEntries(resources[UnitSettingMapIndex].map);
      uassert(actual == expected);
   }

   void NumberOfElementsInTheUserSettingMapShouldBe(uint16_t expected)
   {
      uint16_t actual = ConstArrayMap_NumberOfEntries(resources[UserSettingMapIndex].map);
      uassert(actual == expected);
   }

   void NumberOfElementsInTheRfidMapShouldBe(uint16_t expected)
   {
      uint16_t actual = ConstArrayMap_NumberOfEntries(resources[RfidMapIndex].map);
      uassert(actual == expected);
   }

   void NumberOfElementsInTheUsageProfileMapShouldBe(uint16_t expected)
   {
      uint16_t actual = ConstArrayMap_NumberOfEntries(resources[UsageProfileIndex].map);
      uassert(actual == expected);
   }

   void NumberOfElementsInTheFaultSnapshotMapShouldBe(uint16_t expected)
   {
      uint16_t actual = ConstArrayMap_NumberOfEntries(resources[FaultSnapshotMapIndex].map);
      uassert(actual == expected);
   }

   void NumberOfElementsInTheCycleHistoryMapShouldBe(uint16_t expected)
   {
      uint16_t actual = ConstArrayMap_NumberOfEntries(resources[CycleHistoryMapIndex].map);
      uassert(actual == expected);
   }

   void CheckNvErdCount()
   {
      char string[200];

      if(NvErdCount != 1)
      {
         snprintf(
            string,
            sizeof(string),
            "An ERD of type Nv has been added, types can only be NvProtected, NvUserSetting, NvUnitSetting, NvRfid, NvUsageProfile, NvFaultSnapshot, or NvCycleHistory");
         FAIL(string);
      }
   }

   void CheckReadWriteBuffers()
   {
      char string[200];

      for(uint8_t i = 0; i < EepromPartitionIndex_PartitionCount; i++)
      {
         if(resources[i].readWriteBufferSize != 24)
         {
            snprintf(
               string,
               sizeof(string),
               "Need to update NonVolatileDataSourceReadWriteBuffer; the max ERD size has changed from 24 to %d",
               resources[i].readWriteBufferSize);
            FAIL(string);
         }
      }
   }
};

TEST(AsyncNvMapConfigurations, TheModuleShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(AsyncNvMapConfigurations, ThereCanOnlyBeOneNvErd)
{
   CheckNvErdCount();
}

TEST(AsyncNvMapConfigurations, TheReadWriteBufferSizeShouldNotChange)
{
   TheModuleIsInitialized();
   CheckReadWriteBuffers();
}

TEST(AsyncNvMapConfigurations, TheConfigurationShouldContainFiveMaps)
{
   Given TheModuleIsInitialized();
   TheMapCountShouldBe(ExpectedMapCount);
}

TEST(AsyncNvMapConfigurations, TheMetadataErdShouldBeCorrect)
{
   Given TheModuleIsInitialized();

   TheMetadataErdShouldBe(configuration->metadataErd, NVErd_NvMetadata);
}

TEST(AsyncNvMapConfigurations, TheAsyncMapsShouldContainTheCorrectAmountOfElementsInTheCorrectOrder)
{
   Given TheModuleIsInitialized();

   The NumberOfElementsInTheProtectedMapShouldBe(NUM_ELEMENTS(expectedNvProtectedElements));
   The NumberOfElementsInTheUserSettingMapShouldBe(NUM_ELEMENTS(expectedNvUserSettingElements));
   The NumberOfElementsInTheUnitSettingMapShouldBe(NUM_ELEMENTS(expectedNvUnitSettingElements));
   The NumberOfElementsInTheRfidMapShouldBe(NUM_ELEMENTS(expectedNvRfidElements));
   The NumberOfElementsInTheUsageProfileMapShouldBe(NUM_ELEMENTS(expectedNvUsageProfileElements));
   The NumberOfElementsInTheFaultSnapshotMapShouldBe(NUM_ELEMENTS(expectedNvFaultSnapshotElements));
   The NumberOfElementsInTheCycleHistoryMapShouldBe(NUM_ELEMENTS(expectedNvCycleHistoryElements));
}
