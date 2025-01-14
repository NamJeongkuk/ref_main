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
   .feelerArmPositionErd = Erd_IceMaker1_FeelerArmPosition,
   .aluminumMoldIceMakerHsmStateErd = Erd_IceMaker1_StateMachineState,
   .iceMakerFullStatusErd = Erd_IceMaker1_FullStatus
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
      DataModel_Write(dataModel, Erd_IceMaker1_FeelerArmPosition, &position);
   }

   void FeelerArmPositionShouldBe(FeelerArmPosition_t expected)
   {
      FeelerArmPosition_t actual;
      DataModel_Read(dataModel, Erd_IceMaker1_FeelerArmPosition, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_t state)
   {
      DataModel_Write(dataModel, Erd_IceMaker1_StateMachineState, &state);
   }

   void IceMakerFullStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker1_FullStatus, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketNotFullAndIceMakerIsInFreezeState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Freeze);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldSetIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketFullAndIceMakerIsInFreezeState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Freeze);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(SET);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketNotFullAndIceMakerIsInHarvestState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Harvest);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketFullAndIceMakerIsInHarvestState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Harvest);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketNotFullAndIceMakerIsInThermistorFaultState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_ThermistorFault);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenInitializedWhileFeelerArmPositionIsBucketFullAndIceMakerIsInThermistorFaultState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_ThermistorFault);

   When TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenFeelerArmPositionIsBucketNotFullWhileIceMakerFullStatusIsSet)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmPositionShouldBe(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Freeze);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(SET);

   When FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   FeelerArmPositionShouldBe(FeelerArmPosition_BucketNotFull);
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldClearIceMakerFullStatusWhenIceMakerIsChangedToHarvestStateWhileIceMakerFullStatusIsSet)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Freeze);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(SET);

   When AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Harvest);
   IceMakerFullStatusShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldSetIceMakerFullStatusWhenFeelerArmPositionIsChangedToBucketFullWhileIceMakerInFreezeState)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Freeze);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);

   When FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   IceMakerFullStatusShouldBe(SET);
}

TEST(AluminumMoldIceMakerFullStatusUpdater, ShouldSetIceMakerFullStatusWhenIceMakerIsChangedToFreezeWhileFeelerArmPositionIsChangedToBucketFull)
{
   Given FeelerArmPositionIs(FeelerArmPosition_BucketFull);
   FeelerArmPositionShouldBe(FeelerArmPosition_BucketFull);
   Given AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Harvest);
   Given TheModuleIsInitialized();
   IceMakerFullStatusShouldBe(CLEAR);

   When AluminumMoldIceMakerStateMachineStateIs(IceMakerStateMachineState_Freeze);
   IceMakerFullStatusShouldBe(SET);
}
