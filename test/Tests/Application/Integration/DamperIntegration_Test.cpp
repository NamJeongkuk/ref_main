/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "StackConfigurator.h"
#include "Constants_Time.h"
#include "Vote.h"
#include "Rx2xxResetSource.h"
#include "PersonalityParametricData.h"
#include "TddPersonality.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define Then
#define And

enum
{
   Invalid = false,
   Valid = true,
   InitialTemperatureInDegFx100 = 3200,
   ShortDelayInMs = 5,
};

TEST_GROUP(DamperIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const SingleDamperData_t *freshFoodDamperData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      freshFoodDamperData = PersonalityParametricData_Get(dataModel)->freshFoodDamperData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void TargetThermistorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidOverrideValue, set);
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidOverrideRequest, &state);
   }

   void SourceThermistorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidOverrideValue, set);
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidOverrideRequest, &state);
   }

   void SourceTemperatureIsLessThanSourceCompartmentMaximumTemperatureToRunCheck()
   {
      TemperatureDegFx100_t temperature = freshFoodDamperData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1;
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100, &temperature);
   }

   void DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_t expectedState)
   {
      DamperFreezePreventionFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DamperFreezePreventionFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void TargetTemperatureIs(TemperatureDegFx100_t targetTemperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &targetTemperature);
   }

   void DamperOpens()
   {
      When FreezerDefrostDamperPositionVoteIsOpenAndCare();
      DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
      StepRequestShouldBeDoorPositionOpen();
      StepperMotorControlRequestShouldBe(SET);
      StepperMotorDriveEnableShouldBe(SET);
      DamperCurrentPositionShouldBe(DamperPosition_Closed);

      After(freshFoodDamperData->stepsToOpen * (freshFoodDamperData->delayBetweenStepEventsInMs + 1));
      DamperCurrentPositionShouldBe(DamperPosition_Closed);

      After(1);
      DamperCurrentPositionShouldBe(DamperPosition_Open);
   }

   void DamperCurrentPositionShouldBe(DamperPosition_t expectedPosition)
   {
      DamperPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_FreshFoodDamperCurrentPosition, &actualPosition);

      CHECK_EQUAL(expectedPosition, actualPosition);
   }

   void FactoryDamperPositionVoteIsOpenAndCare()
   {
      DamperVotedPosition_t vote;
      vote.position = DamperPosition_Open;
      vote.care = Vote_Care;

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_FactoryVote, &vote);
   }

   void FactoryDamperPositionVoteIsOpenAndDontCare()
   {
      DamperVotedPosition_t vote;
      vote.position = DamperPosition_Open;
      vote.care = Vote_DontCare;

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_FactoryVote, &vote);
   }

   void GridDamperPositionVoteIsOpenAndCare()
   {
      DamperVotedPosition_t vote;
      vote.position = DamperPosition_Open;
      vote.care = Vote_Care;

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_GridVote, &vote);
   }

   void FreezerDefrostDamperPositionVoteIsClosedAndCare()
   {
      DamperVotedPosition_t vote;
      vote.position = DamperPosition_Closed;
      vote.care = Vote_Care;

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &vote);
   }

   void FreezerDefrostDamperPositionVoteIsOpenAndCare()
   {
      DamperVotedPosition_t vote;
      vote.position = DamperPosition_Open;
      vote.care = Vote_Care;

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &vote);
   }

   void DefrostDamperPositionVoteShouldBeOpenAndCare()
   {
      DamperVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DefrostVote, &vote);

      CHECK_EQUAL(DamperPosition_Open, vote.position);
      CHECK_TRUE(vote.care);
   }

   void FactoryDamperHeaterVoteIsOnAndCare()
   {
      PercentageDutyCycleVote_t vote;
      vote.percentageDutyCycle = PercentageDutyCycle_Max;
      vote.care = Vote_Care;

      DataModel_Write(dataModel, Erd_FreshFoodDamperHeater_FactoryVote, &vote);
   }

   void DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_FALSE(vote.care);
   }

   void DamperFreezePreventionShouldVoteDamperHeaterOn()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Max, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void DamperFreezePreventionShouldVoteDamperHeaterOff()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Min, vote.percentageDutyCycle);
      CHECK_FALSE(vote.care);
   }

   void DamperHeaterDefrostSyncShouldVoteDamperHeaterOn()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Max, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Min, vote.percentageDutyCycle);
      CHECK_FALSE(vote.care);
   }

   void DamperHeaterWinningErdShouldBe(Erd_t expectedErd)
   {
      Erd_t actualErd;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_WinningVoteErd, &actualErd);

      CHECK_EQUAL(expectedErd, actualErd);
   }

   void ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_t expectedDutyCycle)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_ResolvedVote, &vote);

      CHECK_EQUAL(expectedDutyCycle, vote.percentageDutyCycle);
   }

   void FreezerDefrostHeaterStateIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerDefrostHeaterRelay, &state);
   }

   void FreezerDefrostHeaterVoteErdIsOffAndCare()
   {
      HeaterVotedState_t vote;
      vote.care = Vote_Care;
      vote.state = RelayState_Off;

      DataModel_Write(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &vote);
   }

   void FreezerDefrostHeaterVoteErdIsOffAndDontCare()
   {
      HeaterVotedState_t vote;
      vote.care = Vote_DontCare;
      vote.state = RelayState_Off;

      DataModel_Write(dataModel, Erd_FreezerDefrostHeater_DefrostVote, &vote);
   }

   void DamperPositionWinningVoteErdShouldBe(Erd_t expectedErd)
   {
      Erd_t actualErd;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_WinningVoteErd, &actualErd);

      CHECK_EQUAL(expectedErd, actualErd);
   }

   void DamperPositionResolvedVoteShouldBe(DamperPosition_t expectedPosition)
   {
      DamperVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_ResolvedVote, &vote);

      CHECK_EQUAL(expectedPosition, vote.position);
      CHECK_TRUE(vote.care);
   }

   void DamperPositionShouldBeDontCareForMaxOpenTimeVote()
   {
      DamperVotedPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_MaxOpenTimeVote, &actualPosition);

      CHECK_FALSE(actualPosition.care);
   }

   void DamperPositionShouldBeDamperClosedAndCareForMaxOpenTimeVote()
   {
      DamperVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_MaxOpenTimeVote, &vote);

      CHECK_EQUAL(DamperPosition_Closed, vote.position);
      CHECK_TRUE(vote.care);
   }

   void FreezePreventionDamperPositionVoteShouldBeClosedAndCare()
   {
      DamperVotedPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DamperFreezePreventionVote, &actualPosition);

      CHECK_EQUAL(DamperPosition_Closed, actualPosition.position);
      CHECK_TRUE(actualPosition.care);
   }

   void FreezePreventionDamperPositionVoteShouldBeOpenAndCare()
   {
      DamperVotedPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DamperFreezePreventionVote, &actualPosition);

      CHECK_EQUAL(DamperPosition_Open, actualPosition.position);
      CHECK_TRUE(actualPosition.care);
   }

   void StepRequestShouldBeDoorPositionClosed()
   {
      StepperPositionRequest_t stepRequest;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorPositionRequest, &stepRequest);

      CHECK_EQUAL(TurningDirection_Clockwise, stepRequest.direction);
      CHECK_EQUAL(freshFoodDamperData->stepsToClose, stepRequest.stepsToMove);
   }

   void StepRequestShouldBeDoorPositionOpen()
   {
      StepperPositionRequest_t stepRequest;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorPositionRequest, &stepRequest);

      CHECK_EQUAL(TurningDirection_CounterClockwise, stepRequest.direction);
      CHECK_EQUAL(freshFoodDamperData->stepsToOpen, stepRequest.stepsToMove);
   }

   void StepRequestShouldBeDoorPositionHome()
   {
      StepperPositionRequest_t stepRequest;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorPositionRequest, &stepRequest);

      CHECK_EQUAL(TurningDirection_Clockwise, stepRequest.direction);
      CHECK_EQUAL(freshFoodDamperData->stepsToHome, stepRequest.stepsToMove);
   }

   void StepperMotorControlRequestShouldBe(bool expectedRequest)
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorControlRequest, &actualRequest);

      CHECK_EQUAL(expectedRequest, actualRequest);
   }

   void StepperMotorDriveEnableShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorDriveEnable, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void StepsAreSetToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 0;
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);
   }

   void StepsShouldBeSetToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(dataModelDouble.dataModel, Erd_FreshFoodDamperStepperMotorPositionRequest, &position);

      CHECK_EQUAL(0, position.stepsToMove);
   }

   void HomingHasCompleted()
   {
      StepRequestShouldBeDoorPositionHome();
      StepperMotorControlRequestShouldBe(SET);
      StepperMotorDriveEnableShouldBe(SET);
      After(freshFoodDamperData->stepsToHome * (freshFoodDamperData->delayBetweenStepEventsInMs + 1) + 1);
      StepsShouldBeSetToZero();
      DamperCurrentPositionShouldBe(DamperPosition_Closed);
   }

   void GivenThatTheApplicationHasStartedAndDamperIsInIdle()
   {
      Given TargetThermistorIs(Invalid);
      Given SourceThermistorIs(Valid);
      Given SourceTemperatureIsLessThanSourceCompartmentMaximumTemperatureToRunCheck();

      GivenApplicationHasBeenInitialized();
      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState()
   {
      Given TargetThermistorIs(Valid);
      Given SourceThermistorIs(Valid);
      Given SourceTemperatureIsLessThanSourceCompartmentMaximumTemperatureToRunCheck();

      GivenApplicationHasBeenInitialized();
      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   }
};

