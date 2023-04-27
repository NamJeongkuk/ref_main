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

#define Given
#define And
#define When

enum
{
   Invalid = false,
   SkipInitialOnTimeDisabled = false,
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

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(
         dataModel,
         Erd_SabbathMode,
         &state);
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

   void MoldThermistorShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(
         dataModel,
         Erd_AluminumMoldIceMakerMoldThermistorIsValid,
         &actualState);

      CHECK_EQUAL(expectedState, actualState);
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

   void AluminumMoldIceMakerTestRequestIs(AluminumMoldIceMakerTestRequest_t request)
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

   void WhenIceMakerMoldHeaterControlRequestShouldBe(
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

   void GivenIceMakerHasEnteredHarvestAndFillTubeAndMoldHeatersHaveBeenVotedOn()
   {
      GivenIceMakerIs(ENABLED);
      GivenSabbathModeIs(DISABLED);
      GivenMoldThermistorAdcCountIs(ValidHarvestAdcCount);
      WhenTheRakePositionIs(RakePosition_NotHome);

      WhenApplicationHasBeenInitialized();
      MoldThermistorShouldBe(Valid);
      AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

      WhenIceMakerMoldHeaterControlRequestShouldBe(
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
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabled)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabledAndSabbathModeIsEnabled)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInThermistorFaultStateWhenMoldThermistorIsInvalidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenRakePositionIsHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   WhenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInHarvestStateWhenRakePositionIsNotHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToThermistorFaultStateWhenIceMakerBecomesEnabledButThermistorIsInvalid)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   GivenIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToThermistorFaultStateWhenSabbathModeBecomesDisabledButThermistorIsInvalid)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(InvalidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   GivenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToHarvestStateWhenIceMakerBecomesEnabledAndSabbathModeIsDisabledAndThermistorIsValidAndRakePositionIsNotHome)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   GivenIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToHarvestStateWhenSabbathModeBecomesDisabledAndIceMakerIsEnabledAndThermistorIsValidAndRakePositionIsNotHome)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   GivenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenIceMakerBecomesEnabledAndSabbathModeIsDisabledAndThermistorIsValidAndRakePositionIsHome)
{
   GivenIceMakerIs(DISABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   WhenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   GivenIceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenSabbathModeBecomesDisabledAndIceMakerIsEnabledAndThermistorIsValidAndRakePositionIsHome)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(ENABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   WhenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   GivenSabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffIceMakerWaterValveAndHeaterWhenEnteringStateIdleFreeze)
{
   GivenIceMakerIs(ENABLED);
   GivenSabbathModeIs(DISABLED);
   GivenMoldThermistorAdcCountIs(ValidAdcCount);
   WhenTheRakePositionIs(RakePosition_Home);

   WhenApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   AluminumMoldIceMakerTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IceMakerWaterValveShouldVote(WaterValveState_On, Vote_Care);

   GivenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   IceMakerWaterValveShouldVote(WaterValveState_Off, Vote_DontCare);
   IceMakerHeaterShouldVote(HeaterState_Off, Vote_DontCare);

   IceMakerWaterValveRelayShouldBe(OFF);
   IceMakerHeaterRelayShouldBe(OFF);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffIceMakerMotorWhenEnteringStateIdleFreeze)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(CLEAR);

   After(1);
   RakeControllerRequestShouldBe(SET);
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);

   GivenSabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   IceMakerMotorShouldVote(MotorState_Off, Vote_DontCare);
   IceMakerMotorRelayShouldVote(OFF);
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

   WhenIceMakerMoldHeaterControlRequestShouldBe(
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

   RakeControllerRequestShouldBe(SET);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   WhenTheRakePositionIs(RakePosition_NotHome);

   After(iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);

   After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(SET);

   After(iceMakerData->harvestData.maximumHarvestTimeInMinutes * MSEC_PER_MIN - iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - iceMakerData->harvestData.rakeNotHomeTestTimeInSeconds * MSEC_PER_SEC - iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionToFreezeStateWhenFillTubeTimerHasExpiredAndRakeCompletedRevolutionFlagHasBeenSet)
{
   GivenIceMakerIsInHarvestStateAndRakeIsNotHome();

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC);

   RakeControllerRequestShouldBe(SET);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketNotFull);
   WhenTheRakePositionIs(RakePosition_NotHome);

   WhenFeelerArmPositionIs(FeelerArmPosition_BucketFull);

   After(iceMakerData->harvestData.feelerArmTestTimeInSeconds * MSEC_PER_SEC);

   WhenTheRakePositionIs(RakePosition_Home);
   RakeCompletedRevolutionShouldBe(SET);

   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_Care);

   After(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);

   FillTubeHeaterVoteAndCareShouldBe(iceMakerData->fillTubeHeaterData.freezeThawFillTubeHeaterDutyCyclePercentage, Vote_DontCare);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}
