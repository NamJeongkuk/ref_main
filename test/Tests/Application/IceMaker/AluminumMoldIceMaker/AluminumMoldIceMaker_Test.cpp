/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AluminumMoldIceMaker.h"
#include "AluminumMoldIceMakerHsmState.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "TddPersonality.h"
#include "Signal.h"
#include "RakePosition.h"
#include "AluminumMoldIceMakerTestRequest.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "SystemErds.h"

#define Given
#define When
#define Then

static const AluminumMoldIceMakerConfig_t config = {
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState,
   .iceMakerWaterValveVoteErd = Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
   .iceMakerHeaterVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
   .iceMakerMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .harvestCountIsReadyToHarvestErd = Erd_HarvestCountIsReadyToHarvest,
   .iceMakerFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMakerEnabledOverrideResolved,
   .sabbathModeErd = Erd_SabbathMode,
   .fillTubeHeaterVoteErd = Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote,
   .moldHeaterControlRequestErd = Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_AluminumMoldIceMakerRakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved,
   .skipFillRequestErd = Erd_AluminumMoldIceMakerSkipFillRequest,
   .rakeControlRequestErd = Erd_AluminumMoldIceMakerRakeControlRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_AluminumMoldIceMakerVote,
   .waterFillMonitoringRequest = Erd_AluminumMoldIceMakerWaterFillMonitoringRequest,
   .stopFillSignalErd = Erd_AluminumMoldIceMakerStopFillSignal,
   .rakePosition = Erd_AluminumMoldIceMakerRakePosition,
   .freezerIceRateTriggerSignal = Erd_FreezerIceRateTriggerSignal,
   .aluminumMoldIceMakerTestRequestErd = Erd_AluminumMoldIceMakerTestRequest,
};