TEST(DamperIntegration, ShouldInitialize)
{
   GivenApplicationHasBeenInitialized();
}

TEST(DamperIntegration, ShouldStartInDamperFreezePreventionStateIdle)
{
   GivenThatTheApplicationHasStartedAndDamperIsInIdle();
}

TEST(DamperIntegration, ShouldStartInDamperFreezePreventionStateMonitoringTemperatureChanges)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
}

TEST(DamperIntegration, ShouldTurnOnDamperHeaterWhenTemperatureDoesNotDecreaseByMinimumTemperatureChangeAfterDamperOpensThenVoteToTurnHeaterOffAfterMaxTimeOpenIsReached)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();

   When TargetTemperatureIs(InitialTemperatureInDegFx100);
   And DamperOpens();
   And TargetTemperatureIs(InitialTemperatureInDegFx100 - (freshFoodDamperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After((freshFoodDamperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   After((freshFoodDamperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MoveDamper);
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Min);

   After(
      (freshFoodDamperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN) -
      1 -
      (freshFoodDamperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) -
      (freshFoodDamperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN),
      1000);
   DamperPositionShouldBeDontCareForMaxOpenTimeVote();
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_DamperFreezePreventionVote);
   FreezePreventionDamperPositionVoteShouldBeClosedAndCare();

   After(1);
   DamperPositionShouldBeDamperClosedAndCareForMaxOpenTimeVote();
}

TEST(DamperIntegration, ShouldResetMinimumTemperatureChangeTimerWhenDamperPositionChanges)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();

   When DamperOpens();
   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_DefrostVote);
   DefrostDamperPositionVoteShouldBeOpenAndCare();
   And TargetTemperatureIs(InitialTemperatureInDegFx100);
   After(ShortDelayInMs);

   When FreezerDefrostDamperPositionVoteIsClosedAndCare();
   StepRequestShouldBeDoorPositionClosed();
   StepperMotorControlRequestShouldBe(SET);
   StepperMotorDriveEnableShouldBe(SET);
   DamperCurrentPositionShouldBe(DamperPosition_Open);
   DamperPositionResolvedVoteShouldBe(DamperPosition_Closed);

   After(freshFoodDamperData->stepsToClose * (freshFoodDamperData->delayBetweenStepEventsInMs + 1) + 1);
   StepsShouldBeSetToZero();
   StepperMotorControlRequestShouldBe(CLEAR);
   DamperCurrentPositionShouldBe(DamperPosition_Closed);

   TargetTemperatureIs((freshFoodDamperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - InitialTemperatureInDegFx100) - 1);
   After((freshFoodDamperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   After(freshFoodDamperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   DamperCurrentPositionShouldBe(DamperPosition_Closed);

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MoveDamper);

   FreezePreventionDamperPositionVoteShouldBeOpenAndCare();
}

TEST(DamperIntegration, ShouldContinueRequestingForDamperToCloseWhenMaxTimeOpenIsReachedButIsNotThePriority)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();

   When FactoryDamperPositionVoteIsOpenAndCare();
   And TargetTemperatureIs(InitialTemperatureInDegFx100);
   And StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Open);
   And TargetTemperatureIs(freshFoodDamperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - InitialTemperatureInDegFx100);
   After((freshFoodDamperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN) - 1);
   DamperPositionShouldBeDontCareForMaxOpenTimeVote();
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_FactoryVote);
   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);

   After(1);
   DamperPositionShouldBeDamperClosedAndCareForMaxOpenTimeVote();
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_FactoryVote);
   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);

   When FactoryDamperPositionVoteIsOpenAndDontCare();
   DamperPositionResolvedVoteShouldBe(DamperPosition_Closed);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_MaxOpenTimeVote);
}

