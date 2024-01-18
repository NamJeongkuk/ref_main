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
#include "IceMakerTestRequest.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "SystemErds.h"

static const AluminumMoldIceMakerConfig_t config = {
   .aluminumMoldIceMakerHsmStateErd = Erd_AluminumMoldIceMakerHsmState,
   .iceMakerWaterValveVoteErd = Erd_AluminumMoldIceMakerWaterValve_IceMakerVote,
   .moldHeaterVoteErd = Erd_IceMaker0_HeaterRelay_IceMakerVote,
   .rakeMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_IceMaker1_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_IceMaker0_FeelerArmMonitoringRequest,
   .feelerArmPositionErd = Erd_IceMaker0_FeelerArmPosition,
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker1_HarvestCountIsReadyToHarvest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .fillTubeHeaterVoteErd = Erd_FillTubeHeater_AluminumMoldIceMakerVote,
   .moldHeaterControlRequestErd = Erd_IceMaker0_MoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_IceMaker0_RakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved,
   .skipFillRequestErd = Erd_IceMaker0_SkipFillRequest,
   .rakeControlRequestErd = Erd_IceMaker0_RakeControlRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_AluminumMoldIceMakerVote,
   .waterFillMonitoringRequestErd = Erd_AluminumMoldIceMakerWaterFillMonitoringRequest,
   .stopIceMakerFillSignalErd = Erd_AluminumMoldIceMakerStopFillSignal,
   .rakePositionErd = Erd_IceMaker0_RakePosition,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .freezerIceRateIsActiveErd = Erd_Freezer_IceRateIsActive,
   .aluminumMoldIceMakerTestRequestErd = Erd_IceMaker1_TestRequest,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus,
   .coolingSystemOffStatusErd = Erd_CoolingOffStatus,
   .dispensingInhibitedReasonErd = Erd_DispensingInhibitedReason,
   .iceMakerFillInhibitedReasonErd = Erd_IceMakerFillInhibitedReason
};