TEST_GROUP(AluminumMoldIceMaker)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;
   EventSubscription_t iceWaterValveOnChangeSubscription;

   static void IceMakerWaterValveVoteChanged(void * context, const void * args)
   {
      IGNORE(context);
      IGNORE(args);
      mock().actualCall("Ice Maker Water Valve Vote Changed");
   }

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;

      EventSubscription_Init(&iceWaterValveOnChangeSubscription, &instance, IceMakerWaterValveVoteChanged);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TheModuleIsInitialized()
   {
      AluminumMoldIceMaker_Init(&instance, dataModel, &config);
   }

   void AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_t expected)
   {
      AluminumMoldIceMakerHsmState_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHsmState,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void IceMakerWaterValveVoteShouldBe(bool expected)
   {
      WaterValveVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
         &vote);

      CHECK_EQUAL(expected, vote.state);
      CHECK_TRUE(vote.care);
   }

   void IceMakerHeaterVoteIsSet()
   {
      HeaterVotedState_t vote = {
         .state = HeaterState_On,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
         &vote);
   }

   void IceMakerHeaterVoteShouldBe(bool expected)
   {
      HeaterVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
         &vote);

      CHECK_EQUAL(expected, vote.state);
      CHECK_TRUE(vote.care);
   }

   void IceMakerMotorVoteIsSet()
   {
      AluminumMoldIceMakerMotorVotedState_t vote = {
         .state = MotorState_On,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
         &vote);
   }

   void IceMakerMotorVoteShouldBe(bool expected)
   {
      AluminumMoldIceMakerMotorVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
         &vote);

      CHECK_EQUAL(expected, vote.state);
      CHECK_TRUE(vote.care);
   }

   void HarvestCountCalculationRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_HarvestCountCalculationRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FeelerArmMonitoringRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_FeelerArmMonitoringRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void HarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void HarvestCountIsNotReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         clear);
   }

   void IceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void IceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void FeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void FeelerArmIsNotReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         clear);
   }

   void InitializedIntoFreezeStateAndWaitingForHarvestCountToBeReadyForHarvest()
   {
      Given HarvestCountIsNotReadyToHarvest();
      Given IceMakerTemperatureIsReadyToHarvest();
      Given FeelerArmIsReadyToEnterHarvest();
      Given AluminumMoldIceMakerIsInFreezeState();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void InitializedIntoFreezeStateAndWaitingForFeelerArmToBeReadyToEnterHarvest()
   {
      Given HarvestCountIsReadyToHarvest();
      Given IceMakerTemperatureIsReadyToHarvest();
      Given FeelerArmIsNotReadyToEnterHarvest();
      Given AluminumMoldIceMakerIsInFreezeState();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void SabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathMode,
         &state);
   }

   void IceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledOverrideResolved,
         &state);
   }

   void IceMakerIsEnabledAndSabbathModeIsDisabled()
   {
      IceMakerIs(ENABLED);
      SabbathModeIs(DISABLED);
   }

   void IceMakerIsDisabledAndSabbathModeIsEnabled()
   {
      IceMakerIs(DISABLED);
      SabbathModeIs(ENABLED);
   }

   void AluminumMoldIceMakerIsInFreezeState()
   {
      Given IceMakerIs(ENABLED);
      Given SabbathModeIs(DISABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void Given AluminumMoldIceMakerIsInIdleFreezeState()
   {
      Given IceMakerIs(DISABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   }

   void SabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState()
   {
      Given IceMakerIs(ENABLED);
      Given SabbathModeIs(DISABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When SabbathModeIs(ENABLED);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   }

   void IceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState()
   {
      Given IceMakerIs(ENABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerIs(DISABLED);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   }

   void SabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
      Given SabbathModeIs(DISABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given HarvestCountIsReadyToHarvest();
      Given IceMakerTemperatureIsNotReadyToHarvest();
      Given FeelerArmIsReadyToEnterHarvest();
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given HarvestCountIsReadyToHarvest();
      Given IceMakerTemperatureIsNotReadyToHarvest();
      Given FeelerArmIsReadyToEnterHarvest();
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix()
   {
      Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      After(1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   }

   void AluminumMoldIceMakerIsInHarvestFault()
   {
      Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

      After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
   }

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault()
   {
      Given IceMakerIs(ENABLED);
      Given SabbathModeIs(DISABLED);
      Given MoldThermistorIsInvalid();
      Given TheRakePositionIs(RakePosition_NotHome);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
   }
   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote,
         &vote);

      CHECK_EQUAL(expectedDutyCycle, vote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, vote.care);
   }

   void MoldHeaterControlRequestIs(IceMakerMoldHeaterControlRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
         &request);
   }

   void MoldHeaterControlRequestShouldBe(IceMakerMoldHeaterControlRequest_t expectedRequest)
   {
      IceMakerMoldHeaterControlRequest_t actualRequest;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
         &actualRequest);

      CHECK_EQUAL(expectedRequest.enable, actualRequest.enable);
      CHECK_EQUAL(expectedRequest.skipInitialOnTime, actualRequest.skipInitialOnTime);
      CHECK_EQUAL(expectedRequest.onTemperatureInDegFx100, actualRequest.onTemperatureInDegFx100);
      CHECK_EQUAL(expectedRequest.offTemperatureInDegFx100, actualRequest.offTemperatureInDegFx100);
   }

   void MoldHeaterControlRequestShouldBeCleared()
   {
      IceMakerMoldHeaterControlRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
         &actual);

      CHECK_EQUAL(false, actual.enable);
   }

   void StopFillSignalIs(bool signal)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote, &signal);
   }

   void IceMakerMotorVoteIs(MotorState_t state)
   {
      AluminumMoldIceMakerMotorVotedState_t vote = {
         .state = state,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote, &vote);
   }

   void IceMakerWaterValveVoteIs(WaterValveState_t state)
   {
      WaterValveVotedState_t vote = {
         .state = state,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerWaterValve_IceMakerVote, &vote);
   }

   void IceMakerHeaterVoteIs(HeaterState_t state)
   {
      HeaterVotedState_t vote = {
         .state = state,
         .care = true
      };
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote, &vote);
   }

   void IceMakerIsolationWaterValveVoteIs(WaterValveState_t state)
   {
      WaterValveVotedState_t vote = {
         .state = state,
         .care = true
      };
      DataModel_Write(dataModel, Erd_IsolationWaterValve_AluminumMoldIceMakerVote, &vote);
   }

   void MoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void MoldThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, clear);
   }

   void SkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void SkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void RakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }

   void RakeDidNotCompleteFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, clear);
   }

   void TheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void SkipFillRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void RakeControllerRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerRakeControlRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void IsolationIceMakerWaterValveVoteShouldBeDontCare()
   {
      WaterValveVotedState_t vote;
      DataModel_Read(dataModel, Erd_IsolationWaterValve_AluminumMoldIceMakerVote, &vote);

      CHECK_EQUAL(Vote_DontCare, vote.care);
   }

   void IsolationIceMakerWaterValveVoteShouldBe(bool expected)
   {
      WaterValveVotedState_t vote;
      DataModel_Read(dataModel, Erd_IsolationWaterValve_AluminumMoldIceMakerVote, &vote);

      CHECK_EQUAL(expected, vote.state);
      CHECK_EQUAL(Vote_Care, vote.care);
   }

   void WaterFillMonitoringRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerWaterFillMonitoringRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void AluminumMoldIceMakerIsInFillState()
   {
      Given MoldThermistorIsValid();
      Given SkipFillRequestIsClear();
      Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      When RakeCompletesFullRevolution();
      After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   }

   void StopFillSignalChanges()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_AluminumMoldIceMakerStopFillSignal);
   }

   void FreezerTriggerIceRateSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_FreezerIceRateTriggerSignal, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void RakeCompletesItsSecondFullRevolutionInHarvestFixState()
   {
      Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

      When RakeCompletesFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

      When RakeDidNotCompleteFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

      When RakeCompletesFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void ReadyToEnterHarvest()
   {
      HarvestCountIsNotReadyToHarvest();
      FeelerArmIsNotReadyToEnterHarvest();

      When HarvestCountIsReadyToHarvest();
      FeelerArmIsReadyToEnterHarvest();
   }

   void MaxHarvestTimeExpires()
   {
      After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);

      When RakeCompletesFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      After(1);
   }

   void ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerTestRequest,
         &request);
   }

   void ExternalTestRequestShouldBe(AluminumMoldIceMakerTestRequest_t expected)
   {
      AluminumMoldIceMakerTestRequest_t actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerTestRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void ExpectNoChangeInIceMakerWaterValveVote()
   {
      DataModel_Subscribe(dataModel, Erd_AluminumMoldIceMakerWaterValve_IceMakerVote, &iceWaterValveOnChangeSubscription);
      mock().expectNoCall("No Call");
   }

   void UnsubscribeFromIceWaterValveOnChangeSubscription()
   {
      DataModel_Unsubscribe(dataModel, Erd_AluminumMoldIceMakerWaterValve_IceMakerVote, &iceWaterValveOnChangeSubscription);
   }
};

