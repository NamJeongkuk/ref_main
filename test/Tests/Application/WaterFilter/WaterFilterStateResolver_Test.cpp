/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "WaterFilterStateResolver.h"
#include "WaterFilterState.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static const WaterFilterStateResolverConfig_t config = {
   .enableDemoModeStatusErd = Erd_EnableDemoModeStatus,
   .leakDetectedErd = Erd_LeakDetected,
   .filterErrorErd = Erd_FilterError,
   .bypassPlugInstalledErd = Erd_BypassPlugInstalled,
   .waterFilterLifeStatusErd = Erd_WaterFilterLifeStatus,
   .resolvedFilterStateErd = Erd_WaterFilterState
};

TEST_GROUP(WaterFilterStateResolver)
{
   WaterFilterStateResolver_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      WaterFilterStateResolver_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenDemoModeIs(uint8_t status)
   {
      DataModel_Write(dataModel, Erd_EnableDemoModeStatus, &status);
   }

   void GivenLeakDetectedIs(uint8_t status)
   {
      DataModel_Write(dataModel, Erd_LeakDetected, &status);
   }

   void GivenFilterErrorIs(uint8_t status)
   {
      DataModel_Write(dataModel, Erd_FilterError, &status);
   }

   void GivenBypassPlugInstalledIs(uint8_t status)
   {
      DataModel_Write(dataModel, Erd_BypassPlugInstalled, &status);
   }

   void GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_t status)
   {
      DataModel_Write(dataModel, Erd_WaterFilterLifeStatus, &status);
   }

   void WhenDemoModeIs(uint8_t status)
   {
      GivenDemoModeIs(status);
   }

   void WhenLeakDetectedIs(uint8_t status)
   {
      GivenLeakDetectedIs(status);
   }

   void WhenFilterErrorIs(uint8_t status)
   {
      GivenFilterErrorIs(status);
   }

   void WhenBypassPlugInstalledIs(uint8_t status)
   {
      GivenBypassPlugInstalledIs(status);
   }

   void WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_t status)
   {
      GivenTheWaterFilterLifeStatusIs(status);
   }

   void WaterFilterStateShouldBe(WaterFilterState_t expected)
   {
      WaterFilterState_t actual;
      DataModel_Read(dataModel, Erd_WaterFilterState, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToGoodWhenDemoModeIsEnabledAndIsTheHighestPriorityStateUponInit)
{
   GivenDemoModeIs(ENABLED);
   GivenLeakDetectedIs(SET);
   GivenFilterErrorIs(SET);
   GivenBypassPlugInstalledIs(SET);
   GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToLeakWhenALeakIsDetectedAndIsTheHighestPriorityStateUponInit)
{
   GivenLeakDetectedIs(SET);
   GivenFilterErrorIs(SET);
   GivenBypassPlugInstalledIs(SET);
   GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Leak);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToErrorWhenAFilterErrorIsDetectedAndIsTheHighestPriorityStateUponInit)
{
   GivenFilterErrorIs(SET);
   GivenBypassPlugInstalledIs(SET);
   GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Error);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToBypassWhenABypassPlugIsDetectedAndIsTheHighestPriorityStateUponInit)
{
   GivenBypassPlugInstalledIs(SET);
   GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Bypass);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToExpiredWhenTheFilterLifeStatusIsExpiredAndIsTheHighestPriorityStateUponInit)
{
   GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Expired);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToReplaceWhenTheFilterLifeStatusIsReplaceAndIsTheHighestPriorityStateUponInit)
{
   GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Replace);
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Replace);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToGoodWhenNoLeakOrErrorOrBypassPlugIsDetectedAndTheFilterLifeStatusIsNotExpiredOrReplaceUponInit)
{
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToTheNewlyChangedStateWhenTheStateIsAHigherPriority)
{
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Replace);
   WaterFilterStateShouldBe(WaterFilterState_Replace);

   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   WaterFilterStateShouldBe(WaterFilterState_Expired);

   WhenBypassPlugInstalledIs(SET);
   WaterFilterStateShouldBe(WaterFilterState_Bypass);

   WhenFilterErrorIs(SET);
   WaterFilterStateShouldBe(WaterFilterState_Error);

   WhenLeakDetectedIs(SET);
   WaterFilterStateShouldBe(WaterFilterState_Leak);

   WhenDemoModeIs(SET);
   WaterFilterStateShouldBe(WaterFilterState_Good);
}

TEST(WaterFilterStateResolver, ShouldResolveTheWaterFilterStateAsGoodWhenDemoModeIsEnabledAndLowerPriorityStatesGetSet)
{
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenDemoModeIs(ENABLED);
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenLeakDetectedIs(SET);
   WhenFilterErrorIs(SET);
   WhenBypassPlugInstalledIs(SET);
   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   WaterFilterStateShouldBe(WaterFilterState_Good);
}

TEST(WaterFilterStateResolver, ShouldResolveTheWaterFilterStateAsLeakWhenLeakDetectedIsSetAndLowerPriorityStatesGetSet)
{
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenLeakDetectedIs(SET);
   WaterFilterStateShouldBe(WaterFilterState_Leak);

   WhenFilterErrorIs(SET);
   WhenBypassPlugInstalledIs(SET);
   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   WaterFilterStateShouldBe(WaterFilterState_Leak);
}

TEST(WaterFilterStateResolver, ShouldResolveTheWaterFilterStateAsErrorWhenErrorDetectedIsSetAndLowerPriorityStatesGetSet)
{
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenFilterErrorIs(SET);
   WaterFilterStateShouldBe(WaterFilterState_Error);

   WhenBypassPlugInstalledIs(SET);
   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   WaterFilterStateShouldBe(WaterFilterState_Error);
}

TEST(WaterFilterStateResolver, ShouldResolveTheWaterFilterStateAsBypassWhenBypassPlugInstalledIsSetAndLowerPriorityStatesGetSet)
{
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenBypassPlugInstalledIs(SET);
   WaterFilterStateShouldBe(WaterFilterState_Bypass);

   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   WaterFilterStateShouldBe(WaterFilterState_Bypass);
}

TEST(WaterFilterStateResolver, ShouldSetWaterFilterStateToTheNextHighestPriorityStateWhenAHigherPriorityStateIsCleared)
{
   GivenDemoModeIs(ENABLED);
   GivenLeakDetectedIs(SET);
   GivenFilterErrorIs(SET);
   GivenBypassPlugInstalledIs(SET);
   GivenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Expired);
   GivenInitialization();
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenDemoModeIs(CLEAR);
   WaterFilterStateShouldBe(WaterFilterState_Leak);

   WhenLeakDetectedIs(CLEAR);
   WaterFilterStateShouldBe(WaterFilterState_Error);

   WhenFilterErrorIs(CLEAR);
   WaterFilterStateShouldBe(WaterFilterState_Bypass);

   WhenBypassPlugInstalledIs(CLEAR);
   WaterFilterStateShouldBe(WaterFilterState_Expired);

   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   WaterFilterStateShouldBe(WaterFilterState_Good);

   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_Replace);
   WaterFilterStateShouldBe(WaterFilterState_Replace);

   WhenTheWaterFilterLifeStatusIs(WaterFilterLifeStatus_NoOrderNecessary);
   WaterFilterStateShouldBe(WaterFilterState_Good);
}