TEST(DamperIntegration, ShouldCloseDamperWhenDefrostVotesForDamperOpenAndThenMaxOpenTimeoutIsReached)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   And HomingHasCompleted();
   And TargetTemperatureIs(InitialTemperatureInDegFx100);
   And FreezerDefrostDamperPositionVoteIsOpenAndCare();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_DefrostVote);

   When StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Open);

   When TargetTemperatureIs(freshFoodDamperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - InitialTemperatureInDegFx100);
   After((freshFoodDamperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN) - 1);
   DamperPositionShouldBeDontCareForMaxOpenTimeVote();

   After(1);
   DamperPositionShouldBeDamperClosedAndCareForMaxOpenTimeVote();
   DamperPositionResolvedVoteShouldBe(DamperPosition_Closed);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_MaxOpenTimeVote);
   StepRequestShouldBeDoorPositionClosed();

   When StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Closed);
   DamperPositionShouldBeDontCareForMaxOpenTimeVote();
}

TEST(DamperIntegration, ShouldCloseDamperPositionWhenDefrostVotesForDamperOpenAndThenDamperFreezePreventionVotesForDamperClosed)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();
   And FreezerDefrostDamperPositionVoteIsOpenAndCare();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_DefrostVote);

   When StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Open);

   When TargetTemperatureIs(InitialTemperatureInDegFx100);
   After((freshFoodDamperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   After(freshFoodDamperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MoveDamper);

   DamperCurrentPositionShouldBe(DamperPosition_Open);
   FreezePreventionDamperPositionVoteShouldBeClosedAndCare();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Closed);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_DamperFreezePreventionVote);
   StepRequestShouldBeDoorPositionClosed();

   When StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Closed);
}

