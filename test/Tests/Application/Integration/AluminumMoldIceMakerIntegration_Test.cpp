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
#include "Constants_Time.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "CppUTestExt/MockSupport.h"

#define And

enum
{
   Invalid = false,
   SkipInitialOnTimeDisabled = false,
   Inactive = false,
   Active = true,
   Valid = true,
   InvalidAdcCount = 5375,
   ValidAdcCount = 13967,
   ValidHarvestAdcCount = 42494
};

TEST_GROUP(AluminumMoldIceMakerIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   const AluminumMoldIceMakerData_t *iceMakerData;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   EventSubscription_t harvestCountIsReadyToHarvestSubscription;
   EventSubscription_t feelerArmIsReadyToHarvestSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;
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

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathMode,
         &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_t expectedState)
   {
      AluminumMoldIceMakerHsmState_t actualState;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHsmState,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenIceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledOverrideRequest,
         set);
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledOverrideValue,
         &state);
   }

   void GivenMoldThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistor_AdcCount,
         &count);
   }

   void WhenMoldThermistorAdcCountIs(AdcCounts_t count)
   {
      GivenMoldThermistorAdcCountIs(count);
   }

   void MoldThermistorAdcCountShouldBe(AdcCounts_t expectedCount)
   {
      AdcCounts_t actualCount;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistor_AdcCount,
         &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void MoldThermistorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistorIsValid,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTheMoldThermistorIsInvalid()
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideRequest,
         set);

      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideValue,
         clear);
   }

   void GivenTheMoldThermistorIsInvalid()
   {
      WhenTheMoldThermistorIsInvalid();
   }

   void WhenTheMoldThermistorIsValid()
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideRequest,
         set);

      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideValue,
         set);
   }

   void GivenTheMoldThermistorIsValid()
   {
      WhenTheMoldThermistorIsValid();
   }

   void GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_t state)
   {
      GivenIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);

      if(state == AluminumMoldIceMakerHsmState_Freeze)
      {
         GivenMoldThermistorAdcCountIs(ValidAdcCount);
         GivenTheRakePositionIs(RakePosition_Home);
      }
      else if(state == AluminumMoldIceMakerHsmState_Harvest)
      {
         GivenMoldThermistorAdcCountIs(ValidAdcCount);
         GivenTheRakePositionIs(RakePosition_NotHome);
      }
      else if(state == AluminumMoldIceMakerHsmState_ThermistorFault)
      {
         GivenMoldThermistorAdcCountIs(InvalidAdcCount);
      }
      WhenApplicationHasBeenInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(state);
   }

   void WhenApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_t state)
   {
      GivenTheApplicationHasBeenInitializedAndEntersState(state);
   }

   void WhenTheIceMakerIs(bool state)
   {
      GivenIceMakerIs(state);
   }

   void GivenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerRakePosition,
         &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerRakePosition,
         &position);
   }

   void IceMakerWaterValveShouldVote(WaterValveState_t expectedState, Vote_t expectedVoteCare)
   {
      WaterValveVotedState_t actualVote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
         &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_EQUAL(expectedVoteCare, actualVote.care);
   }

   void IceMakerWaterValveRelayShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerWaterValveRelay,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void IceMakerHeaterShouldVote(HeaterState_t expectedState, Vote_t expectedVoteCare)
   {
      HeaterVotedState_t actualVote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
         &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_EQUAL(expectedVoteCare, actualVote.care);
   }

   void IceMakerHeaterRelayShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHeaterRelay,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void IceMakerMotorShouldVote(MotorState_t expectedState, Vote_t expectedVoteCare)
   {
      AluminumMoldIceMakerMotorVotedState_t actualVote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
         &actualVote);

      CHECK_EQUAL(expectedState, actualVote.state);
      CHECK_EQUAL(expectedVoteCare, actualVote.care);
   }

   void IceMakerMotorRelayShouldVote(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_IceMakerRakeMotorRelay,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void AluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerTestRequest,
         &request);
   }

   void RakeControllerRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerRakeControlRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void WhenAluminumMoldIceMakerTemperatureIs(TemperatureDegFx100_t temperature)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void WhenFeelerArmPositionIs(FeelerArmPosition_t state)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerFeelerArmPosition,
         &state);
   }

   void IceMakerMoldHeaterControlRequestShouldBe(
      bool enable,
      bool skipInitialOnTime,
      TemperatureDegFx100_t onTemperature,
      TemperatureDegFx100_t offTemperature)
   {
      IceMakerMoldHeaterControlRequest_t expectedRequest = {
         .enable = enable,
         .skipInitialOnTime = skipInitialOnTime,
         .onTemperatureInDegFx100 = onTemperature,
         .offTemperatureInDegFx100 = offTemperature,
      };
      IceMakerMoldHeaterControlRequest_t actualRequest;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
         &actualRequest);

      MEMCMP_EQUAL(&expectedRequest, &actualRequest, sizeof(expectedRequest));
   }

   void TheMoldHeaterVoteShouldBeCareAnd(bool expected)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
         &actual);

      CHECK_EQUAL(expected, actual.state);
      CHECK_TRUE(actual.care);
   }

   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote,
         &actual);

      CHECK_EQUAL(expectedDutyCycle, actual.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void RakeCompletedRevolutionShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerRakeCompletedRevolution,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheIsolationWaterValveRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IsolationValveRelay,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheIceMakerRakeMotorRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IceMakerRakeMotorRelay,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheFeelerArmPositionIs(FeelerArmPosition_t position)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerFeelerArmPosition,
         &position);
   }

   void GivenIceMakerHasEnteredHarvestAndFillTubeAndMoldHeatersHaveBeenVotedOn()
   {
      GivenIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenMoldThermistorAdcCountIs(ValidHarvestAdcCount);
      WhenTheRakePositionIs(RakePosition_NotHome);

      WhenApplicationHasBeenInitialized();
      MoldThermistorShouldBe(Valid);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      IceMakerMoldHeaterControlRequestShouldBe(
         ENABLED,
         SkipInitialOnTimeDisabled,
         iceMakerData->harvestData.heaterOnTemperatureInDegFx100,
         iceMakerData->harvestData.heaterOffTemperatureInDegFx100);

      FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
      IceMakerHeaterRelayShouldBe(ON);
      TheMoldHeaterVoteShouldBeCareAnd(ENABLED);
   }

   void GivenIceMakerIsInHarvestStateAndRakeIsNotHome()
   {
      GivenIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenMoldThermistorAdcCountIs(ValidHarvestAdcCount);
      WhenTheRakePositionIs(RakePosition_NotHome);

      WhenApplicationHasBeenInitialized();
      MoldThermistorShouldBe(Valid);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void WhenTheFeelerArmReadyToEnterHarvestIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         &state);
   }

   void TheFeelerArmReadyToEnterHarvestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFeelerArmIsReadyToEnterHarvest()
   {
      WhenTheFeelerArmReadyToEnterHarvestIs(Active);
   }

   void WhenTheHarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void GivenTheHarvestCountIsReadyToHarvest()
   {
      WhenTheHarvestCountIsReadyToHarvest();
   }

   void TheHarvestCountReadyToHarvestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheHarvestCountReadyShouldNotChange()
   {
      mock().expectNoCall("HarvestCountReadyHasChanged");
   }

   static void HarvestCountReadyHasChangedState(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_HarvestCountIsReadyToHarvest)
      {
         const bool *state = (const bool *)args->data;

         mock().actualCall("HarvestCountReadyHasChanged").withParameter("state", *state);
      }
   }

   void GivenHarvestCountReadySubscriptionIsInitializedAndSubscribedTo()
   {
      EventSubscription_Init(
         &harvestCountIsReadyToHarvestSubscription,
         NULL,
         HarvestCountReadyHasChangedState);

      DataModel_SubscribeAll(
         dataModel,
         &harvestCountIsReadyToHarvestSubscription);
   }

   void HarvestCountIsReadyToHarvestShouldChangeTo(bool state)
   {
      mock().expectOneCall("HarvestCountReadyHasChanged").withParameter("state", state);
   }

   void TheFeelerArmIsReadyToHarvestShouldNotChange()
   {
      mock().expectNoCall("FeelerArmReadyToHarvestHasChanged");
   }

   void TheFeelerArmIsReadyToHarvestShouldChangeTo(bool state)
   {
      mock().expectOneCall("FeelerArmReadyToHarvestHasChanged").withParameter("state", state);
   }

   static void FeelerArmIsReadyToHarvestHasChangedStates(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_FeelerArmIsReadyToEnterHarvest)
      {
         const bool *state = (const bool *)args->data;

         mock().actualCall("FeelerArmReadyToHarvestHasChanged").withParameter("state", *state);
      }
   }

   void GivenFeelerArmIsReadyToHarvestSubscriptionIsInitializedAndSubscribedTo()
   {
      EventSubscription_Init(
         &feelerArmIsReadyToHarvestSubscription,
         NULL,
         FeelerArmIsReadyToHarvestHasChangedStates);

      DataModel_SubscribeAll(
         dataModel,
         &feelerArmIsReadyToHarvestSubscription);
   }

   void TheHarvestCountCalculationRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_HarvestCountCalculationRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 + 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest,
         set);

      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100,
         &temperature);
   }

   void GivenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature()
   {
      WhenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature();
   }

   void WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 100;

      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest,
         set);

      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100,
         &temperature);
   }

   void GivenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature()
   {
      WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();
   }

   void TheFreezerIceRateShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_Freezer_IceRateIsActive,
         &actual);

      CHECK_EQUAL(expected, actual);
   };

   void WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerTestRequest,
         &request);
   }
};

