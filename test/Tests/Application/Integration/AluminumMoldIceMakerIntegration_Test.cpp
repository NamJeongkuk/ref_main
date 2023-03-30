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
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

#define Given
#define And
#define When

enum
{
   Invalid = false,
   Valid = true,
   InvalidAdcCount = 5375,
   ValidAdcCount = 13967,
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

   void ApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void SabbathModeIs(bool state)
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

   void IceMakerIs(bool state)
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

   void MoldThermistorAdcCountIs(AdcCounts_t count)
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

   void TheRakePositionIs(RakePosition_t position)
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
         Erd_IceMakerMotorRelay,
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
};

TEST(AluminumMoldIceMakerIntegration, ShouldInitialize)
{
   Given ApplicationHasBeenInitialized();
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenSabbathModeIsEnabled)
{
   Given SabbathModeIs(ENABLED);
   And IceMakerIs(ENABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabled)
{
   Given IceMakerIs(DISABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInIdleFreezeStateWhenIceMakerIsDisabledAndSabbathModeIsEnabled)
{
   Given IceMakerIs(DISABLED);
   And SabbathModeIs(ENABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInThermistorFaultStateWhenMoldThermistorIsInvalidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInFreezeStateWhenRakePositionIsHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_Home);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldStartInHarvestStateWhenRakePositionIsNotHomeAndMoldThermistorIsValidAndSabbathModeIsDisabledAndIceMakerIsEnabled)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToThermistorFaultStateWhenIceMakerBecomesEnabledButThermistorIsInvalid)
{
   Given IceMakerIs(DISABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When IceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToThermistorFaultStateWhenSabbathModeBecomesDisabledButThermistorIsInvalid)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(ENABLED);
   And MoldThermistorAdcCountIs(InvalidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Invalid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When SabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_ThermistorFault);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToHarvestStateWhenIceMakerBecomesEnabledAndSabbathModeIsDisabledAndThermistorIsValidAndRakePositionIsNotHome)
{
   Given IceMakerIs(DISABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When IceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToHarvestStateWhenSabbathModeBecomesDisabledAndIceMakerIsEnabledAndThermistorIsValidAndRakePositionIsNotHome)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(ENABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When SabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenIceMakerBecomesEnabledAndSabbathModeIsDisabledAndThermistorIsValidAndRakePositionIsHome)
{
   Given IceMakerIs(DISABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_Home);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When IceMakerIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTransitionFromIdleFreezeStateToFreezeStateWhenSabbathModeBecomesDisabledAndIceMakerIsEnabledAndThermistorIsValidAndRakePositionIsHome)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(ENABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_Home);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   When SabbathModeIs(DISABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffIceMakerWaterValveAndHeaterWhenEnteringStateIdleFreeze)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_Home);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Freeze);

   When AluminumMoldIceMakerTestRequestIs(AluminumMoldIceMakerTestRequest_Fill);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Fill);
   IceMakerWaterValveShouldVote(WaterValveState_On, Vote_Care);

   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   IceMakerWaterValveShouldVote(WaterValveState_Off, Vote_DontCare);
   IceMakerHeaterShouldVote(HeaterState_Off, Vote_DontCare);

   IceMakerWaterValveRelayShouldBe(OFF);
   IceMakerHeaterRelayShouldBe(OFF);
}

TEST(AluminumMoldIceMakerIntegration, ShouldTurnOffIceMakerMotorWhenEnteringStateIdleFreeze)
{
   Given IceMakerIs(ENABLED);
   And SabbathModeIs(DISABLED);
   And MoldThermistorAdcCountIs(ValidAdcCount);
   And TheRakePositionIs(RakePosition_NotHome);

   When ApplicationHasBeenInitialized();
   MoldThermistorShouldBe(Valid);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_Harvest);

   After(iceMakerData->harvestData.initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC - 1);
   RakeControllerRequestShouldBe(CLEAR);

   After(1);
   RakeControllerRequestShouldBe(SET);
   IceMakerMotorShouldVote(MotorState_On, Vote_Care);

   When SabbathModeIs(ENABLED);
   AluminumMoldIceMakerHsmStateShouldBe(AluminumMoldIceMakerHsmState_IdleFreeze);

   IceMakerMotorShouldVote(MotorState_Off, Vote_DontCare);
   IceMakerMotorRelayShouldVote(OFF);
}
