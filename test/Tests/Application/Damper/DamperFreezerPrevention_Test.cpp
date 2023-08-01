/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "DamperFreezePrevention.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "HeaterVotedState.h"
}

#include "CppUTest/TestHarness.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

enum
{
   InitialTemperatureInDegFx100 = 3200
};

#define Given
#define Change
#define When
#define Then
#define And

static const DamperFreezePreventionConfiguration_t config = {
   .damperHeaterVoteErd = Erd_FreshFoodDamperHeater_DamperFreezePreventionVote,
   .damperPositionVoteErd = Erd_FreshFoodDamperPosition_DamperFreezePreventionVote,
   .sourceThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .targetThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved,
   .sourceFilteredTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .targetFilteredTemperatureErd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .damperCurrentPositionErd = Erd_FreshFoodDamperCurrentPosition,
   .timerModuleErd = Erd_TimerModule,
   .damperFreezePreventionFsmStateErd = Erd_DamperFreezePreventionFsmState
};

TEST_GROUP(DamperFreezePrevention)
{
   DamperFreezePrevention_t instance;
   I_DataModel_t *dataModel;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const SingleDamperData_t *freshFoodDamperParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      freshFoodDamperParametricData = PersonalityParametricData_Get(dataModel)->freshFoodDamperData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TheModuleIsInitialized()
   {
      DamperFreezePrevention_Init(&instance, dataModelDouble.dataModel, &config, freshFoodDamperParametricData);
   }

   void DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_t expectedState)
   {
      DamperFreezePreventionFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DamperFreezePreventionFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DamperCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDamperCurrentPosition, &position);
   }

   void DamperOpens()
   {
      DamperCurrentPositionIs(DamperPosition_Open);
   }

   void DamperCloses()
   {
      DamperCurrentPositionIs(DamperPosition_Closed);
   }

   void DamperPositionShouldBeVotedDontCare()
   {
      DamperVotedPosition_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(false, vote.care);
   }

   void DamperPositionShouldBeVoted(DamperPosition_t position)
   {
      DamperVotedPosition_t actualPosition;
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_DamperFreezePreventionVote, &actualPosition);

      CHECK_EQUAL(true, actualPosition.care);
      CHECK_EQUAL(position, actualPosition.position);
   }

   void DamperHeaterShouldNotBeVotedFor()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_FALSE(vote.care);
   }

   void DamperHeaterShouldBeVotedOn()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Max, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void DamperHeaterShouldBeVotedOff()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_FreshFoodDamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Min, vote.percentageDutyCycle);
      CHECK_FALSE(vote.care);
   }

   void TargetThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, set);
   }

   void TargetThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, clear);
   }

   void SourceThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, set);
   }

   void SourceThermistorIsInvalid()
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, clear);
   }

   void TargetTemperatureIs(TemperatureDegFx100_t targetTemperature)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &targetTemperature);
   }

   void SourceTemperatureIs(TemperatureDegFx100_t sourceTemperature)
   {
      DataModel_Write(dataModel, Erd_Freezer_FilteredTemperatureResolvedInDegFx100, &sourceTemperature);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid()
   {
      Given TargetThermistorIsInvalid();
      Given SourceThermistorIsValid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsValid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsValid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndSourceThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid()
   {
      Given TargetThermistorIsInvalid();
      Given SourceThermistorIsValid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsInvalid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsValidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInMonitoringTemperatureChange()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsValid();
      Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   }

   void DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsClosedAndDamperHeaterHasBeenVotedOn()
   {
      Given DamperFreezePreventionIsInMonitoringTemperatureChange();
      Given TargetTemperatureIs(InitialTemperatureInDegFx100);
      Given DamperOpens();

      When DamperCloses();
      TargetTemperatureIs(InitialTemperatureInDegFx100 + (freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
      After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
      DamperHeaterShouldNotBeVotedFor();

      After(1);
      DamperHeaterShouldBeVotedOn();
      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);
   }

   void DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsOpenAndDamperHeaterHasBeenVotedOn()
   {
      Given DamperFreezePreventionIsInMonitoringTemperatureChange();
      Given TargetTemperatureIs(InitialTemperatureInDegFx100);
      Given DamperCloses();

      When DamperOpens();
      TargetTemperatureIs(InitialTemperatureInDegFx100 - (freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
      After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
      DamperHeaterShouldNotBeVotedFor();

      After(1);
      DamperHeaterShouldBeVotedOn();
      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);
   }

   void DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsClosed()
   {
      Given DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsClosedAndDamperHeaterHasBeenVotedOn();

      After(freshFoodDamperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
      DamperHeaterShouldBeVotedOn();

      After(1);
      DamperHeaterShouldBeVotedOff();
   }

   void DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen()
   {
      Given DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsOpenAndDamperHeaterHasBeenVotedOn();

      After(freshFoodDamperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
      DamperHeaterShouldBeVotedOn();

      After(1);
      DamperHeaterShouldBeVotedOff();
   }

   void DamperFreezePreventionHasJustVotedDontCareForDamperPosition()
   {
      Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
      DamperPositionShouldBeVoted(DamperPosition_Closed);

      When DamperCurrentPositionIs(DamperPosition_Closed);
      DamperPositionShouldBeVotedDontCare();
   }
};

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfTargetThermistorIsInvalidAndSourceThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsInvalid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfTargetThermistorIsValidAndSourceThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsInvalid();
   Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfSourceTemperatureIsGreaterThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoMonitoringTemperatureChangeStateIfTargetThermistorIsValidAndSourceThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
}

TEST(DamperFreezePrevention, ShouldTransitionFromIdleToMonitoringTemperatureChangeWhenTargetThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid)
{
   Given DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid();

   When TargetThermistorIsValid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
}

TEST(DamperFreezePrevention, ShouldTransitionFromIdleToMonitoringTemperatureChangeWhenSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsValid)
{
   Given DamperFreezePreventionIsInIdleStateAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsValid();

   When SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
}

TEST(DamperFreezePrevention, ShouldTransitionFromIdleToMonitoringTemperatureChangeWhenSourceThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValid)
{
   Given DamperFreezePreventionIsInIdleStateAndSourceThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValid();

   When SourceThermistorIsValid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
}

TEST(DamperFreezePrevention, ShouldNotTransitionFromIdleToMonitoringTemperatureChangeWhenSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsInvalid)
{
   Given DamperFreezePreventionIsInIdleStateAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsInvalid();

   When SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldNotTransitionFromIdleToMonitoringTemperatureChangeWhenSourceThermistorChangesFromInvalidToValidAndTargetThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionIsInIdleStateAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsInvalid();

   When SourceThermistorIsValid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldNotTransitionFromIdleToMonitoringTemperatureChangeWhenSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsInvalidAndSourceThermistorIsValid)
{
   Given DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid();

   When SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldNotTransitionFromIdleToMonitoringTemperatureChangeWhenTargetThermistorChangesFromInvalidToValidAndSourceThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid();

   When TargetThermistorIsValid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldNotTransitionFromIdleToMonitoringTemperatureChangeWhenTargetThermistorChangesFromInvalidToValidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid)
{
   Given DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid();

   When TargetThermistorIsValid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldNotTransitionFromIdleToMonitoringTemperatureChangeWhenSourceThermistorChangesFromInvalidToValidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValid)
{
   Given DamperFreezePreventionIsInIdleStateAndTargetThermistorIsValidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid();

   When SourceThermistorIsValid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTurnOnDamperHeaterWhenTemperatureDoesNotDecreaseByMinimumTemperatureChangeAfterDamperOpens)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();
   Given TargetTemperatureIs(InitialTemperatureInDegFx100);

   When DamperOpens();
   TargetTemperatureIs(InitialTemperatureInDegFx100 - (freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
   DamperHeaterShouldNotBeVotedFor();

   After(1);
   DamperHeaterShouldBeVotedOn();
}

TEST(DamperFreezePrevention, ShouldTurnOnDamperHeaterWhenTemperatureDoesNotIncreaseByMinimumTemperatureChangeAfterDamperCloses)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();
   Given TargetTemperatureIs(InitialTemperatureInDegFx100);
   Given DamperOpens();

   When DamperCloses();
   TargetTemperatureIs(InitialTemperatureInDegFx100 + (freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
   DamperHeaterShouldNotBeVotedFor();

   After(1);
   DamperHeaterShouldBeVotedOn();
}

TEST(DamperFreezePrevention, ShouldTransitionFromMonitoringTemperatureChangeToIdleWhenTargetThermistorIsInvalid)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();

   When TargetThermistorIsInvalid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionFromMonitoringTemperatureChangeToIdleWhenSourceThermistorIsInvalid)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();

   When SourceThermistorIsInvalid();
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionFromMonitoringTemperatureChangeToIdleWhenSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();

   When SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionFromMonitoringTemperatureChangeToIdleWhenSourceTemperatureIsGreaterThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();

   When SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTurnOffDamperHeaterAfterDamperHeaterOnTime)
{
   Given DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsClosedAndDamperHeaterHasBeenVotedOn();

   After(freshFoodDamperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
   DamperHeaterShouldBeVotedOn();

   After(1);
   DamperHeaterShouldBeVotedOff();
}

TEST(DamperFreezePrevention, ShouldVoteToOpenDamperIfDamperIsClosedWhenDamperHeaterIsTurnedOff)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsClosed();
   DamperPositionShouldBeVoted(DamperPosition_Open);
}

TEST(DamperFreezePrevention, ShouldVoteToCloseDamperIfDamperIsOpenWhenDamperHeaterIsTurnedOff)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   DamperPositionShouldBeVoted(DamperPosition_Closed);
}

TEST(DamperFreezePrevention, ShouldVoteDontCareForDamperPositionWhenCurrentDamperPositionChangesToOpenAfterVotingForOpenPosition)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsClosed();
   DamperPositionShouldBeVoted(DamperPosition_Open);

   When DamperCurrentPositionIs(DamperPosition_Open);
   DamperPositionShouldBeVotedDontCare();
}

TEST(DamperFreezePrevention, ShouldVoteDontCareForDamperPositionWhenCurrentDamperPositionChangesToClosedAfterVotingForClosedPosition)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   DamperPositionShouldBeVoted(DamperPosition_Closed);

   When DamperCurrentPositionIs(DamperPosition_Closed);
   DamperPositionShouldBeVotedDontCare();
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterVotingDontCareForDamperPositionIfTargetThermistorIsInvalid)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   DamperPositionShouldBeVoted(DamperPosition_Closed);

   TargetThermistorIsInvalid();
   When DamperCurrentPositionIs(DamperPosition_Closed);
   DamperPositionShouldBeVotedDontCare();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterVotingDontCareForDamperPositionIfSourceThermistorIsInvalid)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   DamperPositionShouldBeVoted(DamperPosition_Closed);

   SourceThermistorIsInvalid();
   When DamperCurrentPositionIs(DamperPosition_Closed);
   DamperPositionShouldBeVotedDontCare();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterVotingDontCareForDamperPositionIfSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   DamperPositionShouldBeVoted(DamperPosition_Closed);

   SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
   When DamperCurrentPositionIs(DamperPosition_Closed);
   DamperPositionShouldBeVotedDontCare();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterVotingDontCareForDamperPositionIfSourceTemperatureIsGreaterThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   DamperPositionShouldBeVoted(DamperPosition_Closed);

   SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
   When DamperCurrentPositionIs(DamperPosition_Closed);
   DamperPositionShouldBeVotedDontCare();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToMonitoringTemperatureChangeAfterVotingDontCareForDamperPositionIfTargetThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   DamperPositionShouldBeVoted(DamperPosition_Closed);

   TargetThermistorIsValid();
   SourceTemperatureIs(freshFoodDamperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   When DamperCurrentPositionIs(DamperPosition_Closed);
   DamperPositionShouldBeVotedDontCare();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
}

TEST(DamperFreezePrevention, ShouldAttemptToDefrostDamperRepeatedly)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();
   Given TargetTemperatureIs(InitialTemperatureInDegFx100);

   When DamperOpens();
   TargetTemperatureIs(InitialTemperatureInDegFx100 - (freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   After(freshFoodDamperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN);
   DamperPositionShouldBeVoted(DamperPosition_Closed);

   When DamperCloses();
   DamperPositionShouldBeVotedDontCare();

   When DamperOpens();
   After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   After(freshFoodDamperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN);
   DamperPositionShouldBeVoted(DamperPosition_Closed);
}

TEST(DamperFreezePrevention, ShouldAttemptToDefrostEvenIfItDoesntOnTheFirstTry)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();
   Given TargetTemperatureIs(InitialTemperatureInDegFx100);

   When DamperOpens();
   TargetTemperatureIs(InitialTemperatureInDegFx100 - (freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100));
   After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   DamperHeaterShouldNotBeVotedFor();
   DamperPositionShouldBeVotedDontCare();

   TargetTemperatureIs(InitialTemperatureInDegFx100 - (freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(freshFoodDamperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   After(freshFoodDamperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN);
   DamperPositionShouldBeVoted(DamperPosition_Closed);
}