TEST_GROUP(AluminumMoldIceMaker)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;
   AluminumMoldIceMakerData_t iceMakerModifiedData;

   EventSubscription_t iceWaterValveOnChangeSubscription;
   EventSubscription_t dataModelOnChangeSubscription;
   EventSubscription_t rakeControllerRequestOnChangeSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->iceMakerSlots->slot1Data->aluminumMoldData;
      iceMakerModifiedData = *iceMakerData;

      EventSubscription_Init(&dataModelOnChangeSubscription, &instance, DataModelChanged);
      EventSubscription_Init(&rakeControllerRequestOnChangeSubscription, &instance, RakeControllerRequestChanged);
   }

   void GivenFillTubeHeaterDutyCycleAndOnTimeAreZeroInParametric()
   {
      iceMakerModifiedData.harvestData.fillTubeHeaterDutyCyclePercentage = 0;
      iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds = 0;
      iceMakerModifiedData.harvestData.initialMinimumHeaterOnTimeInSeconds = 0;
   }

   void GivenFillTubeHeaterOnTimeEqualToMaxHarvestTime()
   {
      iceMakerModifiedData.harvestData.maximumHarvestTimeInMinutes = 4;
      iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds = iceMakerModifiedData.harvestData.maximumHarvestTimeInMinutes * SECONDS_PER_MINUTE;
   }

   static void DataModelChanged(void *context, const void *args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *onChangeData = (const DataModelOnDataChangeArgs_t *)args;

      if(onChangeData->erd == Erd_FillTubeHeater_AluminumMoldIceMakerVote)
      {
         mock().actualCall("Ice Maker Fill Tube Heater Vote Changed");
      }
      else if(onChangeData->erd == Erd_AluminumMoldIceMakerWaterValve_IceMakerVote)
      {
         mock().actualCall("Ice Maker Water Valve Vote Changed");
      }
   }

   static void RakeControllerRequestChanged(void *context, const void *args)
   {
      IGNORE(context);
      IGNORE(args);
      mock().actualCall("Ice Maker Rake Controller Request Changed");
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenTheModuleIsInitialized()
   {
      AluminumMoldIceMaker_Init(&instance, dataModel, &config, iceMakerData);
   }

   void GivenTheModuleIsInitializedWithZeroFillTubeHeaterDutyCycleAndOnTimeForParametric()
   {
      GivenFillTubeHeaterDutyCycleAndOnTimeAreZeroInParametric();
      AluminumMoldIceMaker_Init(&instance, dataModel, &config, &iceMakerModifiedData);
   }

   void GivenTheModuleIsInitializedWithFillTubeHeaterOnTimeEqualToMaximumHarvestTimeForParametric()
   {
      GivenFillTubeHeaterOnTimeEqualToMaxHarvestTime();
      AluminumMoldIceMaker_Init(&instance, dataModel, &config, &iceMakerModifiedData);
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

      if(expected)
      {
         CHECK_TRUE(vote.care);
      }
      else
      {
         CHECK_FALSE(vote.care);
      }
   }

   void GivenIceMakerHeaterVoteIsSet()
   {
      HeaterVotedState_t vote = {
         .state = HeaterState_On,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_IceMaker0_HeaterRelay_IceMakerVote,
         &vote);
   }

   void IceMakerHeaterVoteShouldBe(bool expected)
   {
      HeaterVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_HeaterRelay_IceMakerVote,
         &vote);

      CHECK_EQUAL(expected, vote.state);
      CHECK_FALSE(vote.care);
   }

   void GivenIceMakerMotorVoteIsSet()
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
      CHECK_FALSE(vote.care);
   }

   void HarvestCountCalculationRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_HarvestCountCalculationRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FeelerArmMonitoringRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_FeelerArmMonitoringRequest,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenHarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_HarvestCountIsReadyToHarvest,
         set);
   }

   void WhenHarvestCountIsReadyToHarvest()
   {
      GivenHarvestCountIsReadyToHarvest();
   }

   void GivenHarvestCountIsNotReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_HarvestCountIsReadyToHarvest,
         clear);
   }

   void WhenHarvestCountIsNotReadyToHarvest()
   {
      GivenHarvestCountIsNotReadyToHarvest();
   }

   void GivenIceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void WhenIceMakerTemperatureIsNotReadyToHarvest()
   {
      GivenIceMakerTemperatureIsNotReadyToHarvest();
   }

   void GivenIceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void WhenIceMakerTemperatureIsReadyToHarvest()
   {
      GivenIceMakerTemperatureIsReadyToHarvest();
   }

   void GivenFeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void WhenFeelerArmIsReadyToEnterHarvest()
   {
      GivenFeelerArmIsReadyToEnterHarvest();
   }

   void GivenFeelerArmIsNotReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         clear);
   }

   void WhenFeelerArmIsNotReadyToEnterHarvest()
   {
      GivenFeelerArmIsNotReadyToEnterHarvest();
   }

   void GivenInitializedIntoFreezeStateAndWaitingForHarvestCountToBeReadyForHarvest()
   {
      GivenHarvestCountIsNotReadyToHarvest();
      GivenIceMakerTemperatureIsReadyToHarvest();
      GivenFeelerArmIsReadyToEnterHarvest();
      GivenTheAluminumMoldIceMakerIsInFreezeState();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void GivenInitializedIntoFreezeStateAndWaitingForFeelerArmToBeReadyToEnterHarvest()
   {
      GivenHarvestCountIsReadyToHarvest();
      GivenIceMakerTemperatureIsReadyToHarvest();
      GivenFeelerArmIsNotReadyToEnterHarvest();
      GivenTheAluminumMoldIceMakerIsInFreezeState();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
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

   void GivenTheIceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMakerEnabledResolved,
         &state);
   }

   void WhenTheIceMakerIs(bool state)
   {
      GivenTheIceMakerIs(state);
   }

   void GivenTheCoolingSystemOffIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_CoolingOffStatus,
         &state);
   }

   void WhenTheCoolingSystemOffChangesTo(bool state)
   {
      GivenTheCoolingSystemOffIs(state);
   }

   void GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
   }

   void GivenTheAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenTheIceMakerIs(DISABLED);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void GivenTheCoolingSystemIsOffAndAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenTheCoolingSystemOffChangesTo(true);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void GivenSabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenHarvestCountIsReadyToHarvest();
      GivenIceMakerTemperatureIsNotReadyToHarvest();
      GivenFeelerArmIsReadyToEnterHarvest();
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenHarvestCountIsReadyToHarvest();
      GivenIceMakerTemperatureIsNotReadyToHarvest();
      GivenFeelerArmIsReadyToEnterHarvest();
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithZeroFillTubeHeaterDutyCycleAndOnTime()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenHarvestCountIsReadyToHarvest();
      GivenIceMakerTemperatureIsNotReadyToHarvest();
      GivenFeelerArmIsReadyToEnterHarvest();
      GivenTheModuleIsInitializedWithZeroFillTubeHeaterDutyCycleAndOnTimeForParametric();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithFillTubeHeaterOnTimeEqualToMaxHarvestTime()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenHarvestCountIsReadyToHarvest();
      GivenIceMakerTemperatureIsNotReadyToHarvest();
      GivenFeelerArmIsReadyToEnterHarvest();
      GivenTheModuleIsInitializedWithFillTubeHeaterOnTimeEqualToMaximumHarvestTimeForParametric();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      After(1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   }

   void WhenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      After(1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   }

   void GivenTheAluminumMoldIceMakerIsInHarvestFault()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

      After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
   }

   void GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenMoldThermistorIsInvalid();
      GivenTheRakePositionIs(RakePosition_NotHome);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
   }

   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(
         dataModel,
         Erd_FillTubeHeater_AluminumMoldIceMakerVote,
         &vote);

      CHECK_EQUAL(expectedDutyCycle, vote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, vote.care);
   }

   void MoldHeaterControlRequestShouldBe(IceMakerMoldHeaterControlRequest_t expectedRequest)
   {
      IceMakerMoldHeaterControlRequest_t actualRequest;
      DataModel_Read(
         dataModel,
         Erd_IceMaker0_MoldHeaterControlRequest,
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
         Erd_IceMaker0_MoldHeaterControlRequest,
         &actual);

      CHECK_EQUAL(false, actual.enable);
   }

   void GivenIceMakerMotorVoteIs(MotorState_t state)
   {
      AluminumMoldIceMakerMotorVotedState_t vote = {
         .state = state,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote, &vote);
   }

   void GivenIceMakerWaterValveVoteIs(WaterValveState_t state)
   {
      WaterValveVotedState_t vote = {
         .state = state,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerWaterValve_IceMakerVote, &vote);
   }

   void GivenIceMakerHeaterVoteIs(HeaterState_t state)
   {
      HeaterVotedState_t vote = {
         .state = state,
         .care = true
      };
      DataModel_Write(dataModel, Erd_IceMaker0_HeaterRelay_IceMakerVote, &vote);
   }

   void GivenTheIceMakerIsolationWaterValveVoteIs(WaterValveState_t state)
   {
      WaterValveVotedState_t vote = {
         .state = state,
         .care = true
      };
      DataModel_Write(dataModel, Erd_IsolationWaterValve_AluminumMoldIceMakerVote, &vote);
   }

   void GivenTheMoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void WhenMoldThermistorIsValid()
   {
      GivenTheMoldThermistorIsValid();
   }

   void GivenMoldThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, clear);
   }

   void WhenMoldThermistorIsInvalid()
   {
      GivenMoldThermistorIsInvalid();
   }

   void GivenSkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_SkipFillRequest, set);
   }

   void GivenTheSkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_SkipFillRequest, clear);
   }

   void WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime()
   {
      After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
      RakeControllerRequestShouldBe(SET);

      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, set);
   }

   void WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFault()
   {
      RakeControllerRequestShouldBe(SET);
      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, set);
   }

   void WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix()
   {
      RakeControllerRequestShouldBe(SET);
      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, set);
   }

   void WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires()
   {
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
      RakeControllerRequestShouldBe(SET);

      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, set);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      After(1);
   }

   void WhenFillTubeHeaterOnTimeExpiresBeforeRakeCompletesRevolution()
   {
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      RakeControllerRequestShouldBe(SET);
      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, set);
   }

   void WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeHasOneMillisecondLeft()
   {
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
      RakeControllerRequestShouldBe(SET);

      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, set);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   }

   void WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet()
   {
      After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC);
      WhenRakeCompletedRevolutionIsCleared();
      RakeControllerRequestShouldBe(CLEAR);

      After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC);
      RakeControllerRequestShouldBe(SET);
   }

   void GivenRakeDidNotCompleteFullRevolution()
   {
      DataModel_Write(dataModel, Erd_IceMaker0_RakeCompletedRevolution, clear);
   }

   void WhenRakeCompletedRevolutionIsCleared()
   {
      GivenRakeDidNotCompleteFullRevolution();
   }

   void GivenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_RakePosition, &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      GivenTheRakePositionIs(position);
   }

   void SkipFillRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0_SkipFillRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void RakeControllerRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker0_RakeControlRequest, &actual);

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

      if(expected)
      {
         CHECK_EQUAL(Vote_Care, vote.care);
      }
      else
      {
         CHECK_EQUAL(Vote_DontCare, vote.care);
      }
   }

   void WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_t expected)
   {
      IceMakerWaterFillMonitoringRequest_t actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerWaterFillMonitoringRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenAluminumMoldIceMakerIsInFillState()
   {
      GivenTheMoldThermistorIsValid();
      GivenTheSkipFillRequestIsClear();
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   }

   void GivenAluminumMoldIceMakerIsInIdleFillState()
   {
      GivenAluminumMoldIceMakerIsInFillState();

      WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_Dispensing);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
   }

   void GivenAluminumMoldIceMakerIsInFillStateAfterIdleFillState()
   {
      GivenAluminumMoldIceMakerIsInIdleFillState();

      WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);
      WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   }

   void WhenStopFillSignalChanges()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_AluminumMoldIceMakerStopFillSignal);
   }

   void FreezerTriggerIceRateSignalShouldBe(Signal_t expected)
   {
      Signal_t actual;
      DataModel_Read(dataModel, Erd_FreezerIceRateTriggerSignal, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenIceRateActiveErdStateIs(bool state)
   {
      DataModel_Write(dataModel, Erd_Freezer_IceRateIsActive, &state);
   }

   void WhenIceRateActiveErdStateBecomes(bool state)
   {
      GivenIceRateActiveErdStateIs(state);
   }

   void GivenRakeCompletesItsSecondFullRevolutionInHarvestFixState()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

      WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

      WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

      WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void WhenReadyToEnterHarvest()
   {
      GivenHarvestCountIsNotReadyToHarvest();
      GivenFeelerArmIsNotReadyToEnterHarvest();

      WhenHarvestCountIsReadyToHarvest();
      WhenFeelerArmIsReadyToEnterHarvest();
   }

   void GivenBucketIsFull()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_FeelerArmPosition,
         set);
   }

   void GivenBucketIsNotFull()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0_FeelerArmPosition,
         clear);
   }

   void WhenBucketIsNotFull()
   {
      GivenBucketIsNotFull();
   }

   void GivenReadyToEnterHarvest()
   {
      WhenReadyToEnterHarvest();
   }

   void GivenTheExternalTestRequestIs(IceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_TestRequest,
         &request);
   }

   void WhenExternalTestRequestIs(IceMakerTestRequest_t request)
   {
      GivenTheExternalTestRequestIs(request);
   }

   void ExternalTestRequestShouldBe(IceMakerTestRequest_t expected)
   {
      IceMakerTestRequest_t actual;
      DataModel_Read(dataModel, Erd_IceMaker1_TestRequest, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void ExpectNoChangeInIceMakerWaterValveVote()
   {
      DataModel_SubscribeAll(dataModel, &dataModelOnChangeSubscription);
      mock().expectNoCall("No Call");
   }

   void UnsubscribeFromIceWaterValveOnChangeSubscription()
   {
      DataModel_UnsubscribeAll(dataModel, &dataModelOnChangeSubscription);
   }

   void ExpectNoChangeInIceMakerFillTubeHeaterVote()
   {
      DataModel_SubscribeAll(dataModel, &dataModelOnChangeSubscription);
      mock().expectNoCall("No Call");
   }

   void ExpectNoChangeInRakeControllerRequest()
   {
      DataModel_Subscribe(dataModel, Erd_IceMaker0_RakeControlRequest, &rakeControllerRequestOnChangeSubscription);
      mock().expectNoCall("No Call");
   }

   void WhenDispensingRequestStatusIs(DispensingRequestSelection_t selection, DispenseStatus_t status)
   {
      DispensingRequestStatus_t dispensingRequestStatus;
      DataModel_Read(dataModel, Erd_DispensingRequestStatus, &dispensingRequestStatus);

      dispensingRequestStatus.selection = selection;
      dispensingRequestStatus.status = status;
      DataModel_Write(dataModel, Erd_DispensingRequestStatus, &dispensingRequestStatus);
   }

   void GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution()
   {
      GivenTheMoldThermistorIsValid();
      GivenTheSkipFillRequestIsClear();
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
      WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   }

   void WhenIceMakerBecomes(bool state)
   {
      GivenTheIceMakerIs(state);
   }

   void GivenDispensingIsInhibitedByRfid()
   {
      DispensingInhibitedReasonBitmap_t bitmap;
      DataModel_Read(dataModel, Erd_DispensingInhibitedReason, &bitmap);

      BITMAP_SET(bitmap.bitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      DataModel_Write(dataModel, Erd_DispensingInhibitedReason, &bitmap);
   }

   void WhenDispensingIsInhibitedByRfid()
   {
      GivenDispensingIsInhibitedByRfid();
   }

   void WhenDispensingIsNotInhibitedByRfid()
   {
      DispensingInhibitedReasonBitmap_t bitmap;
      DataModel_Read(dataModel, Erd_DispensingInhibitedReason, &bitmap);

      BITMAP_CLEAR(bitmap.bitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      DataModel_Write(dataModel, Erd_DispensingInhibitedReason, &bitmap);
   }

   void GivenDispensingIsNotInhibitedByRfid()
   {
      WhenDispensingIsNotInhibitedByRfid();
   }

   void WhenIceMakerFillInhibitedDueToNewFilterIs(bool status)
   {
      IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReason;
      DataModel_Read(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);

      if(status)
      {
         BITMAP_SET(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToNewFilter);
      }
      else
      {
         BITMAP_CLEAR(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToNewFilter);
      }

      DataModel_Write(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);
   }

   void WhenIceMakerFillInhibitedDueToRfidFilterIs(bool status)
   {
      IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReason;
      DataModel_Read(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);

      if(status)
      {
         BITMAP_SET(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToRfidFilter);
      }
      else
      {
         BITMAP_CLEAR(&iceMakerFillInhibitedReason, IceMakerFillInhibitedReason_DueToRfidFilter);
      }

      DataModel_Write(
         dataModel,
         Erd_IceMakerFillInhibitedReason,
         &iceMakerFillInhibitedReason);
   }
};

TEST(AluminumMoldIceMaker, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsNotHomeAndAluminumMoldThermistorIsValid)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsHomeAndAluminumMoldThermistorIsValid)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsEnabledAndIceMakerIsEnabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsEnabledAndIceMakerIsDisabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndRakeIsHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndMoldThermistorIsInvalid)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorIsInvalid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsInvalid)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorIsInvalid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsInvalidAndRakeIsHome)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorIsInvalid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsValidAndRakeIsHome)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffWaterValveOnEntryToFreezeState)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerHeaterOnEntryToFreezeState)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   IceMakerHeaterVoteShouldBe(HeaterState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerMotorOnEntryToFreezeState)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   IceMakerMotorVoteShouldBe(MotorState_Off);
}

