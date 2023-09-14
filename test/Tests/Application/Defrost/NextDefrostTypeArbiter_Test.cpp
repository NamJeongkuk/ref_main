/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "NextDefrostTypeArbiter.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"

#define Given
#define After
#define And
#define When

static const NextDefrostTypeArbiterConfig_t config = {
   .nextDefrostTypeErd = Erd_NextDefrostType,
   .defrostingErd = Erd_Defrosting,
   .numberOfFreshFoodDefrostsErd = Erd_NumberOfFreshFoodDefrosts,
   .numberOfFreshFoodDefrostsBeforeAFullDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFullDefrost,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .freezerFilteredTemperatureTooWarmAtPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUp
};

TEST_GROUP(NextDefrostTypeArbiter)
{
   NextDefrostTypeArbiter_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;
   const DefrostData_t *defrostData;
   const EnhancedSabbathData_t *enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble, TddPersonality_DevelopmentDualEvaporator);
      dataModel = referDataModelTestDouble.dataModel;

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
      enhancedSabbathData = PersonalityParametricData_Get(dataModel)->enhancedSabbathData;
   }

   void TheModuleIsInitialized()
   {
      NextDefrostTypeArbiter_Init(&instance, dataModel, &config);
   }

   void NextDefrostTypeIs(DefrostType_t nextDefrostType)
   {
      DataModel_Write(dataModel, Erd_NextDefrostType, &nextDefrostType);
   }

   void NextDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostType, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CurrentDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FreezerDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }

   void HasConvertibleCompartmentIs(bool state)
   {
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, &state);
   }

   void ConvertibleCompartmentDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, &state);
   }

   void ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_t state)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentState, &state);
   }

   void DefrostingStateIsChangedFromFalseToTrueNTimes(uint8_t numberOfDefrosting)
   {
      for(uint8_t i = 0; i < numberOfDefrosting; i++)
      {
         DataModel_Write(dataModel, Erd_Defrosting, clear);
         DataModel_Write(dataModel, Erd_Defrosting, set);
      }
   }

   void DefrostingIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Defrosting, &state);
   }

   void NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(
         dataModel,
         Erd_NumberOfFreshFoodDefrostsBeforeAFullDefrost,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void EnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &state);
   }

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
   }

   void NumberOfFreshFoodDefrostsIs(uint8_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfFreshFoodDefrosts, &count);
   }

   void NumberOfFreshFoodDefrostsShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_NumberOfFreshFoodDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUp, &state);
   }
};

TEST(NextDefrostTypeArbiter, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(NextDefrostTypeArbiter, ShouldNotSetNextDefrostTypeOnInitWhenNumberOfFreshFoodDefrostsBeforeAFreezerDefrostIsGreaterThanZeroAndFreezerCompartmentIsNotTooWarmAndEepromWasNotCleared)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
}

TEST(NextDefrostTypeArbiter, ShouldNotResetFreshFoodDefrostCountOnInit)
{
   Given NumberOfFreshFoodDefrostsIs(5);
   Given TheModuleIsInitialized();

   NumberOfFreshFoodDefrostsShouldBe(5);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalCountWhenFreezerDefrostIsAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalCountWhenItHasAConvertibleCompartmentAndConvertibleCompartmentDefrostIsAbnormalAndConvertibleCompartmentIsActingAsFreezerOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledAndFreezerDefrostWasAbnormalIsSetOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentDefrostIsNotAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndDoesNotHaveConvertibleCompartmentOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentTypeIsFreshFoodOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFullWhenFreshFoodDefrostCountReachesNumberOfFreshFoodDefrostsWhileCurrentDefrostTypeIsFreshFood)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost - 1);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFreshFoodAndResetFreshFoodDefrostCountToZeroWhenDefrostStateIsChangedFromFalseToTrueWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   Given NextDefrostTypeIs(DefrostType_Full);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given NumberOfFreshFoodDefrostsIs(10);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   And NumberOfFreshFoodDefrostsShouldBe(0);
}