TEST(DamperIntegration, ShouldGiveCorrectPriorityWhenGridAndDefrostVoteOnDamperPosition)
{
   Given FreezerDefrostHeaterStateIs(OFF);
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();
   And GridDamperPositionVoteIsOpenAndCare();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_GridVote);

   When StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Open);

   When FreezerDefrostDamperPositionVoteIsClosedAndCare();
   And StepsAreSetToZero();
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_DefrostVote);
   DamperCurrentPositionShouldBe(DamperPosition_Closed);
}

TEST(DamperIntegration, ShouldSubscribeToDefrostHeaterStateChangeWhenEnabledInParametricAndFreezerDefrostHeaterVoteIsCareThenUnsubscribeWhenFreezeDefrostHeaterIsDontCare)
{
   Given FreezerDefrostHeaterStateIs(OFF);
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();

   When DamperOpens();
   And TargetTemperatureIs(InitialTemperatureInDegFx100);
   After((freshFoodDamperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   When FreezerDefrostHeaterVoteErdIsOffAndCare();
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Max);

   After((freshFoodDamperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MoveDamper);
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Min);

   When FreezerDefrostHeaterStateIs(ON);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOn();
   DamperHeaterWinningErdShouldBe(Erd_FreshFoodDamperHeater_DefrostHeaterSyncVote);

   When FreezerDefrostHeaterVoteErdIsOffAndDontCare();
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();

   When FreezerDefrostHeaterStateIs(OFF);
   Then FreezerDefrostHeaterStateIs(ON);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();
}

TEST(DamperIntegration, FactoryVoteShouldTakePriorityForDamperHeater)
{
   Given FreezerDefrostHeaterStateIs(OFF);
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();

   When FactoryDamperHeaterVoteIsOnAndCare();
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Max);

   When DamperOpens();
   And TargetTemperatureIs(InitialTemperatureInDegFx100);
   After((freshFoodDamperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);
   DamperHeaterWinningErdShouldBe(Erd_FreshFoodDamperHeater_FactoryVote);

   When FreezerDefrostHeaterVoteErdIsOffAndCare();
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Max);

   After((freshFoodDamperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MoveDamper);
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Max);
   DamperHeaterWinningErdShouldBe(Erd_FreshFoodDamperHeater_FactoryVote);

   When FreezerDefrostHeaterStateIs(ON);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOn();
   DamperHeaterWinningErdShouldBe(Erd_FreshFoodDamperHeater_FactoryVote);
}

