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
   .iceMakerHeaterVoteErd = Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote,
   .iceMakerMotorVoteErd = Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .harvestCountIsReadyToHarvestErd = Erd_HarvestCountIsReadyToHarvest,
   .iceMakerFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMaker0EnableStatus,
   .sabbathModeErd = Erd_SabbathMode,
   .fillTubeHeaterVoteErd = Erd_FillTubeHeater_AluminumMoldIceMakerVote,
   .moldHeaterControlRequestErd = Erd_AluminumMoldIceMakerMoldHeaterControlRequest,
   .rakeCompletedRevolutionErd = Erd_AluminumMoldIceMakerRakeCompletedRevolution,
   .moldThermistorIsValidErd = Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved,
   .skipFillRequestErd = Erd_AluminumMoldIceMakerSkipFillRequest,
   .rakeControlRequestErd = Erd_AluminumMoldIceMakerRakeControlRequest,
   .isolationWaterValveVoteErd = Erd_IsolationWaterValve_AluminumMoldIceMakerVote,
   .waterFillMonitoringRequestErd = Erd_AluminumMoldIceMakerWaterFillMonitoringRequest,
   .stopFillSignalErd = Erd_AluminumMoldIceMakerStopFillSignal,
   .rakePositionErd = Erd_AluminumMoldIceMakerRakePosition,
   .freezerIceRateTriggerSignalErd = Erd_FreezerIceRateTriggerSignal,
   .aluminumMoldIceMakerTestRequestErd = Erd_AluminumMoldIceMakerTestRequest,
   .dispensingRequestStatusErd = Erd_DispensingRequestStatus
};

