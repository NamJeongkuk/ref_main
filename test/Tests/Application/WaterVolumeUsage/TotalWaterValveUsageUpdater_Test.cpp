/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "TotalWaterValveUsageUpdater.h"
#include "RfidTypes.h"
}

#include "CppUTest/TestHarness.h"
#include "DataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomeWaterVolumeUsageInOuncesX100_1 = 7384,
   SomeWaterVolumeUsageInOuncesX100_2 = 8360928,
   SomeWaterVolumeUsageInOuncesX100_3 = 20381021
};

enum
{
   Erd_LastWaterVolumeUsageInOuncesX100_1,
   Erd_LastWaterVolumeUsageInOuncesX100_2,
   Erd_LastWaterVolumeUsageInOuncesX100_3,
   Erd_CumulativeWaterVolumeUsageInOuncesX100
};

static const DataModel_TestDoubleConfigurationEntry_t erdTable[] = {
   { Erd_LastWaterVolumeUsageInOuncesX100_1, sizeof(VolumeInOuncesX100_t) },
   { Erd_LastWaterVolumeUsageInOuncesX100_2, sizeof(VolumeInOuncesX100_t) },
   { Erd_LastWaterVolumeUsageInOuncesX100_3, sizeof(VolumeInOuncesX100_t) },
   { Erd_CumulativeWaterVolumeUsageInOuncesX100, sizeof(VolumeInOuncesX100_t) }
};

static const Erd_t waterVolumeUsageInOuncesX100Erds[] = {
   Erd_LastWaterVolumeUsageInOuncesX100_1,
   Erd_LastWaterVolumeUsageInOuncesX100_2,
   Erd_LastWaterVolumeUsageInOuncesX100_3
};

static const ErdList_t waterVolumeUsageInOuncesX100ErdList = {
   .erds = waterVolumeUsageInOuncesX100Erds,
   .numberOfErds = NUM_ELEMENTS(waterVolumeUsageInOuncesX100Erds)
};

static const TotalWaterValveUsageUpdaterConfig_t totalWaterValveUsageUpdaterConfig = {
   .waterVolumeUsageInOuncesX100ErdList = waterVolumeUsageInOuncesX100ErdList,
   .cumulativeWaterVolumeUsageInOuncesX100Erd = Erd_CumulativeWaterVolumeUsageInOuncesX100
};

TEST_GROUP(TotalWaterValveUsageUpdater)
{
   TotalWaterValveUsageUpdater_t instance;

   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      DataModel_TestDouble_Init(
         &dataModelTestDouble,
         erdTable,
         NUM_ELEMENTS(erdTable));
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitializedForCumulativeWaterVolumeUsageInOuncesX100()
   {
      TotalWaterValveUsageUpdater_Init(
         &instance,
         dataModel,
         &totalWaterValveUsageUpdaterConfig);
   }

   void GivenLastWaterVolumeUsageInOuncesX100Is(Erd_t erd, VolumeInOuncesX100_t waterVolumeUsageInOuncesX100)
   {
      DataModel_Write(
         dataModel,
         erd,
         &waterVolumeUsageInOuncesX100);
   }

   void GivenAllLastWaterVolumeUsageInOuncesX100ErdsAre(VolumeInOuncesX100_t waterVolumeUsageInOuncesX100)
   {
      GivenLastWaterVolumeUsageInOuncesX100Is(Erd_LastWaterVolumeUsageInOuncesX100_1, waterVolumeUsageInOuncesX100);
      GivenLastWaterVolumeUsageInOuncesX100Is(Erd_LastWaterVolumeUsageInOuncesX100_2, waterVolumeUsageInOuncesX100);
      GivenLastWaterVolumeUsageInOuncesX100Is(Erd_LastWaterVolumeUsageInOuncesX100_3, waterVolumeUsageInOuncesX100);
   }

   void WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_t erd, VolumeInOuncesX100_t waterVolumeUsageInOuncesX100)
   {
      GivenLastWaterVolumeUsageInOuncesX100Is(erd, waterVolumeUsageInOuncesX100);
   }

   void CumulativeWaterVolumeUsageInOuncesX100ShouldBe(VolumeInOuncesX100_t expected)
   {
      VolumeInOuncesX100_t actual;
      DataModel_Read(
         dataModel,
         Erd_CumulativeWaterVolumeUsageInOuncesX100,
         &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(TotalWaterValveUsageUpdater, ShouldWriteTheSumToCumulativeWaterFilterInOuncesX100ErdWhenAnyOfLastWaterVolumeUsageInOuncesX100ErdsChangeAfterInit)
{
   GivenAllLastWaterVolumeUsageInOuncesX100ErdsAre(SomeWaterVolumeUsageInOuncesX100_1);
   GivenTheModuleIsInitializedForCumulativeWaterVolumeUsageInOuncesX100();

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_1, SomeWaterVolumeUsageInOuncesX100_3);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(1 * SomeWaterVolumeUsageInOuncesX100_3);

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_2, SomeWaterVolumeUsageInOuncesX100_3);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(2 * SomeWaterVolumeUsageInOuncesX100_3);

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_3, SomeWaterVolumeUsageInOuncesX100_3);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(3 * SomeWaterVolumeUsageInOuncesX100_3);
}

TEST(TotalWaterValveUsageUpdater, ShouldNotChangeCumulativeWaterVolumeUsageInOuncesX100WhenAnyOfLastWaterVolumeUsageInOuncesX100ErdsChangeToZero)
{
   GivenAllLastWaterVolumeUsageInOuncesX100ErdsAre(SomeWaterVolumeUsageInOuncesX100_1);
   GivenTheModuleIsInitializedForCumulativeWaterVolumeUsageInOuncesX100();

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_1, SomeWaterVolumeUsageInOuncesX100_2);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(1 * SomeWaterVolumeUsageInOuncesX100_2);

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_2, SomeWaterVolumeUsageInOuncesX100_2);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(2 * SomeWaterVolumeUsageInOuncesX100_2);

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_3, SomeWaterVolumeUsageInOuncesX100_2);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(3 * SomeWaterVolumeUsageInOuncesX100_2);

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_1, 0);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(3 * SomeWaterVolumeUsageInOuncesX100_2);

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_1, 0);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(3 * SomeWaterVolumeUsageInOuncesX100_2);

   WhenLastWaterVolumeUsageInOuncesX100ChangesTo(Erd_LastWaterVolumeUsageInOuncesX100_1, 0);
   CumulativeWaterVolumeUsageInOuncesX100ShouldBe(3 * SomeWaterVolumeUsageInOuncesX100_2);
}