TEST(AluminumMoldIceMaker, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsNotHomeAndAluminumMoldThermistorIsValid)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_NotHome);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsHomeAndAluminumMoldThermistorIsValid)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_Home);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToIdleFreezeStateOnInitIfSabbathModeIsEnabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(ENABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_NotHome);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToIdleFreezeStateOnInitIfSabbathModeIsEnabledAndIceMakerIsDisabled)
{
   Given IceMakerIs(DISABLED);
   Given SabbathModeIs(ENABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_NotHome);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoIdleToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabled)
{
   Given IceMakerIs(DISABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_NotHome);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoIdleToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndRakeIsHome)
{
   Given IceMakerIs(DISABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_Home);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoIdleToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndMoldThermistorIsInvalid)
{
   Given IceMakerIs(DISABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsInvalid();
   Given TheRakePositionIs(RakePosition_NotHome);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsInvalid)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsInvalid();
   Given TheRakePositionIs(RakePosition_NotHome);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsInvalidAndRakeIsHome)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsInvalid();
   Given TheRakePositionIs(RakePosition_Home);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsValidAndRakeIsHome)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_Home);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffWaterValveOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerHeaterOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   IceMakerHeaterVoteShouldBe(HeaterState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerMotorOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   IceMakerMotorVoteShouldBe(MotorState_Off);
}

