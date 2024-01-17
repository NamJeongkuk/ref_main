/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceMakerMoldHeaterController.h"
#include "IceMakerMoldHeaterControlRequest.h"
#include "HeaterVotedState.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "PersonalityParametricData.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   DontSkipInitialOnTime = false,
   SkipInitialOnTime = true,

   SomeInitialMotorOnTemperatureInDegFx100 = 3560,
   SomeOnTemperatureInDegFx100 = SomeInitialMotorOnTemperatureInDegFx100 + 1000,
   SomeOffTemperatureInDegFx100 = SomeInitialMotorOnTemperatureInDegFx100 + 2000,

   SomeOtherOnTemperatureInDegFx100 = 1000,
   SomeOtherOffTemperatureInDegFx100 = 2000,
};

static const IceMakerMoldHeaterControllerConfig_t config = {
   .moldHeaterControlRequestErd = Erd_IceMaker0_MoldHeaterControlRequest,
   .moldHeaterVoteErd = Erd_IceMaker0_HeaterRelay_IceMakerVote,
   .moldFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .timerModuleErd = Erd_TimerModule,
};

TEST_GROUP(IceMakerMoldHeaterController)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleDouble;
   IceMakerMoldHeaterController_t instance;
   const AluminumMoldIceMakerHarvestData_t *harvestData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      harvestData = &PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot1Data->aluminumMoldData->harvestData;
   }

   void WhenIceMakerMoldHeaterControlRequestIs(
      bool enable,
      bool skipInitialOnTime,
      TemperatureDegFx100_t onTemperature,
      TemperatureDegFx100_t offTemperature)
   {
      IceMakerMoldHeaterControlRequest_t request = {
         .enable = enable,
         .skipInitialOnTime = skipInitialOnTime,
         .onTemperatureInDegFx100 = onTemperature,
         .offTemperatureInDegFx100 = offTemperature,
      };

      DataModel_Write(dataModel, Erd_IceMaker0_MoldHeaterControlRequest, &request);
   }

   void WhenIceMakerMoldHeaterControlRequestIsDisabled()
   {
      IceMakerMoldHeaterControlRequest_t request = {
         .enable = false,
         .skipInitialOnTime = SkipInitialOnTime,
         .onTemperatureInDegFx100 = SomeOnTemperatureInDegFx100,
         .offTemperatureInDegFx100 = SomeOffTemperatureInDegFx100,
      };

      DataModel_Write(dataModel, Erd_IceMaker0_MoldHeaterControlRequest, &request);
   }

   void GivenIceMakerMoldTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100, &temperature);
   }

   void WhenTheIceMakerMoldTemperatureChangesTo(TemperatureDegFx100_t temperature)
   {
      GivenIceMakerMoldTemperatureIs(temperature);
   }

   void TheMoldHeaterVoteShouldBeCareAnd(bool expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_HeaterRelay_IceMakerVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_TRUE(actual.care);
   }

   void TheMoldHeaterVoteShouldBeDontCareAnd(bool expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_HeaterRelay_IceMakerVote, &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_FALSE(actual.care);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void GivenInitialization()
   {
      IceMakerMoldHeaterController_Init(
         &instance,
         dataModel,
         &config,
         harvestData);
   }

   void GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOff()
   {
      GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100 + 1);
      GivenInitialization();
      WhenIceMakerMoldHeaterControlRequestIs(
         ENABLED,
         DontSkipInitialOnTime,
         SomeOnTemperatureInDegFx100,
         SomeOffTemperatureInDegFx100);

      TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
   }

   void GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOn()
   {
      GivenIceMakerMoldTemperatureIs(SomeOnTemperatureInDegFx100 + 1);
      GivenInitialization();
      WhenIceMakerMoldHeaterControlRequestIs(
         ENABLED,
         DontSkipInitialOnTime,
         SomeOnTemperatureInDegFx100,
         SomeOffTemperatureInDegFx100);

      TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
   }

   void GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOnWithoutSkippingTheInitalTime()
   {
      GivenIceMakerMoldTemperatureIs(SomeOnTemperatureInDegFx100);
      GivenInitialization();

      WhenIceMakerMoldHeaterControlRequestIs(
         ENABLED,
         DontSkipInitialOnTime,
         SomeOnTemperatureInDegFx100,
         SomeOffTemperatureInDegFx100);

      After(harvestData->initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
      TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
   }

   void GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOffWithoutSkippingTheInitalTime()
   {
      GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100);
      GivenInitialization();

      WhenIceMakerMoldHeaterControlRequestIs(
         ENABLED,
         DontSkipInitialOnTime,
         SomeOnTemperatureInDegFx100,
         SomeOffTemperatureInDegFx100);

      After(harvestData->initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
      TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
   }
};

