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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "SystemErds.h"

#define Given
#define When

static const AluminumMoldIceMakerConfig_t config = {
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState,
   .waterValveVoteErd = Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
   .iceMakerHeaterVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
   .iceMakerMotorVoteErd = Erd_AluminumMoldIceMakerMotor_IceMakerVote,
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
   .moldThermistorIsValidErd = Erd_AluminumMoldIceMakerMoldThermistorIsValidResolved,
   .skipFillRequestErd = Erd_AluminumMoldIceMakerSkipFillRequest,
   .rakeControlRequestErd = Erd_AluminumMoldIceMakerRakeControlRequest
};

TEST_GROUP(AluminumMoldIceMaker)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;
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

   void WaterValveVoteShouldBe(bool expected)
   {
      WaterValveVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
         &vote);

      CHECK_EQUAL(expected, vote.state);
      CHECK_TRUE(vote.care);
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

   void IceMakerMotorVoteShouldBe(bool expected)
   {
      AluminumMoldIceMakerMotorVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMotor_IceMakerVote,
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

   void AluminumMoldIceMakerIsInFreezeState()
   {
      Given IceMakerIs(ENABLED);
      Given SabbathModeIs(DISABLED);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void SabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState()
   {
      Given IceMakerIs(ENABLED);
      Given SabbathModeIs(DISABLED);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When SabbathModeIs(ENABLED);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   }

   void IceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState()
   {
      Given IceMakerIs(ENABLED);
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerIs(DISABLED);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   }

   void SabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
      Given SabbathModeIs(DISABLED);
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

   void FillTubeHeaterVoteShouldBe(PercentageDutyCycle_t expectedDutyCycle)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote,
         &vote);

      CHECK_EQUAL(expectedDutyCycle, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void MoldHeaterControlRequestForHarvestShouldBe(IceMakerMoldHeaterControlRequest_t expectedRequest)
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

   void MoldHeaterControlRequestForHarvestShouldBeCleared()
   {
      IceMakerMoldHeaterControlRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
         &actual);

      CHECK_EQUAL(false, actual.enable);
   }

   void MoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistorIsValidResolved, set);
   }

   void MoldThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistorIsValidResolved, clear);
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
};

TEST(AluminumMoldIceMaker, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(DISABLED);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToIdleFreezeStateOnInitIfSabbathModeIsEnabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   Given SabbathModeIs(ENABLED);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToIdleFreezeStateOnInitIfSabbathModeIsEnabledAndIceMakerIsDisabled)
{
   Given IceMakerIs(DISABLED);
   Given SabbathModeIs(ENABLED);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabled)
{
   Given IceMakerIs(DISABLED);
   Given SabbathModeIs(DISABLED);
   Given TheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffWaterValveOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   WaterValveVoteShouldBe(OFF);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerHeaterOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   IceMakerHeaterVoteShouldBe(OFF);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerMotorOnEntryToFreezeState)
{
   Given AluminumMoldIceMakerIsInFreezeState();
   IceMakerMotorVoteShouldBe(OFF);
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

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnFillTubeHeaterWithFreezeThawFillTubeHeaterDutyCycleOnEntryToHarvest)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   FillTubeHeaterVoteShouldBe(iceMakerData->harvestData.freezeThawFillTubeHeaterDutyCyclePercentage);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffFillTubeHeaterAfterFillTubeHeaterOnTime)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteShouldBe(iceMakerData->harvestData.freezeThawFillTubeHeaterDutyCyclePercentage);

   After(1);
   FillTubeHeaterVoteShouldBe(PercentageDutyCycle_Min);
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
   MoldHeaterControlRequestForHarvestShouldBe(expectedRequest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixWhenMaxHarvestTimeReached)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldVoteFillTubeHeaterOffWhenTransitioningFromHarvestToHarvestFix)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   FillTubeHeaterVoteShouldBe(iceMakerData->harvestData.freezeThawFillTubeHeaterDutyCyclePercentage);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   FillTubeHeaterVoteShouldBe(PercentageDutyCycle_Min);
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

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
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

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldClearRakeControllerRequestWhenRakeCompletesFullRevolutionDuringHarvest)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);

   When RakeCompletesFullRevolution();
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldClearRakeControllerRequestWhenTransitioningFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   RakeControllerRequestShouldBe(SET);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsClearAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
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

TEST(AluminumMoldIceMaker, ShouldDisableMoldHeaterControlRequestWhenTransitioningFromHarvestToHarvestFix)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);

   IceMakerMoldHeaterControlRequest_t harvestRequestOnEntry;
   harvestRequestOnEntry.enable = true;
   harvestRequestOnEntry.skipInitialOnTime = false;
   harvestRequestOnEntry.onTemperatureInDegFx100 = iceMakerData->harvestData.heaterOnTemperatureInDegFx100;
   harvestRequestOnEntry.offTemperatureInDegFx100 = iceMakerData->harvestData.heaterOffTemperatureInDegFx100;
   MoldHeaterControlRequestForHarvestShouldBe(harvestRequestOnEntry);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   MoldHeaterControlRequestForHarvestShouldBeCleared();
}

TEST(AluminumMoldIceMaker, ShouldClearSkipFillFlagWhenTransitioningFromHarvestToHarvestFix)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given RakeDidNotCompleteFullRevolution();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   SkipFillRequestShouldBe(SET);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   SkipFillRequestShouldBe(CLEAR);
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

      iceMakerData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;
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

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
      Given HarvestCountIsReadyToHarvest();
      Given IceMakerTemperatureIsNotReadyToHarvest();
      Given FeelerArmIsReadyToEnterHarvest();
      Given TheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      When IceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void FillTubeHeaterVoteShouldBe(PercentageDutyCycle_t dutyCycle)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote,
         &vote);

      CHECK_EQUAL(dutyCycle, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void MoldHeaterControlRequestForHarvestShouldBe(IceMakerMoldHeaterControlRequest_t request)
   {
      IceMakerMoldHeaterControlRequest_t actual;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
         &actual);

      CHECK_EQUAL(request.enable, actual.enable);
      CHECK_EQUAL(request.skipInitialOnTime, actual.skipInitialOnTime);
      CHECK_EQUAL(request.onTemperatureInDegFx100, actual.onTemperatureInDegFx100);
      CHECK_EQUAL(request.offTemperatureInDegFx100, actual.offTemperatureInDegFx100);
   }

   void MoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistorIsValidResolved, set);
   }

   void MoldThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistorIsValidResolved, clear);
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

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldVoteToTurnOffFillTubeHeaterAfterFillTubeHeaterOnTime)
{
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteShouldBe(iceMakerData->harvestData.freezeThawFillTubeHeaterDutyCyclePercentage);

   After(1);
   FillTubeHeaterVoteShouldBe(PercentageDutyCycle_Min);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFreezeStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsSet)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsSet();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
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

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
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

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   When RakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsClear)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
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

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldClearRakeControllerRequestWhenTransitioningFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
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
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionAfterFillTubeHeaterTimerExpiresWhenExitConditionsToGoToFillAreMetIfAlreadyExitedHarvest)
{
   Given MoldThermistorIsValid();
   Given SkipFillRequestIsClear();
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
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

      iceMakerData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;
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

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
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
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistorIsValidResolved, set);
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

      iceMakerData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;
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

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      Given IceMakerIs(ENABLED);
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
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistorIsValidResolved, set);
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
