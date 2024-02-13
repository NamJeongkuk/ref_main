/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CabinetTemperatureExceededMonitor.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   CLOSED = false,
   OPEN = true
};

enum
{
   SomeTemperatureInDegF = 40,
   SomeTemperatureInDegFX100 = SomeTemperatureInDegF * 100,
   TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 = 2000,
   TemperatureLowerHysteresisLimitAboveSetpointInDegFX100 = 1500
};

static const Erd_t doors[] = {
   Erd_RightSideFreshFoodDoorStatusResolved,
   Erd_LeftSideFreshFoodDoorStatusResolved
};

static const ErdList_t doorErdList = {
   .erds = doors,
   .numberOfErds = NUM_ELEMENTS(doors)
};

static const CabinetTemperatureExceededMonitorConfiguration_t config = {
   .doorErdList = doorErdList,
   .cabinetFilteredTemperatureInDegFX100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .cabinetThermistorIsValidErd = Erd_FreshFoodThermistor_IsValidResolved,
   .cabinetSetpointTemperatureInDegFErd = Erd_FreshFoodSetpointStatus,
   .defrostHsmStateErd = Erd_DefrostHsmState,
   .faultErd = Erd_FreshFoodTemperatureExceededFault
};

static const CabinetTemperatureExceededData_t minimumDoorTimeEqualToDefrostTimeData = {
   .temperatureUpperHysteresisLimitAboveSetpointInDegFX100 = TemperatureUpperHysteresisLimitAboveSetpointInDegFX100,
   .temperatureLowerHysteresisLimitAboveSetpointInDegFX100 = TemperatureLowerHysteresisLimitAboveSetpointInDegFX100,
   .minimumTimeSincePowerOnInMinutes = 8 * MINUTES_PER_HOUR,
   .minimumTimeDoorsMustBeClosedInMinutes = 1 * MINUTES_PER_HOUR,
   .minimumTimeSinceDefrostHasCompletedInMinutes = 1 * MINUTES_PER_HOUR
};

static const CabinetTemperatureExceededData_t minimumDoorTimeLessThanDefrostTimeData = {
   .temperatureUpperHysteresisLimitAboveSetpointInDegFX100 = TemperatureUpperHysteresisLimitAboveSetpointInDegFX100,
   .temperatureLowerHysteresisLimitAboveSetpointInDegFX100 = TemperatureLowerHysteresisLimitAboveSetpointInDegFX100,
   .minimumTimeSincePowerOnInMinutes = 8 * MINUTES_PER_HOUR,
   .minimumTimeDoorsMustBeClosedInMinutes = 1 * MINUTES_PER_HOUR,
   .minimumTimeSinceDefrostHasCompletedInMinutes = 2 * MINUTES_PER_HOUR
};

static const CabinetTemperatureExceededData_t minimumDoorTimeGreaterThanDefrostTimeData = {
   .temperatureUpperHysteresisLimitAboveSetpointInDegFX100 = TemperatureUpperHysteresisLimitAboveSetpointInDegFX100,
   .temperatureLowerHysteresisLimitAboveSetpointInDegFX100 = TemperatureLowerHysteresisLimitAboveSetpointInDegFX100,
   .minimumTimeSincePowerOnInMinutes = 8 * MINUTES_PER_HOUR,
   .minimumTimeDoorsMustBeClosedInMinutes = 2 * MINUTES_PER_HOUR,
   .minimumTimeSinceDefrostHasCompletedInMinutes = 1 * MINUTES_PER_HOUR
};