TEST_GROUP(DamperIntegration_MaxOpenTimeZero)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const SingleDamperData_t *freshFoodDamperData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentMaxOpenDamperTimerZero);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      freshFoodDamperData = PersonalityParametricData_Get(dataModelDouble.dataModel)->freshFoodDamperData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void TargetThermistorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidOverrideValue, set);
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidOverrideRequest, &state);
   }

   void SourceThermistorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidOverrideValue, set);
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidOverrideRequest, &state);
   }

   void SourceTemperatureIsLessThanSourceCompartmentMaximumTemperatureToRunCheck()
   {
      TemperatureDegFx100_t temperature = freshFoodDamperData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1;
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureOverrideValueInDegFx100, &temperature);
   }

   void DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_t expectedState)
   {
      DamperFreezePreventionFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DamperFreezePreventionFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState()
   {
      Given TargetThermistorIs(Valid);
      Given SourceThermistorIs(Valid);
      Given SourceTemperatureIsLessThanSourceCompartmentMaximumTemperatureToRunCheck();

      GivenApplicationHasBeenInitialized();
      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   }

   void TargetTemperatureIs(TemperatureDegFx100_t targetTemperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &targetTemperature);
   }

   void StepsAreSetToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 0;
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_FreshFoodDamperStepperMotorPositionRequest,
         &position);
   }

   void DamperCurrentPositionShouldBe(DamperPosition_t expectedPosition)
   {
      DamperPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_FreshFoodDamperCurrentPosition, &actualPosition);

      CHECK_EQUAL(expectedPosition, actualPosition);
   }

   void HomingHasCompleted()
   {
      When StepsAreSetToZero();
      DamperCurrentPositionShouldBe(DamperPosition_Closed);
   }

   void DamperPositionShouldBeDontCareForMaxOpenTimeVote()
   {
      DamperVotedPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_MaxOpenTimeVote, &actualPosition);

      CHECK_EQUAL(Vote_DontCare, actualPosition.care);
   }

   void GridDamperPositionVoteIsOpenAndCare()
   {
      DamperVotedPosition_t vote;
      vote.position = DamperPosition_Open;
      vote.care = Vote_Care;

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_GridVote, &vote);
   }

   void DamperPositionWinningVoteErdShouldBe(Erd_t expectedErd)
   {
      Erd_t actualErd;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_WinningVoteErd, &actualErd);

      CHECK_EQUAL(expectedErd, actualErd);
   }

   void DamperPositionResolvedVoteShouldBe(DamperPosition_t expectedPosition)
   {
      DamperVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_ResolvedVote, &vote);

      CHECK_EQUAL(expectedPosition, vote.position);
      CHECK_TRUE(vote.care);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, 1000);
   }
};

TEST(DamperIntegration_MaxOpenTimeZero, ShouldNotVoteToCloseDamperHeaterWhenMaxTimeToReachIsSetToZeroInParametric)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();
   And GridDamperPositionVoteIsOpenAndCare();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_GridVote);

   When StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Open);

   When TargetTemperatureIs(freshFoodDamperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - InitialTemperatureInDegFx100);
   After(freshFoodDamperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN);
   DamperPositionShouldBeDontCareForMaxOpenTimeVote();

   After(1);
   DamperPositionShouldBeDontCareForMaxOpenTimeVote();
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_GridVote);
   DamperCurrentPositionShouldBe(DamperPosition_Open);
}
