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
   .iceMakerHeaterVoteErd = Erd_AluminumMoldIceMakerHeater_IceMakerVote,
   .iceMakerMotorVoteErd = Erd_AluminumMoldIceMakerMotor_IceMakerVote,
   .harvestCountCalculationRequestErd = Erd_HarvestCountCalculationRequest,
   .feelerArmMonitoringRequestErd = Erd_FeelerArmMonitoringRequest,
   .harvestCountIsReadyToHarvestErd = Erd_HarvestCountIsReadyToHarvest,
   .iceMakerFilteredTemperatureInDegFx100Erd = Erd_AluminumMoldIceMaker_FilteredTemperatureInDegFx100,
   .feelerArmIsReadyToEnterHarvestErd = Erd_FeelerArmIsReadyToEnterHarvest,
   .iceMakerEnabledErd = Erd_IceMakerEnabledResolved,
   .sabbathModeErd = Erd_SabbathMode
};

TEST_GROUP(AluminumMoldIceMaker)
{
   AluminumMoldIceMaker_t instance;
   ReferDataModel_TestDouble_t referDataModelTestDouble;
   I_DataModel_t *dataModel;
   const AluminumMoldIceMakerData_t *iceMakerData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&referDataModelTestDouble);
      dataModel = referDataModelTestDouble.dataModel;

      iceMakerData = PersonalityParametricData_Get(dataModel)->aluminumMoldIceMakerData;
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
         Erd_AluminumMoldIceMakerHeater_IceMakerVote,
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
         Erd_AluminumMoldIceMaker_FilteredTemperatureInDegFx100,
         &temperature);
   }

   void IceMakerTemperatureIsReadyToHarvest()
   {
      TemperatureDegFx100_t temperature = iceMakerData->freezeData.maximumHarvestTemperatureInDegFx100 - 1;
      DataModel_Write(
         dataModel,
         Erd_AluminumMoldIceMaker_FilteredTemperatureInDegFx100,
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
         Erd_IceMakerEnabledResolved,
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

   void SabbathModeIsDisabledAndAluminumMoldIceMakerIsHarvest()
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

   void IceMakerIsEnabledAndAluminumMoldIceMakerIsHarvest()
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
   Given SabbathModeIsDisabledAndAluminumMoldIceMakerIsHarvest();
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
   Given IceMakerIsEnabledAndAluminumMoldIceMakerIsHarvest();
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