TEST(IceMakerMoldHeaterController, ShouldTurnOnMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTime)
{
   GivenInitialization();
   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);

   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOnMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeButMoldTemperatureIsGreaterThanMotorOnTempAndLessThanRequestedOnTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOnTemperatureInDegFx100 - 1);
   GivenInitialization();
   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);

   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOnMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeButMoldTemperatureIsGreaterThanMotorOnTempAndGreaterThanRequestedOnTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOnTemperatureInDegFx100 + 1);
   GivenInitialization();
   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);

   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOnMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeButMoldTemperatureIsGreaterThanMotorOnTempAndLessThanRequestedOffTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100 - 1);
   GivenInitialization();
   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);

   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeButMoldTemperatureIsGreaterThanMotorOnTempAndGreaterThanRequestedOffTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100 + 1);
   GivenInitialization();
   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);

   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldKeepMoldHeaterOffWhenIceMoldTemperatureIsOffAndStillLessThanOffTemperature)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOff();
   GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100 - 1);

   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldKeepMoldHeaterOnWhenIceMoldTemperatureIsGreaterThanTheRequestedOnTemperature)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOn();
   GivenIceMakerMoldTemperatureIs(SomeOnTemperatureInDegFx100 + 1);

   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffMoldHeaterWhenIceMoldTemperatureIsGreaterThanRequestedOffTemperature)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOn();
   GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100 + 1);

   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffMoldHeaterWhenIceMakerMoldHeaterControlRequestIsDisable)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOn();
   WhenIceMakerMoldHeaterControlRequestIsDisabled();

   TheMoldHeaterVoteShouldBeDontCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeAfterSomeInitialMotorOnTimeAndTheMoldIceMakerTemperatureIsGreaterThanTheOnTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOtherOffTemperatureInDegFx100);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOtherOnTemperatureInDegFx100,
      SomeOtherOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   After((harvestData->initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC) - 1);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   After(1);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldKeepMoldHeaterOnWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeAfterSomeInitialMotorOnTimeAndTheMoldIceMakerTemperatureIsLessThanTheOffTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOtherOffTemperatureInDegFx100 - 1);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOtherOnTemperatureInDegFx100,
      SomeOtherOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   After((harvestData->initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC) - 1);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   After(1);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldNotTurnOffMoldHeaterDuringTheInitalTimeWhenIceMakerMoldTemperatureChangesToAnOffTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOtherOffTemperatureInDegFx100 - 1);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOtherOnTemperatureInDegFx100,
      SomeOtherOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOtherOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesSkipInitialOnTimeIfMoldTemperatureIsGreaterThanRequestedOffTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100 + 1);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      SkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOnMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesSkipInitialOnTimeIfMoldTemperatureIsLessThanRequestedOnTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOnTemperatureInDegFx100 - 1);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      SkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOnMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesSkipInitialOnTimeIfMoldTemperatureIsTheSameAsTheRequestedOnTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOnTemperatureInDegFx100);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      SkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesSkipInitialOnTimeIfMoldTemperatureIsTheSameAsTheRequestedOffTemperature)
{
   GivenIceMakerMoldTemperatureIs(SomeOffTemperatureInDegFx100);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      SkipInitialOnTime,
      SomeOnTemperatureInDegFx100,
      SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffAndNotCareAboutMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeIfIceMakerMoldHeaterControlIsRequestedToDisable)
{
   GivenIceMakerMoldTemperatureIs(SomeOtherOffTemperatureInDegFx100);
   GivenInitialization();

   WhenIceMakerMoldHeaterControlRequestIs(
      ENABLED,
      DontSkipInitialOnTime,
      SomeOtherOnTemperatureInDegFx100,
      SomeOtherOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   WhenIceMakerMoldHeaterControlRequestIsDisabled();
   TheMoldHeaterVoteShouldBeDontCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffAndNotCareAboutMoldHeaterWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesSkipInitialOnTimeIfIceMakerMoldHeaterControlIsRequestedToDisable)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOn();

   WhenIceMakerMoldHeaterControlRequestIsDisabled();
   TheMoldHeaterVoteShouldBeDontCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldTurnOffAndNotCareAboutMoldHeaterWhenIceMakerMoldHeaterControlRequestIsDisabledAndDoesSkipInitialOnTimeIfIceMakerMoldHeaterControlIsRequestedToDisable)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOff();

   WhenIceMakerMoldHeaterControlRequestIsDisabled();
   TheMoldHeaterVoteShouldBeDontCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldToggleMoldHeaterOnAndOffWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesSkipInitialOnTimeAsMoldTemperatureChangesFromAnOnTemperatureToAnOffTemperature)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOn();

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOnTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldToggleMoldHeaterOffAndOnWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesSkipInitialOnTimeAsMoldTemperatureChangesFromAnOffTemperatureToAnOnTemperature)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOff();

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOnTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(IceMakerMoldHeaterController, ShouldToggleMoldHeaterOnAndOffWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeAsMoldTemperatureChangesFromAnOnTemperatureToAnOffTemperature)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOnWithoutSkippingTheInitalTime();

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOnTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
}

TEST(IceMakerMoldHeaterController, ShouldToggleMoldHeaterOffAndOnWhenIceMakerMoldHeaterControlRequestIsEnabledAndDoesNotSkipInitialOnTimeAsMoldTemperatureChangesFromAnOffTemperatureToAnOnTemperature)
{
   GivenTheIceMakerMoldHeaterControllerIsInitializedAndTheMoldHeaterIsVotedToBeOffWithoutSkippingTheInitalTime();

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOnTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   WhenTheIceMakerMoldTemperatureChangesTo(SomeOffTemperatureInDegFx100);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}
