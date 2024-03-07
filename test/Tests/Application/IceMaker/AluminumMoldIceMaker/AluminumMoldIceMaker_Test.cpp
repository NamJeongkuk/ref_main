/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AluminumMoldIceMaker.h"
#include "IceMakerStateMachineState.h"
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
   .aluminumMoldIceMakerHsmStateErd = Erd_IceMaker1_StateMachineState,
   .iceMakerWaterValveVoteErd = Erd_IceMaker1_WaterValve_IceMakerVote,
   .moldHeaterVoteErd = Erd_IceMaker1_HeaterRelay_IceMakerVote,
   .rakeMotorVoteErd = Erd_IceMaker1_RakeMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_IceMaker1_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_IceMaker1_FeelerArmMonitoringRequest,
   .feelerArmPositionErd = Erd_IceMaker1_FeelerArmPosition,
   .harvestCountIsReadyToHarvestErd = Erd_IceMaker1_HarvestCountIsReadyToHarvest,
   .moldFilteredTemperatureInDegFx100Erd = Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_IceMaker1_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMaker1_EnabledResolved,
   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .fillTubeHeaterVoteErd = Erd_IceMaker1_FillTubeHeater_IceMakerVote,
   .moldHeaterControlRequestErd = Erd_IceMaker1_MoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_IceMaker1_RakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_IceMaker1_MoldThermistor_IsValidResolved,
   .skipFillRequestErd = Erd_IceMaker1_SkipFillRequest,
   .rakeControlRequestErd = Erd_IceMaker1_RakeControlRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_IceMaker1Vote,
   .waterFillMonitoringRequestErd = Erd_IceMaker1_WaterFillMonitoringRequest,
   .stopIceMakerFillSignalErd = Erd_IceMaker1_StopFillSignal,
   .rakePositionErd = Erd_IceMaker1_RakePosition,
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

      if(onChangeData->erd == Erd_IceMaker1_FillTubeHeater_IceMakerVote)
      {
         mock().actualCall("Ice Maker Fill Tube Heater Vote Changed");
      }
      else if(onChangeData->erd == Erd_IceMaker1_WaterValve_IceMakerVote)
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

   void AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_t expected)
   {
      IceMakerStateMachineState_t actual;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_StateMachineState,
         &actual);

      CHECK_EQUAL(expected, actual);
   }

   void IceMakerWaterValveVoteShouldBe(bool expected)
   {
      WaterValveVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_WaterValve_IceMakerVote,
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
         Erd_IceMaker1_HeaterRelay_IceMakerVote,
         &vote);
   }

   void IceMakerHeaterVoteShouldBe(bool expected)
   {
      HeaterVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_HeaterRelay_IceMakerVote,
         &vote);

      CHECK_EQUAL(expected, vote.state);
      CHECK_FALSE(vote.care);
   }

   void GivenIceMakerMotorVoteIsSet()
   {
      IceMakerMotorVotedState_t vote = {
         .state = IceMakerMotorState_Run,
         .care = Vote_Care
      };

      DataModel_Write(
         dataModel,
         Erd_IceMaker1_RakeMotor_IceMakerVote,
         &vote);
   }

   void IceMakerMotorVoteShouldBe(bool expected)
   {
      IceMakerMotorVotedState_t vote;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_RakeMotor_IceMakerVote,
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
         Erd_IceMaker1_FeelerArmMonitoringRequest,
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
         Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
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
         Erd_IceMaker1_MoldThermistor_FilteredTemperatureResolvedInDegFx100,
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
         Erd_IceMaker1_FeelerArmIsReadyToEnterHarvest,
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
         Erd_IceMaker1_FeelerArmIsReadyToEnterHarvest,
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenInitializedIntoFreezeStateAndWaitingForFeelerArmToBeReadyToEnterHarvest()
   {
      GivenHarvestCountIsReadyToHarvest();
      GivenIceMakerTemperatureIsReadyToHarvest();
      GivenFeelerArmIsNotReadyToEnterHarvest();
      GivenTheAluminumMoldIceMakerIsInFreezeState();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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

   void GivenEnhancedSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_EnhancedSabbathModeEnable,
         &state);
   }

   void WhenEnhancedSabbathModeIs(bool state)
   {
      GivenEnhancedSabbathModeIs(state);
   }

   void GivenTheIceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_EnabledResolved,
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
      GivenEnhancedSabbathModeIs(DISABLED);
   }

   void GivenTheAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenEnhancedSabbathModeIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenEnhancedSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenEnhancedSabbathModeIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

      WhenTheIceMakerIs(DISABLED);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenTheCoolingSystemIsOffAndAluminumMoldIceMakerIsInFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

      WhenTheCoolingSystemOffChangesTo(true);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   }

   void GivenSabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenEnhancedSabbathModeIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenHarvestCountIsReadyToHarvest();
      GivenIceMakerTemperatureIsNotReadyToHarvest();
      GivenFeelerArmIsReadyToEnterHarvest();
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

      WhenIceMakerTemperatureIsReadyToHarvest();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
   }

   void GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

      After(1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
   }

   void WhenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

      After(1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
   }

   void GivenTheAluminumMoldIceMakerIsInHarvestFault()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

      After(iceMakerData->harvestFixData.maximumHarvestFixTimeInMinutes * MSEC_PER_MIN);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
   }

   void GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenEnhancedSabbathModeIs(DISABLED);
      GivenMoldThermistorIsInvalid();
      GivenTheRakePositionIs(RakePosition_NotHome);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
   }

   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t expectedDutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_FillTubeHeater_IceMakerVote,
         &vote);

      CHECK_EQUAL(expectedDutyCycle, vote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, vote.care);
   }

   void MoldHeaterControlRequestShouldBe(IceMakerMoldHeaterControlRequest_t expectedRequest)
   {
      IceMakerMoldHeaterControlRequest_t actualRequest;
      DataModel_Read(
         dataModel,
         Erd_IceMaker1_MoldHeaterControlRequest,
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
         Erd_IceMaker1_MoldHeaterControlRequest,
         &actual);

      CHECK_EQUAL(false, actual.enable);
   }

   void GivenIceMakerMotorVoteIs(IceMakerMotorState_t state)
   {
      IceMakerMotorVotedState_t vote = {
         .state = state,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_IceMaker1_RakeMotor_IceMakerVote, &vote);
   }

   void GivenIceMakerWaterValveVoteIs(WaterValveState_t state)
   {
      WaterValveVotedState_t vote = {
         .state = state,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_IceMaker1_WaterValve_IceMakerVote, &vote);
   }

   void GivenIceMakerHeaterVoteIs(HeaterState_t state)
   {
      HeaterVotedState_t vote = {
         .state = state,
         .care = true
      };
      DataModel_Write(dataModel, Erd_IceMaker1_HeaterRelay_IceMakerVote, &vote);
   }

   void GivenTheIceMakerIsolationWaterValveVoteIs(WaterValveState_t state)
   {
      WaterValveVotedState_t vote = {
         .state = state,
         .care = true
      };
      DataModel_Write(dataModel, Erd_IsolationWaterValve_IceMaker1Vote, &vote);
   }

   void GivenTheMoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_IceMaker1_MoldThermistor_IsValidResolved, set);
   }

   void WhenMoldThermistorIsValid()
   {
      GivenTheMoldThermistorIsValid();
   }

   void GivenMoldThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_IceMaker1_MoldThermistor_IsValidResolved, clear);
   }

   void WhenMoldThermistorIsInvalid()
   {
      GivenMoldThermistorIsInvalid();
   }

   void GivenSkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_IceMaker1_SkipFillRequest, set);
   }

   void GivenTheSkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_IceMaker1_SkipFillRequest, clear);
   }

   void WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime()
   {
      After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
      RakeControllerRequestShouldBe(SET);

      DataModel_Write(dataModel, Erd_IceMaker1_RakeCompletedRevolution, set);
   }

   void WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFault()
   {
      RakeControllerRequestShouldBe(SET);
      DataModel_Write(dataModel, Erd_IceMaker1_RakeCompletedRevolution, set);
   }

   void WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix()
   {
      RakeControllerRequestShouldBe(SET);
      DataModel_Write(dataModel, Erd_IceMaker1_RakeCompletedRevolution, set);
   }

   void WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires()
   {
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
      RakeControllerRequestShouldBe(SET);

      DataModel_Write(dataModel, Erd_IceMaker1_RakeCompletedRevolution, set);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

      After(1);
   }

   void WhenFillTubeHeaterOnTimeExpiresBeforeRakeCompletesRevolution()
   {
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

      RakeControllerRequestShouldBe(SET);
      DataModel_Write(dataModel, Erd_IceMaker1_RakeCompletedRevolution, set);
   }

   void WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeHasOneMillisecondLeft()
   {
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
      RakeControllerRequestShouldBe(SET);

      DataModel_Write(dataModel, Erd_IceMaker1_RakeCompletedRevolution, set);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
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
      DataModel_Write(dataModel, Erd_IceMaker1_RakeCompletedRevolution, clear);
   }

   void WhenRakeCompletedRevolutionIsCleared()
   {
      GivenRakeDidNotCompleteFullRevolution();
   }

   void GivenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_IceMaker1_RakePosition, &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      GivenTheRakePositionIs(position);
   }

   void SkipFillRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker1_SkipFillRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void RakeControllerRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMaker1_RakeControlRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void IsolationIceMakerWaterValveVoteShouldBeDontCare()
   {
      WaterValveVotedState_t vote;
      DataModel_Read(dataModel, Erd_IsolationWaterValve_IceMaker1Vote, &vote);

      CHECK_EQUAL(Vote_DontCare, vote.care);
   }

   void IsolationIceMakerWaterValveVoteShouldBe(bool expected)
   {
      WaterValveVotedState_t vote;
      DataModel_Read(dataModel, Erd_IsolationWaterValve_IceMaker1Vote, &vote);

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
      DataModel_Read(dataModel, Erd_IceMaker1_WaterFillMonitoringRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenAluminumMoldIceMakerIsInFillState()
   {
      GivenTheMoldThermistorIsValid();
      GivenTheSkipFillRequestIsClear();
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

      WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
      After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   }

   void GivenAluminumMoldIceMakerIsInIdleFillState()
   {
      GivenAluminumMoldIceMakerIsInFillState();

      WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_Dispensing);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
   }

   void GivenAluminumMoldIceMakerIsInFillStateAfterIdleFillState()
   {
      GivenAluminumMoldIceMakerIsInIdleFillState();

      WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);
      WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   }

   void WhenStopFillSignalChanges()
   {
      Signal_SendViaErd(DataModel_AsDataSource(dataModel), Erd_IceMaker1_StopFillSignal);
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
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

      WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

      WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
      AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
         Erd_IceMaker1_FeelerArmPosition,
         set);
   }

   void GivenBucketIsNotFull()
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker1_FeelerArmPosition,
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
      DataModel_Subscribe(dataModel, Erd_IceMaker1_RakeControlRequest, &rakeControllerRequestOnChangeSubscription);
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
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsHomeAndAluminumMoldThermistorIsValid)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsEnabledAndIceMakerIsEnabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfEnhancedSabbathModeIsEnabledAndIceMakerIsEnabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(ENABLED);
   GivenEnhancedSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsEnabledAndIceMakerIsDisabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfEnhancedSabbathModeIsEnabledAndIceMakerIsDisabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenEnhancedSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToHarvestStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndRakePositionIsNotHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndRakeIsHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndMoldThermistorIsInvalid)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenMoldThermistorIsInvalid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsInvalid)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenMoldThermistorIsInvalid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToThermistorFaultStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsInvalidAndRakeIsHome)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenMoldThermistorIsInvalid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsEnabledAndAluminumMoldThermistorIsValidAndRakeIsHome)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   IceMakerMotorVoteShouldBe(IceMakerMotorState_Off);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndIceMakerTemperatureIsNotReadyToHarvest)
{
   GivenHarvestCountIsNotReadyToHarvest();
   GivenIceMakerTemperatureIsNotReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenHarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenHarvestCountIsReadyToHarvestAndFeelerArmIsNotReadyToEnterHarvest)
{
   GivenHarvestCountIsNotReadyToHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenHarvestCountIsReadyToHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionFromFreezeToHarvestWhenFeelerArmIsReadyToHarvestAndHarvestCountIsNotReadyToHarvest)
{
   GivenHarvestCountIsNotReadyToHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenFeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenIceMakerTemperatureIsReadyToHarvestAndHarvestConditionsAreMet)
{
   GivenHarvestCountIsReadyToHarvest();
   GivenIceMakerTemperatureIsNotReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenIceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestWhenFeelerArmIsReadyToEnterHarvestAndHarvestConditionsAreMet)
{
   GivenHarvestCountIsReadyToHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenFeelerArmIsNotReadyToEnterHarvest();
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenFeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestWhenSabbathIsEnabled)
{
   GivenSabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestAndStillTransitionToFillWhenSabbathModeIsEnabled)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheIceMakerIs(DISABLED);
   WhenSabbathModeIs(ENABLED);
   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestAndStillTransitionToFillWhenIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheIceMakerIs(DISABLED);
   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToThermistorFaultWhenMoldThermistorIsInvalid)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestFaultWhenSabbathModeEnabled)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
}

