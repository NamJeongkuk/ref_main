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
   SkipInitialOnTimeEnabled = true,
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
   const IceMakerFillMonitorData_t *iceMakerFillMonitorData;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   EventSubscription_t harvestCountIsReadyToHarvestSubscription;
   EventSubscription_t feelerArmIsReadyToHarvestSubscription;
   EventSubscription_t rakeCompletedRevolutionSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;
      iceMakerFillMonitorData = iceMakerData->fillData.iceMakerFillMonitorData;
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
         Erd_IceMakerEnabledByGrid,
         &state);
      DataModel_Write(
         dataModel,
         Erd_IceMaker0EnableStatus,
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

   void TheMoldHeaterVoteShouldBe(bool expectedState, Vote_t expectedCare)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
         &actual);

      CHECK_EQUAL(expectedState, actual.state);
      CHECK_EQUAL(expectedCare, actual.care);
   }

   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(
         dataModel,
         Erd_FillTubeHeater_AluminumMoldIceMakerVote,
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
      TheMoldHeaterVoteShouldBe(ON, Vote_Care);
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

   void GivenIceMakerIsInHarvestFixState()
   {
      GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   }

   void GivenIceMakerIsInFillState()
   {
      GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

      After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);

      RakeControllerRequestShouldBe(SET);

      WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
      WhenTheRakePositionIs(RakePosition_NotHome);

      After(iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

      WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);

      After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

      WhenTheRakePositionIs(RakePosition_Home);
      RakeCompletedRevolutionShouldBe(SET);

      After((iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds - iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds - iceMakerData->harvestData.feelerArmTestTimeInSeconds - iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds) * MSEC_PER_SEC - 1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
      IceMakerWaterValveRelayShouldBe(WaterValveState_Off);
      TheIsolationWaterValveRelayShouldBe(WaterValveState_Off);

      After(1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
      IceMakerWaterValveRelayShouldBe(WaterValveState_On);
      TheIsolationWaterValveRelayShouldBe(WaterValveState_On);
   }

   void GivenIceMakerIsInHarvestFaultState()
   {
      GivenIceMakerIsInHarvestFixState();

      After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
   }

   void SkipFillRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerSkipFillRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
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
         Erd_AluminumMoldIceMaker_HarvestCountIsReadyToHarvest,
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
         Erd_AluminumMoldIceMaker_HarvestCountIsReadyToHarvest,
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

      if(args->erd == Erd_AluminumMoldIceMaker_HarvestCountIsReadyToHarvest)
      {
         const bool *state = (const bool *)args->data;

         mock().actualCall("HarvestCountReadyHasChanged").withParameter("state", *state);
      }
   }

   static void RakeCompletedRevolutionHasChangedStates(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_AluminumMoldIceMakerRakeCompletedRevolution)
      {
         const bool *state = (const bool *)args->data;

         mock().actualCall("RakeCompletedRevolutionHasChanged").withParameter("state", *state);
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

   void GivenRakeCompletedRevolutionSubscriptionIsInitializedAndSubscribedTo()
   {
      EventSubscription_Init(
         &rakeCompletedRevolutionSubscription,
         NULL,
         RakeCompletedRevolutionHasChangedStates);

      DataModel_SubscribeAll(
         dataModel,
         &rakeCompletedRevolutionSubscription);
   }

   void HarvestCountIsReadyToHarvestShouldChangeTo(bool state)
   {
      mock().expectOneCall("HarvestCountReadyHasChanged").withParameter("state", state);
   }

   void RakeCompletedRevolutionShouldChangeTo(bool state)
   {
      mock().expectOneCall("RakeCompletedRevolutionHasChanged").withParameter("state", state);
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
         Erd_AluminumMoldIceMaker_HarvestCountCalculationRequest,
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

   void TheAluminumMoldIceMakerTestRequestShouldBe(IceMakerTestRequest_t expected)
   {
      IceMakerTestRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerTestRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FeelerArmMonitoringShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_FeelerArmMonitoringRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenFillStops()
   {
      After(iceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   }

   void WhenFillTubeHeaterTimeReachedAfterRakeCompletedFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
      After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   }

   void WhenTheFeelerArmMovesFromBucketNotFullToFullForFeelerArmTestTime()
   {
      WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
      WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);
      After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   }

   void AfterWaitingForMotorOnAndOffTimeToSetRakeControllerRequestAgainDuringHarvestFix()
   {
      RakeCompletedRevolutionShouldChangeTo(false);
      After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC);
      RakeControllerRequestShouldBe(CLEAR);

      After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC);
      RakeControllerRequestShouldBe(SET);
   }

   void WhenTheRakeRotatesFirstTimeDuringHarvestFix()
   {
      RakeControllerRequestShouldBe(SET);
      WhenTheRakePositionIs(RakePosition_NotHome);

      WhenTheFeelerArmMovesFromBucketNotFullToFullForFeelerArmTestTime();

      RakeCompletedRevolutionShouldChangeTo(true);
      WhenTheRakePositionIs(RakePosition_Home);
      RakeCompletedRevolutionShouldBe(SET);
   }

   void WhenTheRakeRotatesTheSecondTimeDuringHarvestFix()
   {
      RakeControllerRequestShouldBe(SET);
      WhenTheRakePositionIs(RakePosition_NotHome);

      WhenTheFeelerArmMovesFromBucketNotFullToFullForFeelerArmTestTime();

      RakeCompletedRevolutionShouldChangeTo(true);
      RakeCompletedRevolutionShouldChangeTo(false);
      WhenTheRakePositionIs(RakePosition_Home);
   }

   void WhenTheRakeRotatesTwoTimesWhileInHarvestFix()
   {
      WhenTheRakeRotatesFirstTimeDuringHarvestFix();

      AfterWaitingForMotorOnAndOffTimeToSetRakeControllerRequestAgainDuringHarvestFix();

      WhenTheRakeRotatesTheSecondTimeDuringHarvestFix();
   }

   void WhenTheRakeRotatesOneTimeDuringHarvest()
   {
      WhenTheRakePositionIs(RakePosition_NotHome);

      WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);
      After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

      RakeCompletedRevolutionShouldChangeTo(true);
      RakeCompletedRevolutionShouldChangeTo(false);
      WhenTheRakePositionIs(RakePosition_Home);
      RakeCompletedRevolutionShouldBe(SET);
   }

   void WhenFeelerArmMovesToBucketNotFullForMinimumFeelerArmExtensionTime()
   {
      WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
      After(iceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
      TheFeelerArmReadyToEnterHarvestShouldBe(true);
   }

   void WhenRakeRotatesOneTimeAfterInitialMinimumHeaterOnTimeDuringHarvest()
   {
      After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
      RakeControllerRequestShouldBe(SET);

      WhenTheRakeRotatesOneTimeDuringHarvest();
   }

   void AfterEnoughTimeHasPassedToReachHarvestCount()
   {
      After((iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes) * MSEC_PER_MIN);
      After(MSEC_PER_SEC);
   }

   void GivenIceMakerIsInFreezeStateAfterHavingBeenInHarvestFix()
   {
      GivenRakeCompletedRevolutionSubscriptionIsInitializedAndSubscribedTo();
      GivenIceMakerIsInHarvestFixState();

      SkipFillRequestShouldBe(SET);
      RakeControllerRequestShouldBe(SET);

      WhenTheRakeRotatesTwoTimesWhileInHarvestFix();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void GivenTheAluminumMoldIceMakerIsInFreezeState()
   {
      GivenIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);

      WhenApplicationHasBeenInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void FreezerTriggerIceRateSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_FreezerIceRateTriggerSignal, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void IceRateActiveErdStateShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_Freezer_IceRateIsActive, &actual);
      CHECK_EQUAL(actual, expected);
   }

   void WhenRakeCompletedRevolution()
   {
      WhenTheRakePositionIs(RakePosition_NotHome);
      After(iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

      WhenTheFeelerArmPositionIs(FeelerArmPosition_BucketFull);
      After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

      WhenTheRakePositionIs(RakePosition_Home);
   }

   void WhenDispensingRequestIs(DispensingAction_t action, DispensingRequestSelection_t selection)
   {
      DispensingRequest_t request = {
         .action = action,
         .selection = selection,
         .specialOptions = DispensingSpecialOptions_EmptyRequest,
         .padding = UINT8_MAX,
         .preciseFillOuncesx100 = UINT16_MAX
      };
      DataModel_Write(dataModel, Erd_DispensingRequest, &request);
   }

   void GivenIceMakerIsInIdleFillState()
   {
      GivenIceMakerIsInFillState();

      WhenDispensingRequestIs(DispensingAction_Start, DispenseSelection_ColdWater);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
   }

   void WhenHarvestFinishes()
   {
      After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);

      RakeControllerRequestShouldBe(Active);

      WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
      WhenTheRakePositionIs(RakePosition_NotHome);

      After(iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

      WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);

      After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

      WhenTheRakePositionIs(RakePosition_Home);
      RakeCompletedRevolutionShouldBe(Active);

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN -
         iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC -
         iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC -
         iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   }
};

