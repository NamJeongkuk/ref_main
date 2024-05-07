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
#include "Constants_Binary.h"
#include "Vote.h"
#include "Rx2xxResetSource.h"
#include "PersonalityParametricData.h"
#include "TddPersonality.h"
#include "EventQueueInterruptSafePlugin.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "uassert_test.h"
#include "Interrupt_TestDouble.h"
#include "PersonalityTestSetup.h"

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
   Interrupt_TestDouble_t *interruptTestDouble;
   const DamperData_t *damperData;

   void setup(PersonalityId_t personality)
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, personality);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);

      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_FastTickInterrupt);

      damperData = PersonalityParametricData_Get(dataModel)->damperData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void AfterTheEventQueueIsRun()
   {
      EventQueue_InterruptSafe_Run(EventQueueInterruptSafePlugin_GetInterruptSafeEventQueue());
   }

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
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
      TemperatureDegFx100_t temperature = damperData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1;
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
      StepRequestShouldBeDamperPositionOpen();
      StepperMotorControlRequestShouldBe(SET);
      StepperMotorDriveEnableShouldBe(SET);
      DamperCurrentPositionShouldBe(DamperPosition_Closed);

      AfterNInterrupts(damperData->damperStepData->stepsToOpen * (damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1));
      DamperCurrentPositionShouldBe(DamperPosition_Closed);

      AfterNInterrupts(1);
      AfterTheEventQueueIsRun();
      DamperCurrentPositionShouldBe(DamperPosition_Open);
   }

   void DamperCurrentPositionShouldBe(DamperPosition_t expectedPosition)
   {
      DamperPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_DamperCurrentPosition, &actualPosition);

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

      DataModel_Write(dataModel, Erd_DamperHeater_FactoryVote, &vote);
   }

   void DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_FALSE(vote.care);
   }

   void DamperFreezePreventionShouldVoteDamperHeaterOn()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Max, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void DamperFreezePreventionShouldVoteDamperHeaterOff()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Min, vote.percentageDutyCycle);
      CHECK_FALSE(vote.care);
   }

   void DamperHeaterDefrostSyncShouldVoteDamperHeaterOn()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DefrostHeaterSyncVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Max, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DefrostHeaterSyncVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Min, vote.percentageDutyCycle);
      CHECK_FALSE(vote.care);
   }

   void DamperHeaterWinningErdShouldBe(Erd_t expectedErd)
   {
      Erd_t actualErd;
      DataModel_Read(dataModel, Erd_DamperHeater_WinningVoteErd, &actualErd);

      CHECK_EQUAL(expectedErd, actualErd);
   }

   void ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_t expectedDutyCycle)
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_ResolvedVote, &vote);

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

   void FullCycleRequestShouldBeClear()
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperFullCycleRequest, &actualRequest);
      CHECK_FALSE(actualRequest);
   }

   void FullCycleRequestShouldBeSet()
   {
      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperFullCycleRequest, &actualRequest);
      CHECK_TRUE(actualRequest);
   }

   void FullCycleRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DamperFullCycleRequest, &state);
   }

   void StepRequestShouldBeDamperPositionClosed()
   {
      StepperPositionRequest_t stepRequest;
      DataModel_Read(dataModel, Erd_DamperStepperMotorPositionRequest, &stepRequest);

      CHECK_EQUAL(damperData->damperStepData->stepsToClose, stepRequest.stepsToMove);
      CHECK_EQUAL(TurningDirection_Clockwise, stepRequest.direction);
   }

   void StepRequestShouldBeDamperPositionOpen()
   {
      StepperPositionRequest_t stepRequest;
      DataModel_Read(dataModel, Erd_DamperStepperMotorPositionRequest, &stepRequest);

      CHECK_EQUAL(damperData->damperStepData->stepsToOpen, stepRequest.stepsToMove);
      CHECK_EQUAL(TurningDirection_CounterClockwise, stepRequest.direction);
   }

   void StepRequestShouldBeDamperPositionHome()
   {
      StepperPositionRequest_t stepRequest;
      DataModel_Read(dataModel, Erd_DamperStepperMotorPositionRequest, &stepRequest);

      CHECK_EQUAL(damperData->stepsToHome, stepRequest.stepsToMove);
      CHECK_EQUAL(TurningDirection_Clockwise, stepRequest.direction);
   }

   void StepRequestShouldBeDamperPositionFullCycleOpen()
   {
      StepperPositionRequest_t stepRequest;
      DataModel_Read(dataModel, Erd_DamperStepperMotorPositionRequest, &stepRequest);

      CHECK_EQUAL(damperData->stepsToHome, stepRequest.stepsToMove);
      CHECK_EQUAL(TurningDirection_CounterClockwise, stepRequest.direction);
   }

   void EnsureTwistIceMakerIsNotRequestingControl(void)
   {
      DataModel_Write(dataModel, Erd_TwistIceMakerMotorControlRequest, clear);
   }

   void StepperMotorControlRequestShouldBe(bool expectedRequest)
   {
      EnsureTwistIceMakerIsNotRequestingControl();

      bool actualRequest;
      DataModel_Read(dataModel, Erd_DamperStepperMotorControlRequest, &actualRequest);

      CHECK_EQUAL(expectedRequest, actualRequest);
   }

   void StepperMotorDriveEnableShouldBe(bool expectedState)
   {
      bool actualState;
      DataModel_Read(dataModel, Erd_DamperStepperMotorDriveEnable, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void StepsAreSetToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &position);

      position.stepsToMove = 0;
      DataModel_Write(
         dataModelDouble.dataModel,
         Erd_DamperStepperMotorPositionRequest,
         &position);
   }

   void StepsShouldBeSetToZero()
   {
      StepperPositionRequest_t position;
      DataModel_Read(dataModelDouble.dataModel, Erd_DamperStepperMotorPositionRequest, &position);

      CHECK_EQUAL(0, position.stepsToMove);
   }

   void HomingHasCompleted()
   {
      StepRequestShouldBeDamperPositionHome();
      StepperMotorControlRequestShouldBe(SET);
      StepperMotorDriveEnableShouldBe(SET);
      AfterNInterrupts(damperData->stepsToHome * (damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1) + 1);
      AfterTheEventQueueIsRun();
      StepsShouldBeSetToZero();
      DamperCurrentPositionShouldBe(DamperPosition_Closed);
   }

   DamperPosition_t DamperCurrentPosition()
   {
      DamperPosition_t currentPosition;
      DataModel_Read(
         dataModelDouble.dataModel,
         Erd_DamperCurrentPosition,
         &currentPosition);

      return currentPosition;
   }

   void FullCycleHasCompleted()
   {
      if(DamperCurrentPosition() == DamperPosition_Open)
      {
         StepRequestShouldBeDamperPositionClosed();
         StepperMotorControlRequestShouldBe(SET);
         StepperMotorDriveEnableShouldBe(SET);
         DamperCurrentPositionShouldBe(DamperPosition_Open);

         AfterNInterrupts(damperData->damperStepData->stepsToClose * (damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1) + 1);
         AfterTheEventQueueIsRun();
      }

      StepRequestShouldBeDamperPositionFullCycleOpen();
      StepperMotorControlRequestShouldBe(SET);
      StepperMotorDriveEnableShouldBe(SET);
      DamperCurrentPositionShouldBe(DamperPosition_Closed);
      AfterNInterrupts(damperData->stepsToHome * (damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1) + 1);
      AfterTheEventQueueIsRun();
      DamperCurrentPositionShouldBe(DamperPosition_Open);

      StepRequestShouldBeDamperPositionHome();
      StepperMotorControlRequestShouldBe(SET);
      StepperMotorDriveEnableShouldBe(SET);
      AfterNInterrupts(damperData->stepsToHome * (damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1) + 1);
      AfterTheEventQueueIsRun();
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

#define PERSONALITIES(ENTRY) \
   ENTRY(TddPersonality_DevelopmentSingleEvaporator)

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
   And TargetTemperatureIs(InitialTemperatureInDegFx100 - (damperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After((damperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   After((damperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Min);

   FullCycleRequestShouldBeSet();
   When FullCycleHasCompleted();
   FullCycleRequestShouldBeClear();
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
   StepRequestShouldBeDamperPositionClosed();
   StepperMotorControlRequestShouldBe(SET);
   StepperMotorDriveEnableShouldBe(SET);
   DamperCurrentPositionShouldBe(DamperPosition_Open);
   DamperPositionResolvedVoteShouldBe(DamperPosition_Closed);

   AfterNInterrupts(damperData->damperStepData->stepsToClose * (damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1) + 1);
   AfterTheEventQueueIsRun();
   StepsShouldBeSetToZero();
   StepperMotorControlRequestShouldBe(CLEAR);
   DamperCurrentPositionShouldBe(DamperPosition_Closed);

   TargetTemperatureIs((damperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - InitialTemperatureInDegFx100) - 1);
   After((damperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   After(damperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   DamperCurrentPositionShouldBe(DamperPosition_Closed);

   FullCycleRequestShouldBeClear();
   StepsShouldBeSetToZero();
   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);

   FullCycleRequestShouldBeSet();
   When FullCycleHasCompleted();
   FullCycleRequestShouldBeClear();
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
   FullCycleRequestShouldBeClear();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);

   // This lowers the temperature periodically so as to prevent the damper freeze prevention from closing damper.
   // Also this assumes that the max open time is a multiple of the the minimum temperature change time.
   for(int i = 1; i <= (damperData->maxTimeForDamperToBeOpenInMinutes / damperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes); i++)
   {
      TargetTemperatureIs(InitialTemperatureInDegFx100 - (damperData->targetCompartmentMinimumTemperatureChangeInDegFx100 * i));
      After((damperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN));
      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   }

   FullCycleRequestShouldBeSet();
   When FullCycleHasCompleted();
   FullCycleRequestShouldBeClear();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperCurrentPositionShouldBe(DamperPosition_Closed);

   When DamperOpens();
   FullCycleRequestShouldBeClear();
   StepsShouldBeSetToZero();
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
   After((damperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionVoteForDamperHeaterShouldBeDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   After(damperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);

   DamperCurrentPositionShouldBe(DamperPosition_Open);
   FullCycleRequestShouldBeSet();
   When FullCycleHasCompleted();
   FullCycleRequestShouldBeClear();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperCurrentPositionShouldBe(DamperPosition_Closed);

   When DamperOpens();
   StepsShouldBeSetToZero();
}

TEST(DamperIntegration, ShouldBeAbleToFullCycleDamper)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();
   When DamperOpens();

   When FullCycleRequestIs(SET);
   FullCycleRequestShouldBeSet();
   DamperCurrentPositionShouldBe(DamperPosition_Open);

   When FullCycleHasCompleted();
   FullCycleRequestShouldBeClear();
   DamperCurrentPositionShouldBe(DamperPosition_Closed);

   AfterNInterrupts(damperData->damperStepData->stepsToOpen * (damperData->delayBetweenStepEventsInHundredsOfMicroseconds + 1) + 1);
   AfterTheEventQueueIsRun();
   DamperCurrentPositionShouldBe(DamperPosition_Open);
   StepsShouldBeSetToZero();
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
   After((damperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);

   When FreezerDefrostHeaterVoteErdIsOffAndCare();
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Max);

   After((damperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Min);

   When FreezerDefrostHeaterStateIs(ON);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOn();
   DamperHeaterWinningErdShouldBe(Erd_DamperHeater_DefrostHeaterSyncVote);

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
   After((damperData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);
   DamperHeaterWinningErdShouldBe(Erd_DamperHeater_FactoryVote);

   When FreezerDefrostHeaterVoteErdIsOffAndCare();
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOffAndDontCare();
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Max);

   After((damperData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN) - 1);
   DamperFreezePreventionShouldVoteDamperHeaterOn();

   After(1);
   DamperFreezePreventionShouldVoteDamperHeaterOff();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   ResolvedDamperHeaterVoteShouldBe(PercentageDutyCycle_Max);
   DamperHeaterWinningErdShouldBe(Erd_DamperHeater_FactoryVote);

   When FreezerDefrostHeaterStateIs(ON);
   DamperHeaterDefrostSyncShouldVoteDamperHeaterOn();
   DamperHeaterWinningErdShouldBe(Erd_DamperHeater_FactoryVote);
}

#undef PERSONALITIES
#define PERSONALITIES(ENTRY) \
   ENTRY(TddPersonality_DevelopmentMaxOpenDamperTimerZero)

TEST(DamperIntegration, ShouldNotVoteToCloseDamperHeaterWhenMaxTimeToReachIsSetToZeroInParametric)
{
   GivenThatTheApplicationHasStartedAndDamperIsInMonitoringTemperatureChangesState();
   When HomingHasCompleted();
   And GridDamperPositionVoteIsOpenAndCare();

   DamperPositionResolvedVoteShouldBe(DamperPosition_Open);
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_GridVote);

   When StepsAreSetToZero();
   DamperCurrentPositionShouldBe(DamperPosition_Open);

   When TargetTemperatureIs(damperData->targetCompartmentMinimumTemperatureChangeInDegFx100 - InitialTemperatureInDegFx100);
   After(damperData->maxTimeForDamperToBeOpenInMinutes * MSEC_PER_MIN);
   FullCycleRequestShouldBeClear();

   After(1);
   FullCycleRequestShouldBeClear();
   DamperPositionWinningVoteErdShouldBe(Erd_FreshFoodDamperPosition_GridVote);
   DamperCurrentPositionShouldBe(DamperPosition_Open);
}