TEST(AluminumMoldIceMaker, ShouldNotInterruptHarvestFaultWhenIceMakeIsDisabled)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToHarvestWhenRakeCompletesFullRevolution)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFault();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotUpdateRakeControllerRequestAfterMotorControlTimeWhenMoldThermistorIsInvalidInHarvestFix)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC + 1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->harvestData.fillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToThermistorFaultWhenMoldThermistorIsInvalid)
{
   GivenTheMoldThermistorIsValid();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToThermistorFaultWhenMoldThermistorIsInvalid)
{
   GivenTheMoldThermistorIsValid();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsClearAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenRakeDidNotCompleteFullRevolution();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionAfterMaxHarvestTimerExpiresIfAlreadyExitedHarvest)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenRakeDidNotCompleteFullRevolution();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFault);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnIceMakerWaterValveWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnIsolationWaterValveWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   IsolationIceMakerWaterValveVoteShouldBeDontCare();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldStartWaterFillMonitoringWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldStopWaterFillMonitoringWhenExitingFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);
}

TEST(AluminumMoldIceMaker, ShouldStartWaterFillMonitoringWhenEnteringFillStateAgain)
{
   GivenAluminumMoldIceMakerIsInFillState();
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeExpires();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenReadyToEnterHarvestConditionsAreMetWhileSabbathModeIsEnabled)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();

   WhenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenReadyToEnterHarvestConditionsAreMetWhileEnhancedSabbathModeIsEnabled)
{
   GivenEnhancedSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();

   WhenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenSabbathModeIsDisabledWhileTheOtherConditionsAreMet)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   GivenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenEnhancedSabbathModeIsDisabledWhileTheOtherConditionsAreMet)
{
   GivenEnhancedSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   GivenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenEnhancedSabbathModeIs(DISABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenReadyToEnterHarvestConditionsAreMetWhileIceMakerIsDisabled)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();

   WhenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenIceMakerIsEnabledWhileTheOtherConditionsAreMet)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();
   GivenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenCoolingSystemIsTurnedOnWhileTheOtherConditionsAreMet)
{
   GivenTheCoolingSystemIsOffAndAluminumMoldIceMakerIsInFreezeState();
   GivenReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenTheCoolingSystemOffChangesTo(false);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToFreezeWhenSabbathModeBecomesEnabledInFillStateUntilTheFillIsComplete)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToFreezeWhenIceMakerBecomesDisabledInFillStateUntilTheFillIsComplete)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToIdleFreezeWhenTheCoolingSystemIsOffInFillStateUntilTheFillIsComplete)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenTheCoolingSystemOffChangesTo(true);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerWaterValveAndIsolationWaterValveWhenExitingFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_Off);
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldIncrementFreezerTriggerIceRateSignalWhenTransitioningToTheFreezeStateExceptForInitialTransition)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenReadyToEnterHarvest();
   FreezerTriggerIceRateSignalShouldBe(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();
   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(2);

   WhenHarvestCountIsNotReadyToHarvest();
   FreezerTriggerIceRateSignalShouldBe(3);

   WhenFeelerArmIsNotReadyToEnterHarvest();
   FreezerTriggerIceRateSignalShouldBe(4);

   WhenHarvestCountIsReadyToHarvest();
   FreezerTriggerIceRateSignalShouldBe(5);

   WhenFeelerArmIsReadyToEnterHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenRakeCompletedRevolutionIsCleared();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();
   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(6);
}