TEST(AluminumMoldIceMaker, ShouldRequestToMonitorFeelerArmOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   HarvestCountCalculationRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldRequestToCalculateHarvestCountOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   FeelerArmMonitoringRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndHarvestConditionsAreMet)
{
   Given HarvestCountIsNotReadyToHarvest();
   Given IceMakerTemperatureIsReadyToHarvest();
   Given FeelerArmIsReadyToEnterHarvest();
   Given AluminumMoldIceMakerIsInFreezeState();

   When HarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndIceMakerTemperatureIsNotReadyToHarvest)
{
   Given HarvestCountIsNotReadyToHarvest();
   Given IceMakerTemperatureIsNotReadyToHarvest();
   Given FeelerArmIsReadyToEnterHarvest();
   Given AluminumMoldIceMakerIsInFreezeState();

   When HarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndFeelerArmIsNotReadyToEnterHarvest)
{
   Given HarvestCountIsNotReadyToHarvest();
   Given IceMakerTemperatureIsReadyToHarvest();
   Given FeelerArmIsNotReadyToEnterHarvest();
   Given AluminumMoldIceMakerIsInFreezeState();

   When HarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenFeelerArmIsReadyToHarvestAndHarvestCountIsNotReadyToHarvest)
{
   Given HarvestCountIsNotReadyToHarvest();
   Given IceMakerTemperatureIsReadyToHarvest();
   Given FeelerArmIsNotReadyToEnterHarvest();
   Given AluminumMoldIceMakerIsInFreezeState();

   When FeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenIceMakerTemperatureIsReadyToHarvestAndHarvestConditionsAreMet)
{
   Given HarvestCountIsReadyToHarvest();
   Given IceMakerTemperatureIsNotReadyToHarvest();
   Given FeelerArmIsReadyToEnterHarvest();
   Given AluminumMoldIceMakerIsInFreezeState();

   When IceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenFeelerArmIsReadyToEnterHarvestAndHarvestConditionsAreMet)
{
   Given HarvestCountIsReadyToHarvest();
   Given IceMakerTemperatureIsReadyToHarvest();
   Given FeelerArmIsNotReadyToEnterHarvest();
   Given AluminumMoldIceMakerIsInFreezeState();

   When FeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldRequestToStopCalculatingHarvestCountWhenTransitioningFromFreezeToHarvest)
{
   Given InitializedIntoFreezeStateAndWaitingForHarvestCountToBeReadyForHarvest();
   HarvestCountCalculationRequestShouldBe(SET);

   When HarvestCountIsReadyToHarvest();
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldRequestToStopMonitoringFeelerArmWhenTransitioningFromFreezeToHarvest)
{
   Given InitializedIntoFreezeStateAndWaitingForFeelerArmToBeReadyToEnterHarvest();
   HarvestCountCalculationRequestShouldBe(SET);

   When FeelerArmIsReadyToEnterHarvest();
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToIdleFreezeWhenSabbathIsEnabled)
{
   Given AluminumMoldIceMakerIsInFreezeState();

   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenSabbathIsEnabled)
{
   Given SabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToFreezeWhenSabbathIsDisabled)
{
   Given SabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();

   When SabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToHarvestWhenSabbathIsDisabledWhileRakePositionIsNotHome)
{
   Given IceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   Given AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When TheRakePositionIs(RakePosition_NotHome);
   When IceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToHarvestWhenIceMakerIsEnabledWhileRakePositionIsNotHome)
{
   Given SabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When TheRakePositionIs(RakePosition_NotHome);
   When SabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToIdleFreezeWhenIceMakerIsDisabled)
{
   Given AluminumMoldIceMakerIsInFreezeState();

   When IceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenIceMakerIsDisabled)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When IceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToFreezeWhenIceMakerIsEnabled)
{
   Given IceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState();

   When IceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffWaterValveOnEntryToIdleFreezeState)
{
   Given IceMakerWaterValveVoteIs(WaterValveState_On);
   Given AluminumMoldIceMakerIsInIdleFreezeState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerHeaterOnEntryToIdleFreezeState)
{
   Given IceMakerHeaterVoteIsSet();
   Given AluminumMoldIceMakerIsInIdleFreezeState();
   IceMakerHeaterVoteShouldBe(HeaterState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerMotorOnEntryToIdleFreezeState)
{
   Given IceMakerMotorVoteIsSet();
   Given AluminumMoldIceMakerIsInIdleFreezeState();
   IceMakerMotorVoteShouldBe(MotorState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnFillTubeHeaterWithFreezeThawFillTubeHeaterDutyCycleOnEntryToHarvest)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
}

TEST(AluminumMoldIceMaker, ShouldVoteFillTubeHeaterDontCareAfterFillTubeHeaterOnTime)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker, ShouldRequestRakeControllerAfterInitialMinimumHeaterOnTimeOnEntryToHarvest)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(CLEAR);

   After(1);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldSetMoldHeaterControlRequestOnEntryToHarvest)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   IceMakerMoldHeaterControlRequest_t expectedRequest;
   expectedRequest.enable = true;
   expectedRequest.skipInitialOnTime = false;
   expectedRequest.onTemperatureInDegFx100 = iceMakerData->harvestData.heaterOnTemperatureInDegFx100;
   expectedRequest.offTemperatureInDegFx100 = iceMakerData->harvestData.heaterOffTemperatureInDegFx100;
   MoldHeaterControlRequestShouldBe(expectedRequest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixWhenMaxHarvestTimeReached)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToThermistorFaultWhenMoldThermistorIsInvalid)
{
   Given AluminumMoldIceMakerIsInHarvestFault();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToIdleFreezeWhenSabbathModeEnabled)
{
   Given AluminumMoldIceMakerIsInHarvestFault();

   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToIdleFreezeWhenIceMakeIsDisbled)
{
   Given AluminumMoldIceMakerIsInHarvestFault();

   When IceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldClearMoldHeaterControlRequestOnEntryToHarvestFault)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   MoldHeaterControlRequestShouldBeCleared();
}

