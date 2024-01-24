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
   ValidAdcCount = 43904,
   ValidHarvestAdcCount = 42494,
   AdcCountAboveMaximumHarvestTemperature = 47104,
   AdcCountBelowMaximumHarvestTemperature = 50112
};

TEST_GROUP(AluminumMoldIceMakerIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   const AluminumMoldIceMakerData_t *iceMakerData;
   const IceMakerFillMonitorData_t *iceMakerFillMonitorData;
   const SensorDataSensorType_t *moldThermistorData;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   EventSubscription_t harvestCountIsReadyToHarvestSubscription;
   EventSubscription_t feelerArmIsReadyToHarvestSubscription;
   EventSubscription_t rakeCompletedRevolutionSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot1Data->aluminumMoldData;
      iceMakerFillMonitorData = iceMakerData->fillData.iceMakerFillMonitorData;
      moldThermistorData = PersonalityParametricData_Get(dataModel)->sensorData->iceMaker1MoldThermistor;
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
         Erd_SabbathModeEnable,
         &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_t expectedState)
   {
      IceMakerStateMachineState_t actualState;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_StateMachineState,
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
         Erd_IceMaker1_EnableStatus,
         &state);
   }

   void GivenMoldThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_MoldThermistor_AdcCount,
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
         Erd_IceMaker1_MoldThermistor_AdcCount,
         &actualCount);

      CHECK_EQUAL(expectedCount, actualCount);
   }

   void MoldThermistorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_MoldThermistorIsValid,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void WhenTheMoldThermistorIsInvalid()
   {
      AdcCounts_t invalidCount = InvalidAdcCount;
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_MoldThermistor_AdcCount,
         &invalidCount);

      After(moldThermistorData->badReadingCounterMax * MSEC_PER_SEC);
   }

   void GivenTheMoldThermistorIsInvalid()
   {
      WhenTheMoldThermistorIsInvalid();
   }

   void WhenTheMoldThermistorIsValid()
   {
      AdcCounts_t validCount = ValidAdcCount;
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_MoldThermistor_AdcCount,
         &validCount);

      After(moldThermistorData->badReadingCounterMax * MSEC_PER_SEC);
   }

   void GivenTheMoldThermistorIsValid()
   {
      WhenTheMoldThermistorIsValid();
   }

   void GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_t state)
   {
      GivenIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);

      if(state == IceMakerStateMachineState_Freeze)
      {
         GivenMoldThermistorAdcCountIs(ValidAdcCount);
         GivenTheRakePositionIs(RakePosition_Home);
      }
      else if(state == IceMakerStateMachineState_Harvest)
      {
         GivenMoldThermistorAdcCountIs(ValidAdcCount);
         GivenTheRakePositionIs(RakePosition_NotHome);
      }
      else if(state == IceMakerStateMachineState_ThermistorFault)
      {
         GivenMoldThermistorAdcCountIs(InvalidAdcCount);
      }
      WhenApplicationHasBeenInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(state);
   }

   void WhenApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_t state)
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
         Erd_IceMaker0_RakePosition,
         &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_RakePosition,
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

   void IceMakerHeaterRelayShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_HeaterRelay,
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

   void RakeControllerRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_RakeControlRequest,
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
         Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void GivenFeelerArmPositionIs(FeelerArmPosition_t state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_FeelerArmPosition,
         &state);
   }

   void WhenFeelerArmPositionIs(FeelerArmPosition_t state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_FeelerArmPosition,
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
         Erd_IceMaker0_MoldHeaterControlRequest,
         &actualRequest);

      MEMCMP_EQUAL(&expectedRequest, &actualRequest, sizeof(expectedRequest));
   }

   void TheMoldHeaterVoteShouldBe(bool expectedState, Vote_t expectedCare)
   {
      HeaterVotedState_t actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_HeaterRelay_IceMakerVote,
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
         Erd_IceMaker0_RakeCompletedRevolution,
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

   void WhenTheFeelerArmPositionIs(FeelerArmPosition_t position)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_FeelerArmPosition,
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

      IceMakerMoldHeaterControlRequestShouldBe(
         ENABLED,
         SkipInitialOnTimeDisabled,
         iceMakerData->harvestData.heaterOnTemperatureInDegFx100,
         iceMakerData->harvestData.heaterOffTemperatureInDegFx100);

      FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
   }

   void GivenIceMakerIsInHarvestFixState()
   {
      GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
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

      After((iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds - iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds - iceMakerData->harvestData.feelerArmTestTimeInSeconds - iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds) * MSEC_PER_SEC - 1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
      IceMakerWaterValveRelayShouldBe(WaterValveState_Off);
      TheIsolationWaterValveRelayShouldBe(WaterValveState_Off);

      After(1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
      IceMakerWaterValveRelayShouldBe(WaterValveState_On);
      TheIsolationWaterValveRelayShouldBe(WaterValveState_On);
   }

   void GivenIceMakerIsInHarvestFaultState()
   {
      GivenIceMakerIsInHarvestFixState();

      After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
   }

   void SkipFillRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_SkipFillRequest,
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
         Erd_IceMaker1_HarvestCountIsReadyToHarvest,
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
         Erd_IceMaker1_HarvestCountIsReadyToHarvest,
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

      if(args->erd == Erd_IceMaker1_HarvestCountIsReadyToHarvest)
      {
         const bool *state = (const bool *)args->data;

         mock().actualCall("HarvestCountReadyHasChanged").withParameter("state", *state);
      }
   }

   static void RakeCompletedRevolutionHasChangedStates(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_IceMaker0_RakeCompletedRevolution)
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
         Erd_IceMaker1_HarvestCountCalculationRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature()
   {
      AdcCounts_t count = AdcCountAboveMaximumHarvestTemperature;
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_MoldThermistor_AdcCount,
         &count);
   }

   void GivenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature()
   {
      WhenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature();
   }

   void WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature()
   {
      AdcCounts_t count = AdcCountBelowMaximumHarvestTemperature;
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_MoldThermistor_AdcCount,
         &count);
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
         Erd_IceMaker1_TestRequest,
         &request);
   }

   void TheAluminumMoldIceMakerTestRequestShouldBe(IceMakerTestRequest_t expected)
   {
      IceMakerTestRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_TestRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FeelerArmMonitoringShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_FeelerArmMonitoringRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenFillStops()
   {
      After(iceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * 100);
   }

   void WhenFillTubeHeaterTimeReachedAfterRakeCompletedFullRevolution()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, set);
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenFreezerIceRateIsNotActive()
   {
      DataModel_Write(dataModel, Erd_Freezer_IceRateIsActive, clear);
   }

   void GivenTheAluminumMoldIceMakerIsInFreezeState()
   {
      GivenIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
      GivenFreezerIceRateIsNotActive();

      WhenApplicationHasBeenInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenInitializedWhileIceMakerIsDisabledAndSabbathModeIsDisabledAndRakeIsHomeAndThermistorIsValid)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenInitializedWhileIceMakerIsDisabledAndSabbathModeIsEnabledAndThermistorIsValidAndRakePositionIsHome)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInThermistorFaultStateWhenMoldThermistorIsInvalidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenRakePositionIsHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInHarvestStateWhenRakePositionIsNotHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotTransitionFromFreezeToHarvestUntilIceMakerTemperatureDropsBelowMaximumHarvestTemperature)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);
   GivenHarvestCountReadySubscriptionIsInitializedAndSubscribedTo();
   GivenTheIceMakerMoldTemperatureIsAboveMaximumHarvestAndBelowFreezeIntegrationTemperature();

   TheHarvestCountReadyShouldNotChange();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   TheHarvestCountCalculationRequestShouldBe(Active);

   HarvestCountIsReadyToHarvestShouldChangeTo(Active);
   After(10 * iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   HarvestCountIsReadyToHarvestShouldChangeTo(Inactive);
   WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();

   After(MSEC_PER_SEC);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotTransitionFromFreezeToHarvestUntilBucketIsNotFullForMinimumExtensionTime)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);
   GivenFeelerArmIsReadyToHarvestSubscriptionIsInitializedAndSubscribedTo();
   GivenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();
   GivenTheHarvestCountIsReadyToHarvest();

   TheFeelerArmReadyToEnterHarvestShouldBe(Inactive);
   TheFeelerArmIsReadyToHarvestShouldNotChange();
   WhenTheFeelerArmPositionIs(FeelerArmPosition_BucketFull);
   After(iceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenTheFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   After(iceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   TheFeelerArmIsReadyToHarvestShouldChangeTo(Active);
   TheFeelerArmIsReadyToHarvestShouldChangeTo(Inactive);
   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromFreezeToHarvestIfIceMakerMoldTemperaturesIsBelowMaximumHarvestTemperatureAndMinimumFreezeTimeIsElapsed)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);

   WhenTheFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   After(iceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN);

   WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();

   After(
      iceMakerData->freezeData.harvestCountCalculatorData->minimumFreezeTimeInMinutes * MSEC_PER_MIN -
      iceMakerData->freezeData.minimumFeelerArmExtensionTimeInMinutes * MSEC_PER_MIN -
      1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldActivateFreezerIceRateWhenTheMoldThermistorIsValidInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_ThermistorFault);

   WhenTheMoldThermistorIsValid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   TheFreezerIceRateShouldBe(Active);
}