TEST(AluminumMoldIceMaker, ShouldNotTriggerFreezerIceRateWhenSabbathModeEnabledAndIceMakerDisabled)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenIceMakerBecomes(DISABLED);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenFillTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToFillStateWhenFillTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToFillStateWhenFillTestIsRequested)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldStayInThermistorFaultStateWhenFillTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionOutOfFillStateWhenFillTestIsRequestedWhileFillIsAlreadyTakingPlace)
{
   GivenAluminumMoldIceMakerIsInFillState();
   ExpectNoChangeInIceMakerWaterValveVote();

   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   UnsubscribeFromIceWaterValveOnChangeSubscription();

   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   GivenIceMakerMotorVoteIs(IceMakerMotorState_Run);

   WhenMoldThermistorIsInvalid();
   IceMakerMotorVoteShouldBe(IceMakerMotorState_Off);
}

TEST(AluminumMoldIceMaker, ShouldBeInThermistorFaultStateIfThermistorIsInvalidBeforeInitialization)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenMoldThermistorIsInvalid();
   GivenTheModuleIsInitialized();

   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultFromFreezeWhenThermistorIsInvalid)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultFromHarvestWhenThermistorIsInvalid)
{
   GivenSabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionOutOfFillStateWhenSignalStopFillNotReceivedAndThermistorIsInvalid)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToFreezeAfterFillWhenReceivedSignalStopFillAndThermistorIsValid)
{
   GivenAluminumMoldIceMakerIsInFillState();

   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToFreezeStateWhenSignalStopFillReceivedAndThermistorIsValidAndRakeIsHome)
{
   GivenAluminumMoldIceMakerIsInFillState();
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToHarvestStateWhenSignalStopFillReceivedAndThermistorIsValidAndRakeIsNotHome)
{
   GivenAluminumMoldIceMakerIsInFillState();
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultAfterFillWhenSignalStopFillReceivedAndThermistorIsInvalid)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToThermistorFaultAfterHarvestFixWhenThermistorIsInvalid)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromThermistorFaultToFreezeWhenThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsHome)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);

   WhenMoldThermistorIsValid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldSetSkipFillRequestAndShouldTransitionFromThermistorFaultStateToHarvestWhenThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabledAndRakeIsNotHome)
{
   GivenTheIceMakerIsEnabledAndSabbathModeIsDisabled();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheSkipFillRequestIsClear();
   GivenTheModuleIsInitialized();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);

   WhenMoldThermistorIsValid();
   SkipFillRequestShouldBe(set);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldCompleteFillEvenIfThermistorBecomesInvalid)
{
   GivenTheModuleIsInitialized();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToThermistorFaultWhenThermistorIsInvalidAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenMoldThermistorIsInvalid();
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToHarvestWhenRakeIsNotHomeAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldSetSkipFillFlagWhenTransitioningFromFillToHarvestWhenRakeIsNotHomeAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
   SkipFillRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToFreezeWhenRakeIsHomeAfterFillStops)
{
   GivenTheModuleIsInitialized();
   GivenTheMoldThermistorIsValid();
   GivenAluminumMoldIceMakerIsInFillState();

   WhenTheRakePositionIs(RakePosition_Home);
   WhenStopFillSignalChanges();

   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeAfterSecondRevolutionWhenReEnteringIntoHarvestFixState)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   WhenRakeCompletedRevolutionIsCleared();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenIceMakerTemperatureIsNotReadyToHarvest();
   WhenIceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeAfterSecondRevolutionWhenReEnteringIntoHarvestFixAfterFirstRevolutionAndMoldThermistorIsInvalidInTheState)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
   WhenRakeCompletedRevolutionIsCleared();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenMoldThermistorIsValid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);

   WhenMotorOnTimeExpiresAndRakeRequestIsClearedThenMotorOffTimeExpiresAndRakeRequestIsSet();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentDuringHarvestFix();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFixToHarvestStateWhenHarvestTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToHarvestStateWhenHarvestTestIsRequested)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFillToHarvestStateWhenHarvestTestIsRequested)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToHarvestStateWhenHarvestTestIsRequested)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterMaxHarvestTimeWhenSabbathModeIsEnabledAndHarvestIsEnteredViaTestRequest)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterMaxHarvestTimeWhenIceMakerDisabledAndHarvestIsEnteredViaTestRequest)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterFillTubeHeaterOnTimeExpiredWhenSabbathModeIsEnabledAndHarvestIsEnteredViaTestRequest)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldProceedToFillStateAfterFillTubeHeaterOnTimeWhenIceMakerDisabledAndHarvestIsEnteredViaTestRequest)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenWaterDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenAutofillDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Autofill, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenIceMakerFillIsInhibitedDueToNewFilterWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenIceMakerFillInhibitedDueToNewFilterIs(SET);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldPauseFillMonitoringAndTransitionToIdleFillStateWhenIceMakerFillIsInhibitedDueToRfidFilterWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenIceMakerFillInhibitedDueToRfidFilterIs(SET);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Pause);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToCompletedSuccessfullyWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToSensorErrorWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_SensorError);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDisabledOrBlockedWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DisabledOrBlocked);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToHitMaxVolumeWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_HitMaxVolume);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToHitMaxTimeWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_HitMaxTime);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDidNotReceiveDispenseRequestAfterHeartbeatWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DidNotReceiveDispenseRequestAfterHeartbeat);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToPreciseFillAmountReachedWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_PreciseFillAmountReached);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToBadCommandWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_BadCommand);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDispenseInhibitedDueToDoorOpenWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DispenseInhibitedDueToDoorOpen);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDispenseInhibitedBecauseRedundantCupSwitchPressIsRequiredWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DispenseInhibitedBecauseRedundantCupSwitchPressIsRequired);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldResumeFillMonitoringAndTransitionToFillStateWhenDispensingStatusChangesToDispenseInhibitedDueToRfidErrorOrLeakWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_DispenseInhibitedDueToRfidErrorOrLeak);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Resume);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotPauseFillMonitoringWhenCrushedIceDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_CrushedIce, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotPauseFillMonitoringWhenCubedIceDispensingIsStartedWhileInFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();

   WhenDispensingRequestStatusIs(DispensingRequestSelection_CubedIce, DispenseStatus_Dispensing);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenIceMakerFillInhibitedDueToNewFilterIs(CLEAR);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenIceMakerFillInhibitedDueToRfidFilterIs(CLEAR);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldNotDelayFillMonitoringWhenEnteringFillStateDuringCubedIceDispensing)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_CubedIce, DispenseStatus_Dispensing);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldNotDelayMonitoringWhenEnteringFillStateDuringCrushedIceDispensing)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After(iceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_CrushedIce, DispenseStatus_Dispensing);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToFillStateWhenFillTestIsRequestedWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionToThermistorFaultStateWhenMoldThermistorIsInvalidWhileInIdleFillState)
{
   GivenAluminumMoldIceMakerIsInIdleFillState();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_IdleFill);
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
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenFillTubeHeaterOnTimeExpiresBeforeRakeCompletesRevolution();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldSetRakeControllerRequestWhenTransitioningFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenRakeDidNotCompleteFullRevolution();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - 1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
   RakeControllerRequestShouldBe(SET);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
   RakeControllerRequestShouldBe(SET);
}

