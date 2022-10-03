/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DefrostParameterSelector.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Binary.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Invalid = false,
   Valid = true
};

enum
{
   SomeMoreMinutes = 100
};

TEST_GROUP(DefrostParameterSelector_SingleEvap)
{
   DefrostParameterSelector_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DefrostData_t *defrostData;
   const EnhancedSabbathData_t *enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
      enhancedSabbathData = PersonalityParametricData_Get(dataModel)->enhancedSabbathData;
   }

   void DefrostParameterSelectorIsInitialized()
   {
      DefrostParameterSelector_Init(&instance, dataModel);
   }

   void MaxPrechillTimeInMinutesShouldBe(uint8_t expectedMinutes)
   {
      uint8_t actualMinutes;
      DataModel_Read(dataModel, Erd_MaxPrechillTimeInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(uint8_t expectedDefrosts)
   {
      uint8_t actualDefrosts;
      DataModel_Read(dataModel, Erd_NumberOfFreshFoodDefrostsBeforeAFreezerDefrost, &actualDefrosts);

      CHECK_EQUAL(expectedDefrosts, actualDefrosts);
   }

   void PostDwellExitTimeInMinutesShouldBe(uint8_t expectedMinutes)
   {
      uint8_t actualMinutes;
      DataModel_Read(dataModel, Erd_PostDwellExitTimeInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void CurrentDefrostIsFreshFoodOnly()
   {
      DataModel_Write(dataModel, Erd_DefrostIsFreshFoodOnly, set);
   }

   void CurrentDefrostIsNotFreshFoodOnly()
   {
      DataModel_Write(dataModel, Erd_DefrostIsFreshFoodOnly, clear);
   }

   void EnhancedSabbathIsEnabled()
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathMode, set);
   }

   void EnhancedSabbathIsDisabled()
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathMode, clear);
   }

   void PreviousDefrostsAreNormal()
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, clear);
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, clear);
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, clear);
   }

   void TimeWhenReadyToDefrostInMinutesShouldBe(uint16_t expectedTimeInMinutes)
   {
      uint16_t actualTimeInMinutes;
      DataModel_Read(dataModel, Erd_TimeInMinutesUntilReadyToDefrost, &actualTimeInMinutes);

      CHECK_EQUAL(expectedTimeInMinutes, actualTimeInMinutes);
   }

   void PreviousFreshFoodDefrostIsAbnormal()
   {
      DataModel_Write(dataModel, Erd_FreshFoodDefrostWasAbnormal, set);
   }

   void PreviousFreezerDefrostIsAbnormal()
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostWasAbnormal, set);
   }

   void PreviousConvertibleCompartmentDefrostIsAbnormal()
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentDefrostWasAbnormal, set);
   }

   void FreezerEvaporatorThermistorValidityIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerEvaporatorThermistorIsValid, &state);
   }

   void MaxTimeBetweenDefrostsInMinutesIs(uint16_t timeInMinutes)
   {
      DataModel_Write(dataModel, Erd_MaxTimeBetweenDefrostsInMinutes, &timeInMinutes);
   }

   void PreviousDefrostsAreNormalAndTimeWhenReadyToDefrostIsMaximumTimeBetweenDefrostsInMinutes()
   {
      Given PreviousDefrostsAreNormal();
      And MaxTimeBetweenDefrostsInMinutesIs(defrostData->maxTimeBetweenDefrostsInMinutes);
      And FreezerEvaporatorThermistorValidityIs(Valid);
      And DefrostParameterSelectorIsInitialized();

      TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->maxTimeBetweenDefrostsInMinutes);
   }
};

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodOnlyIfCurrentDefrostIsFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeIfCurrentDefrostIsNotFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->maxPrechillTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodOnlyIfCurrentDefrostIsFreshFoodOnlyChangesFromFalseToTrue)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->maxPrechillTimeInMinutes);

   When CurrentDefrostIsFreshFoodOnly();

   MaxPrechillTimeInMinutesShouldBe(defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeIfCurrentDefrostIsFreshFoodOnlyChangesFromTrueToFalse)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);

   When CurrentDefrostIsNotFreshFoodOnly();

   MaxPrechillTimeInMinutesShouldBe(defrostData->maxPrechillTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetPostDwellExitTimeToPostDwellExitTimeWhenUnitIsSingleEvap)
{
   Given DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToDefrostParametricWhenEnhancedSabbathIsDisabledOnInit)
{
   Given EnhancedSabbathIsDisabled();
   And DefrostParameterSelectorIsInitialized();

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToEnhancedSabbathParametricWhenEnhancedSabbathIsEnabledOnInit)
{
   Given EnhancedSabbathIsEnabled();
   And DefrostParameterSelectorIsInitialized();

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToEnhancedSabbathParametricWhenEnhancedSabbathIsEnabled)
{
   Given EnhancedSabbathIsDisabled();
   And DefrostParameterSelectorIsInitialized();

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When EnhancedSabbathIsEnabled();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToDefrostParametricWhenEnhancedSabbathIsDisabled)
{
   Given EnhancedSabbathIsEnabled();
   And DefrostParameterSelectorIsInitialized();

   When EnhancedSabbathIsDisabled();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData->numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostInMinutesShouldBeMaxTimeBetweenDefrostsWhenPreviousDefrostsWereNormalAndFreezerEvaporatorThermistorIsValid)
{
   Given PreviousDefrostsAreNormal();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And MaxTimeBetweenDefrostsInMinutesIs(defrostData->maxTimeBetweenDefrostsInMinutes);
   And DefrostParameterSelectorIsInitialized();

   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->maxTimeBetweenDefrostsInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostInMinutesShouldBeMinimumTimeBetweenDefrostsAbnormalRunTimeWhenPreviousFreshFoodDefrostWasAbnormal)
{
   Given PreviousFreshFoodDefrostIsAbnormal();
   And DefrostParameterSelectorIsInitialized();

   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostInMinutesShouldBeMinimumTimeBetweenDefrostsAbnormalRunTimeWhenPreviousFreezerDefrostWasAbnormal)
{
   Given PreviousFreezerDefrostIsAbnormal();
   And DefrostParameterSelectorIsInitialized();

   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostInMinutesShouldBeMinimumTimeBetweenDefrostsAbnormalRunTimeWhenPreviousConvertibleCompartmentDefrostWasAbnormal)
{
   Given PreviousConvertibleCompartmentDefrostIsAbnormal();
   And DefrostParameterSelectorIsInitialized();

   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostShouldUpdateToMinimumTimeBetweenDefrostsAbnormalRunTimeWhenPreviousFreshFoodDefrostIsAbnormal)
{
   Given PreviousDefrostsAreNormalAndTimeWhenReadyToDefrostIsMaximumTimeBetweenDefrostsInMinutes();

   When PreviousFreshFoodDefrostIsAbnormal();
   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostShouldUpdateToMinimumTimeBetweenDefrostsAbnormalRunTimeWhenPreviousFreezerDefrostIsAbnormal)
{
   Given PreviousDefrostsAreNormalAndTimeWhenReadyToDefrostIsMaximumTimeBetweenDefrostsInMinutes();

   When PreviousFreezerDefrostIsAbnormal();
   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostShouldUpdateToMinimumTimeBetweenDefrostsAbnormalRunTimeWhenPreviousConvertibleCompartmentDefrostIsAbnormal)
{
   Given PreviousDefrostsAreNormalAndTimeWhenReadyToDefrostIsMaximumTimeBetweenDefrostsInMinutes();

   When PreviousConvertibleCompartmentDefrostIsAbnormal();
   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostShouldUpdateToMinimumTimeBetweenDefrostsAbnormalRunTimeWhenFreezerEvaporatorThermistorBecomesInvalid)
{
   Given PreviousDefrostsAreNormalAndTimeWhenReadyToDefrostIsMaximumTimeBetweenDefrostsInMinutes();

   When FreezerEvaporatorThermistorValidityIs(Invalid);
   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->minimumTimeBetweenDefrostsAbnormalRunTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, TimeWhenReadyToDefrostShouldUpdateToADifferentMaxTimeBetweenDefrostsWhenMaxTimeBetweenDefrostsChangesWhileFreezerEvaporatorThermistorIsValid)
{
   Given PreviousDefrostsAreNormal();
   And FreezerEvaporatorThermistorValidityIs(Valid);
   And MaxTimeBetweenDefrostsInMinutesIs(defrostData->maxTimeBetweenDefrostsInMinutes);
   And DefrostParameterSelectorIsInitialized();

   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->maxTimeBetweenDefrostsInMinutes);

   When MaxTimeBetweenDefrostsInMinutesIs(defrostData->maxTimeBetweenDefrostsInMinutes + SomeMoreMinutes);
   TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->maxTimeBetweenDefrostsInMinutes + SomeMoreMinutes);
}