TEST_GROUP(AluminumMoldIceMaker)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;
   EventSubscription_t iceWaterValveOnChangeSubscription;
   EventSubscription_t dataModelOnChangeSubscription;
   EventSubscription_t rakeControllerRequestOnChangeSubscription;

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

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&referDataModelTestDouble);

      iceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->aluminumMoldIceMakerData;

      EventSubscription_Init(&dataModelOnChangeSubscription, &instance, DataModelChanged);
      EventSubscription_Init(&rakeControllerRequestOnChangeSubscription, &instance, RakeControllerRequestChanged);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenTheModuleIsInitialized()
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

   void GivenHarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
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
         Erd_HarvestCountIsReadyToHarvest,
         clear);
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
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void GivenFeelerArmIsNotReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         clear);
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
         Erd_SabbathMode,
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
         Erd_IceMaker0EnableStatus,
         &state);
   }

   void WhenTheIceMakerIs(bool state)
   {
      GivenTheIceMakerIs(state);
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

   void GivenAluminumMoldIceMakerIsInIdleFreezeState()
   {
      GivenTheIceMakerIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   }

   void GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenSabbathModeIs(ENABLED);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   }

   void GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState()
   {
      GivenTheIceMakerIs(ENABLED);
      GivenTheMoldThermistorIsValid();
      GivenTheRakePositionIs(RakePosition_Home);
      GivenTheModuleIsInitialized();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

      WhenTheIceMakerIs(DISABLED);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
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
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote, &vote);
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
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void GivenTheSkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void WhenTheRakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }

   void GivenRakeDidNotCompleteFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, clear);
   }

   void WhenRakeDidNotCompleteFullRevolution()
   {
      GivenRakeDidNotCompleteFullRevolution();
   }

   void GivenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      GivenTheRakePositionIs(position);
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

      WhenTheRakeCompletesFullRevolution();
      After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

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

   void GivenRakeCompletesItsSecondFullRevolutionInHarvestFixState()
   {
      GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();

      WhenTheRakeCompletesFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

      WhenRakeDidNotCompleteFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

      WhenTheRakeCompletesFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   }

   void WhenReadyToEnterHarvest()
   {
      GivenHarvestCountIsNotReadyToHarvest();
      GivenFeelerArmIsNotReadyToEnterHarvest();

      WhenHarvestCountIsReadyToHarvest();
      WhenFeelerArmIsReadyToEnterHarvest();
   }

   void WhenRakeCompletesRevolutionAndFillTubeHeaterTimerExpires()
   {
      After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);

      WhenTheRakeCompletesFullRevolution();
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      After(1);
   }

   void GivenTheExternalTestRequestIs(IceMakerTestRequest_t request)
   {
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMakerTestRequest,
         &request);
   }

   void WhenExternalTestRequestIs(IceMakerTestRequest_t request)
   {
      GivenTheExternalTestRequestIs(request);
   }

   void ExternalTestRequestShouldBe(IceMakerTestRequest_t expected)
   {
      IceMakerTestRequest_t actual;
      DataModel_Read(dataModel, Erd_AluminumMoldIceMakerTestRequest, &actual);
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

   void UnsubscribeFromFillTubeHeaterOnChangeSubscription()
   {
      DataModel_UnsubscribeAll(dataModel, &dataModelOnChangeSubscription);
   }

   void ExpectNoChangeInRakeControllerRequest()
   {
      DataModel_Subscribe(dataModel, Erd_AluminumMoldIceMakerRakeControlRequest, &rakeControllerRequestOnChangeSubscription);
      mock().expectNoCall("No Call");
   }

   void UnsubscribeFromRakeControllerRequestOnChangeSubscription()
   {
      DataModel_Unsubscribe(dataModel, Erd_AluminumMoldIceMakerRakeControlRequest, &rakeControllerRequestOnChangeSubscription);
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
      WhenTheRakeCompletesFullRevolution();
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

TEST(AluminumMoldIceMaker, ShouldGoToIdleFreezeStateOnInitIfSabbathModeIsEnabledAndIceMakerIsEnabled)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToIdleFreezeStateOnInitIfSabbathModeIsEnabledAndIceMakerIsDisabled)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoIdleToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabled)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoIdleToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndRakeIsHome)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoIdleToFreezeStateOnInitIfSabbathModeIsDisabledAndIceMakerIsDisabledAndMoldThermistorIsInvalid)
{
   GivenTheIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorIsInvalid();
   GivenTheRakePositionIs(RakePosition_NotHome);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
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

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToIdleFreezeWhenSabbathIsEnabled)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenSabbathIsEnabled)
{
   GivenSabbathModeIsDisabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToFreezeWhenSabbathIsDisabled)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToHarvestWhenSabbathIsDisabledWhileRakePositionIsNotHome)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToHarvestWhenIceMakerIsEnabledWhileRakePositionIsNotHome)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromFreezeToIdleFreezeWhenIceMakerIsDisabled)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToIdleFreezeWhenIceMakerIsDisabled)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToFreezeWhenIceMakerIsEnabled)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState();

   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffWaterValveOnEntryToIdleFreezeState)
{
   GivenIceMakerWaterValveVoteIs(WaterValveState_On);
   GivenAluminumMoldIceMakerIsInIdleFreezeState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerHeaterOnEntryToIdleFreezeState)
{
   GivenIceMakerHeaterVoteIsSet();
   GivenAluminumMoldIceMakerIsInIdleFreezeState();
   IceMakerHeaterVoteShouldBe(HeaterState_Off);
}

TEST(AluminumMoldIceMaker, ShouldTurnOffIceMakerMotorOnEntryToIdleFreezeState)
{
   GivenIceMakerMotorVoteIsSet();
   GivenAluminumMoldIceMakerIsInIdleFreezeState();
   IceMakerMotorVoteShouldBe(MotorState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnFillTubeHeaterWithFreezeThawFillTubeHeaterDutyCycleOnEntryToHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);
}

