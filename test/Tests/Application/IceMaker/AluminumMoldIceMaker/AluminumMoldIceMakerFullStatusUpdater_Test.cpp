/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AluminumMoldIceMakerFullStatusUpdater.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
};

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define And
#define When

static const AluminumMoldIceMakerFullStatusUpdaterConfig_t config = {
   .feelerArmPositionErd = Erd_AluminumMoldIceMakerFeelerArmPosition,
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState,
   .iceMakerFullStatusErd = Erd_IceMaker0FullStatus
};

TEST_GROUP(AluminumMoldIceMakerFullStatusUpdater)
{
   AluminumMoldIceMakerFullStatusUpdater_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      AluminumMoldIceMakerFullStatusUpdater_Init(&instance, dataModel, &config);
   }

   void FeelerArmPositionIs(FeelerArmPosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerFeelerArmPosition, &position);
   }

   void AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_t state)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerHsmState, &state);
   }

   void IceMakerFullStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0FullStatus, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketNotFullAndIceMakerIsInFreezeState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Freeze);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldSetIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketFullAndIceMakerIsInFreezeState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Freeze);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(SET);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketNotFullAndIceMakerIsInHarvestState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Harvest);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketFullAndIceMakerIsInHarvestState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Harvest);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketNotFullAndIceMakerIsInThermistorFaultState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_ThermistorFault);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketFullAndIceMakerIsInThermistorFaultState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_ThermistorFault);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenFeelerArmPositionIsBucketNotFullWhileIceMakerFullStatusIsSet)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Freeze);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenIceMakerIsChangedToHarvestStateWhileIceMakerFullStatusIsSet)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Freeze);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(SET);

   When AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Harvest);
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldSetIceMakerFullStatusWhenFeelerArmPositionIsChangedToBucketFullWhileIceMakerInFreezeState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Freeze);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   IceMakerFullStatusShouldBe(SET);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldSetIceMakerFullStatusWhenIceMakerIsChangedToFreezeWhileFeelerArmPositionIsChangedToBucketFull)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Harvest);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);

   When AluminumMoldIceMakerHsmStateIs(AluminumMoldIceMakerHsmState_Freeze);
   IceMakerFullStatusShouldBe(SET);
}