TEST(AluminumMoldIceMaker, ShouldRequestToMonitorFeelerArmOnEntryToFreezeState)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   HarvestCountCalculationRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldRequestToCalculateHarvestCountOnEntryToFreezeState)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   FeelerArmMonitoringRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndHarvestConditionsAreMet)
{
   GivenHarvestCountIsNotReadyToHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenHarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndIceMakerTemperatureIsNotReadyToHarvest)
{
   GivenHarvestCountIsNotReadyToHarvest();
   GivenIceMakerTemperatureIsNotReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenHarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndFeelerArmIsNotReadyToEnterHarvest)
{
   GivenHarvestCountIsNotReadyToHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenHarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenFeelerArmIsReadyToHarvestAndHarvestCountIsNotReadyToHarvest)
{
   GivenHarvestCountIsNotReadyToHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenFeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenIceMakerTemperatureIsReadyToHarvestAndHarvestConditionsAreMet)
{
   GivenHarvestCountIsReadyToHarvest();
   GivenIceMakerTemperatureIsNotReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenIceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenFeelerArmIsReadyToEnterHarvestAndHarvestConditionsAreMet)
{
   GivenHarvestCountIsReadyToHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenFeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldRequestToStopCalculatingHarvestCountWhenTransitioningFromFreezeToHarvest)
{
   GivenInitializedIntoFreezeStateAndWaitingForHarvestCountToBeReadyForHarvest();
   HarvestCountCalculationRequestShouldBe(SET);

   WhenHarvestCountIsReadyToHarvest();
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldRequestToStopMonitoringFeelerArmWhenTransitioningFromFreezeToHarvest)
{
   GivenInitializedIntoFreezeStateAndWaitingForFeelerArmToBeReadyToEnterHarvest();
   HarvestCountCalculationRequestShouldBe(SET);

   WhenFeelerArmIsReadyToEnterHarvest();
   HarvestCountCalculationRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenSabbathIsEnabled)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestWhenSabbathIsEnabled)
{
   GivenSabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestAndStillTransitionToFillWhenSabbathModeIsEnabled)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheIceMakerIs(DISABLED);
   WhenSabbathModeIs(ENABLED);
   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestAndStillTransitionToFillWhenIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheIceMakerIs(DISABLED);
   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnFillTubeHeaterWithFreezeThawFillTubeHeaterDutyCycleOnEntryToHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);
}

