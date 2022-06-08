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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define The

static const DefrostData_t defrostData = {
   .freezerDoorIncrementFactorInSecondsPerSecond = 348,
   .freshFoodDoorIncrementFactorInSecondsPerSecond = 87,
   .minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes = 6 * 60,
   .maxTimeBetweenDefrostsInMinutes = 32 * 60,
   .prechillFreezerSetpointInDegFx100 = -600,
   .prechillFreshFoodSetpointInDegFx100 = 4600,
   .prechillConvertibleCompartmentSetpointInDegFx100 = -600,
   .prechillFreezerEvapExitTemperatureInDegFx100 = -3000,
   .prechillConvertibleCompartmentEvapExitTemperatureInDegFx100 = -3000,
   .maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes = 10,
   .maxPrechillTimeForFreshFoodOnlyDefrostInMinutes = 20,
   .defrostDoorHoldoffTimeForFreshFoodAndFreezerInMinutes = 60,
   .defrostDoorHoldoffTimeForFreshFoodOnlyInMinutes = 50,
   .defrostMaxHoldoffTimeInMinutes = 60,
   .maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds = 60,
   .freshFoodFanDefrostFreshFoodEvapExitTemperatureInDegFx100 = 3600,
   .freshFoodFanDefrostFreshFoodFanMaxOnTimeInMinutes = 10,
   .convertibleCompartmentFanDefrostConvertibleCompartmentEvapExitTemperatureInDegFx100 = 3200,
   .convertibleCompartmentFanDefrostConvertibleCompartmentFanMaxOnTimeInMinutes = 10,
   .freezerDefrostHeaterMaxOnTimeInMinutes = 60,
   .freezerInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 30,
   .freezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
   .freshFoodInvalidThermistorDefrostHeaterMaxOnTimeInMinutes = 20,
   .freshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutes = 60,
   .convertibleCompartmentAsFreshFoodAbnormalDefrostHeaterMaxOnTimeInMinutes = 21,
   .convertibleCompartmentAsFreezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 35,
   .defrostDwellTimeInMinutes = 7,
   .freshFoodAndFreezerPostDwellFreezerExitTemperatureInDegFx100 = -1000,
   .dwellThreeWayValvePosition = ValvePosition_A,
   .postDwellThreeWayValvePositionForFreshFoodAndFreezer = ValvePosition_A,
   .freshFoodPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .freezerPostDefrostPullDownExitTemperatureInDegFx100 = 4000,
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 2,
   .numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost = 1,
   .freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes = 10,
   .freshFoodOnlyPostDwellExitTimeInMinutes = 10,
   .defrostPeriodicTimeoutInSeconds = 1,
   .threeWayValvePositionToExitIdle = ValvePosition_B,
   .threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B,
   .threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B,
   .threeWayValvePositionToCountAsPrechillTime = ValvePosition_B,
   .threeWayValveTimePriorToPrechillCountsAsPrechillTime = true
};

static const FreshFoodOnlyDefrostArbitratorConfiguration_t config = {
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .defrostIsFreshFoodOnlyErd = Erd_DefrostIsFreshFoodOnly
};

TEST_GROUP(FreshFoodOnlyDefrostArbitrator)
{
   FreshFoodOnlyDefrostArbitrator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void Initialization()
   {
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