TEST(AluminumMoldIceMakerIntegration, ShouldInitialize)
{
   WhenApplicationHasBeenInitialized();
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenSabbathModeIsEnabled)
{
   GivenSabbathModeIs(ENABLED);
   GivenIceMakerIs(ENABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabled)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabledAndSabbathModeIsEnabled)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInThermistorFaultStateWhenMoldThermistorIsInvalidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenRakePositionIsHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInHarvestStateWhenRakePositionIsNotHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotTransitionFromFreezeToHarvestUntilIceMakerTemperatureDropsBelowMaximumHarvestTemperature)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);
   GivenHarvestCountReadySubscriptionIsInitializedAndSubscribedTo();
   GivenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature();

   TheHarvestCountReadyShouldNotChange();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   TheHarvestCountCalculationRequestShouldBe(Active);

   HarvestCountIsReadyToHarvestShouldChangeTo(Active);
   After(10 * iceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   HarvestCountIsReadyToHarvestShouldChangeTo(Inactive);
   WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotTransitionFromFreezeToHarvestUntilBucketIsNotFullForMinimumExtensionTime)
{
   GivenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);
   GivenFeelerArmIsReadyToHarvestSubscriptionIsInitializedAndSubscribedTo();
   GivenTheHarvestCountIsReadyToHarvest();

   TheFeelerArmReadyToEnterHarvestShouldBe(Inactive);
   TheFeelerArmIsReadyToHarvestShouldNotChange();
   WhenTheFeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(iceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   After(iceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   TheFeelerArmIsReadyToHarvestShouldChangeTo(Active);
   TheFeelerArmIsReadyToHarvestShouldChangeTo(Inactive);
   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromFreezeToHarvestIfIceMakerMoldTemperaturesIsBelowMaximumHarvestTemperatureAndMinimumFreezeTimeIsElapsed)
{
   GivenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();
   GivenFeelerArmIsReadyToEnterHarvest();

   WhenApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);
   After(iceMakerData->freezeData.minimumFreezeTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldActivateFreezerIceRateWhenTheMoldThermistorIsValidInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenTheMoldThermistorIsValid();
   TheFreezerIceRateShouldBe(Active);
}

TEST(AluminumMoldIceMakerIntegration, ShouldInactivateFreezerIceRateWhenInitializedIntoFreeze)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);
   TheFreezerIceRateShouldBe(Inactive);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromFreezeToIdleFreezeWhenSabbathModeEnabled)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);
   GivenSabbathModeIs(ENABLED);

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromFreezeToIdleFreezeWhenIceMakerIsDisabled)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToThermistorFaultStateWhenIceMakerBecomesEnabledButThermistorIsInvalid)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToThermistorFaultStateWhenSabbathModeBecomesDisabledButThermistorIsInvalid)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToHarvestStateWhenIceMakerBecomesEnabledAndSabbathModeIsDisabledAndThermistorIsValidAndRakePositionIsNotHome)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenApplicationHasBeenInitialized();

   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToHarvestStateWhenSabbathModeBecomesDisabledAndIceMakerIsEnabledAndThermistorIsValidAndRakePositionIsNotHome)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenApplicationHasBeenInitialized();

   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenIceMakerBecomesEnabledAndSabbathModeIsDisabledAndThermistorIsValidAndRakePositionIsHome)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);
   GivenApplicationHasBeenInitialized();

   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenSabbathModeBecomesDisabledAndIceMakerIsEnabledAndThermistorIsValidAndRakePositionIsHome)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);
   GivenApplicationHasBeenInitialized();

   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffIceMakerWaterValveAndHeaterWhenEnteringStateIdleFreeze)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IceMakerWaterValveRelayShouldBe(WaterValveState_On);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   IceMakerWaterValveShouldVote(WaterValveState_Off, Vote_DontCare);
   IceMakerHeaterShouldVote(HeaterState_Off, Vote_DontCare);

   IceMakerWaterValveRelayShouldBe(OFF);
   IceMakerHeaterRelayShouldBe(OFF);
   IceMakerMoldHeaterControlRequestShouldBe(
      DISABLED,
      DISABLED,
      OFF,
      OFF);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffIceMakerRakeMotorWhenEnteringStateIdleFreeze)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(Inactive);

   After(1);
   RakeControllerRequestShouldBe(Active);
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   IceMakerMotorShouldVote(MotorState_Off, Vote_DontCare);
   TheIceMakerRakeMotorRelayShouldBe(OFF);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOnMoldHeaterAndFillTubeHeaterAtDutyCycleUponEnteringHarvestState)
{
   GivenIceMakerHasEnteredHarvestAndFillTubeAndMoldHeatersHaveBeenVotedOn();
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOnAndOffMoldHeaterAtParametricallyDefinedTemperatures)
{
   GivenIceMakerHasEnteredHarvestAndFillTubeAndMoldHeatersHaveBeenVotedOn();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);

   WhenAluminumMoldIceMakerTemperatureIs(iceMakerData->harvestData.heaterOffTemperatureInDegFx100);

   IceMakerHeaterRelayShouldBe(ON);
   TheMoldHeaterVoteShouldBeCareAnd(ENABLED);

   After(1);
   IceMakerHeaterRelayShouldBe(OFF);
   TheMoldHeaterVoteShouldBeCareAnd(DISABLED);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOnTheRakeMotorAfterMoldHeaterHasBeenOnForParametricallyDefinedTime)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   IceMakerMoldHeaterControlRequestShouldBe(
      ENABLED,
      DISABLED,
      iceMakerData->harvestData.heaterOnTemperatureInDegFx100,
      iceMakerData->harvestData.heaterOffTemperatureInDegFx100);

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   IceMakerMotorShouldVote(MotorState_Off, Vote_DontCare);

   After(1);
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFreezeStateWhenRakeCompletedRevolutionFlagHasBeenSet)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);

   RakeControllerRequestShouldBe(Active);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   WhenTheRakePositionIs(RakePosition_NotHome);

   After(iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);

   After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(Active);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC - iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFreezeStateWhenFillTubeTimerHasExpiredAndRakeCompletedRevolutionFlagHasBeenSet)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);

   RakeControllerRequestShouldBe(Active);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);

   After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(Active);

   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}