TEST(AluminumMoldIceMaker, ShouldVoteFillTubeHeaterDontCareAfterFillTubeHeaterOnTime)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker, ShouldRequestRakeControllerAfterInitialMinimumHeaterOnTimeOnEntryToHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(CLEAR);

   After(1);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldSetMoldHeaterControlRequestOnEntryToHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   IceMakerMoldHeaterControlRequest_t expectedRequest;
   expectedRequest.enable = true;
   expectedRequest.skipInitialOnTime = false;
   expectedRequest.onTemperatureInDegFx100 = iceMakerData->harvestData.heaterOnTemperatureInDegFx100;
   expectedRequest.offTemperatureInDegFx100 = iceMakerData->harvestData.heaterOffTemperatureInDegFx100;
   MoldHeaterControlRequestShouldBe(expectedRequest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixWhenMaxHarvestTimeReached)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToThermistorFaultWhenMoldThermistorIsInvalid)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestFaultWhenSabbathModeEnabled)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestFaultWhenIceMakeIsDisabled)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
}

TEST(AluminumMoldIceMaker, ShouldClearMoldHeaterControlRequestOnEntryToHarvestFault)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   MoldHeaterControlRequestShouldBeCleared();
}

TEST(AluminumMoldIceMaker, ShouldSetRakeMotorControlRequestOnEntryToHarvestFault)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldClearRakeMotorControlRequestAfterRakeMotorControlTimeWhileTheMotorHasBeenOn)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(SET);

   After(1);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldRepeatTurningOnAndOffRakeMotorAfterRakeMotorControlTime)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

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
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   After(iceMakerData->harvestFaultData.harvestFaultMaxTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToHarvestWhenRakeCompletesFullRevolution)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFault();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotUpdateRakeControllerRequestAfterMotorControlTimeWhenMoldThermistorIsInvalidInHarvestFix)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldUpdateRakeControllerRequestAfterMotorControlTimeWhenSabbathModeIsEnabledInHarvestFault)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   RakeControllerRequestShouldBe(SET);

   WhenSabbathModeIs(ENABLED);
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldUpdateRakeControllerRequestAfterMotorControlTimeWhenIceMakerIsDisabledInHarvestFault)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   RakeControllerRequestShouldBe(SET);

   WhenTheIceMakerIs(DISABLED);
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldVoteFillTubeHeaterDontCareWhenTransitioningFromHarvestToHarvestFix)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC + 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToThermistorFaultWhenMoldThermistorIsInvalid)
{
   GivenTheMoldThermistorIsValid();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToThermistorFaultWhenMoldThermistorIsInvalid)
{
   GivenTheMoldThermistorIsValid();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsClearAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotClearRakeControllerRequestUntilAfterWeMoveOnFromHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsSet)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenRakeDidNotCompleteFullRevolution();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenRakeDidNotCompleteFullRevolution();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionAfterMaxHarvestTimerExpiresIfAlreadyExitedHarvest)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenRakeDidNotCompleteFullRevolution();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldSetSkipFillRequestOnEntryToHarvestFix)
{
   GivenTheSkipFillRequestIsClear();
   WhenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   SkipFillRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldSetMoldHeaterControlRequestOnEntryToHarvestFix)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

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
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

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
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotRepeatedlyRequestRakeControllerAccordingToMotorOnOffTimeOnExitToHarvestFix)
{
   GivenRakeCompletesItsSecondFullRevolutionInHarvestFixState();

   After(iceMakerData->harvestFixData.motorOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFixData.motorOffTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldDisableMoldHeaterControlRequestOnExitToHarvestFix)
{
   GivenRakeCompletesItsSecondFullRevolutionInHarvestFixState();

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
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFault);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnIceMakerWaterValveWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnIsolationWaterValveWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   IsolationIceMakerWaterValveVoteShouldBeDontCare();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldStartWaterFillMonitoringWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldStopWaterFillMonitoringWhenExitingFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(AluminumMoldIceMaker, ShouldStartWaterFillMonitoringWhenEnteringFillStateAgain)
{
   GivenAluminumMoldIceMakerIsInFillState();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenReadyToEnterHarvestConditionsAreMetWhileSabbathModeIsEnabled)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();

   WhenReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenSabbathModeIsDisabledWhileTheOtherConditionsAreMet)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   GivenReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenReadyToEnterHarvestConditionsAreMetWhileIceMakerIsDisabled)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();

   WhenReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenIceMakerIsEnabledWhileTheOtherConditionsAreMet)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();
   GivenReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenCoolingSystemIsTurnedOnWhileTheOtherConditionsAreMet)
{
   GivenTheCoolingSystemIsOffAndAluminumMoldIceMakerIsInFreezeState();
   GivenReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenTheCoolingSystemOffChangesTo(false);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToFreezeWhenSabbathModeBecomesEnabledInFillStateUntilTheFillIsComplete)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToFreezeWhenIceMakerBecomesDisabledInFillStateUntilTheFillIsComplete)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToIdleFreezeWhenTheCoolingSystemIsOffInFillStateUntilTheFillIsComplete)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenTheCoolingSystemOffChangesTo(true);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerWaterValveAndIsolationWaterValveWhenExitingFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_Off);
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldIncrementFreezerTriggerIceRateSignalWhenTransitioningToTheFreezeStateExceptForInitialTransition)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenReadyToEnterHarvest();
   FreezerTriggerIceRateSignalShouldBe(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();
   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(2);

   WhenHarvestCountIsNotReadyToHarvest();
   FreezerTriggerIceRateSignalShouldBe(3);

   WhenFeelerArmIsNotReadyToEnterHarvest();
   FreezerTriggerIceRateSignalShouldBe(4);

   WhenHarvestCountIsReadyToHarvest();
   FreezerTriggerIceRateSignalShouldBe(5);

   WhenFeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenRakeCompletedRevolutionIsCleared();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();
   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(6);
}

