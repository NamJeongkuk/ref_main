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
#include "PersonalityParametricData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

static const EnhancedSabbathData_t enhancedSabbathData = {
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 3
};

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
   .freezerAbnormalDefrostHeaterMaxOnTimeInMinutes = 32,
   .freezerDefrostTerminationTemperatureInDegFx100 = 5900,
   .freshFoodDefrostTerminationTemperatureInDegFx100 = 4460,
   .convertibleCompartmentDefrostTerminationTemperatureInDegFx100 = 4460,
   .freshFoodDefrostHeaterMaxOnTimeInMinutes = 60,
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
   .freshFoodOnlyPostDwellExitTimeInMinutes = 20,
   .defrostPeriodicTimeoutInSeconds = 1,
   .threeWayValvePositionToExitIdle = ValvePosition_B,
   .threeWayValvePositionForMaxPrechillHoldoff = ValvePosition_B,
   .threeWayValvePositionToExtendDefrostWithFreshFoodCycleDefrost = ValvePosition_B
};

static const EvaporatorData_t singleEvaporatorData = {
   .numberOfEvaporators = 1
};

static const EvaporatorData_t dualEvaporatorData = {
   .numberOfEvaporators = 2
};

TEST_GROUP(DefrostParameterSelector_SingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetEnhancedSabbath(&personalityParametricData, &enhancedSabbathData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &singleEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void DefrostParameterSelectorIsInitialized()
   {
      DefrostParameterSelector_Init(dataModel);
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
};

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodOnlyIfCurrentDefrostIsFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodAndFreezerDefrostsIfCurrentDefrostIsNotFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData.maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodOnlyIfCurrentDefrostIsFreshFoodOnlyChangesFromFalseToTrue)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData.maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes);

   When CurrentDefrostIsFreshFoodOnly();

   MaxPrechillTimeInMinutesShouldBe(defrostData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetMaxPrechillTimeToMaxPrechillTimeForFreshFoodAndFreezerDefrostsIfCurrentDefrostIsFreshFoodOnlyChangesFromTrueToFalse)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   MaxPrechillTimeInMinutesShouldBe(defrostData.maxPrechillTimeForFreshFoodOnlyDefrostInMinutes);

   When CurrentDefrostIsNotFreshFoodOnly();

   MaxPrechillTimeInMinutesShouldBe(defrostData.maxPrechillTimeForFreshFoodAndFreezerDefrostsInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetPostDwellExitTimeToFreshFoodAndFreezerPostDwellExitTimeWhenUnitIsSingleEvap)
{
   Given DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData.freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToDefrostParametricWhenEnhancedSabbathIsDisabledOnInit)
{
   Given EnhancedSabbathIsDisabled();
   And DefrostParameterSelectorIsInitialized();

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToEnhancedSabbathParametricWhenEnhancedSabbathIsEnabledOnInit)
{
   Given EnhancedSabbathIsEnabled();
   And DefrostParameterSelectorIsInitialized();

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToEnhancedSabbathParametricWhenEnhancedSabbathIsEnabled)
{
   Given EnhancedSabbathIsDisabled();
   And DefrostParameterSelectorIsInitialized();

   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);

   When EnhancedSabbathIsEnabled();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(enhancedSabbathData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST(DefrostParameterSelector_SingleEvap, ShouldSetNumberOfFreshFoodDefrostsBeforeAFreezerDefrostToDefrostParametricWhenEnhancedSabbathIsDisabled)
{
   Given EnhancedSabbathIsEnabled();
   And DefrostParameterSelectorIsInitialized();

   When EnhancedSabbathIsDisabled();
   NumberOfFreshFoodDefrostsBeforeAFreezerDefrostShouldBe(defrostData.numberOfFreshFoodDefrostsBeforeFreezerDefrost);
}

TEST_GROUP(DefrostParameterSelector_DualEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetDefrost(&personalityParametricData, &defrostData);
      PersonalityParametricData_TestDouble_SetEvaporator(&personalityParametricData, &dualEvaporatorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   }

   void DefrostParameterSelectorIsInitialized()
   {
      DefrostParameterSelector_Init(dataModel);
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

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodAndFreezerPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsNotFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData.freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyOnInit)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData.freshFoodOnlyPostDwellExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodAndFreezerPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyChangesFromTrueToFalse)
{
   Given CurrentDefrostIsFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData.freshFoodOnlyPostDwellExitTimeInMinutes);

   When CurrentDefrostIsNotFreshFoodOnly();

   PostDwellExitTimeInMinutesShouldBe(defrostData.freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes);
}

TEST(DefrostParameterSelector_DualEvap, ShouldSetPostDwellExitTimeToFreshFoodPostDwellExitTimeWhenUnitIsDualEvapAndCurrentDefrostIsFreshFoodOnlyChangesFromFalseToTrue)
{
   Given CurrentDefrostIsNotFreshFoodOnly();
   And DefrostParameterSelectorIsInitialized();

   PostDwellExitTimeInMinutesShouldBe(defrostData.freshFoodAndFreezerPostDwellFreezerExitTimeInMinutes);

   When CurrentDefrostIsFreshFoodOnly();

   PostDwellExitTimeInMinutesShouldBe(defrostData.freshFoodOnlyPostDwellExitTimeInMinutes);
}