TEST(AluminumMoldIceMakerIntegration, ShouldInitialize)
{
   WhenApplicationHasBeenInitialized();
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenInitializedWhileSabbathModeIsEnabledAndIceMakerIsEnabledAndRakeIsHomeAndThermistorIsValid)
{
   GivenSabbathModeIs(ENABLED);
   GivenIceMakerIs(ENABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenInitializedWhileIceMakerIsDisabledAndSabbathModeIsDisabledAndRakeIsHomeAndThermistorIsValid)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenInitializedWhileIceMakerIsDisabledAndSabbathModeIsEnabledAndThermistorIsValidAndRakePositionIsHome)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
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
   After(10 * iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN);
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
   After(iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldActivateFreezerIceRateWhenTheMoldThermistorIsValidInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenTheMoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   TheFreezerIceRateShouldBe(Active);
}

TEST(AluminumMoldIceMakerIntegration, ShouldInactivateFreezerIceRateWhenInitializedIntoFreeze)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);
   TheFreezerIceRateShouldBe(Inactive);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInFreezeWhenSabbathModeEnabled)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInFreezeWhenIceMakerIsDisabled)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
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
   TheMoldHeaterVoteShouldBe(ON, Vote_Care);

   After(1);
   IceMakerHeaterRelayShouldBe(OFF);
   TheMoldHeaterVoteShouldBe(OFF, Vote_Care);
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

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN -
      iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC -
      iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC -
      iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
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

