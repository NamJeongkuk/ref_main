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
#include "Rx2xxResetSource.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

enum
{
   CLOSED,
   OPEN
};

enum
{
   UserRequestedPercentage = 50
};

static const Erd_t bspLightsErdList[] = {
   Erd_FreshFoodBackWallLight_RampingPwm,
   Erd_FreshFoodTopLight_RampingPwm,
   Erd_FreezerBackWallLight_RampingPwm,
   Erd_FreezerTopLight_RampingPwm
};

enum
{
   SabbathGpioDelayInMilliseconds = 100, // Electron travel time
   WaitForEepromWritesToCompleteTimeInMsec = 1000
};

TEST_GROUP(SabbathIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   EventSubscription_t freshFoodTopLightOnChangeSubscription;
   EventSubscription_t freshFoodBackWallLightOnChangeSubscription;
   const SabbathData_t *sabbathData;
   const LightingData_t *lightingData;
   CompressorTimes_t compressorTimes;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleSpeedCompressorGridIntegration);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      sabbathData = PersonalityParametricData_Get(dataModel)->sabbathData;
      lightingData = PersonalityParametricData_Get(dataModel)->lightingData;
      compressorTimes = PersonalityParametricData_Get(dataModel)->compressorData->compressorTimes;
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void WhenApplicationIsInitialized()
   {
      GivenApplicationHasBeenInitialized();
   }

   void WhenRefrigeratorResets()
   {
      After(WaitForEepromWritesToCompleteTimeInMsec);
      ReferDataModel_TestDouble_Reset(&dataModelDouble);
      WhenApplicationIsInitialized();
   }

   void GivenSabbathModeIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SabbathModeEnable, &state);
   }

   void WhenSabbathModeIs(bool state)
   {
      GivenSabbathModeIs(state);
   }

   void SabbathModeShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_SabbathModeEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenDimmableLight0PercentLevelStatusIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, Erd_DimmableLight0PercentLevelStatus, &percentageDutyCycle);
   }

   static void FreshFoodBackWallLightRampingPwmChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const RampingPwmDutyCycle_t *pwmDutyCycle = (const RampingPwmDutyCycle_t *)_args;

      mock().actualCall("FreshFoodBackWallLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle->pwmDutyCycle);
   }

   void GivenFreshFoodBackWallLightRampingPwmDutyCycleSubscriptionIsInitialized()
   {
      EventSubscription_Init(
         &freshFoodBackWallLightOnChangeSubscription,
         NULL,
         FreshFoodBackWallLightRampingPwmChanged);

      DataModel_Subscribe(
         dataModel,
         Erd_FreshFoodBackWallLight_RampingPwm,
         &freshFoodBackWallLightOnChangeSubscription);
   }

   void TheFreshFoodBackWallLightRampingPwmDutyCycleShouldChangeTo(PwmDutyCycle_t pwmDutyCycle)
   {
      mock().expectOneCall("FreshFoodBackWallLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle);
   }

   static void FreshFoodTopLightRampingPwmChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const RampingPwmDutyCycle_t *pwmDutyCycle = (const RampingPwmDutyCycle_t *)_args;

      mock().actualCall("FreshFoodTopLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle->pwmDutyCycle);
   }

   void GivenFreshFoodTopLightRampingPwmDutyCycleSubscriptionIsInitialized()
   {
      EventSubscription_Init(
         &freshFoodTopLightOnChangeSubscription,
         NULL,
         FreshFoodTopLightRampingPwmChanged);

      DataModel_Subscribe(
         dataModel,
         Erd_FreshFoodTopLight_RampingPwm,
         &freshFoodTopLightOnChangeSubscription);
   }

   void TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo(PwmDutyCycle_t pwmDutyCycle)
   {
      mock().expectOneCall("FreshFoodTopLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle);
   }

   void WhenTheRightSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatusResolved, &state);
   }

   void TheFreshFoodTopLightRampingPwmShouldBe(RampingPwmDutyCycle_t expectedRampingPwmDutyCycle)
   {
      RampingPwmDutyCycle_t actualRampingPwmDutyCycle;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodTopLight_RampingPwm,
         &actualRampingPwmDutyCycle);

      CHECK_EQUAL(expectedRampingPwmDutyCycle.pwmDutyCycle, actualRampingPwmDutyCycle.pwmDutyCycle);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingUpCountPerMsec, actualRampingPwmDutyCycle.rampingUpCountPerMsec);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingDownCountPerMsec, actualRampingPwmDutyCycle.rampingDownCountPerMsec);
   }

   void TheFreshFoodBackWallLightRampingPwmShouldBe(RampingPwmDutyCycle_t expectedRampingPwmDutyCycle)
   {
      RampingPwmDutyCycle_t actualRampingPwmDutyCycle;
      DataModel_Read(
         dataModel,
         Erd_FreshFoodBackWallLight_RampingPwm,
         &actualRampingPwmDutyCycle);

      CHECK_EQUAL(expectedRampingPwmDutyCycle.pwmDutyCycle, actualRampingPwmDutyCycle.pwmDutyCycle);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingUpCountPerMsec, actualRampingPwmDutyCycle.rampingUpCountPerMsec);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingDownCountPerMsec, actualRampingPwmDutyCycle.rampingDownCountPerMsec);
   }

   void GivenFreshFoodLightsAreTurnedOnWhenAFreshFoodDoorIsOpened()
   {
      GivenDimmableLight0PercentLevelStatusIs(UserRequestedPercentage);
      WhenTheRightSideFreshFoodDoorIs(OPEN);

      TheFreshFoodTopLightRampingPwmShouldBe({
         .pwmDutyCycle = (uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000),
         .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
         .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec,
      });
      TheFreshFoodBackWallLightRampingPwmShouldBe({
         .pwmDutyCycle = (uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000),
         .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
         .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec,
      });
   }

   void GivenFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatus, &state);
   }

   void FreshFoodDoorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_RightSideFreshFoodDoorStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatus, &state);
   }

   void FreezerDoorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_LeftSideFreezerDoorStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodDoorStatusResolvedShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_RightSideFreshFoodDoorStatusResolved, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodDoorSabbathOverrideRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_RightSideFreshFoodDoorStatus_SabbathOverrideRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodDoorSabbathOverrideValueShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_RightSideFreshFoodDoorStatus_SabbathOverrideValue, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreezerDoorStatusResolvedShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_LeftSideFreezerDoorStatusResolved, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenCompressorFactoryVoteIs(bool state)
   {
      CompressorVotedSpeed_t votedSpeed;
      votedSpeed.speed = state;
      votedSpeed.care = Vote_Care;
      DataModel_Write(dataModel, Erd_CompressorSpeed_FactoryVote, &votedSpeed);
   }

   void FreezerEvapFanShouldBeOn()
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFan_Pwm, &actual);

      CHECK_TRUE(actual > 0);
   }

   void FreezerEvapFanShouldBeOff()
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFan_Pwm, &actual);

      CHECK_TRUE(actual == 0);
   }

   void CondenserFanShouldBeOn()
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_CondenserFan_Pwm, &actual);

      CHECK_TRUE(actual > 0);
   }

   void CondenserFanShouldBeOff()
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_CondenserFan_Pwm, &actual);

      CHECK_TRUE(actual == 0);
   }

   void CompressorStateShouldBe(CompressorState_t expected)
   {
      CompressorState_t actual;
      DataModel_Read(dataModel, Erd_CompressorState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CompressorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorRelay, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenCoolingOffRequestIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CoolingOffRequest, &state);
   }

   void CoolingOffStatusShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CoolingOffStatus, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(SabbathIntegration, ShouldEnterSabbath)
{
   GivenSabbathModeIs(DISABLED);
   GivenApplicationHasBeenInitialized();

   WhenSabbathModeIs(ENABLED);
   SabbathModeShouldBe(ENABLED);
}

