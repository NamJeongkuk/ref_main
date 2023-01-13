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

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      defrostData = PersonalityParametricData_Get(dataModel)->defrostData;
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

   void PostDwellExitTimeInMinutesShouldBe(uint8_t expectedMinutes)
   {
      uint8_t actualMinutes;
      DataModel_Read(dataModel, Erd_PostDwellExitTimeInMinutes, &actualMinutes);

      CHECK_EQUAL(expectedMinutes, actualMinutes);
   }

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
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

   void ThereIsAConvertibleCompartment()
   {
      DataModel_Write(dataModel, Erd_HasConvertibleCompartment, set);
   }

   void PreviousDefrostsAreNormalAndTimeWhenReadyToDefrostIsMaximumTimeBetweenDefrostsInMinutes()
   {
      Given PreviousDefrostsAreNormal();
      And MaxTimeBetweenDefrostsInMinutesIs(defrostData->idleData.maxTimeBetweenDefrostsInMinutes);
      And FreezerEvaporatorThermistorValidityIs(Valid);
      And DefrostParameterSelectorIsInitialized();

      TimeWhenReadyToDefrostInMinutesShouldBe(defrostData->idleData.maxTimeBetweenDefrostsInMinutes);
   }
};

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodOnlyIfCurrentDefrostIsFreshFoodOnlyOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeIfCurrentDefrostIsNotFreshFoodOnlyOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->prechillData.maxPrechillTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodOnlyIfCurrentDefrostIsFreshFoodOnlyChangesFromFalseToTrue)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->prechillData.maxPrechillTimeInMinutes);

   When CurrentDefrostTypeIs(DefrostType_FreshFood);

   MaxPrechillTimeInMinutesShouldBe(defrostData->prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeIfCurrentDefrostIsFreshFoodOnlyChangesFromTrueToFalse)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData->prechillData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);

   When CurrentDefrostTypeIs(DefrostType_Full);

   MaxPrechillTimeInMinutesShouldBe(defrostData->prechillData.maxPrechillTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetPostDwellExitTimeToPostDwellExitTimeWhenUnitIsSingleEvap)
{
   Given DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellData.postDwellExitTimeInMinutes);
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

   void CurrentDefrostTypeIs(DefrostType_t defrostType)
   {
      DataModel_Write(dataModel, Erd_CurrentDefrostType, &defrostType);
   }
};

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsNotFreshFoodOnlyOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellData.postDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodOnlyPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyOnInit)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellData.freshFoodOnlyPostDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyChangesFromTrueToFalse)
{
   Given CurrentDefrostTypeIs(DefrostType_FreshFood);
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellData.freshFoodOnlyPostDwellExitTimeInMinutes);

   When CurrentDefrostTypeIs(DefrostType_Full);

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellData.postDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodOnlyPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyChangesFromFalseToTrue)
{
   Given CurrentDefrostTypeIs(DefrostType_Full);
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellData.postDwellExitTimeInMinutes);

   When CurrentDefrostTypeIs(DefrostType_FreshFood);

   PostDwellExitTimeInMinutesShouldBe(defrostData->postDwellData.freshFoodOnlyPostDwellExitTimeInMinutes);
}
