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
   .damperHeaterVoteErd = Erd_DamperHeater_DamperFreezePreventionVote,
   .sourceThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .targetThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved,
   .sourceFilteredTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .targetFilteredTemperatureErd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .damperCurrentPositionErd = Erd_DamperCurrentPosition,
   .timerModuleErd = Erd_TimerModule,
   .damperFreezePreventionFsmStateErd = Erd_DamperFreezePreventionFsmState,
   .damperFullCycleRequestErd = Erd_DamperFullCycleRequest
};

TEST_GROUP(DamperFreezePrevention)
{
   DamperFreezePrevention_t instance;
   I_DataModel_t *dataModel;
   ReferDataModel_TestDouble_t dataModelDouble;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   const DamperData_t *damperParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      damperParametricData = PersonalityParametricData_Get(dataModel)->damperData;
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void TheModuleIsInitialized()
   {
      DamperFreezePrevention_Init(&instance, dataModelDouble.dataModel, &config, damperParametricData);
   }

   void DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_t expectedState)
   {
      DamperFreezePreventionFsmState_t actualState;
      DataModel_Read(dataModel, Erd_DamperFreezePreventionFsmState, &actualState);

      CHECK_EQUAL(expectedState, actualState);
   }

   void DamperCurrentPositionIs(DamperPosition_t position)
   {
      DataModel_Write(dataModel, Erd_DamperCurrentPosition, &position);
   }

   void DamperOpens()
   {
      DamperCurrentPositionIs(DamperPosition_Open);
   }

   void DamperCloses()
   {
      DamperCurrentPositionIs(DamperPosition_Closed);
   }

   void WhenFullCycleRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DamperFullCycleRequest, &state);
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

   void DamperHeaterShouldNotBeVotedFor()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_FALSE(vote.care);
   }

   void DamperHeaterShouldBeVotedOn()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DamperFreezePreventionVote, &vote);

      CHECK_EQUAL(PercentageDutyCycle_Max, vote.percentageDutyCycle);
      CHECK_TRUE(vote.care);
   }

   void DamperHeaterShouldBeVotedOff()
   {
      PercentageDutyCycleVote_t vote;
      DataModel_Read(dataModel, Erd_DamperHeater_DamperFreezePreventionVote, &vote);

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
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsValid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsValid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValidAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndSourceThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndTargetThermistorIsValid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsValid()
   {
      Given TargetThermistorIsInvalid();
      Given SourceThermistorIsValid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsInvalid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInIdleStateAndTargetThermistorIsValidAndSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePreventionAndSourceThermistorIsInvalid()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsInvalid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
   }

   void DamperFreezePreventionIsInMonitoringTemperatureChange()
   {
      Given TargetThermistorIsValid();
      Given SourceThermistorIsValid();
      Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
      Given TheModuleIsInitialized();

      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
   }

   void DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsClosedAndDamperHeaterHasBeenVotedOn()
   {
      Given DamperFreezePreventionIsInMonitoringTemperatureChange();
      Given TargetTemperatureIs(InitialTemperatureInDegFx100);
      Given DamperOpens();

      When DamperCloses();
      TargetTemperatureIs(InitialTemperatureInDegFx100 + (damperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
      After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
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
      TargetTemperatureIs(InitialTemperatureInDegFx100 - (damperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
      After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
      DamperHeaterShouldNotBeVotedFor();

      After(1);
      DamperHeaterShouldBeVotedOn();
      DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_DamperHeaterOn);
   }

   void DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsClosed()
   {
      Given DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsClosedAndDamperHeaterHasBeenVotedOn();

      After(damperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
      DamperHeaterShouldBeVotedOn();

      After(1);
      DamperHeaterShouldBeVotedOff();
   }

   void DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen()
   {
      Given DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsOpenAndDamperHeaterHasBeenVotedOn();

      After(damperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
      DamperHeaterShouldBeVotedOn();

      After(1);
      DamperHeaterShouldBeVotedOff();
   }
};

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfTargetThermistorIsInvalidAndSourceThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsInvalid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfTargetThermistorIsValidAndSourceThermistorIsInvalidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsInvalid();
   Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfSourceTemperatureIsGreaterThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoIdleStateIfSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
   Given TheModuleIsInitialized();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldInitializeIntoMonitoringTemperatureChangeStateIfTargetThermistorIsValidAndSourceThermistorIsValidAndSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given TargetThermistorIsValid();
   Given SourceThermistorIsValid();
   Given SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
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

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
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

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
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

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
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
   TargetTemperatureIs(InitialTemperatureInDegFx100 - (damperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
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
   TargetTemperatureIs(InitialTemperatureInDegFx100 + (damperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN - 1);
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

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionFromMonitoringTemperatureChangeToIdleWhenSourceTemperatureIsGreaterThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTurnOffDamperHeaterAfterDamperHeaterOnTime)
{
   Given DamperFreezePreventionIsInDamperHeaterOnStateAndDamperIsClosedAndDamperHeaterHasBeenVotedOn();

   After(damperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN - 1);
   DamperHeaterShouldBeVotedOn();

   After(1);
   DamperHeaterShouldBeVotedOff();
}

TEST(DamperFreezePrevention, ShouldStartFullCycleIfDamperIsClosedWhenDamperHeaterIsTurnedOff)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsClosed();
   FullCycleRequestShouldBeSet();
}

TEST(DamperFreezePrevention, ShouldStartFullCycleIfDamperIsOpenWhenDamperHeaterIsTurnedOff)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   FullCycleRequestShouldBeSet();
}

TEST(DamperFreezePrevention, ShouldNotStartFullCycleAgainWhenCurrentDamperPositionChangesToOpenAfterRequestingFullCycleOnce)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsClosed();
   FullCycleRequestShouldBeSet();

   WhenFullCycleRequestIs(false);
   When DamperCurrentPositionIs(DamperPosition_Open);
   FullCycleRequestShouldBeClear();
}

TEST(DamperFreezePrevention, ShouldNotStartFullCycleAgainWhenCurrentDamperPositionChangesToClosedAfterRequestingFullCycleOnce)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   FullCycleRequestShouldBeSet();

   WhenFullCycleRequestIs(false);
   When DamperCurrentPositionIs(DamperPosition_Closed);
   FullCycleRequestShouldBeClear();
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterRequestingDamperFullCycleIfTargetThermistorIsInvalid)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   FullCycleRequestShouldBeSet();

   WhenFullCycleRequestIs(false);
   When TargetThermistorIsInvalid();
   When DamperCurrentPositionIs(DamperPosition_Closed);
   FullCycleRequestShouldBeClear();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterRequestingDamperFullCycleIfSourceThermistorIsInvalid)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   FullCycleRequestShouldBeSet();

   WhenFullCycleRequestIs(false);
   When SourceThermistorIsInvalid();
   When DamperCurrentPositionIs(DamperPosition_Closed);
   FullCycleRequestShouldBeClear();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterRequestingFullCycleIfSourceTemperatureIsEqualToMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   FullCycleRequestShouldBeSet();

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100);
   WhenFullCycleRequestIs(false);
   When DamperCurrentPositionIs(DamperPosition_Closed);
   FullCycleRequestShouldBeClear();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToIdleAfterRequestingDamperFullCycleIfSourceTemperatureIsGreaterThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   FullCycleRequestShouldBeSet();

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 + 1);
   WhenFullCycleRequestIs(false);
   When DamperCurrentPositionIs(DamperPosition_Closed);
   FullCycleRequestShouldBeClear();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_Idle);
}