TEST_GROUP(DefrostParameterSelector_DualEvap)
{
   DefrostParameterSelector_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DefrostData_t *defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
   }

   void DefrostParameterSelectorIsInitialized()
   {
      DefrostParameterSelector_Init(&instance, dataModel);
   }

   void PostDwellExitTimeInMinutesShouldBe(uint8_t expectedMinutes)
   {
      uint8_t actualMinutes;
      DataModel_Read(dataModel, Erd_PostDwellExitTimeInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void CurrentDefrostIsFreshFoodOnly()
   {
      DataModel_Write(dataModel, Erd_DefrostIsFreshFoodOnly, set);
   }

   void CurrentDefrostIsNotFreshFoodOnly()
   {
      DataModel_Write(dataModel, Erd_DefrostIsFreshFoodOnly, clear);
   }
};

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsNotFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodOnlyPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->freshFoodOnlyPostDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyChangesFromTrueToFalse)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->freshFoodOnlyPostDwellExitTimeInMinutes);

   When CurrentDefrostIsNotFreshFoodOnly();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodOnlyPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyChangesFromFalseToTrue)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellExitTimeInMinutes);

   When CurrentDefrostIsFreshFoodOnly();

   PostDwellExitTimeInMinutesShouldBe(defrostData->freshFoodOnlyPostDwellExitTimeInMinutes);
}
