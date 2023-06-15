/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Constants_Binary.h"
#include "BooleanRequestStatusUpdater.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static const BooleanRequestStatusErdPair_t erdPairs[] = {
   { Erd_PresenceSensingEnableRequest, Erd_PresenceSensingEnableStatus },
   { Erd_TurboFreezeRequest, Erd_TurboFreezeStatus }
};

static const BooleanRequestStatusUpdaterConfig_t config = {
   .requestStatusErdPairs = erdPairs,
   .numberOfRequestStatusErdPairs = NUM_ELEMENTS(erdPairs)
};

TEST_GROUP(BooleanRequestStatusUpdater)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   BooleanRequestStatusUpdater_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      BooleanRequestStatusUpdater_Init(
         &instance,
         &config,
         dataModel);
   }

   void WhenTheModuleIsInitialized()
   {
      GivenTheModuleIsInitialized();
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

TEST(BooleanRequestStatusUpdater, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(BooleanRequestStatusUpdater, ShouldNotChangeStatusOnInitIfRequestIsSetAndStatusIsClear)
{
   GivenARequestErdIs(SET);
   GivenAStatusErdIs(CLEAR);

   GivenTheModuleIsInitialized();
   AStatusErdShouldBe(CLEAR);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusUpdater, ShouldNotChangeStatusOnInitIfRequestIsClearAndStatusIsSet)
{
   GivenARequestErdIs(CLEAR);
   GivenAStatusErdIs(SET);

   GivenTheModuleIsInitialized();
   AStatusErdShouldBe(SET);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusUpdater, ShouldChangeStatusAndResetRequestWhenRequestChanges)
{
   GivenARequestErdIs(CLEAR);
   GivenAStatusErdIs(CLEAR);
   GivenTheModuleIsInitialized();

   WhenARequestErdIs(SET);
   AStatusErdShouldBe(SET);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusUpdater, ShouldResetRequestWithoutUpdatingStatusIfRequestChangesToStatus)
{
   GivenARequestErdIs(CLEAR);
   GivenAStatusErdIs(SET);
   GivenTheModuleIsInitialized();

   WhenARequestErdIs(SET);
   AStatusErdShouldBe(SET);
   ARequestErdShouldBe(BooleanRequestStatus_ResetValue);
}

TEST(BooleanRequestStatusUpdater, ShouldChangeStatusAndResetRequestWhenADifferentRequestChanges)
{
   GivenAnotherRequestErdIs(CLEAR);
   GivenAnotherStatusErdIs(CLEAR);
   GivenTheModuleIsInitialized();

   WhenAnotherRequestErdIs(SET);
   AnotherStatusErdShouldBe(SET);
   AnotherRequestErdShouldBe(BooleanRequestStatus_ResetValue);
}