TEST_GROUP(CabinetTemperatureExceededMonitor)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   CabinetTemperatureExceededMonitor_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime()
   {
      CabinetTemperatureExceededMonitor_Init(
         &instance,
         dataModel,
         &config,
         &minimumDoorTimeEqualToDefrostTimeData);
   }

   void GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeLessThanDefrostTime()
   {
      CabinetTemperatureExceededMonitor_Init(
         &instance,
         dataModel,
         &config,
         &minimumDoorTimeLessThanDefrostTimeData);
   }

   void GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime()
   {
      CabinetTemperatureExceededMonitor_Init(
         &instance,
         dataModel,
         &config,
         &minimumDoorTimeGreaterThanDefrostTimeData);
   }

   void GivenTheLeftSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorStatusResolved, &state);
   }

   void WhenTheLeftSideFreshFoodDoorIs(bool state)
   {
      GivenTheLeftSideFreshFoodDoorIs(state);
   }

   void GivenTheRightSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatusResolved, &state);
   }

   void WhenTheRightSideFreshFoodDoorIs(bool state)
   {
      GivenTheRightSideFreshFoodDoorIs(state);
   }

   void GivenAllDoorsAre(bool state)
   {
      GivenTheLeftSideFreshFoodDoorIs(state);
      GivenTheRightSideFreshFoodDoorIs(state);
   }

   void GivenDefrostHsmStateIs(DefrostHsmState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostHsmState, &state);
   }

   void WhenDefrostHsmStateIs(DefrostHsmState_t state)
   {
      GivenDefrostHsmStateIs(state);
   }

   void GivenCabinetFilteredTemperatureIs(TemperatureDegFx100_t temperatureDegFX100)
   {
      DataModel_Write(dataModel, Erd_FreshFood_FilteredTemperatureResolvedInDegFx100, &temperatureDegFX100);
   }

   void WhenCabinetFilteredTemperatureIs(TemperatureDegFx100_t temperatureDegFX100)
   {
      GivenCabinetFilteredTemperatureIs(temperatureDegFX100);
   }

   void GivenCabinetThermistorIsValidIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void WhenCabinetThermistorIsValidIs(bool state)
   {
      GivenCabinetThermistorIsValidIs(state);
   }

   void GivenSetpointTemperatureIs(Setpoint_t temperatureF)
   {
      DataModel_Write(dataModel, Erd_FreshFoodSetpointStatus, &temperatureF);
   }

   void WhenSetpointTemperatureIs(Setpoint_t temperatureF)
   {
      GivenSetpointTemperatureIs(temperatureF);
   }

   void FaultShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodTemperatureExceededFault, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint()
   {
      GivenCabinetThermistorIsValidIs(true);
      GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
      GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   }

   void GivenCabinetFilteredTemperatureIsValidAndEqualToSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint()
   {
      GivenCabinetThermistorIsValidIs(true);
      GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100);
      GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   }

   void GivenFaultIsSetAndCabinetThermistorIsValid()
   {
      GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
      GivenAllDoorsAre(CLOSED);
      GivenDefrostHsmStateIs(DefrostHsmState_Idle);
      GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime();

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
      FaultShouldBe(CLEAR);

      After(1);
      FaultShouldBe(CLEAR);

      After((minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes - minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes) * MSEC_PER_MIN - 1);
      FaultShouldBe(CLEAR);

      After(1);
      FaultShouldBe(SET);
   }

   void GivenFaultIsSetAndThenCabinetThermistorWentInvalid()
   {
      GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
      GivenAllDoorsAre(CLOSED);
      GivenDefrostHsmStateIs(DefrostHsmState_Idle);
      GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime();

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
      FaultShouldBe(CLEAR);

      After(1);
      FaultShouldBe(CLEAR);

      After((minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes - minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes) * MSEC_PER_MIN - 1);
      FaultShouldBe(CLEAR);

      After(1);
      FaultShouldBe(SET);

      WhenCabinetThermistorIsValidIs(false);
   }

   void GivenAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime()
   {
      GivenAllDoorsAre(CLOSED);
      GivenDefrostHsmStateIs(DefrostHsmState_Idle);
      GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);
      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN);
   }

   void GivenAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeGreaterThanMinimumDefrostTime()
   {
      GivenAllDoorsAre(CLOSED);
      GivenDefrostHsmStateIs(DefrostHsmState_Idle);
      GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime();

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);
      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN);
   }
};

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHsmHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndThenDefrostHsmHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostHsmStateIs(DefrostHsmState_Dwell);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);

   WhenDefrostHsmStateIs(DefrostHsmState_Idle);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenDefrostHsmHasBeenInIdleForMinimumTimeAndThenAllDoorsHaveBeenClosedForMinimumTimeAndItsBeenMinimumTimeSincePowerOnAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenTheLeftSideFreshFoodDoorIs(CLOSED);
   GivenTheRightSideFreshFoodDoorIs(OPEN);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);

   WhenTheRightSideFreshFoodDoorIs(CLOSED);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHsmHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeLessThanDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeLessThanDefrostTime();

   After(minimumDoorTimeLessThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeLessThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);

   After((minimumDoorTimeLessThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes - minimumDoorTimeLessThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes) * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHsmHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeGreaterThanDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime();

   After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);

   After((minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes - minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes) * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultWhenDoorOpensBeforeMinimumTimeEvenIfMinimumDefrostTimeIsMetAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeLessThanDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeLessThanDefrostTime();

   After(minimumDoorTimeLessThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeLessThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   WhenTheLeftSideFreshFoodDoorIs(OPEN);
   After(1);
   FaultShouldBe(CLEAR);

   After((minimumDoorTimeLessThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes - minimumDoorTimeLessThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes) * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultWhenDefrostTransitionsOutOfIdleEvenIfAllDoorsHaveBeenClosedForMinimumTimeAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeGreaterThanDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime();

   After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   WhenDefrostHsmStateIs(DefrostHsmState_Dwell);
   After(1);
   FaultShouldBe(CLEAR);

   After((minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes - minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes) * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultIfCabinetTemperatureIsEqualToUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHsmHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOn)
{
   GivenCabinetFilteredTemperatureIsValidAndEqualToSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetTheFaultAgainOrClearItWhenDefrostHsmStateChangesToANonIdleStateAfterFaultHasAlreadyBeenSetAndThenItIsInIdleAgainForMinimumTime)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenDefrostHsmStateIs(DefrostHsmState_Prechill);
   WhenDefrostHsmStateIs(DefrostHsmState_Idle);

   After(minimumDoorTimeLessThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(SET);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetTheFaultAgainOrClearItWhenADoorOpensAfterFaultHasAlreadyBeenSetAndThenAllDoorsAreClosedAgainForMinimumTime)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenTheLeftSideFreshFoodDoorIs(OPEN);
   WhenTheLeftSideFreshFoodDoorIs(CLOSED);

   After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(SET);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldClearFaultWhenCabinetTemperatureDropsBelowSetpointTemperaturePlusLowerHysteresisLimitAboveSetpointWhileAllDoorsAreClosedAndDefrostIsInIdleAndCabinetThermistorIsValid)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureLowerHysteresisLimitAboveSetpointInDegFX100 - 1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotClearFaultWhenCabinetTemperatureDropsBelowSetpointTemperaturePlusLowerHysteresisLimitAboveSetpointWhileAllDoorsAreClosedAndDefrostIsInIdleAndCabinetThermistorIsInvalid)
{
   GivenFaultIsSetAndThenCabinetThermistorWentInvalid();

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureLowerHysteresisLimitAboveSetpointInDegFX100 - 1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotClearFaultWhenCabinetTemperatureDecreasesToSetpointTemperaturePlusLowerHysteresisLimitAboveSetpointWhileAllDoorsAreClosedAndDefrostIsInIdleAndCabinetThermistorIsValid)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureLowerHysteresisLimitAboveSetpointInDegFX100);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultAgainWhenCabinetTemperatureRisesAboveSetpointTemperaturePlusUpperHysteresisLimitAboveSetpointAndAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureLowerHysteresisLimitAboveSetpointInDegFX100 - 1);
   FaultShouldBe(CLEAR);

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultAgainWhenCabinetTemperatureRisesAboveSetpointTemperaturePlusUpperHysteresisLimitAboveSetpointIfAndAllDoorsHaveNotBeenClosedForMinimumTimeAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureLowerHysteresisLimitAboveSetpointInDegFX100 - 1);
   FaultShouldBe(CLEAR);

   WhenTheLeftSideFreshFoodDoorIs(OPEN);

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultAgainWhenCabinetTemperatureRisesAboveSetpointTemperaturePlusUpperHysteresisLimitAboveSetpointIfAndAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHasNotBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureLowerHysteresisLimitAboveSetpointInDegFX100 - 1);
   FaultShouldBe(CLEAR);

   WhenDefrostHsmStateIs(DefrostHsmState_Prechill);

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenSetpointChangesSuchThatCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenCabinetThermistorIsValidIs(true);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF + 1);
   GivenAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenSetpointTemperatureIs(SomeTemperatureInDegF);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenSetpointChangesSuchThatCabinetTemperatureIsLessThanLowerHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenCabinetThermistorIsValidIs(true);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF + 1);
   GivenAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenSetpointTemperatureIs(SomeTemperatureInDegF);
   FaultShouldBe(SET);

   WhenSetpointTemperatureIs(SomeTemperatureInDegF + (TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 - TemperatureLowerHysteresisLimitAboveSetpointInDegFX100) / 100 + 1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenCabinetThermistorBecomesValidAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValidWithMinimumDoorTimeEqualToMinimumDefrostTime)
{
   GivenCabinetThermistorIsValidIs(false);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   GivenAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenCabinetThermistorIsValidIs(true);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenCabinetThermistorBecomesValidAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValidWithMinimumDoorTimeGreaterThanMinimumDefrostTime)
{
   GivenCabinetThermistorIsValidIs(false);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   GivenAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeGreaterThanMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenCabinetThermistorIsValidIs(true);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultWhenDefrostHsmHasBeenInIdleForMinimumTimeAndThenAllDoorsHaveBeenClosedForMinimumTimeAndItsBeenMinimumTimeSincePowerOnAndCabinetTemperatureIsEqualToUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndEqualToSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenTheLeftSideFreshFoodDoorIs(CLOSED);
   GivenTheRightSideFreshFoodDoorIs(OPEN);
   GivenDefrostHsmStateIs(DefrostHsmState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);

   WhenTheRightSideFreshFoodDoorIs(CLOSED);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultWhenCabinetThermistorIsInvalidAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValidWithMinimumDoorTimeEqualToMinimumDefrostTime)
{
   GivenCabinetThermistorIsValidIs(true);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   GivenAllDoorsHaveBeenClosedAndDefrostHsmHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenCabinetThermistorIsValidIs(false);
   FaultShouldBe(CLEAR);

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   FaultShouldBe(CLEAR);
}