TEST(DamperFreezePrevention, ShouldTransitionToMonitoringTemperatureChangeAfterRequestingDamperFullCycleIfSourceTemperatureIsLessThanMaxSourceTemperatureForFreezePrevention)
{
   Given DamperFreezePreventionHasJustVotedForDamperHeaterOffAndDamperIsOpen();
   FullCycleRequestShouldBeSet();

   When SourceTemperatureIs(damperParametricData->sourceCompartmentMaximumTemperatureToRunCheckInDegFx100 - 1);
   WhenFullCycleRequestIs(false);
   When DamperCurrentPositionIs(DamperPosition_Closed);
   FullCycleRequestShouldBeClear();

   DamperFreezePreventionFsmStateShouldBe(DamperFreezePreventionFsmState_MonitoringTemperatureChange);
}

TEST(DamperFreezePrevention, ShouldAttemptToDefrostDamperRepeatedly)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();
   Given TargetTemperatureIs(InitialTemperatureInDegFx100);

   When DamperOpens();
   When TargetTemperatureIs(InitialTemperatureInDegFx100 - (damperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   After(damperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN);
   FullCycleRequestShouldBeSet();

   WhenFullCycleRequestIs(false);
   When DamperCloses();
   FullCycleRequestShouldBeClear();

   When DamperOpens();
   After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   After(damperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN);
   FullCycleRequestShouldBeSet();
}

TEST(DamperFreezePrevention, ShouldAttemptToDefrostEvenIfItDoesntOnTheFirstTry)
{
   Given DamperFreezePreventionIsInMonitoringTemperatureChange();
   Given TargetTemperatureIs(InitialTemperatureInDegFx100);

   When DamperOpens();
   When TargetTemperatureIs(InitialTemperatureInDegFx100 - (damperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100));
   After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   DamperHeaterShouldNotBeVotedFor();
   FullCycleRequestShouldBeClear();

   When TargetTemperatureIs(InitialTemperatureInDegFx100 - (damperParametricData->targetCompartmentMinimumTemperatureChangeInDegFx100 - 1));
   After(damperParametricData->targetCompartmentMinimumTemperatureChangeTimeInMinutes * MSEC_PER_MIN);
   After(damperParametricData->targetCompartmentDamperHeaterOnTimeInMinutes * MSEC_PER_MIN);
   FullCycleRequestShouldBeSet();
}
