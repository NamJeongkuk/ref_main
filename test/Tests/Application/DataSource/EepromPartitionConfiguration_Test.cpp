/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "EepromPartitionConfiguration.h"
#include "EepromPartitionIndex.h"
#include "uassert.h"
}

enum
{
   ExpectedProtectedPartionIndex,
   ExpectedUnitSettingPartionIndex,
   ExpectedUserSettingPartionIndex,
   ExpectedRfidPartionIndex,
   ExpectedUsageProfilePartionIndex,
   ExpectedFaultSnapshotPartionIndex,
   ExpectedCycleHistoryPartionIndex,
   ExpectedNumberOfPartitions
};

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

TEST_GROUP(EepromPartitionConfiguration)
{
   const PartitionedEepromPartitionConfiguration_t *configuration;

   void setup()
   {
      configuration = EepromPartitionConfiguration_Init();
   }
};

TEST(EepromPartitionConfiguration, AssertThatNumberOfPartitionsIsSix)
{
   uassert(configuration->partitionCount == ExpectedNumberOfPartitions);
}

TEST(EepromPartitionConfiguration, AssertTheEnumOrderIsCorrect)
{
   uassert((uint8_t)EepromPartitionIndex_NvProtected == (uint8_t)ExpectedProtectedPartionIndex);
   uassert((uint8_t)EepromPartitionIndex_NvUnitSetting == (uint8_t)ExpectedUnitSettingPartionIndex);
   uassert((uint8_t)EepromPartitionIndex_NvUserSetting == (uint8_t)ExpectedUserSettingPartionIndex);
   uassert((uint8_t)EepromPartitionIndex_NvRfid == (uint8_t)ExpectedRfidPartionIndex);
   uassert((uint8_t)EepromPartitionIndex_NvUsageProfile == (uint8_t)ExpectedUsageProfilePartionIndex);
   uassert((uint8_t)EepromPartitionIndex_NvFaultSnapshot == (uint8_t)ExpectedFaultSnapshotPartionIndex);
   uassert((uint8_t)EepromPartitionIndex_NvCycleHistory == (uint8_t)ExpectedCycleHistoryPartionIndex);
}

TEST(EepromPartitionConfiguration, AssertThatPartitionIndicesAreCorrect)
{
   uassert(configuration->elements[EepromPartitionIndex_NvProtected].startAddress == configuration->elements[ExpectedProtectedPartionIndex].startAddress);
   uassert(configuration->elements[EepromPartitionIndex_NvUnitSetting].startAddress == configuration->elements[ExpectedUnitSettingPartionIndex].startAddress);
   uassert(configuration->elements[EepromPartitionIndex_NvUserSetting].startAddress == configuration->elements[ExpectedUserSettingPartionIndex].startAddress);
   uassert(configuration->elements[EepromPartitionIndex_NvRfid].startAddress == configuration->elements[ExpectedRfidPartionIndex].startAddress);
   uassert(configuration->elements[EepromPartitionIndex_NvUsageProfile].startAddress == configuration->elements[ExpectedUsageProfilePartionIndex].startAddress);
   uassert(configuration->elements[EepromPartitionIndex_NvFaultSnapshot].startAddress == configuration->elements[ExpectedFaultSnapshotPartionIndex].startAddress);
   uassert(configuration->elements[EepromPartitionIndex_NvCycleHistory].startAddress == configuration->elements[ExpectedCycleHistoryPartionIndex].startAddress);
}
