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
   .freshFoodDefrostCountErd = Erd_FreshFoodDefrostCount,
   .numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd = Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeStatus,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .clearedDefrostEepromStartupErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .freezerFilteredTemperatureTooWarmAtPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmAtPowerUp
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
      ClearedDefrostEepromStartupIs(CLEAR);
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

   void NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(
         dataModel,
         Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void EnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeStatus, &state);
   }

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
   }

   void FreshFoodDefrostCountIs(uint8_t count)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostCount, &count);
   }

   void FreshFoodDefrostCountShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodDefrostCount, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ClearedDefrostEepromStartupIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Eeprom_ClearedDefrostEepromStartup, &state);
   }

   void FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmAtPowerUp, &state);
   }
};

TEST(NextDefrostTypeArbiter, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFreshFoodOnInitWhenNumberOfFreshFoodDefrostsBeforeAFreezerDefrostIsGreaterThanZero)
{
   Given NextDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
}

TEST(NextDefrostTypeArbiter, ShouldSetFreshFoodDefrostCountToZeroOnInit)
{
   Given FreshFoodDefrostCountIs(5);
   Given TheModuleIsInitialized();

   FreshFoodDefrostCountShouldBe(0);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalCountWhenFreezerDefrostIsAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalCountWhenItHasAConvertibleCompartmentAndConvertibleCompartmentDefrostIsAbnormalAndConvertibleCompartmentIsActingAsFreezerOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledAndFreezerDefrostWasAbnormalIsSetOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentDefrostIsNotAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndDoesNotHaveConvertibleCompartmentOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentTypeIsFreshFoodOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFullWhenFreshFoodDefrostCountReachesNumberOfFreshFoodDefrostsWhileCurrentDefrostTypeIsFreshFood)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

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
   Given FreshFoodDefrostCountIs(10);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
   And FreshFoodDefrostCountShouldBe(0);
}

TEST(NextDefrostTypeArbiter, ShouldNotIncreaseFreshFoodDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFreshFood)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   Given TheModuleIsInitialized();
   FreshFoodDefrostCountShouldBe(0);

   When DefrostingIs(false);
   FreshFoodDefrostCountShouldBe(0);
}

TEST(NextDefrostTypeArbiter, ShouldNotResetFreshFoodDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   Given FreshFoodDefrostCountIs(5);

   When DefrostingIs(false);
   FreshFoodDefrostCountShouldBe(5);
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
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When EnhancedSabbathModeIs(true);
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalFreezeDefrostCountWhenFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When FreezerDefrostWasAbnormalIs(true);
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentDefrostWasAbnormalIsTrueAndConvertibleCompartmentStateChangesToFreezer)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentStateIsFreezerAndConvertibleCompartmentDefrostWasAbnormalChangesToTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When ConvertibleCompartmentDefrostWasAbnormalIs(true);
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalWhenEnhancedSabbathModeIsDisabledWhileFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When EnhancedSabbathModeIs(false);
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfFreshFoodDefrostsToNormalWhenFreezerDefrostWasAbnormalIsFalseWhileEnhancedSabbathModeIsDisabled)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeAbnormalFreezerDefrost);

   When FreezerDefrostWasAbnormalIs(false);
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->prechillPrepData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNextDefrostTypeToFullWhenDefrostEepromClearedFlagIsSetOnInit)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given ClearedDefrostEepromStartupIs(SET);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNextDefrostTypeToFullWhenFreezerFilteredTemperatureTooWarmAtPowerUpFlagIsSetOnInit)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given ClearedDefrostEepromStartupIs(CLEAR);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(SET);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFreshFoodAfterTheTypeIsFullWithDefrostEepromClearedOnInit)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given ClearedDefrostEepromStartupIs(SET);
   Given TheModuleIsInitialized();
   Given CurrentDefrostTypeIs(DefrostType_Full);
   NextDefrostTypeShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_FreshFood);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFreshFoodAfterTheTypeIsFullWithTooWarmAtPowerUpOnInit)
{
   Given NextDefrostTypeIs(DefrostType_FreshFood);
   Given ClearedDefrostEepromStartupIs(CLEAR);
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
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeStatus, &state);
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

TEST(NextDefrostTypeArbiterWithNumberOfFreshFoodDefrostCountIsZero, ShouldNotChangeDefrostTypeToFreshFoodAfterDefrostingStateChangesToTrue)
{
   Given TheModuleIsInitialized();
   Given CurrentDefrostTypeIs(DefrostType_Full);
   NextDefrostTypeShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_Full);
}
