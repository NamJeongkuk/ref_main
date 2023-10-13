/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "DataModelErdPointerAccess.h"
#include "Rx2xxResetSource.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

enum
{
   WaitForEepromWritesToCompleteTimeInMsec = 1000
};

TEST_GROUP(SabbathModesIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void WhenApplicationIsInitialized()
   {
      GivenApplicationHasBeenInitialized();
   }

   void WhenRefrigeratorResets()
   {
      After(WaitForEepromWritesToCompleteTimeInMsec);
      ReferDataModel_TestDouble_Reset(&dataModelDouble);
      WhenApplicationIsInitialized();
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathModeEnable, &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_EnhancedSabbathModeEnable, &state);
   }

   void WhenEnhancedSabbathModeIs(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void SabbathModeShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_SabbathModeEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void EnhancedSabbathModeShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_EnhancedSabbathModeEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(SabbathModesIntegration, ShouldStayInSabbathModeThroughAPowerCycle)
{
   GivenSabbathModeIs(SET);
   GivenEnhancedSabbathModeIs(CLEAR);
   GivenApplicationHasBeenInitialized();

   WhenRefrigeratorResets();
   SabbathModeShouldBe(SET);
}

TEST(SabbathModesIntegration, ShouldStayInEnhancedSabbathModeThroughAPowerCycle)
{
   GivenSabbathModeIs(CLEAR);
   GivenEnhancedSabbathModeIs(SET);
   GivenApplicationHasBeenInitialized();

   WhenRefrigeratorResets();
   EnhancedSabbathModeShouldBe(SET);
}

TEST(SabbathModesIntegration, ShouldKeepBothModesEnabledThroughAPowerCycle)
{
   GivenSabbathModeIs(SET);
   GivenEnhancedSabbathModeIs(SET);
   GivenApplicationHasBeenInitialized();

   WhenRefrigeratorResets();
   SabbathModeShouldBe(SET);
   EnhancedSabbathModeShouldBe(SET);
}

TEST(SabbathModesIntegration, ShouldDisableBothSabbathModesWhenEnhancedSabbathModeIsDisabled)
{
   GivenSabbathModeIs(SET);
   GivenEnhancedSabbathModeIs(SET);
   GivenApplicationHasBeenInitialized();

   WhenEnhancedSabbathModeIs(CLEAR);
   SabbathModeShouldBe(CLEAR);
   EnhancedSabbathModeShouldBe(CLEAR);
}

TEST(SabbathModesIntegration, ShouldNotDisableEnhancedSabbathModeWhenSabbathModeIsDisabled)
{
   GivenSabbathModeIs(SET);
   GivenEnhancedSabbathModeIs(SET);
   GivenApplicationHasBeenInitialized();

   WhenSabbathModeIs(CLEAR);
   EnhancedSabbathModeShouldBe(SET);
}
