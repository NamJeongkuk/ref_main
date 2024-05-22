/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FeaturePanModeRequestHandler.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   ResetValue = UINT8_MAX,

   SomeMinimumTemperature = 1,
   SomeDefaultTemperature = 5,
   SomeMaximumTemperature = 10,

   SomeOtherMinimumTemperature = 20,
   SomeOtherDefaultTemperature = 30,
   SomeOtherMaximumTemperature = 40,
};

static const FeaturePanData_t featurePanData = {
   .featurePanMinAllowedMode = FeaturePanCurrentMode_Mode1,
   .featurePanMaxAllowedMode = FeaturePanCurrentMode_Mode5
};

TEST_GROUP(FeaturePanModeRequestHandler)
{
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleDouble;

   FeaturePanModeRequestHandler_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      FeaturePanModeRequestHandler_Init(&instance, dataModel, &featurePanData);
   }

   void WhenTheRequestIs(FeaturePanCurrentMode_t mode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanCurrentModeRequest, &mode);
   }

   void WhenTheStatusIs(FeaturePanCurrentMode_t mode)
   {
      DataModel_Write(dataModel, Erd_FeaturePanCurrentModeStatus, &mode);
   }

   void TheStatusShouldBe(FeaturePanCurrentMode_t expected)
   {
      FeaturePanCurrentMode_t actual;
      DataModel_Read(dataModel, Erd_FeaturePanCurrentModeStatus, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void TheAlertSignalShouldBe(Signal_t expectedValue)
   {
      Signal_t actualValue;
      DataModel_Read(dataModel, Erd_FeaturePan_ResetThermalShiftOffsetSignal, &actualValue);
      CHECK_EQUAL(expectedValue, actualValue);
   }
};

TEST(FeaturePanModeRequestHandler, ShouldWriteCurrentModeStatusToDefaultIfUninitializedWhenModuleIsInitialized)
{
   WhenTheStatusIs(FeaturePanCurrentMode_NotControllable);
   GivenTheModuleIsInitialized();

   TheStatusShouldBe(FeaturePanCurrentMode_Mode1);
}

TEST(FeaturePanModeRequestHandler, ShouldNotWriteCurrentModeStatusToDefaultIfInitializedWhenModuleIsInitialized)
{
   WhenTheStatusIs(FeaturePanCurrentMode_Mode2);
   GivenTheModuleIsInitialized();

   TheStatusShouldBe(FeaturePanCurrentMode_Mode2);
}

TEST(FeaturePanModeRequestHandler, ShouldHandleAndResolveStatusWhenValidFeaturePanModesAreRequested)
{
   GivenTheModuleIsInitialized();
   TheAlertSignalShouldBe(0);

   WhenTheRequestIs(FeaturePanCurrentMode_Mode1);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode1);
   TheAlertSignalShouldBe(1);

   WhenTheRequestIs(FeaturePanCurrentMode_Mode2);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode2);
   TheAlertSignalShouldBe(2);

   WhenTheRequestIs(FeaturePanCurrentMode_Mode3);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode3);
   TheAlertSignalShouldBe(3);

   WhenTheRequestIs(FeaturePanCurrentMode_Mode4);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode4);
   TheAlertSignalShouldBe(4);

   WhenTheRequestIs(FeaturePanCurrentMode_Mode5);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode5);
   TheAlertSignalShouldBe(5);

   WhenTheRequestIs(FeaturePanCurrentMode_Mode6);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode5);
   TheAlertSignalShouldBe(5);

   WhenTheRequestIs(FeaturePanCurrentMode_Mode7);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode5);
   TheAlertSignalShouldBe(5);

   WhenTheRequestIs(FeaturePanCurrentMode_Off);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode5);
   TheAlertSignalShouldBe(5);
}

TEST(FeaturePanModeRequestHandler, ShouldNotHandleAndResolveStatusWhenNotControllableFeaturePanModeIsRequested)
{
   GivenTheModuleIsInitialized();

   WhenTheRequestIs(FeaturePanCurrentMode_Mode1);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode1);

   WhenTheRequestIs(FeaturePanCurrentMode_NotControllable);
   TheStatusShouldBe(FeaturePanCurrentMode_Mode1);
}