TEST(AluminumMoldIceMaker, ShouldSetRakeMotorControlRequestOnEntryToHarvestFault)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldClearRakeMotorControlRequestAfterRakeMotorControlTimeWhileTheMotorHasBeenOn)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(SET);

   After(1);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldRepeatTurningOnAndOffRakeMotorAfterRakeMotorControlTime)
{
   Given AluminumMoldIceMakerIsInHarvestFault();

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(SET);
   After(1);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(CLEAR);
   After(1);
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(SET);
   After(1);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToHarvestWhenHarvestFaultMaxTimeExpired)
{
   Given AluminumMoldIceMakerIsInHarvestFault();

   After(iceMakerData->harvestFaultData.harvestFaultMaxTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToHarvetWhenRakeCompletesFullRevolution)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotUpdateRakeControllerRequestAfterMotorControlTimeWhenMoldThermistorIsInvalidInHarvestFix)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldNotUpdateRakeControllerRequestAfterMotorControlTimeWhenSabbathModeIsEnabledInHarvestFault)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldNotUpdateRakeControllerRequestAfterMotorControlTimeWhenIceMakerIsDisabledInHarvestFault)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   When IceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldVoteFillTubeHeaterDontCareWhenTransitioningFromHarvestToHarvestFix)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToThermistorFaultWhenMoldThermistorIsInvalid)
{
   Given MoldThermistorIsValid();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToThermistorFaultWhenMoldThermistorIsInvalid)
{
   Given MoldThermistorIsValid();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsClearAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotClearRakeControllerRequestUntilAfterWeMoveOnFromHarvest)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);

   When RakeCompletesFullRevolution();
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldNotClearRakeControllerRequestWhenTransitioningFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   RakeControllerRequestShouldBe(SET);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsClearAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsSet)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsClear)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionAfterMaxHarvestTimerExpiresIfAlreadyExitedHarvest)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldSetSkipFillRequestOnEntryToHarvestFix)
{
   Given SkipFillRequestIsClear();
   When IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   SkipFillRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldSetMoldHeaterControlRequestOnEntryToHarvestFix)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   IceMakerMoldHeaterControlRequest_t expectedRequest = {
      .enable = true,
      .skipInitialOnTime = true,
      .onTemperatureInDegFx100 = iceMakerData->harvestFixData.heaterOnTemperatureInDegFx100,
      .offTemperatureInDegFx100 = iceMakerData->harvestFixData.heaterOffTemperatureInDegFx100
   };
   MoldHeaterControlRequestShouldBe(expectedRequest);
}