TEST(AluminumMoldIceMakerIntegration, ShouldTurnRakeMotorOnAndOffAccordingToParametricallyDefinedTime)
{
   GivenIceMakerIsInHarvestFixState();
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);

   After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC - 1);
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);

   After(1);
   IceMakerMotorShouldVote(MotorState_Off, Vote_Care);

   After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC - 1);
   IceMakerMotorShouldVote(MotorState_Off, Vote_Care);

   After(1);
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnMoldHeaterOnUponEnteringHarvestFixState)
{
   GivenIceMakerIsInHarvestFixState();

   IceMakerMoldHeaterControlRequestShouldBe(
      ENABLED,
      SkipInitialOnTimeEnabled,
      iceMakerData->harvestFixData.heaterOnTemperatureInDegFx100,
      iceMakerData->harvestFixData.heaterOffTemperatureInDegFx100);

   IceMakerHeaterRelayShouldBe(ON);
   TheMoldHeaterVoteShouldBe(ON, Vote_Care);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromHarvestFixToFreezeStateWhenRakeMotorCompletesTwoRevolutions)
{
   GivenRakeCompletedRevolutionSubscriptionIsInitializedAndSubscribedTo();
   GivenIceMakerIsInHarvestFixState();

   SkipFillRequestShouldBe(SET);
   RakeControllerRequestShouldBe(SET);

   WhenTheRakeRotatesTwoTimesWhileInHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldSetSkipFillAfterTheNextNormalHarvestAfterHavingBeenInHarvestFix)
{
   GivenIceMakerIsInFreezeStateAfterHavingBeenInHarvestFix();

   WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();
   WhenFeelerArmMovesToBucketNotFullForMinimumFeelerArmExtensionTime();

   AfterEnoughTimeHasPassedToReachHarvestCount();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenRakeRotatesOneTimeAfterInitialMinimumHeaterOnTimeDuringHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   IceMakerWaterValveRelayShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOnAndOffMoldHeaterAtParametricallyDefinedTemperaturesInHarvestFixState)
{
   GivenIceMakerIsInHarvestFixState();

   IceMakerMoldHeaterControlRequestShouldBe(
      ENABLED,
      SkipInitialOnTimeEnabled,
      iceMakerData->harvestFixData.heaterOnTemperatureInDegFx100,
      iceMakerData->harvestFixData.heaterOffTemperatureInDegFx100);

   IceMakerHeaterRelayShouldBe(ON);
   TheMoldHeaterVoteShouldBe(ON, Vote_Care);

   WhenAluminumMoldIceMakerTemperatureIs(iceMakerData->harvestFixData.heaterOffTemperatureInDegFx100);
   IceMakerHeaterRelayShouldBe(OFF);

   WhenAluminumMoldIceMakerTemperatureIs(iceMakerData->harvestFixData.heaterOnTemperatureInDegFx100);
   IceMakerHeaterRelayShouldBe(ON);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestFaultStateWhenRakeMotorDoesNotCompleteTheSecondRevolutionWithinMaxHarvestFixTime)
{
   GivenIceMakerIsInHarvestFixState();

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenTheFeelerArmMovesFromBucketNotFullToFullForFeelerArmTestTime();

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(SET);

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldDoTwoNewRevolutionsToExitHarvestFixTheSecondTimeItIsInHarvestFix)
{
   GivenIceMakerIsInHarvestFixState();

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenTheFeelerArmMovesFromBucketNotFullToFullForFeelerArmTestTime();

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(SET);

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);

   After(iceMakerData->harvestFaultData.harvestFaultMaxTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   GivenRakeCompletedRevolutionSubscriptionIsInitializedAndSubscribedTo();

   WhenTheRakeRotatesFirstTimeDuringHarvestFix();

   RakeCompletedRevolutionShouldChangeTo(false);
   After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC - iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);

   WhenTheRakeRotatesTheSecondTimeDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOnIceMakerWaterValveAndIsolationWaterValveWhenStartingIceMakerFill)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);

   RakeControllerRequestShouldBe(SET);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   WhenTheRakePositionIs(RakePosition_NotHome);

   After(iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);

   After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(SET);

   After((iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds - iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds - iceMakerData->harvestData.feelerArmTestTimeInSeconds - iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds) * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   IceMakerWaterValveRelayShouldBe(WaterValveState_Off);
   TheIsolationWaterValveRelayShouldBe(WaterValveState_Off);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IceMakerWaterValveRelayShouldBe(WaterValveState_On);
   TheIsolationWaterValveRelayShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffWaterValveAfterFillFinishes)
{
   GivenIceMakerIsInFillState();
   IceMakerWaterValveRelayShouldBe(WaterValveState_On);
   TheIsolationWaterValveRelayShouldBe(WaterValveState_On);

   WhenFillStops();
   IceMakerWaterValveRelayShouldBe(WaterValveState_Off);
   TheIsolationWaterValveRelayShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenFillFinishesWhileMoldThermistorIsInvalid)
{
   GivenIceMakerIsInFillState();
   GivenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenFillStops();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFreezeWhenFillFinishesWhileRakePositionIsHome)
{
   GivenIceMakerIsInFillState();
   GivenTheRakePositionIs(RakePosition_Home);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenFillStops();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestWhenFillFinishesWhileRakePositionIsNotHome)
{
   GivenIceMakerIsInFillState();
   GivenTheRakePositionIs(RakePosition_NotHome);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenFillStops();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnRakeMotorOnAndOffAccordingToParametricDefinedTimeInHarvestFaultState)
{
   GivenIceMakerIsInHarvestFaultState();
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffMoldHeaterWhenEnteringHarvestFaultState)
{
   GivenIceMakerIsInHarvestFixState();
   IceMakerMoldHeaterControlRequestShouldBe(
      ENABLED,
      SkipInitialOnTimeEnabled,
      iceMakerData->harvestFixData.heaterOnTemperatureInDegFx100,
      iceMakerData->harvestFixData.heaterOffTemperatureInDegFx100);

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
   IceMakerMoldHeaterControlRequestShouldBe(
      DISABLED,
      SkipInitialOnTimeEnabled,
      iceMakerData->harvestFixData.heaterOnTemperatureInDegFx100,
      iceMakerData->harvestFixData.heaterOffTemperatureInDegFx100);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestIfRakeMotorCompletedRevolutionInHarvestFaultState)
{
   GivenIceMakerIsInHarvestFaultState();

   WhenRakeCompletedRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestIfHarvestFaultMaxTimerExpires)
{
   GivenIceMakerIsInHarvestFaultState();

   After(iceMakerData->harvestFaultData.harvestFaultMaxTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestFixIfHarvestMaxTimerExpires)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestFaultIfHarvestFixMaxTimerExpires)
{
   GivenIceMakerIsInHarvestFixState();

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldResetTestRequestWhenFillTestIsRequested)
{
   GivenIceMakerIsInFreezeStateAfterHavingBeenInHarvestFix();

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   TheAluminumMoldIceMakerTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInFillStateIfFillTestIsRequestedWhileInFillState)
{
   GivenIceMakerIsInFillState();

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInFreezeState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInHarvestState)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInHarvestFixState)
{
   GivenIceMakerIsInHarvestFixState();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInHarvestFaultState)
{
   GivenIceMakerIsInHarvestFaultState();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_ThermistorFault);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldResetTestRequestWhenHarvestTestIsRequested)
{
   GivenIceMakerIsInFillState();

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   TheAluminumMoldIceMakerTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestStateIfHarvestTestIsRequestedWhileInFillState)
{
   GivenIceMakerIsInFillState();

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestStateIfHarvestTestIsRequestedWhileInFreezeState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInHarvestStateIfHarvestTestIsRequestedWhileInHarvestState)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInHarvestFixStateIfHarvestTestIsRequestedWhileInHarvestFixState)
{
   GivenIceMakerIsInHarvestFixState();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestIfHarvestTestIsRequestedWhileInHarvestFaultState)
{
   GivenIceMakerIsInHarvestFaultState();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(IceMakerTestRequest_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInThermistorFaultStateIfHarvestTestIsRequestedWhileInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_ThermistorFault);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldActivateIceRateWhenIceRateSignalIsSent)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   GivenSabbathModeIs(ENABLED);
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(1);
   IceRateActiveErdStateShouldBe(ON);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotActivateIceRateWhenIceRateSignalNotSent)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   FreezerTriggerIceRateSignalShouldBe(0);
   IceRateActiveErdStateShouldBe(OFF);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInFreeze)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInHarvest)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInHarvestFix)
{
   GivenIceMakerIsInHarvestFixState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInHarvestFault)
{
   GivenIceMakerIsInHarvestFaultState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInIdleFill)
{
   GivenIceMakerIsInIdleFillState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffRakeMotorWhenTheMoldThermistorIsInvalid)
{
   GivenIceMakerIsInHarvestFixState();
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);

   WhenTheMoldThermistorIsInvalid();
   IceMakerMotorShouldVote(MotorState_Off, Vote_DontCare);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffMoldHeaterWhenTheMoldThermistorIsInvalid)
{
   GivenIceMakerIsInHarvestFixState();
   TheMoldHeaterVoteShouldBe(ON, Vote_Care);

   WhenTheMoldThermistorIsInvalid();
   TheMoldHeaterVoteShouldBe(OFF, Vote_DontCare);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFreezeWhenTheThermistorIsValidWhileRakePositionIsHomeInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_ThermistorFault);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenTheMoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestWhenTheThermistorIsValidWhileRakePositionIsNotHomeInThermistorFaultStateAndThenGoToFreezeStateAfterHarvestingIsFinished)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(AluminumMoldIceMakerHsmState_ThermistorFault);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenTheMoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenHarvestFinishes();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldPauseFillDuringDispensingAndResumeWhenDispensingStops)
{
   GivenIceMakerIsInFillState();

   WhenDispensingRequestIs(DispensingAction_Start, DispenseSelection_ColdWater);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);

   WhenDispensingRequestIs(DispensingAction_Stop, DispenseSelection_ColdWater);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldResumeFillForRemainderOfFillTimeAfterDispensingStops)
{
   GivenIceMakerIsInFillState();

   After((iceMakerData->fillData.iceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * MSEC_PER_SEC) / 10 - 1);

   WhenDispensingRequestIs(DispensingAction_Start, DispenseSelection_ColdWater);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);

   WhenDispensingRequestIs(DispensingAction_Stop, DispenseSelection_ColdWater);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}