TEST(AluminumMoldIceMaker, ShouldVoteFillTubeHeaterDontCareAfterFillTubeHeaterOnTime)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
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

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToIdleFreezeWhenSabbathModeEnabled)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestFaultToIdleFreezeWhenIceMakeIsDisbled)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
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

   WhenTheRakeCompletesFullRevolution();
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

TEST(AluminumMoldIceMaker, ShouldNotUpdateRakeControllerRequestAfterMotorControlTimeWhenSabbathModeIsEnabledInHarvestFault)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldNotUpdateRakeControllerRequestAfterMotorControlTimeWhenIceMakerIsDisabledInHarvestFault)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);

   After(iceMakerData->harvestFaultData.rakeMotorControlTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(CLEAR);
}

TEST(AluminumMoldIceMaker, ShouldVoteFillTubeHeaterDontCareWhenTransitioningFromHarvestToHarvestFix)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC + 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
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

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldTransitionFromHarvestToFillStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsClearAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotClearRakeControllerRequestUntilAfterWeMoveOnFromHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);
   RakeControllerRequestShouldBe(SET);

   WhenTheRakeCompletesFullRevolution();
   RakeControllerRequestShouldBe(SET);

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
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

   WhenTheRakeCompletesFullRevolution();
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

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenRakeDidNotCompleteFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolution();
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

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off); // it's voted off in freeze

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOnIsolationWaterValveWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   IsolationIceMakerWaterValveVoteShouldBeDontCare();

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);
}

TEST(AluminumMoldIceMaker, ShouldStartWaterFillMonitoringWhenEnteringFillState)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   After(1);
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

   WhenRakeCompletesRevolutionAndFillTubeHeaterTimerExpires();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIceMakerWaterValveWhenExitingFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   IceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldVoteToTurnOffIsolationWaterValveWhenExitingFillState)
{
   GivenAluminumMoldIceMakerIsInFillState();
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_On);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   IsolationIceMakerWaterValveVoteShouldBe(WaterValveState_Off);
}

TEST(AluminumMoldIceMaker, ShouldIncrementFreezerTriggerIceRateSignalWhenTransitioningToTheFreezeState)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(1);

   WhenTheIceMakerIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   FreezerTriggerIceRateSignalShouldBe(1);

   WhenTheIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(2);
}

TEST(AluminumMoldIceMaker, ShouldNotIncrementFreezerTriggerIceRateSignalWhenTransitioningToTheFreezeStateOnInitialization)
{
   GivenTheIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenTheMoldThermistorIsValid();
   GivenTheRakePositionIs(RakePosition_Home);
   GivenTheModuleIsInitialized();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
   FreezerTriggerIceRateSignalShouldBe(0);

   WhenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
   FreezerTriggerIceRateSignalShouldBe(1);
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

TEST(AluminumMoldIceMaker, ShouldTransitionFromIdleFreezeToFillStateWhenFillTestIsRequested)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();
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

TEST(AluminumMoldIceMaker, ShouldTransitionFromThermistorFaultToFillStateWhenFillTestIsRequested)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault();
   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotLeaveIdleFreezeStateWhenIceMakerIsInDisabledState)
{
   GivenAluminumMoldIceMakerIsInIdleFreezeState();
   GivenTheIceMakerIs(DISABLED);

   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldGoFromFillToIdleStateIfSabbathModeIsEnabledAndIceMakerIsDisabled)
{
   GivenAluminumMoldIceMakerIsInIdleFreezeState();
   GivenSabbathModeIs(ENABLED);

   WhenExternalTestRequestIs(IceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);

   WhenStopFillSignalChanges();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
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

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenRakeDidNotCompleteFullRevolution();
   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   WhenIceMakerTemperatureIsNotReadyToHarvest();
   WhenIceMakerTemperatureIsReadyToHarvest();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenRakeDidNotCompleteFullRevolution();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenRakeDidNotCompleteFullRevolution();
   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker, ShouldGoToFreezeAfterSecondRevolutionWhenReEnteringIntoHarvestFixAfterFirstRevolutionAndMoldThermistorIsInvalidInTheState)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenMoldThermistorIsInvalid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);

   WhenTheRakePositionIs(RakePosition_NotHome);
   WhenMoldThermistorIsValid();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenRakeDidNotCompleteFullRevolution();
   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_HarvestFix);

   WhenRakeDidNotCompleteFullRevolution();
   WhenTheRakeCompletesFullRevolution();
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

   UnsubscribeFromFillTubeHeaterOnChangeSubscription();
}