TEST(AluminumMoldIceMaker, ShouldNotTransitionAfterFillTubeHeaterTimerExpiresWhenExitConditionsToGoToFillAreMetIfAlreadyExitedHarvest)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenRakeCompletesRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTimeDuringFillTubeHeaterOnTimeAndFillTubeHeaterTimeHasOneMillisecondLeft();

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);

   After(1);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleAndOnTimeAreZeroAndSkipFillRequestIsSet)
{
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithZeroFillTubeHeaterDutyCycleAndOnTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleAndOnTimeAreZeroAndSkipFillRequestIsClear)
{
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithZeroFillTubeHeaterDutyCycleAndOnTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndWhenFillTubeHeaterOnTimeEqualsMaxHarvestTimeAndSkipFillRequestIsSet)
{
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithFillTubeHeaterOnTimeEqualToMaxHarvestTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();

   After(iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC -
      iceMakerModifiedData.harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndWhenFillTubeHeaterOnTimeEqualsMaxHarvestTimeAndSkipFillRequestIsClear)
{
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithFillTubeHeaterOnTimeEqualToMaxHarvestTime();

   WhenTheRakeCompletesFullRevolutionAfterRequestSentAfterInitialMinimumHeaterOnTime();

   After(iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC -
      iceMakerModifiedData.harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToHarvestFixAfterFillTubeHeaterOnTimeWhenFillTubeHeaterOnTimeEqualsMaxHarvestTime)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestWithFillTubeHeaterOnTimeEqualToMaxHarvestTime();

   After(iceMakerModifiedData.harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_HarvestFix);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenTheOtherHarvestConditionsAreMetWhileDispensingIsInhibitedByRfid)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenHarvestCountIsReadyToHarvest();
   GivenIceMakerTemperatureIsNotReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenDispensingIsInhibitedByRfid();
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenIceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionToHarvestWhenDispensingIsNotInhibitedByRfidWhileTheOtherHarvestConditionsAreMet)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenHarvestCountIsReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenDispensingIsInhibitedByRfid();
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenDispensingIsNotInhibitedByRfid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldStayInFreezeWhenDispensingIsInhibitedByRfidWhileTheOtherHarvestConditionsAreMet)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenEnhancedSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenHarvestCountIsReadyToHarvest();
   GivenFeelerArmIsReadyToEnterHarvest();
   GivenIceMakerTemperatureIsReadyToHarvest();
   GivenDispensingIsNotInhibitedByRfid();
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);

   WhenDispensingIsInhibitedByRfid();
   AluminumMoldIceMakerStateMachineStateShouldBe(IceMakerStateMachineState_Freeze);
}
