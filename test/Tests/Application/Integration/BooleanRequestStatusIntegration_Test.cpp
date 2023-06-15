/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

TEST_GROUP(BooleanRequestStatusIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void WhenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void GivenApplicationHasBeenInitialized()
   {
      WhenApplicationHasBeenInitialized();
   }

   void GivenAStatusErdIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_PresenceSensingEnableStatus,
         &state);
   }

   void GivenARequestErdIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_PresenceSensingEnableRequest,
         &state);
   }

   void WhenAStatusErdIs(bool state)
   {
      GivenAStatusErdIs(state);
   }

   void WhenARequestErdIs(bool state)
   {
      GivenARequestErdIs(state);
   }

   void ARequestErdShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(
         dataModel,
         Erd_PresenceSensingEnableRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AStatusErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_PresenceSensingEnableStatus,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenAnotherStatusErdIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_TurboFreezeStatus,
         &state);
   }

   void GivenAnotherRequestErdIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_TurboFreezeRequest,
         &state);
   }

   void WhenAnotherStatusErdIs(bool state)
   {
      GivenAnotherStatusErdIs(state);
   }

   void WhenAnotherRequestErdIs(bool state)
   {
      GivenAnotherRequestErdIs(state);
   }

   void AnotherStatusErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_TurboFreezeStatus,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AnotherRequestErdShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(
         dataModel,
         Erd_TurboFreezeRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(BooleanRequestStatusIntegration, ShouldNotChangeStatusOnInitIfRequestIsSetAndStatusIsClear)
{
   GivenARequestErdIs(SET);
   GivenAStatusErdIs(CLEAR);

   WhenApplicationHasBeenInitialized();
   AStatusErdShouldBe(CLEAR);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusIntegration, ShouldNotChangeStatusOnInitIfRequestIsClearAndStatusIsSet)
{
   GivenARequestErdIs(CLEAR);
   GivenAStatusErdIs(SET);

   WhenApplicationHasBeenInitialized();
   AStatusErdShouldBe(SET);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusIntegration, ShouldChangeStatusAndResetRequestWhenRequestChanges)
{
   GivenARequestErdIs(CLEAR);
   GivenAStatusErdIs(CLEAR);
   GivenApplicationHasBeenInitialized();

   WhenARequestErdIs(SET);
   AStatusErdShouldBe(SET);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusIntegration, ShouldResetRequestWithoutUpdatingStatusIfRequestChangesToStatus)
{
   GivenARequestErdIs(CLEAR);
   GivenAStatusErdIs(SET);
   GivenApplicationHasBeenInitialized();

   WhenARequestErdIs(SET);
   AStatusErdShouldBe(SET);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusIntegration, ShouldChangeStatusAndResetRequestWhenADifferentRequestChanges)
{
   GivenAnotherRequestErdIs(CLEAR);
   GivenAnotherStatusErdIs(CLEAR);
   GivenApplicationHasBeenInitialized();

   WhenAnotherRequestErdIs(SET);
   AnotherStatusErdShouldBe(SET);
   AnotherRequestErdShouldBe(BooleanRequestStatus_ResetValue);
}
