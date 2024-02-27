/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "NextDefrostTypeOverrideArbiter.h"
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

static const NextDefrostTypeOverrideArbiterConfig_t config = {
   .nextDefrostTypeOverrideErd = Erd_NextDefrostTypeOverride,
   .defrostingErd = Erd_Defrosting,
   .numberOfSecondaryOnlyDefrostsErd = Erd_NumberOfSecondaryOnlyDefrosts,
   .numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd = Erd_NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .freezerDefrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
   .convertibleCompartmentDefrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .freezerFilteredTemperatureTooWarmAtPowerUpErd = Erd_FreezerFilteredTemperatureTooWarmOnPowerUp
};

TEST_GROUP(NextDefrostTypeOverrideArbiter)
{
   NextDefrostTypeOverrideArbiter_t instance;
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
      NextDefrostTypeOverrideArbiter_Init(&instance, dataModel, &config);
   }

   void NextDefrostTypeOverrideIs(DefrostType_t nextDefrostTypeOverride)
   {
      DataModel_Write(dataModel, Erd_NextDefrostTypeOverride, &nextDefrostTypeOverride);
   }

   void NextDefrostTypeOverrideShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostTypeOverride, &actual);
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

   void NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(
         dataModel,
         Erd_NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost,
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

   void NumberOfSecondaryOnlyDefrostsIs(uint8_t count)
   {
      DataModel_Write(dataModel, Erd_NumberOfSecondaryOnlyDefrosts, &count);
   }

   void NumberOfSecondaryOnlyDefrostsShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_NumberOfSecondaryOnlyDefrosts, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerFilteredTemperatureTooWarmOnPowerUp, &state);
   }
};

TEST(NextDefrostTypeOverrideArbiter, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(NextDefrostTypeOverrideArbiter, ShouldNotSetNextDefrostTypeOverrideOnInitWhenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIsGreaterThanZeroAndFreezerCompartmentIsNotTooWarmAndEepromWasNotCleared)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_SecondaryOnly);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldNotResetSecondaryOnlyDefrostCountOnInit)
{
   Given NumberOfSecondaryOnlyDefrostsIs(5);
   Given TheModuleIsInitialized();

   NumberOfSecondaryOnlyDefrostsShouldBe(5);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalCountWhenFreezerDefrostIsAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalCountWhenItHasAConvertibleCompartmentAndConvertibleCompartmentDefrostIsAbnormalAndConvertibleCompartmentIsActingAsFreezerOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledAndFreezerDefrostWasAbnormalIsSetOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentDefrostIsNotAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndDoesNotHaveConvertibleCompartmentOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentTypeIsFreshFoodOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldSetNextDefrostTypeOverrideToFullWhenSecondaryOnlyDefrostCountReachesNumberOfSecondaryOnlyDefrostsWhileCurrentDefrostTypeIsSecondaryOnly)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_SecondaryOnly);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost - 1);
   NextDefrostTypeOverrideShouldBe(DefrostType_SecondaryOnly);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldSetNextDefrostTypeOverrideToSecondaryOnlyAndResetSecondaryOnlyDefrostCountToZeroWhenDefrostStateIsChangedFromFalseToTrueWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   Given NextDefrostTypeOverrideIs(DefrostType_Full);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given NumberOfSecondaryOnlyDefrostsIs(10);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeOverrideShouldBe(DefrostType_SecondaryOnly);
   And NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldNotIncreaseSecondaryOnlyDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsSecondaryOnly)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsShouldBe(0);

   When DefrostingIs(false);
   NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldNotResetSecondaryOnlyDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   Given NumberOfSecondaryOnlyDefrostsIs(5);

   When DefrostingIs(false);
   NumberOfSecondaryOnlyDefrostsShouldBe(5);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldNotChangeNextDefrostTypeOverrideToSecondaryOnlyWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given DefrostingIs(true);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   Given NextDefrostTypeOverrideIs(DefrostType_Full);

   When DefrostingIs(false);
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabled)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When EnhancedSabbathModeIs(true);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalFreezeDefrostCountWhenFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When FreezerDefrostWasAbnormalIs(true);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentDefrostWasAbnormalIsTrueAndConvertibleCompartmentStateChangesToFreezer)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentStateIsFreezerAndConvertibleCompartmentDefrostWasAbnormalChangesToTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When ConvertibleCompartmentDefrostWasAbnormalIs(true);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalWhenEnhancedSabbathModeIsDisabledWhileFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When EnhancedSabbathModeIs(false);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalWhenFreezerDefrostWasAbnormalIsFalseWhileEnhancedSabbathModeIsDisabled)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);

   When FreezerDefrostWasAbnormalIs(false);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldUpdateCurrentAndNextDefrostTypeOverrideToFullWhenFreezerFilteredTemperatureTooWarmAtPowerUpFlagIsSetOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(SET);
   Given TheModuleIsInitialized();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldNotUpdateCurrentAndNextDefrostTypeOverrideToSecondaryOnlyWhenFreezerFilteredTemperatureTooWarmAtPowerUpFlagAndEepromClearedFlagAreClearOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeOverrideIs(DefrostType_Full);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(CLEAR);
   Given TheModuleIsInitialized();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiter, ShouldSetNextDefrostTypeOverrideToSecondaryOnlyAfterTheTypeIsFullWithTooWarmAtPowerUpOnInit)
{
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(SET);
   Given TheModuleIsInitialized();
   Given CurrentDefrostTypeIs(DefrostType_Full);
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeOverrideShouldBe(DefrostType_SecondaryOnly);
}

TEST_GROUP(NextDefrostTypeOverrideArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero)
{
   NextDefrostTypeOverrideArbiter_t instance;
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
      NextDefrostTypeOverrideArbiter_Init(&instance, dataModel, &config);
   }

   void NextDefrostTypeOverrideIs(DefrostType_t nextDefrostTypeOverride)
   {
      DataModel_Write(dataModel, Erd_NextDefrostTypeOverride, &nextDefrostTypeOverride);
   }

   void NextDefrostTypeOverrideShouldBe(DefrostType_t expected)
   {
      DefrostType_t actual;
      DataModel_Read(dataModel, Erd_NextDefrostTypeOverride, &actual);
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

TEST(NextDefrostTypeOverrideArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeOverrideToFullOnInitWhenEnhancedSabbathModeIsEnabledAndFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeOverrideToFullOnInitWhenEnhancedSabbathModeIsDisabledAndFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeOverrideToFullOnInitWhenEnhancedSabbathModeIsEnabledAndFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(true);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeOverrideToFullOnInitWhenEnhancedSabbathModeIsDisabledAndFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetCurrentAndNextDefrostTypeOverrideToFullWhenNumberOfSecondaryOnlyDefrostCountIsZeroOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeOverrideIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeOverrideArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldNotChangeDefrostTypeToSecondaryOnlyAfterDefrostingStateChangesToTrue)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeOverrideShouldBe(DefrostType_Full);
}
