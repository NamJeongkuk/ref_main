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

TEST(NextDefrostTypeArbiter, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(NextDefrostTypeArbiter, ShouldNotSetNextDefrostTypeOnInitWhenNumberOfSecondaryOnlyDefrostsBeforeAFullDefrostIsGreaterThanZeroAndFreezerCompartmentIsNotTooWarmAndEepromWasNotCleared)
{
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_SecondaryOnly);
}

TEST(NextDefrostTypeArbiter, ShouldNotResetSecondaryOnlyDefrostCountOnInit)
{
   Given NumberOfSecondaryOnlyDefrostsIs(5);
   Given TheModuleIsInitialized();

   NumberOfSecondaryOnlyDefrostsShouldBe(5);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalCountWhenFreezerDefrostIsAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalCountWhenItHasAConvertibleCompartmentAndConvertibleCompartmentDefrostIsAbnormalAndConvertibleCompartmentIsActingAsFreezerOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabledAndFreezerDefrostWasAbnormalIsSetOnInit)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentDefrostIsNotAbnormalOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndDoesNotHaveConvertibleCompartmentOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_Freezer);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalCountWhenEnhancedSabbathIsNotEnabledAndFreezerDefrostIsNotAbnormalAndConvertibleCompartmentTypeIsFreshFoodOnInit)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given HasConvertibleCompartmentIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentStateIs(ConvertibleCompartmentStateType_FreshFood);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToFullWhenSecondaryOnlyDefrostCountReachesNumberOfSecondaryOnlyDefrostsWhileCurrentDefrostTypeIsSecondaryOnly)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost - 1);
   NextDefrostTypeShouldBe(DefrostType_SecondaryOnly);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToSecondaryOnlyAndResetSecondaryOnlyDefrostCountToZeroWhenDefrostStateIsChangedFromFalseToTrueWhileCurrentDefrostTypeIsFull)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   Given NextDefrostTypeIs(DefrostType_Full);
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given NumberOfSecondaryOnlyDefrostsIs(10);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   And NumberOfSecondaryOnlyDefrostsShouldBe(0);
}

TEST(NextDefrostTypeArbiter, ShouldNotIncreaseSecondaryOnlyDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsSecondaryOnly)
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

TEST(NextDefrostTypeArbiter, ShouldNotResetSecondaryOnlyDefrostCountWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFull)
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

TEST(NextDefrostTypeArbiter, ShouldNotChangeNextDefrostTypeToSecondaryOnlyWhenDefrostingChangesToFalseWhileCurrentDefrostTypeIsFull)
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

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToEnhancedSabbathCountWhenEnhancedSabbathIsEnabled)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When EnhancedSabbathModeIs(true);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalFreezeDefrostCountWhenFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When FreezerDefrostWasAbnormalIs(true);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentDefrostWasAbnormalIsTrueAndConvertibleCompartmentStateChangesToFreezer)
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

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToAbnormalFreezeDefrostCountWhenHasConvertibleCompartmentAndConvertibleCompartmentStateIsFreezerAndConvertibleCompartmentDefrostWasAbnormalChangesToTrue)
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

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalWhenEnhancedSabbathModeIsDisabledWhileFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(enhancedSabbathData->numberOfSecondaryOnlyDefrostsBeforeFullDefrost);

   When EnhancedSabbathModeIs(false);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateNumberOfSecondaryOnlyDefrostsToNormalWhenFreezerDefrostWasAbnormalIsFalseWhileEnhancedSabbathModeIsDisabled)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given ConvertibleCompartmentDefrostWasAbnormalIs(false);
   Given TheModuleIsInitialized();
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrostWhenAbnormalIsSet);

   When FreezerDefrostWasAbnormalIs(false);
   NumberOfSecondaryOnlyDefrostsBeforeAFullDefrostShouldBe(defrostData->prechillPrepData.numberOfSecondaryOnlyDefrostsBeforeFullDefrost);
}

TEST(NextDefrostTypeArbiter, ShouldUpdateCurrentAndNextDefrostTypeToFullWhenFreezerFilteredTemperatureTooWarmAtPowerUpFlagIsSetOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(SET);
   Given TheModuleIsInitialized();
   CurrentDefrostTypeShouldBe(DefrostType_Full);
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldNotUpdateCurrentAndNextDefrostTypeToSecondaryOnlyWhenFreezerFilteredTemperatureTooWarmAtPowerUpFlagAndEepromClearedFlagAreClearOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeIs(DefrostType_Full);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(CLEAR);
   Given TheModuleIsInitialized();
   CurrentDefrostTypeShouldBe(DefrostType_SecondaryOnly);
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiter, ShouldSetNextDefrostTypeToSecondaryOnlyAfterTheTypeIsFullWithTooWarmAtPowerUpOnInit)
{
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given FreezerFilteredTemperatureTooWarmAtPowerUpFlagIs(SET);
   Given TheModuleIsInitialized();
   Given CurrentDefrostTypeIs(DefrostType_Full);
   NextDefrostTypeShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_SecondaryOnly);
}

TEST_GROUP(NextDefrostTypeArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero)
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

TEST(NextDefrostTypeArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsEnabledAndFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(false);
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsDisabledAndFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(true);
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsEnabledAndFreezerDefrostWasAbnormalIsTrue)
{
   Given EnhancedSabbathModeIs(true);
   Given FreezerDefrostWasAbnormalIs(true);
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetNextDefrostTypeToFullOnInitWhenEnhancedSabbathModeIsDisabledAndFreezerDefrostWasAbnormalIsFalse)
{
   Given EnhancedSabbathModeIs(false);
   Given FreezerDefrostWasAbnormalIs(false);
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldSetCurrentAndNextDefrostTypeToFullWhenNumberOfSecondaryOnlyDefrostCountIsZeroOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_SecondaryOnly);
   Given NextDefrostTypeIs(DefrostType_SecondaryOnly);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);
   CurrentDefrostTypeShouldBe(DefrostType_Full);
}

TEST(NextDefrostTypeArbiterWithNumberOfSecondaryOnlyDefrostCountIsZero, ShouldNotChangeDefrostTypeToSecondaryOnlyAfterDefrostingStateChangesToTrue)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   Given TheModuleIsInitialized();
   NextDefrostTypeShouldBe(DefrostType_Full);

   After DefrostingStateIsChangedFromFalseToTrueNTimes(1);
   NextDefrostTypeShouldBe(DefrostType_Full);
}