TEST(AluminumMoldIceMakerIntegration, ShouldInactivateFreezerIceRateWhenInitializedIntoFreeze)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);
   TheFreezerIceRateShouldBe(Inactive);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInFreezeWhenSabbathModeEnabled)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInFreezeWhenIceMakerIsDisabled)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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

   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_DontCare);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldSetSkipFillAfterTheNextNormalHarvestAfterHavingBeenInHarvestFix)
{
   GivenIceMakerIsInFreezeStateAfterHavingBeenInHarvestFix();

   WhenTheIceMakerMoldTemperatureIsBelowMaximumHarvestTemperature();
   WhenFeelerArmMovesToBucketNotFullForMinimumFeelerArmExtensionTime();

   AfterEnoughTimeHasPassedToReachHarvestCount();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenRakeRotatesOneTimeAfterInitialMinimumHeaterOnTimeDuringHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldDoTwoNewRevolutionsToExitHarvestFixTheSecondTimeItIsInHarvestFix)
{
   GivenIceMakerIsInHarvestFixState();

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenTheFeelerArmMovesFromBucketNotFullToFullForFeelerArmTestTime();

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(SET);

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);

   After(iceMakerData->harvestFaultData.harvestFaultMaxTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   GivenRakeCompletedRevolutionSubscriptionIsInitializedAndSubscribedTo();

   WhenTheRakeRotatesFirstTimeDuringHarvestFix();

   RakeCompletedRevolutionShouldChangeTo(false);
   After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC - iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);

   WhenTheRakeRotatesTheSecondTimeDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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

   After((iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds - iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds - iceMakerData->harvestData.feelerArmTestTimeInSeconds - iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds) * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
   IceMakerWaterValveRelayShouldBe(WaterValveState_Off);
   TheIsolationWaterValveRelayShouldBe(WaterValveState_Off);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenTheMoldThermistorIsInvalid();
   WhenFillStops();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFreezeWhenFillFinishesWhileRakePositionIsHome)
{
   GivenIceMakerIsInFillState();
   GivenTheRakePositionIs(RakePosition_Home);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenFillStops();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestWhenFillFinishesWhileRakePositionIsNotHome)
{
   GivenIceMakerIsInFillState();
   GivenTheRakePositionIs(RakePosition_NotHome);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenFillStops();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestIfHarvestFaultMaxTimerExpires)
{
   GivenIceMakerIsInHarvestFaultState();

   After(iceMakerData->harvestFaultData.harvestFaultMaxTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestFixIfHarvestMaxTimerExpires)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestFaultIfHarvestFixMaxTimerExpires)
{
   GivenIceMakerIsInHarvestFixState();

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInFreezeState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInHarvestState)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInHarvestFixState)
{
   GivenIceMakerIsInHarvestFixState();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFillStateIfFillTestIsRequestedWhileInHarvestFaultState)
{
   GivenIceMakerIsInHarvestFaultState();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotTransitionToFillStateIfFillTestIsRequestedWhileInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_ThermistorFault);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestStateIfHarvestTestIsRequestedWhileInFreezeState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_Freeze);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInHarvestStateIfHarvestTestIsRequestedWhileInHarvestState)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromHarvestFixtoHarvestStateWhenHarvestTestIsRequested)
{
   GivenIceMakerIsInHarvestFixState();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestIfHarvestTestIsRequestedWhileInHarvestFaultState)
{
   GivenIceMakerIsInHarvestFaultState();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerTestRequest_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStayInThermistorFaultStateIfHarvestTestIsRequestedWhileInThermistorFaultState)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_ThermistorFault);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);

   WhenTheAluminumMoldIceMakerTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldActivateIceRateAndSendIceRateSignalWhenSabbathModeIsDisabledDuringFreezeStateAndHarvestConditionsAreNotMet)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   GivenSabbathModeIs(ENABLED);
   GivenIceMakerIs(ENABLED);

   WhenSabbathModeIs(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(1);
   IceRateActiveErdStateShouldBe(ON);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotActivateIceRateWhileTheInitialStateIsFreeze)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   FreezerTriggerIceRateSignalShouldBe(0);
   IceRateActiveErdStateShouldBe(OFF);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInFreeze)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInHarvest)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInHarvestFix)
{
   GivenIceMakerIsInHarvestFixState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInHarvestFault)
{
   GivenIceMakerIsInHarvestFaultState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldNotTransitionToThermistorFaultWhenTheMoldThermistorIsInvalidWhileInIdleFill)
{
   GivenIceMakerIsInIdleFillState();

   WhenTheMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
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
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_ThermistorFault);
   GivenTheRakePositionIs(RakePosition_Home);

   WhenTheMoldThermistorIsValid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToHarvestWhenTheThermistorIsValidWhileRakePositionIsNotHomeInThermistorFaultStateAndThenGoToFreezeStateAfterHarvestingIsFinished)
{
   GivenTheApplicationHasBeenInitializedAndEntersState(IceMakerStateMachineState_ThermistorFault);
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenTheMoldThermistorIsValid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenHarvestFinishes();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldPauseFillDuringDispensingAndResumeWhenDispensingStops)
{
   GivenIceMakerIsInFillState();

   WhenDispensingRequestIs(DispensingAction_Start, DispenseSelection_ColdWater);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);

   WhenDispensingRequestIs(DispensingAction_Stop, DispenseSelection_ColdWater);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMakerIntegration, ShouldResumeFillForRemainderOfFillTimeAfterDispensingStops)
{
   GivenIceMakerIsInFillState();

   After((iceMakerData->fillData.iceMakerFillMonitorData->timedIceMakerFillInSecondsx10 * MSEC_PER_SEC) / 10 - 1);

   WhenDispensingRequestIs(DispensingAction_Start, DispenseSelection_ColdWater);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);

   WhenDispensingRequestIs(DispensingAction_Stop, DispenseSelection_ColdWater);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}
