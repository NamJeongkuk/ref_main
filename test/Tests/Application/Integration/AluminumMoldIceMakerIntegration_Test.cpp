/*!
 * @file
 * @brief Integration test for Aluminum Mold Ice Maker
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define And
#define When

enum
{
   Invalid = false,
   Valid = true,
   InvalidAdcCount = 5375,
   ValidAdcCount = 13967,
};

TEST_GROUP(AluminumMoldIceMakerIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void ApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void SabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathMode, &state);
   }

   void AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_t expectedState)
   {
      AluminumMoldIceMakerHsmState_t actualState;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerHsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void IceMakerIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMakerEnabledOverrideRequest, set);
      DataModel_Write(dataModel, Erd_IceMakerEnabledOverrideValue, &state);
   }

   void MoldThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_AdcCount, &count);
   }

   void MoldThermistorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerMoldThermistorIsValid, &actualState);

      CHECK_EQUAL(expectedState, actualState);;
   }

   void TheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }
};

TEST(AluminumMoldIceMakerIntegration, ShouldInitialize)
{
   Given ApplicationHasBeenInitialized();
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenSabbathModeIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   And IceMakerIs(ENABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabled)
{
   Given IceMakerIs(DISABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabledAndSabbathModeIsEnabled)
{
   Given IceMakerIs(DISABLED);
   And SabbathModeIs(ENABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInThermistorFaultStateWhenMoldThermistorIsInvalidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenRakePositionIsHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_Home);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInHarvestStateWhenRakePositionIsNotHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}