TEST(AluminumMoldIceMaker, ShouldRepeatedlyRequestRakeControllerAccordingToMotorOnOffTimeWhileInHarvestFix)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   uint8_t repeatNumber = 10;
   do
   {
      After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC - 1);
      RakeControllerRequestShouldBe(SET);

      After(1);
      RakeControllerRequestShouldBe(CLEAR);

      After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC - 1);
      RakeControllerRequestShouldBe(CLEAR);

      After(1);
      RakeControllerRequestShouldBe(SET);
   } while(repeatNumber--);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToFreezeWhenRakeCompletesItsSecondFullRevolutionWhileInHarvestFix)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeDidNotCompleteFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotRepeatedlyRequestRakeControllerAccordingToMotorOnOffTimeOnExitToHarvestFix)
{
   Given RakeCompletesItsSecondFullRevolutionInHarvestFixState();

   After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldDisableMoldHeaterControlRequestOnExitToHarvestFix)
{
   Given RakeCompletesItsSecondFullRevolutionInHarvestFixState();

   IceMakerMoldHeaterControlRequest_t expectedRequest = {
      .enable = false,
      .skipInitialOnTime = true,
      .onTemperatureInDegFx100 = iceMakerData->harvestFixData.heaterOnTemperatureInDegFx100,
      .offTemperatureInDegFx100 = iceMakerData->harvestFixData.heaterOffTemperatureInDegFx100
   };
   MoldHeaterControlRequestShouldBe(expectedRequest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToHarvestFaultWhenMaxHarvestFixTimeReached)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnIceMakerWaterValveWhenEnteringFillState)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off); // it's voted off in freeze

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnIsolationWaterValveWhenEnteringFillState)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   IsolationIceMakerWaterValveVoteShouldBeDontCare();

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldRequestWaterFillMonitoringWhenEnteringFillState)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   WaterFillMonitoringRequestShouldBe(CLEAR);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldClearWaterFillMonitoringRequestWhenExitingFillState)
{
   Given AluminumMoldIceMakerIsInFillState();
   WaterFillMonitoringRequestShouldBe(SET);

   When StopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   WaterFillMonitoringRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldRequestWaterFillMonitoringWhenEnteringFillStateAgain)
{
   Given AluminumMoldIceMakerIsInFillState();
   WaterFillMonitoringRequestShouldBe(SET);

   When StopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   WaterFillMonitoringRequestShouldBe(CLEAR);

   When ReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When MaxHarvestTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerWaterValveWhenExitingFillState)
{
   Given AluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIsolationWaterValveWhenExitingFillState)
{
   Given AluminumMoldIceMakerIsInFillState();
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);

   When StopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldIncrementFreezerTriggerIceRateSignalWhenTransitioningToTheFreezeState)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(ENABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_Home);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   When SabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(1);

   When IceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   FreezerTriggerIceRateSignalShouldBe(1);

   When IceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(2);
}

TEST(AluminumMoldIceMaker, ShouldNotIncrementFreezerTriggerIceRateSignalWhenTransitioningToTheFreezeStateOnInitialization)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(DISABLED);
   Given MoldThermistorIsValid();
   Given TheRakePositionIs(RakePosition_Home);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   When SabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(1);
}