TEST(SabbathIntegration, ShouldExitSabbath)
{
   GivenSabbathModeIs(DISABLED);
   GivenApplicationHasBeenInitialized();

   WhenSabbathModeIs(ENABLED);
   SabbathModeShouldBe(ENABLED);

   WhenSabbathModeIs(DISABLED);
   SabbathModeShouldBe(DISABLED);
}

TEST(SabbathIntegration, ShouldStaySetThroughAPowerCycle)
{
   GivenSabbathModeIs(DISABLED);
   GivenApplicationHasBeenInitialized();

   WhenSabbathModeIs(ENABLED);
   SabbathModeShouldBe(ENABLED);

   WhenRefrigeratorResets();
   SabbathModeShouldBe(ENABLED);
}

TEST(SabbathIntegration, ShouldTurnLightsOffWhenEnteredAndADoorIsOpen)
{
   GivenApplicationHasBeenInitialized();
   GivenFreshFoodLightsAreTurnedOnWhenAFreshFoodDoorIsOpened();
   GivenFreshFoodTopLightRampingPwmDutyCycleSubscriptionIsInitialized();
   GivenFreshFoodBackWallLightRampingPwmDutyCycleSubscriptionIsInitialized();

   TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo(0);
   TheFreshFoodBackWallLightRampingPwmDutyCycleShouldChangeTo(0);
   WhenSabbathModeIs(ENABLED);

   TheFreshFoodBackWallLightRampingPwmShouldBe(
      {
         .pwmDutyCycle = 0,
         .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
         .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec,
      });
   TheFreshFoodTopLightRampingPwmShouldBe(
      {
         .pwmDutyCycle = 0,
         .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
         .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec,
      });
}