TEST(AluminumMoldIceMaker, ShouldNotTriggerFreezerIceRateWhenSabbathModeEnabledAndIceMakerDisabled)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenIceMakerBecomes(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(0);
}

TEST(AluminumMoldIceMaker, ShouldNotTriggerFreezerIceRateWhenSabbathModeEnabled)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenSabbathModeIs(ENABLED);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenIceMakerBecomes(DISABLED);
   WhenIceMakerBecomes(ENABLED);
   FreezerTriggerIceRateSignalShouldBe(0);
}

TEST(AluminumMoldIceMaker, ShouldNotTriggerFreezerIceRateWhenIceMakerDisabled)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenIceMakerBecomes(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenSabbathModeIs(ENABLED);
   WhenSabbathModeIs(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(0);
}

TEST(AluminumMoldIceMaker, ShouldNotTriggerFreezerIceRateWhenBucketIsFull)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   GivenBucketIsFull();
   WhenIceMakerBecomes(DISABLED);
   WhenIceMakerBecomes(ENABLED);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenSabbathModeIs(ENABLED);
   WhenSabbathModeIs(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(0);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTriggerFreezerIceRateWhenBucketIsNotFullAndSabbathModeDisabledAndIceMakerEnabled)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenBucketIsNotFull();
   WhenSabbathModeIs(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(1);

   WhenSabbathModeIs(ENABLED);
   WhenSabbathModeIs(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(2);
}

TEST(AluminumMoldIceMaker, ShouldTriggerFreezerIceRateWhenFreezerIceRateAlgorithmCompletesWhileInFreezeState)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenIceRateActiveErdStateIs(SET);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenBucketIsNotFull();
   WhenIceRateActiveErdStateBecomes(CLEAR);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(1);

   WhenIceRateActiveErdStateBecomes(SET);
   WhenIceRateActiveErdStateBecomes(CLEAR);
   FreezerTriggerIceRateSignalShouldBe(2);
}

TEST(AluminumMoldIceMaker, ShouldNotRetriggerFreezerIceRateWhenFreezerIceRateIsActive)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenIceRateActiveErdStateIs(SET);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenIceRateActiveErdStateBecomes(SET);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenSabbathModeIs(ENABLED);
   WhenSabbathModeIs(DISABLED);
   FreezerTriggerIceRateSignalShouldBe(0);
}

