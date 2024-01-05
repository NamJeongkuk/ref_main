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
   Erd_LeftSideFreshFoodDoorIsOpenResolved
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
   .defrostStateErd = Erd_DefrostState,
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
      DataModel_Write(dataModel, Erd_LeftSideFreshFoodDoorIsOpenResolved, &state);
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

   void GivenDefrostStateIs(DefrostState_t state)
   {
      DataModel_Write(dataModel, Erd_DefrostState, &state);
   }

   void WhenDefrostStateIs(DefrostState_t state)
   {
      GivenDefrostStateIs(state);
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
      GivenDefrostStateIs(DefrostState_Idle);
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
      GivenDefrostStateIs(DefrostState_Idle);
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

   void GivenAllDoorsHaveBeenClosedAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime()
   {
      GivenAllDoorsAre(CLOSED);
      GivenDefrostStateIs(DefrostState_Idle);
      GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);
      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN);
   }

   void GivenAllDoorsHaveBeenClosedAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeGreaterThanMinimumDefrostTime()
   {
      GivenAllDoorsAre(CLOSED);
      GivenDefrostStateIs(DefrostState_Idle);
      GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime();

      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);
      After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN);
   }
};

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostStateIs(DefrostState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndThenDefrostHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostStateIs(DefrostState_Dwell);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);

   WhenDefrostStateIs(DefrostState_Idle);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenDefrostHasBeenInIdleForMinimumTimeAndThenAllDoorsHaveBeenClosedForMinimumTimeAndItsBeenMinimumTimeSincePowerOnAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenTheLeftSideFreshFoodDoorIs(CLOSED);
   GivenTheRightSideFreshFoodDoorIs(OPEN);
   GivenDefrostStateIs(DefrostState_Idle);
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

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeLessThanDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostStateIs(DefrostState_Idle);
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

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOnWithMinimumDoorTimeGreaterThanDefrostTimeDataAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndGreaterThanSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostStateIs(DefrostState_Idle);
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
   GivenDefrostStateIs(DefrostState_Idle);
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
   GivenDefrostStateIs(DefrostState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeGreaterThanDefrostTime();

   After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   WhenDefrostStateIs(DefrostState_Dwell);
   After(1);
   FaultShouldBe(CLEAR);

   After((minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeDoorsMustBeClosedInMinutes - minimumDoorTimeGreaterThanDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes) * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultIfCabinetTemperatureIsEqualToUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHasBeenInIdleForMinimumTimeAndItsBeenMinimumTimeSincePowerOn)
{
   GivenCabinetFilteredTemperatureIsValidAndEqualToSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenAllDoorsAre(CLOSED);
   GivenDefrostStateIs(DefrostState_Idle);
   GivenMonitorIsInitializedWithParametricWithMinimumDoorTimeEqualToDefrostTime();

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSincePowerOnInMinutes * MSEC_PER_MIN);

   After(minimumDoorTimeEqualToDefrostTimeData.minimumTimeSinceDefrostHasCompletedInMinutes * MSEC_PER_MIN - 1);
   FaultShouldBe(CLEAR);

   After(1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetTheFaultAgainOrClearItWhenDefrostStateChangesToANonIdleStateAfterFaultHasAlreadyBeenSetAndThenItIsInIdleAgainForMinimumTime)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenDefrostStateIs(DefrostState_Prechill);
   WhenDefrostStateIs(DefrostState_Idle);

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

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultAgainWhenCabinetTemperatureRisesAboveSetpointTemperaturePlusUpperHysteresisLimitAboveSetpointAndAllDoorsHaveBeenClosedForMinimumTimeAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenFaultIsSetAndCabinetThermistorIsValid();

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureLowerHysteresisLimitAboveSetpointInDegFX100 - 1);
   FaultShouldBe(CLEAR);

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultAgainWhenCabinetTemperatureRisesAboveSetpointTemperaturePlusUpperHysteresisLimitAboveSetpointIfAndAllDoorsHaveNotBeenClosedForMinimumTimeAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
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

   WhenDefrostStateIs(DefrostState_Prechill);

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenSetpointChangesSuchThatCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHasBeenIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenCabinetThermistorIsValidIs(true);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF + 1);
   GivenAllDoorsHaveBeenClosedAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenSetpointTemperatureIs(SomeTemperatureInDegF);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenSetpointChangesSuchThatCabinetTemperatureIsLessThanLowerHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHasBeenIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValid)
{
   GivenCabinetThermistorIsValidIs(true);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF + 1);
   GivenAllDoorsHaveBeenClosedAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenSetpointTemperatureIs(SomeTemperatureInDegF);
   FaultShouldBe(SET);

   WhenSetpointTemperatureIs(SomeTemperatureInDegF + (TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 - TemperatureLowerHysteresisLimitAboveSetpointInDegFX100) / 100 + 1);
   FaultShouldBe(CLEAR);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenCabinetThermistorBecomesValidAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHasBeenIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValidWithMinimumDoorTimeEqualToMinimumDefrostTime)
{
   GivenCabinetThermistorIsValidIs(false);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   GivenAllDoorsHaveBeenClosedAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenCabinetThermistorIsValidIs(true);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldSetFaultWhenCabinetThermistorBecomesValidAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHasBeenIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValidWithMinimumDoorTimeGreaterThanMinimumDefrostTime)
{
   GivenCabinetThermistorIsValidIs(false);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   GivenAllDoorsHaveBeenClosedAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeGreaterThanMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenCabinetThermistorIsValidIs(true);
   FaultShouldBe(SET);
}

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultWhenDefrostHasBeenInIdleForMinimumTimeAndThenAllDoorsHaveBeenClosedForMinimumTimeAndItsBeenMinimumTimeSincePowerOnAndCabinetTemperatureIsEqualToUpperHysteresisLimitAboveSetpointPlusSetpointAndCabinetThermistorIsValid)
{
   GivenCabinetFilteredTemperatureIsValidAndEqualToSetpointTemperaturePlusUpperHysteresisLimitAboveSetpoint();
   GivenTheLeftSideFreshFoodDoorIs(CLOSED);
   GivenTheRightSideFreshFoodDoorIs(OPEN);
   GivenDefrostStateIs(DefrostState_Idle);
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

TEST(CabinetTemperatureExceededMonitor, ShouldNotSetFaultWhenCabinetThermistorIsInvalidAndCabinetTemperatureIsGreaterThanUpperHysteresisLimitAboveSetpointPlusSetpointAndAllDoorsHaveBeenClosedAndDefrostHasBeenIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnAndCabinetThermistorIsValidWithMinimumDoorTimeEqualToMinimumDefrostTime)
{
   GivenCabinetThermistorIsValidIs(true);
   GivenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100);
   GivenSetpointTemperatureIs(SomeTemperatureInDegF);
   GivenAllDoorsHaveBeenClosedAndDefrostHasBeenInIdleForMinimumTimeAndItHasBeenMinimumTimeSincePowerOnWithMinimumDoorTimeEqualToMinimumDefrostTime();
   FaultShouldBe(CLEAR);

   WhenCabinetThermistorIsValidIs(false);
   FaultShouldBe(CLEAR);

   WhenCabinetFilteredTemperatureIs(SomeTemperatureInDegFX100 + TemperatureUpperHysteresisLimitAboveSetpointInDegFX100 + 1);
   FaultShouldBe(CLEAR);
}