TEST(AluminumMoldIceMaker, ShouldRemainInThermistorFaultWhenExternalTestRequestBecomesHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInThermistorFault();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMaker, ShouldRemainInHarvestFixWhenTestRequestBecomesHarvest)
{
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvestFix();
   ExpectNoChangeInRakeControllerRequest();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);

   UnsubscribeFromRakeControllerRequestOnChangeSubscription();
}

TEST(AluminumMoldIceMaker, ShouldEnterHarvestFromIdleFreezeWhenExternalTestRequestBecomesHarvest)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldEnterHarvestFromFreezeWhenExternalTestRequestBecomesHarvest)
{
   GivenTheAluminumMoldIceMakerIsInFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldEnterHarvestFromFillWhenExternalTestRequestBecomesHarvest)
{
   GivenAluminumMoldIceMakerIsInFillState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldEnterHarvestFromHarvestFaultWhenExternalTestRequestBecomesHarvest)
{
   GivenTheAluminumMoldIceMakerIsInHarvestFault();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMaker, ShouldReturnToIdleFreezeStateAfterMaxHarvestTimeIfSabbathModeisEnabledAndHarvestIsEnteredViaTestRequest)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldReturnToIdleFreezeStateAfterMaxHarvestTimeIfIceMakerDisabledAndHarvestIsEnteredViaTestRequest)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldReturnToIdleFreezeStateAfterFillTuberOnTimeExpiredIfSabbathModeisEnabledAndHarvestIsEnteredViaTestRequest)
{
   GivenSabbathIsEnabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMaker, ShouldReturnToIdleFreezeStateAfterFillTubeHeaterOnTimeIfIceMakerDisabledAndHarvestIsEnteredViaTestRequest)
{
   GivenTheIceMakerIsDisabledAndAluminumMoldIceMakerIsInIdleFreezeState();
   WhenExternalTestRequestIs(IceMakerTestRequest_Harvest);

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
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

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_Dispensing);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Stop);

   WhenDispensingRequestStatusIs(DispensingRequestSelection_Water, DispenseStatus_CompletedSuccessfully);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker, ShouldNotDelayFillMonitoringIfWaterDispensingFinishesBeforeFillStateIsEntered)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 2);
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

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   WhenDispensingRequestStatusIs(DispensingRequestSelection_CubedIce, DispenseStatus_Dispensing);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   WaterFillMonitoringRequestShouldBe(IceMakerWaterFillMonitoringRequest_Start);
}