TEST(AluminumMoldIceMaker, ShouldClearExternalTestRequestWhenFillTestIsRequested)
{
   GivenTheModuleIsInitialized();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   ExternalTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToFillStateWhenFillTestIsRequested)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenFillTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToFillStateWhenFillTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToFillStateWhenFillTestIsRequested)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldStayInThermistorFaultStateWhenFillTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionOutOfFillStateWhenFillTestIsRequestedWhileFillIsAlreadyTakingPlace)
{
   GivenAluminumMoldIceMakerIsInFillState();
   ExpectNoChangeInIceMakerWaterValveVote();

   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   UnsubscribeFromIceWaterValveOnChangeSubscription();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerWaterValveWhenMoldThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheMoldThermistorIsValid();
   GivenTheModuleIsInitialized();
   GivenIceMakerWaterValveVoteIs(WaterValveState_On);

   WhenMoldThermistorIsInvalid();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIsolationValveWhenMoldThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheMoldThermistorIsValid();
   GivenTheModuleIsInitialized();
   GivenTheIceMakerIsolationWaterValveVoteIs(WaterValveState_On);

   WhenMoldThermistorIsInvalid();
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerHeaterWhenMoldThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheMoldThermistorIsValid();
   GivenTheModuleIsInitialized();
   GivenIceMakerHeaterVoteIs(HeaterState_On);

   WhenMoldThermistorIsInvalid();
   IceMakerHeaterVoteShouldBe(HeaterState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerMotorWhenMoldThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheMoldThermistorIsValid();
   GivenTheModuleIsInitialized();
   GivenIceMakerMotorVoteIs(MotorState_On);

   WhenMoldThermistorIsInvalid();
   IceMakerMotorVoteShouldBe(MotorState_Off);
}

TEST(AluminumMoldIceMaker, ShouldBeInThermistorFaultStateIfThermistorIsInvalidBeforeInitialization)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenMoldThermistorIsInvalid();
   GivenTheModuleIsInitialized();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultFromFreezeWhenThermistorIsInvalid)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultFromHarvestWhenThermistorIsInvalid)
{
   GivenSabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionOutOfFillStateWhenSignalStopFillNotReceivedAndThermistorIsInvalid)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToFreezeAfterFillWhenReceivedSignalStopFillAndThermistorIsValid)
{
   GivenAluminumMoldIceMakerIsInFillState();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToFreezeStateWhenSignalStopFillReceivedAndThermistorIsValidAndRakeIsHome)
{
   GivenAluminumMoldIceMakerIsInFillState();
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToHarvestStateWhenSignalStopFillReceivedAndThermistorIsValidAndRakeIsNotHome)
{
   GivenAluminumMoldIceMakerIsInFillState();
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultAfterFillWhenSignalStopFillReceivedAndThermistorIsInvalid)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultAfterHarvestFixWhenThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromThermistorFaultToFreezeWhenThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsHome)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenMoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldSetSkipFillRequestAndShouldTransitionFromThermistorFaultStateToHarvestWhenThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsNotHome)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheSkipFillRequestIsClear();
   GivenTheModuleIsInitialized();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenMoldThermistorIsValid();
   SkipFillRequestShouldBe(set);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldCompleteFillEvenIfThermistorBecomesInvalid)
{
   GivenTheModuleIsInitialized();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToThermistorFaultWhenThermistorIsInvalidAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToHarvestWhenRakeIsNotHomeAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldSetSkipFillFlagWhenTransitioningFromFillToHarvestWhenRakeIsNotHomeAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   SkipFillRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToFreezeWhenRakeIsHomeAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenTheRakePositionIs(RakePosition_Home);
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeAfterSecondRevolutionWhenReEnteringIntoHarvestFixState)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   WhenRakeCompletedRevolutionIsCleared();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenIceMakerTemperatureIsNotReadyToHarvest();
   WhenIceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeAfterSecondRevolutionWhenReEnteringIntoHarvestFixAfterFirstRevolutionAndMoldThermistorIsInvalidInTheState)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   WhenRakeCompletedRevolutionIsCleared();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenMoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldSetTestRequestToNoneAfterTestRequestIsSetToHarvest)
{
   GivenTheModuleIsInitialized();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   ExternalTestRequestShouldBe(IceMakerTestRequest_None);
}

