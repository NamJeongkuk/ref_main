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
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "EnhancedSabbathMode.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

enum
{
   WaitForEepromWritesToCompleteTimeInMsec = 1000,
   EnhancedSabbathRunTimePeriodicCheckTimeInMinutes = 1,
   EnhancedSabbathRunTimePeriodicCheckTimeInMsec = EnhancedSabbathRunTimePeriodicCheckTimeInMinutes * MSEC_PER_MIN,
   SomeRunTimeInMinutes = 10,
   SomeOtherRunTimeInMinutes = 2351,
   ThirtyMinutes = 30
};

TEST_GROUP(SabbathModesIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const EnhancedSabbathData_t *enhancedSabbathData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      enhancedSabbathData = PersonalityParametricData_Get(dataModel)->enhancedSabbathData;
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

   void GivenEepromEnhancedSabbathRunTimeInMinutesIs(uint16_t newRunTime)
   {
      DataModel_Write(dataModel, Erd_Eeprom_EnhancedSabbathRunTimeInMinutes, &newRunTime);
   }

   void WhenEepromEnhancedSabbathRunTimeInMinutesIs(uint16_t newRunTime)
   {
      DataModel_Write(dataModel, Erd_Eeprom_EnhancedSabbathRunTimeInMinutes, &newRunTime);
   }

   void EnhancedSabbathRunTimeInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_EnhancedSabbathRunTimeInMinutes, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void EepromEnhancedSabbathRunTimeInMinutesShouldBe(uint16_t expected)
   {
      uint16_t actual;
      DataModel_Read(dataModel, Erd_Eeprom_EnhancedSabbathRunTimeInMinutes, &actual);
      CHECK_EQUAL(expected, actual);
   }

   uint16_t EepromEnhancedSabbathRunTimeInMinutes()
   {
      uint16_t current;
      DataModel_Read(dataModel, Erd_Eeprom_EnhancedSabbathRunTimeInMinutes, &current);
      return current;
   }

   void WhenCoolingOffRequestIs(CoolingSystemRequest_t state)
   {
      DataModel_Write(dataModel, Erd_CoolingOffRequest, &state);
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

TEST(SabbathModesIntegration, ShouldUpdateEepromEnhancedSabbathRunTimeErdEveryThirtyMinutesAfterStatusEnabledAndResetToZeroAfterMaxRunTimeIsReached)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenApplicationHasBeenInitialized();

   uint16_t numberOfEepromUpdatePeriods =
      enhancedSabbathData->maxTimeInEnhancedSabbathModeInMinutes / ThirtyMinutes;

   EepromEnhancedSabbathRunTimeInMinutesShouldBe(0);

   After(ThirtyMinutes * MSEC_PER_MIN);
   uint16_t currentEepromEnhancedSabbathRunTimeInMinutes = EepromEnhancedSabbathRunTimeInMinutes();

   for(uint16_t currentEepromUpdatePeriod = 1; currentEepromUpdatePeriod < numberOfEepromUpdatePeriods - 1; currentEepromUpdatePeriod++)
   {
      currentEepromEnhancedSabbathRunTimeInMinutes = EepromEnhancedSabbathRunTimeInMinutes();
      After(ThirtyMinutes * MSEC_PER_MIN - 1);
      EepromEnhancedSabbathRunTimeInMinutesShouldBe(currentEepromEnhancedSabbathRunTimeInMinutes);

      After(1);
      EepromEnhancedSabbathRunTimeInMinutesShouldBe(currentEepromEnhancedSabbathRunTimeInMinutes + ThirtyMinutes);
   }

   currentEepromEnhancedSabbathRunTimeInMinutes = EepromEnhancedSabbathRunTimeInMinutes();
   After(ThirtyMinutes * MSEC_PER_MIN - 1);
   EepromEnhancedSabbathRunTimeInMinutesShouldBe(currentEepromEnhancedSabbathRunTimeInMinutes);

   After(1);
   EepromEnhancedSabbathRunTimeInMinutesShouldBe(0);
}

TEST(SabbathModesIntegration, ShouldStartEnhancedSabbathRunTimeTimerFromEepromValueWhenEnhancedSabbathIsEnabledOnInit)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenEepromEnhancedSabbathRunTimeInMinutesIs(SomeRunTimeInMinutes);
   GivenApplicationHasBeenInitialized();

   EnhancedSabbathRunTimeInMinutesShouldBe(SomeRunTimeInMinutes);

   After(((enhancedSabbathData->maxTimeInEnhancedSabbathModeInMinutes - SomeRunTimeInMinutes) * MSEC_PER_MIN) - 1);
   EnhancedSabbathRunTimeInMinutesShouldBe(enhancedSabbathData->maxTimeInEnhancedSabbathModeInMinutes - EnhancedSabbathRunTimePeriodicCheckTimeInMinutes);

   After(1);
   EnhancedSabbathRunTimeInMinutesShouldBe(0);
   EepromEnhancedSabbathRunTimeInMinutesShouldBe(0);
}

TEST(SabbathModesIntegration, ShouldHaveTheSameEepromEnhancedSabbathRunTimeInMinutesAfterRefrigeratorIsReset)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenApplicationHasBeenInitialized();

   After((SomeOtherRunTimeInMinutes * MSEC_PER_MIN) - 1);
   uint16_t currentEepromEnhancedSabbathRunTimeInMinutes = EepromEnhancedSabbathRunTimeInMinutes();

   WhenRefrigeratorResets();

   EnhancedSabbathRunTimeInMinutesShouldBe(currentEepromEnhancedSabbathRunTimeInMinutes);
   EepromEnhancedSabbathRunTimeInMinutesShouldBe(currentEepromEnhancedSabbathRunTimeInMinutes);
}

TEST(SabbathModesIntegration, ShouldStartEnhancedSabbathRunTimeTimerFromZeroWhenEnabledAfterPowerUpAndEepromEnhancedSabbathRunTimeIsNonZero)
{
   GivenEnhancedSabbathModeIs(CLEAR);
   GivenEepromEnhancedSabbathRunTimeInMinutesIs(SomeRunTimeInMinutes);
   GivenApplicationHasBeenInitialized();
   EnhancedSabbathRunTimeInMinutesShouldBe(0);

   WhenEnhancedSabbathModeIs(SET);
   EnhancedSabbathRunTimeInMinutesShouldBe(0);

   After((enhancedSabbathData->maxTimeInEnhancedSabbathModeInMinutes * MSEC_PER_MIN) - 1);
   EnhancedSabbathRunTimeInMinutesShouldBe(enhancedSabbathData->maxTimeInEnhancedSabbathModeInMinutes - EnhancedSabbathRunTimePeriodicCheckTimeInMinutes);

   After(1);
   EnhancedSabbathRunTimeInMinutesShouldBe(0);
}

TEST(SabbathModesIntegration, ShouldNotDisableEnhancedSabbathModeAfterResetWhileCoolingSystemIsOff)
{
   GivenEnhancedSabbathModeIs(SET);
   GivenApplicationHasBeenInitialized();

   WhenCoolingOffRequestIs(SET);
   EnhancedSabbathModeShouldBe(SET);

   WhenRefrigeratorResets();
   EnhancedSabbathModeShouldBe(SET);
}
