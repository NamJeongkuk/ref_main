/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FreshFoodOnlyDefrostArbitrator.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define The

static const FreshFoodOnlyDefrostArbitratorConfiguration_t config = {
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .defrostIsFreshFoodOnlyErd = Erd_DefrostIsFreshFoodOnly,
   .defrostParameterSelectorReadyErd = Erd_DefrostParameterSelectorReady
};

TEST_GROUP(FreshFoodOnlyDefrostArbitrator)
{
   FreshFoodOnlyDefrostArbitrator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void Initialization()
   {
      DataModel_Write(dataModel, Erd_DefrostParameterSelectorReady, set);
      FreshFoodOnlyDefrostArbitrator_Init(
         &instance,
         dataModel,
         &config);
   }

   void NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(uint8_t number)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost, &number);
   }

   void FreezerDefrostWasAbnormalErdChangesTo(bool abnormal)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &abnormal);
   }

   void DefrostIsFreshFoodOnlyErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DefrostIsFreshFoodOnly, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(FreshFoodOnlyDefrostArbitrator, ShouldSetFreshFoodOnlyErdToTrueIfNumberOfFreshFoodDefrostsBeforeAFreezerDefrostIsLessThanParametricLimitAndDefrostIsNotAbnormal)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(0);
   And FreezerDefrostWasAbnormalErdChangesTo(false);
   And Initialization();

   DefrostIsFreshFoodOnlyErdShouldBe(true);
}

TEST(FreshFoodOnlyDefrostArbitrator, ShouldSetFreshFoodOnlyErdToFalseIfNumberOfFreshFoodDefrostsBeforeAFreezerDefrostIsGreaterThanOrEqualToParametricLimitAndDefrostIsNotAbnormal)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(3);
   And FreezerDefrostWasAbnormalErdChangesTo(false);
   And Initialization();

   DefrostIsFreshFoodOnlyErdShouldBe(false);
}

TEST(FreshFoodOnlyDefrostArbitrator, ShouldSetFreshFoodOnlyErdToFalseIfNumberOfFreshFoodDefrostsBeforeAFreezerDefrostIsGreaterThanParametricAbnormalLimitAndDefrostIsAbnormal)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(2);
   And FreezerDefrostWasAbnormalErdChangesTo(true);
   And Initialization();

   DefrostIsFreshFoodOnlyErdShouldBe(false);
}

TEST(FreshFoodOnlyDefrostArbitrator, ShouldSetFreshFoodOnlyErdToTrueIfNumberOfFreshFoodDefrostsBeforeAFreezerDefrostUpdatesToParametricLimitAndDefrostIsNotAbnormal)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(0);
   And FreezerDefrostWasAbnormalErdChangesTo(false);
   And Initialization();

   When NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(1);
   DefrostIsFreshFoodOnlyErdShouldBe(true);

   When NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(2);
   DefrostIsFreshFoodOnlyErdShouldBe(false);

   When NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(3);
   DefrostIsFreshFoodOnlyErdShouldBe(false);
}

TEST(FreshFoodOnlyDefrostArbitrator, ShouldSetFreshFoodOnlyErdToTrueIfNumberOfFreshFoodDefrostsBeforeAFreezerDefrostUpdatesToMoreThanParametricAbnormalLimitAndDefrostIsAbnormal)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(0);
   And FreezerDefrostWasAbnormalErdChangesTo(true);
   And Initialization();

   When NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(1);
   DefrostIsFreshFoodOnlyErdShouldBe(true);

   When NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(2);
   DefrostIsFreshFoodOnlyErdShouldBe(false);
}

TEST(FreshFoodOnlyDefrostArbitrator, ShouldSetFreshFoodOnlyErdToTrueIfNumberOfFreshFoodDefrostsBeforeAFreezerDefrostReturnsToLessThanTheParametricLimitAndDefrostIsNotAbnormal)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(3);
   And FreezerDefrostWasAbnormalErdChangesTo(false);
   And Initialization();

   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(0);
   DefrostIsFreshFoodOnlyErdShouldBe(true);
}

TEST(FreshFoodOnlyDefrostArbitrator, ShouldSetFreshFoodOnlyErdToTrueIfNumberOfFreshFoodDefrostsBeforeAFreezerDefrostReturnsToLessThanParametricAbnormalLimitAndDefrostIsAbnormal)
{
   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(2);
   And FreezerDefrostWasAbnormalErdChangesTo(true);
   And Initialization();

   Given NumberOfFreshFoodDefrostsBeforeAFreezerDefrostErdChangesTo(0);
   DefrostIsFreshFoodOnlyErdShouldBe(true);
}