TEST(AluminumMoldIceMaker, ShouldRemainInHarvestWhenTestRequestBecomesHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   ExpectNoChangeInIceMakerFillTubeHeaterVote();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldRemainInThermistorFaultWhenExternalTestRequestBecomesHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToHarvestStateWhenHarvestTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestStateWhenHarvestTestIsRequested)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToHarvestStateWhenHarvestTestIsRequested)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToHarvestStateWhenHarvestTestIsRequested)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterMaxHarvestTimeWhenSabbathModeIsEnabledAndHarvestIsEnteredViaTestRequest)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterMaxHarvestTimeWhenIceMakerDisabledAndHarvestIsEnteredViaTestRequest)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterFillTubeHeaterOnTimeExpiredWhenSabbathModeIsEnabledAndHarvestIsEnteredViaTestRequest)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterFillTubeHeaterOnTimeWhenIceMakerDisabledAndHarvestIsEnteredViaTestRequest)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenWaterDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenAutofillDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Autofill, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenIceMakerFillIsInhibitedDueToNewFilterWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenIceMakerFillInhibitedDueToNewFilterIs(SET);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenIceMakerFillIsInhibitedDueToRfidFilterWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenIceMakerFillInhibitedDueToRfidFilterIs(SET);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToCompletedSuccessfullyWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToSensorErrorWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_SensorError);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDisabledOrBlockedWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DisabledOrBlocked);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToHitMaxVolumeWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_HitMaxVolume);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToHitMaxTimeWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_HitMaxTime);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDidNotReceiveDispenseRequestAfterHeartbeatWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DidNotReceiveDispenseRequestAfterHeartbeat);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToPreciseFillAmountReachedWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_PreciseFillAmountReached);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToBadCommandWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_BadCommand);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDispenseInhibitedDueToDoorOpenWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DispenseInhibitedDueToDoorOpen);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDispenseInhibitedBecauseRedundantCupSwitchPressIsRequiredWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DispenseInhibitedBecauseRedundantCupSwitchPressIsRequired);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDispenseInhibitedDueToRfidErrorOrLeakWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotPauseFillMonitoringWhenCrushedIceDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_CrushedIce, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotPauseFillMonitoringWhenCubedIceDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_CubedIce, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotStartFillMonitoringWhenEnteringFillStateDuringWaterDispensingUntilDispensingIsCompleted)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After((iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds -
            iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds) *
         MSEC_PER_SEC -
      1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_Dispensing);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotStartFillMonitoringWhenEnteringFillStateWhileIceMakerFillInhibitedDueToNewFilterUntilFillIsNotInhibitedDueToNewFilter)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After((iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds -
            iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds) *
         MSEC_PER_SEC -
      1);
   WhenIceMakerFillInhibitedDueToNewFilterIs(SET);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenIceMakerFillInhibitedDueToNewFilterIs(CLEAR);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotStartFillMonitoringWhenEnteringFillStateWhileIceMakerFillInhibitedDueToRfidFilterUntilFillIsNotInhibitedDueToRfidFilter)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After((iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds -
            iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds) *
         MSEC_PER_SEC -
      1);
   WhenIceMakerFillInhibitedDueToRfidFilterIs(SET);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenIceMakerFillInhibitedDueToRfidFilterIs(CLEAR);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotDelayFillMonitoringIfWaterDispensingFinishesBeforeFillStateIsEntered)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC -
      iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 2);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_Dispensing);

   After(1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldNotDelayFillMonitoringWhenEnteringFillStateDuringCubedIceDispensing)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_CubedIce, DispenseStatus_Dispensing);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldNotDelayMonitoringWhenEnteringFillStateDuringCrushedIceDispensing)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_CrushedIce, DispenseStatus_Dispensing);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToFillStateWhenFillTestIsRequestedWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToThermistorFaultStateWhenMoldThermistorIsInvalidWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldStopFillMonitoringWhenReceivingStopFillSignalAfterFillMonitoringIsResumed)
{
   GivenAluminumMoldIceMakerIsInFillStateAfterIdleFillState();

   WhenStopFillSignalChanges();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(AluminumMoldIceMaker, ShouldVoteDontCareForFillTubeHeaterAfterFillTubeHeaterOnTime)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSet)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenFillTubeHeaterOnTimeExpiresBeforeRakeCompletesRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenFillTubeHeaterOnTimeExpiresBeforeRakeCompletesRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldSetRakeControllerRequestWhenTransitioningFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenRakeDidNotCompleteFullRevolution();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   RakeControllerRequestShouldBe(SET);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionAfterFillTubeHeaterTimerExpiresWhenExitConditionsToGoToFillAreMetIfAlreadyExitedHarvest)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeHasOneMillisecondLeft();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleAndOnTimeAreZeroAndSkipFillRequestIsSet)
{
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithZeroFillTubeHeaterDutyCycleAndOnTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleAndOnTimeAreZeroAndSkipFillRequestIsClear)
{
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithZeroFillTubeHeaterDutyCycleAndOnTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndWhenFillTubeHeaterOnTimeEqualsMaxHarvestTimeAndSkipFillRequestIsSet)
{
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithFillTubeHeaterOnTimeEqualToMaxHarvestTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();

   After(iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC -
      iceMakerModifiedData.harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndWhenFillTubeHeaterOnTimeEqualsMaxHarvestTimeAndSkipFillRequestIsClear)
{
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithFillTubeHeaterOnTimeEqualToMaxHarvestTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();

   After(iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC -
      iceMakerModifiedData.harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixAfterFillTubeHeaterOnTimeWhenFillTubeHeaterOnTimeEqualsMaxHarvestTime)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithFillTubeHeaterOnTimeEqualToMaxHarvestTime();

   After(iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenTheOtherHarvestConditionsAreMetWhileDispensingIsInhibitedByRfid)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenHarvestCountIsReadyToHarvest();
   GivenIceMakerTemperatureIsNotReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenDispensingIsInhibitedByRfid();
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenIceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenDispensingIsNotInhibitedByRfidWhileTheOtherHarvestConditionsAreMet)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenHarvestCountIsReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenDispensingIsInhibitedByRfid();
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenDispensingIsNotInhibitedByRfid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenDispensingIsInhibitedByRfidWhileTheOtherHarvestConditionsAreMet)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenHarvestCountIsReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenDispensingIsNotInhibitedByRfid();
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenDispensingIsInhibitedByRfid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}