TEST(AluminumMoldIceMaker, ShouldNotDelayMonitoringWhenEnteringFillStateDuringCrushedIceDispensing)
{
   GivenAluminumMoldIceMakerIsInHarvestStateAndHasCompletedAFullRevolution();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
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

   void GivenTheModuleIsInitialized()
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

   void GivenHarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void GivenHarvestCountIsNotReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         clear);
   }

   void GivenIceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }
   void WhenIceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void GivenFeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void GivenTheIceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0EnableStatus,
         &state);
   }

   void GivenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      GivenTheRakePositionIs(position);
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

   void FillTubeHeaterVoteAndCareShouldBe(PercentageDutyCycle_t dutyCycle, Vote_t expectedCare)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(
         dataModel,
         Erd_FillTubeHeater_AluminumMoldIceMakerVote,
         &vote);

      CHECK_EQUAL(dutyCycle, vote.percentageDutyCycle);
      CHECK_EQUAL(expectedCare, vote.care);
   }

   void GivenTheMoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void WhenMoldThermistorIsValid()
   {
      GivenTheMoldThermistorIsValid();
   }

   void WhenMoldThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, clear);
   }

   void GivenSkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void GivenTheSkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void WhenTheRakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }

   void GivenRakeDidNotCompleteFullRevolution()
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
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(1);
   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFreezeStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsSet)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFillStateWhenFillTubeHeaterTimerExpiredAndRakeHasCompletedFullRevolutionAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSet)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsSet)
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

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldTransitionFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeMaxHarvestTimeHasExpiredAndSkipFillRequestIsClear)
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

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldSetRakeControllerRequestWhenTransitioningFromHarvestToHarvestFixStateWhenRakeHasNotCompletedFullRevolutionBeforeFillTubeHeaterTimerHasExpiredAndSkipFillRequestIsSetAndFillTubeHeaterOnTimeIsEqualToMaxHarvestTime)
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

TEST(AluminumMoldIceMaker_FillTubeHeaterOnTimeLessThanMaxHarvestTime, ShouldNotTransitionAfterFillTubeHeaterTimerExpiresWhenExitConditionsToGoToFillAreMetIfAlreadyExitedHarvest)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   WhenMoldThermistorIsInvalid();
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

   void GivenTheModuleIsInitialized()
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

   void GivenHarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void GivenIceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void WhenIceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void GivenFeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void GivenTheIceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0EnableStatus,
         &state);
   }

   void GivenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      GivenTheRakePositionIs(position);
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

   void GivenTheMoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void GivenSkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void GivenTheSkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void WhenTheRakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }
};

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroDutyCycle, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleIsZeroAndSkipFillRequestIsSet)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroDutyCycle, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterDutyCycleIsZeroAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheRakeCompletesFullRevolution();
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

   void GivenTheModuleIsInitialized()
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

   void GivenHarvestCountIsReadyToHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_HarvestCountIsReadyToHarvest,
         set);
   }

   void GivenIceMakerTemperatureIsNotReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void WhenIceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureResolvedInDegFx100,
         &temperature);
   }

   void GivenFeelerArmIsReadyToEnterHarvest()
   {
      DataModel_Write(
         dataModel,
         Erd_FeelerArmIsReadyToEnterHarvest,
         set);
   }

   void GivenTheIceMakerIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_IceMaker0EnableStatus,
         &state);
   }

   void GivenTheRakePositionIs(RakePosition_t position)
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakePosition, &position);
   }

   void WhenTheRakePositionIs(RakePosition_t position)
   {
      GivenTheRakePositionIs(position);
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

   void GivenTheMoldThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerMoldThermistor_IsValidResolved, set);
   }

   void GivenSkipFillRequestIsSet()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, set);
   }

   void GivenTheSkipFillRequestIsClear()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerSkipFillRequest, clear);
   }

   void WhenTheRakeCompletesFullRevolution()
   {
      DataModel_Write(dataModel, Erd_AluminumMoldIceMakerRakeCompletedRevolution, set);
   }
};

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroOnTime, ShouldTransitionFromHarvestToFreezeStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterHeaterTimeIsZeroAndSkipFillRequestIsSet)
{
   GivenTheMoldThermistorIsValid();
   GivenSkipFillRequestIsSet();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheRakeCompletesFullRevolution();

   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMaker_FillTubeHeaterZeroOnTime, ShouldTransitionFromHarvestToFillStateWhenRakeCompletesFullRevolutionAndFillTubeHeaterHeaterTimeIsZeroAndSkipFillRequestIsClear)
{
   GivenTheMoldThermistorIsValid();
   GivenTheSkipFillRequestIsClear();
   GivenTheIceMakerIsEnabledAndAluminumMoldIceMakerIsInHarvest();

   WhenTheRakeCompletesFullRevolution();
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
}
