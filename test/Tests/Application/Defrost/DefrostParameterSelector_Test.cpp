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
#include "DefrostData_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

static const EnhancedSabbathData_t enhancedSabbathData = {
   .numberOfFreshFoodDefrostsBeforeFreezerDefrost = 3
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

   DefrostData_t defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DefrostData_TestDouble_Init(&defrostData);
      DefrostData_TestDouble_SetFreshFoodOnlyPostDwellExitTimeInMinutes(&defrostData, 20);
      DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(&defrostData, 60);

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
   DefrostData_t defrostData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DefrostData_TestDouble_Init(&defrostData);
      DefrostData_TestDouble_SetFreshFoodOnlyPostDwellExitTimeInMinutes(&defrostData, 20);
      DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(&defrostData, 60);

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