TEST(AluminumMoldIceMaker, ShouldClearExternalTestRequestWhenFillTestIsRequested)
{
   Given TheModuleIsInitialized();
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   ExternalTestRequestShouldBe(AluminumMoldIceMakerTestRequest_None);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToFillStateWhenFillTestIsRequested)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToFillStateWhenFillTestIsRequested)
{
   Given SabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenFillTestIsRequested)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToFillStateWhenFillTestIsRequested)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToFillStateWhenFillTestIsRequested)
{
   Given AluminumMoldIceMakerIsInHarvestFault();
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromThermistorFaultToFillStateWhenFillTestIsRequested)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault();
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotLeaveIdleFreezeStateWhenIceMakerIsInDisabledState)
{
   Given AluminumMoldIceMakerIsInIdleFreezeState();
   Given IceMakerIs(DISABLED);
   
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   When StopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoFromFillToIdleStateIfSabbathModeIsEnabledAndIceMakerIsDisabled)
{
   Given AluminumMoldIceMakerIsInIdleFreezeState();
   Given SabbathModeIs(ENABLED);
   
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   When StopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionOutOfFillStateWhenFillTestIsRequestedWhileFillIsAlreadyTakingPlace)
{
   Given AluminumMoldIceMakerIsInFillState();
   ExpectNoChangeInIceMakerWaterValveVote();
   
   When ExternalTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   Then UnsubscribeFromIceWaterValveOnChangeSubscription();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerWaterValveWhenMoldThermistorIsInvalid)
{
   Given IceMakerIsEnabledAndSabbathModeIsDisabled();
   Given MoldThermistorIsValid();
   Given TheModuleIsInitialized();
   Given IceMakerWaterValveVoteIs(WaterValveState_On);

   When MoldThermistorIsInvalid();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIsolationValveWhenMoldThermistorIsInvalid)
{
   Given IceMakerIsEnabledAndSabbathModeIsDisabled();
   Given MoldThermistorIsValid();
   Given TheModuleIsInitialized();
   Given IceMakerIsolationWaterValveVoteIs(WaterValveState_On);

   When MoldThermistorIsInvalid();
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerHeaterWhenMoldThermistorIsInvalid)
{
   Given IceMakerIsEnabledAndSabbathModeIsDisabled();
   Given MoldThermistorIsValid();
   Given TheModuleIsInitialized();
   Given IceMakerHeaterVoteIs(HeaterState_On);

   When MoldThermistorIsInvalid();
   IceMakerHeaterVoteShouldBe(HeaterState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerMotorWhenMoldThermistorIsInvalid)
{
   Given IceMakerIsEnabledAndSabbathModeIsDisabled();
   Given MoldThermistorIsValid();
   Given TheModuleIsInitialized();
   Given IceMakerMotorVoteIs(MotorState_On);

   When MoldThermistorIsInvalid();
   IceMakerMotorVoteShouldBe(MotorState_Off);
}

TEST(AluminumMoldIceMaker, ShouldBeInThermistorFaultStateIfThermistorIsInvalidBeforeInitialization)
{
   Given IceMakerIsEnabledAndSabbathModeIsDisabled();
   Given MoldThermistorIsInvalid();
   Given TheModuleIsInitialized();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultFromFreezeWhenThermistorIsInvalid)
{
   Given AluminumMoldIceMakerIsInFreezeState();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultFromHarvestWhenThermistorIsInvalid)
{
   Given SabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionOutOfFillStateWhenSignalStopFillNotReceivedAndThermistorIsInvalid)
{
   Given AluminumMoldIceMakerIsInFillState();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToFreezeAfterFillWhenReceivedSignalStopFillAndThermistorIsValid)
{
   Given AluminumMoldIceMakerIsInFillState();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   When StopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultAfterFillWhenSignalStopFillReceivedAndThermistorIsInvalid)
{
   Given AluminumMoldIceMakerIsInFillState();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   When StopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultAfterHarvestFixWhenThermistorIsInvalid)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromThermistorFaultToFreezeWhenThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsHome)
{
   Given IceMakerIsEnabledAndSabbathModeIsDisabled();
   Given TheRakePositionIs(RakePosition_Home);
   Given TheModuleIsInitialized();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   When MoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldSetSkipFillRequestAndShouldTransitionFromThermistorFaultStateToHarvestWhenThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsNotHome)
{
   Given IceMakerIsEnabledAndSabbathModeIsDisabled();
   Given TheRakePositionIs(RakePosition_NotHome);
   Given SkipFillRequestIsClear();
   Given TheModuleIsInitialized();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   When MoldThermistorIsValid();
   SkipFillRequestShouldBe(set);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldCompleteFillEvenIfThermistorBecomesInvalid)
{
   Given TheModuleIsInitialized();
   Given AluminumMoldIceMakerIsInFillState();

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToThermistorFaultWhenThermistorIsInvalidAfterFillStops)
{
   Given TheModuleIsInitialized();
   Given MoldThermistorIsValid();
   Given AluminumMoldIceMakerIsInFillState();

   When MoldThermistorIsInvalid();
   When StopFillSignalChanges();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToHarvestWhenRakeIsNotHomeAfterFillStops)
{
   Given TheModuleIsInitialized();
   Given MoldThermistorIsValid();
   Given AluminumMoldIceMakerIsInFillState();

   When TheRakePositionIs(RakePosition_NotHome);
   When StopFillSignalChanges();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToFreezeWhenRakeIsHomeAfterFillStops)
{
   Given TheModuleIsInitialized();
   Given MoldThermistorIsValid();
   Given AluminumMoldIceMakerIsInFillState();

   When TheRakePositionIs(RakePosition_Home);
   When StopFillSignalChanges();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeAfterSecondRevolutionWhenReEnteringIntoHarvestFixState)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeDidNotCompleteFullRevolution();
   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   When IceMakerTemperatureIsNotReadyToHarvest();
   When IceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeDidNotCompleteFullRevolution();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeDidNotCompleteFullRevolution();
   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeAfterSecondRevolutionWhenReEnteringIntoHarvestFixAfterFirstRevolutionAndMoldThermistorIsInvalidInTheState)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   When TheRakePositionIs(RakePosition_NotHome);
   When MoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeDidNotCompleteFullRevolution();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   When RakeDidNotCompleteFullRevolution();
   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST_GROUP(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble, TddPersonality_DevelopmentSingleEvapFillTubeHeaterOnTimeLessThanMaxHarvestTime);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TheModuleIsInitialized()
   {
      AluminumMoldIceMaker_Init(&instance, dataModel, &config);
   }

   void AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_t expected)
   {
      AluminumMoldIceMakerHsmState_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHsmState,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void HarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void HarvestCountIsNotReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         clear);
   }

   void IceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void IceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void FeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void FeelerArmIsNotReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         clear);
   }

   void IceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledOverrideResolved,
         &state);
   }

   void TheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given HarvestCountIsReadyToHarvest();
      Given IceMakerTemperatureIsNotReadyToHarvest();
      Given FeelerArmIsReadyToEnterHarvest();
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t dutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote,
         &vote);

      CHECK_EQUAL(dutyCycle, vote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, vote.care);
   }

   void MoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void MoldThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, clear);
   }

   void SkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void SkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void RakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }

   void RakeDidNotCompleteFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, clear);
   }

   void RakeControllerRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerRakeControlRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldVoteDontCareForFillTubeHeaterAfterFillTubeHeaterOnTime)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFreezeStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsSet)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFillStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsClear)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSet)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsClear)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsSet)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsClear)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldSetRakeControllerRequestWhenTransitioningFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   RakeControllerRequestShouldBe(SET);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldNotTransitionAfterFillTubeHeaterTimerExpiresWhenExitConditionsToGoToFillAreMetIfAlreadyExitedHarvest)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When MoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST_GROUP(AluminumMoldIceMaker_FillTubeHeaterZeroDutyCycle)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble, TddPersonality_DevelopmentSingleEvapFillTubeHeaterZeroDutyCycle);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TheModuleIsInitialized()
   {
      AluminumMoldIceMaker_Init(&instance, dataModel, &config);
   }

   void AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_t expected)
   {
      AluminumMoldIceMakerHsmState_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHsmState,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void HarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void IceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void IceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void FeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void IceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledOverrideResolved,
         &state);
   }

   void TheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given HarvestCountIsReadyToHarvest();
      Given IceMakerTemperatureIsNotReadyToHarvest();
      Given FeelerArmIsReadyToEnterHarvest();
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void MoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void SkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void SkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void RakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }
};

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroDutyCycle, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleIsZeroAndSkipFillRequestIsSet)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroDutyCycle, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleIsZeroAndSkipFillRequestIsClear)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST_GROUP(AluminumMoldIceMaker_FillTubeHeaterZeroOnTime)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble, TddPersonality_DevelopmentSingleEvapFillTubeHeaterZeroOnTime);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TheModuleIsInitialized()
   {
      AluminumMoldIceMaker_Init(&instance, dataModel, &config);
   }

   void AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_t expected)
   {
      AluminumMoldIceMakerHsmState_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerHsmState,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void HarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void IceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void IceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void FeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void IceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledOverrideResolved,
         &state);
   }

   void TheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
      Given MoldThermistorIsValid();
      Given TheRakePositionIs(RakePosition_Home);
      Given HarvestCountIsReadyToHarvest();
      Given IceMakerTemperatureIsNotReadyToHarvest();
      Given FeelerArmIsReadyToEnterHarvest();
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void MoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void SkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void SkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void RakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }
};

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroOnTime, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterHeaterTimeIsZeroAndSkipFillRequestIsSet)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroOnTime, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterHeaterTimeIsZeroAndSkipFillRequestIsClear)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}