TEST(NextDefrostTypeArbiter, ShouldNotIncreaseFreshFoodDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFreshFood)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsShouldBe(0);

   When DefrostingIs(false);
   NumberOfFreshFoodDefrostsShouldBe(0);
}

TEST(NextDefrostTypeArbiter, ShouldNotResetFreshFoodDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   Given NumberOfFreshFoodDefrostsIs(5);

   When DefrostingIs(false);
   NumberOfFreshFoodDefrostsShouldBe(5);
}

TEST(NextDefrostTypeArbiter, ShouldNotChangeNextDefrostTypeToFreshFoodWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   Given NextDefrostTypeIs(DefrostType_Full);

   When DefrostingIs(false);
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabled)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When EnhancedSabbathModeIs(true);
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalFreezeDefrostCountWhenFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When FreezerDefrostWasAbnormalIs(true);
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentDefrostWasAbnormalIsTrueAndConvertibleCompartmentStateChangesToFreezer)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentStateIsFreezerAndConvertibleCompartmentDefrostWasAbnormalChangesToTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When ConvertibleCompartmentDefrostWasAbnormalIs(true);
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalWhenEnhancedSabbathModeIsDisabledWhileFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When EnhancedSabbathModeIs(false);
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalWhenFreezerDefrostWasAbnormalIsFalseWhileEnhancedSabbathModeIsDisabled)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);

   When FreezerDefrostWasAbnormalIs(false);
   NumberOfFreshFoodDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateCurrentAndNextDefrostTypeToFullWhenFreezerFilteredTemperatureTooWarmAtPowerUpFlagIsSetOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(SET);
   Given TheModuleIsInitialized();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldNotUpdateCurrentAndNextDefrostTypeToFreshFoodWhenFreezerFilteredTemperatureTooWarmAtPowerUpFlagAndEepromClearedFlagAreClearOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given NextDefrostTypeIs(DefrostType_Full);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(CLEAR);
   Given TheModuleIsInitialized();
   CurrentDefrostTypeShouldBe(DefrostType_FreshFood);
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFreshFoodAfterTheTypeIsFullWithTooWarmAtPowerUpOnInit)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(SET);
   Given TheModuleIsInitialized();
   Given CurrentDefrostTypeIs(DefrostType_Full);
   NextDefrostTypeShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
}

TEST_GROUP(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero)
{
   NextDefrostTypeArbiter_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;
   const DefrostData_t *defrostData;
   const EnhancedSabbathData_t *enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = referDataModelTestDouble.dataModel;

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
      enhancedSabbathData = PersonalityParametricData_Get(dataModel)->enhancedSabbathData;
   }

   void TheModuleIsInitialized()
   {
      NextDefrostTypeArbiter_Init(&instance, dataModel, &config);
   }

   void NextDefrostTypeIs(DefrostType_t nextDefrostType)
   {
      DataModel_Write(dataModel, Erd_NextDefrostType, &nextDefrostType);
   }

   void NextDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostType, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CurrentDefrostTypeShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_CurrentDefrostType, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DefrostingStateIsChangedFromFalseToTrueNTimes(uint8_t numberOfDefrosting)
   {
      for(uint8_t i = 0; i < numberOfDefrosting; i++)
      {
         DataModel_Write(dataModel, Erd_Defrosting, clear);
         DataModel_Write(dataModel, Erd_Defrosting, set);
      }
   }

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
   }

   void EnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &state);
   }

   void FreezerDefrostWasAbnormalIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, &state);
   }
};

TEST(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsEnabledAndFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsDisabledAndFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsEnabledAndFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(true);
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsDisabledAndFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero, ShouldSetCurrentAndNextDefrostTypeToFullWhenNumberOfFreshFoodDefrostCountIsZeroOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero, ShouldNotChangeDefrostTypeToFreshFoodAfterDefrostingStateChangesToTrue)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_Full);
}