TEST(SabbathIntegration, ShouldReportDoorsAsClosedWhenSabbathIsActiveAndReturnToActualDoorStateWhenInactive)
{
   GivenSabbathModeIs(DISABLED);
   GivenFreshFoodDoorIs(OPEN);
   GivenFreezerDoorIs(OPEN);
   GivenApplicationHasBeenInitialized();

   FreshFoodDoorStatusResolvedShouldBe(OPEN);
   FreezerDoorStatusResolvedShouldBe(OPEN);

   WhenSabbathModeIs(ENABLED);
   FreshFoodDoorStatusResolvedShouldBe(CLOSED);
   FreezerDoorStatusResolvedShouldBe(CLOSED);

   WhenSabbathModeIs(DISABLED);

   After(SabbathGpioDelayInMilliseconds);
   FreshFoodDoorShouldBe(OPEN);
   FreshFoodDoorStatusResolvedShouldBe(OPEN);
   FreezerDoorShouldBe(OPEN);
   FreezerDoorStatusResolvedShouldBe(OPEN);
}

TEST(SabbathIntegration, ShouldWaitToTurnOnFans)
{
   GivenSabbathModeIs(DISABLED);
   GivenApplicationHasBeenInitialized();

   WhenCompressorFactoryVoteIs(OFF);
   WhenSabbathModeIs(ENABLED);
   WhenCompressorFactoryVoteIs(ON);

   CompressorShouldBe(OFF);
   CompressorStateShouldBe(CompressorState_SabbathDelay);

   After(compressorTimes.sabbathDelayTimeInSeconds * MSEC_PER_SEC - 1);
   CompressorStateShouldBe(CompressorState_SabbathDelay);
   FreezerEvapFanShouldBeOff();
   CondenserFanShouldBeOff();

   After(1);
   CompressorStateShouldBe(CompressorState_Startup);
   FreezerEvapFanShouldBeOn();
   CondenserFanShouldBeOn();
}

TEST(SabbathIntegration, ShouldLeaveCoolingModeOffWhenSabbathIsActive)
{
   GivenSabbathModeIs(DISABLED);
   GivenApplicationHasBeenInitialized();

   WhenCoolingOffRequestIs(true);
   CoolingOffStatusShouldBe(true);

   WhenSabbathModeIs(ENABLED);
   CoolingOffStatusShouldBe(true);
}
