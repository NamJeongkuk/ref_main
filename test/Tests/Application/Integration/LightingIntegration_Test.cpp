/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Closed,
   Open
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

TEST_GROUP(LightingIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   ResetReason_t resetReason;
   EventSubscription_t dataModelOnChangeSubscription;
   const LightingData_t *lightingData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      lightingData = PersonalityParametricData_Get(dataModel)->lightingData;
   }

   void GivenApplicationHasBeenInitialized()
   {
      Application_Init(&instance, dataModel, resetReason);
   }

   static void FreezerBackWallLightRampingPwmChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_FreezerBackWallLight_RampingPwm)
      {
         const RampingPwmDutyCycle_t *pwmDutyCycle = (const RampingPwmDutyCycle_t *)args->data;

         mock().actualCall("FreezerBackWallLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle->pwmDutyCycle);
      }
   }

   void GivenFreezerBackWallLightRampingPwmDutyCycleSubscriptionIsInitialized()
   {
      EventSubscription_Init(
         &dataModelOnChangeSubscription,
         NULL,
         FreezerBackWallLightRampingPwmChanged);

      DataModel_SubscribeAll(
         dataModel,
         &dataModelOnChangeSubscription);
   }

   void TheFreezerBackWallLightRampingPwmDutyCycleShouldChangeTo(PwmDutyCycle_t pwmDutyCycle)
   {
      mock().expectOneCall("FreezerBackWallLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle);
   }

   static void FreshFoodBackWallLightRampingPwmChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_FreshFoodBackWallLight_RampingPwm)
      {
         const RampingPwmDutyCycle_t *pwmDutyCycle = (const RampingPwmDutyCycle_t *)args->data;

         mock().actualCall("FreshFoodBackWallLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle->pwmDutyCycle);
      }
   }

   void GivenFreshFoodBackWallLightRampingPwmDutyCycleSubscriptionIsInitialized()
   {
      EventSubscription_Init(
         &dataModelOnChangeSubscription,
         NULL,
         FreshFoodBackWallLightRampingPwmChanged);

      DataModel_SubscribeAll(
         dataModel,
         &dataModelOnChangeSubscription);
   }

   void TheFreshFoodBackWallLightRampingPwmDutyCycleShouldChangeTo(PwmDutyCycle_t pwmDutyCycle)
   {
      mock().expectOneCall("FreshFoodBackWallLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle);
   }

   static void FreshFoodTopLightRampingPwmChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_FreshFoodTopLight_RampingPwm)
      {
         const RampingPwmDutyCycle_t *pwmDutyCycle = (const RampingPwmDutyCycle_t *)args->data;

         mock().actualCall("FreshFoodTopLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle->pwmDutyCycle);
      }
   }

   void GivenFreshFoodTopLightRampingPwmDutyCycleSubscriptionIsInitialized()
   {
      EventSubscription_Init(
         &dataModelOnChangeSubscription,
         NULL,
         FreshFoodTopLightRampingPwmChanged);

      DataModel_SubscribeAll(
         dataModel,
         &dataModelOnChangeSubscription);
   }

   void TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo(PwmDutyCycle_t pwmDutyCycle)
   {
      mock().expectOneCall("FreshFoodTopLightRampingPwmChanged").withParameter("pwmDutyCycle", pwmDutyCycle);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void TheLightBspErdsShouldBe(PwmDutyCycle_t expectedPwmDutyCycle)
   {
      for(uint8_t i = 0; i < NUM_ELEMENTS(bspLightsErdList); i++)
      {
         RampingPwmDutyCycle_t actualPwmDutyCycle;
         DataModel_Read(
            dataModel,
            bspLightsErdList[i],
            &actualPwmDutyCycle);

         CHECK_EQUAL(expectedPwmDutyCycle, actualPwmDutyCycle.pwmDutyCycle);
      }
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

   void TheFreezerBackWallLightRampingPwmShouldBe(RampingPwmDutyCycle_t expectedRampingPwmDutyCycle)
   {
      RampingPwmDutyCycle_t actualRampingPwmDutyCycle;
      DataModel_Read(
         dataModel,
         Erd_FreezerBackWallLight_RampingPwm,
         &actualRampingPwmDutyCycle);

      CHECK_EQUAL(expectedRampingPwmDutyCycle.pwmDutyCycle, actualRampingPwmDutyCycle.pwmDutyCycle);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingUpCountPerMsec, actualRampingPwmDutyCycle.rampingUpCountPerMsec);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingDownCountPerMsec, actualRampingPwmDutyCycle.rampingDownCountPerMsec);
   }

   void TheFreezerTopLightRampingPwmShouldBe(RampingPwmDutyCycle_t expectedRampingPwmDutyCycle)
   {
      RampingPwmDutyCycle_t actualRampingPwmDutyCycle;
      DataModel_Read(
         dataModel,
         Erd_FreezerTopLight_RampingPwm,
         &actualRampingPwmDutyCycle);

      CHECK_EQUAL(expectedRampingPwmDutyCycle.pwmDutyCycle, actualRampingPwmDutyCycle.pwmDutyCycle);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingUpCountPerMsec, actualRampingPwmDutyCycle.rampingUpCountPerMsec);
      CHECK_EQUAL(expectedRampingPwmDutyCycle.rampingDownCountPerMsec, actualRampingPwmDutyCycle.rampingDownCountPerMsec);
   }

   void GivenDimmableLight0PercentLevelStatusIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, Erd_DimmableLight0PercentLevelStatus, &percentageDutyCycle);
   }

   void WhenDimmableLight0PercentLevelRequestIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, Erd_DimmableLight0PercentLevelRequest, &percentageDutyCycle);
   }

   void GivenDimmableLight1PercentLevelStatusIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, Erd_DimmableLight1PercentLevelStatus, &percentageDutyCycle);
   }

   void WhenDimmableLight1PercentLevelRequestIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, Erd_DimmableLight1PercentLevelRequest, &percentageDutyCycle);
   }

   void GivenDimmableLight2PercentLevelStatusIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, Erd_DimmableLight2PercentLevelStatus, &percentageDutyCycle);
   }

   void WhenDimmableLight2PercentLevelRequestIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, Erd_DimmableLight2PercentLevelRequest, &percentageDutyCycle);
   }

   void WhenTheLeftSideFreezerDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_LeftSideFreezerDoorStatusResolved, &state);
   }

   void GivenTheLeftSideFreezerDoorIs(bool state)
   {
      WhenTheLeftSideFreezerDoorIs(state);
   }

   void WhenTheRightSideFreshFoodDoorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_RightSideFreshFoodDoorStatusResolved, &state);
   }

   void GivenTheRightSideFreshFoodDoorIs(bool state)
   {
      WhenTheRightSideFreshFoodDoorIs(state);
   }

   void NothingShouldHappen()
   {
   }
};

TEST(LightingIntegration, ShouldRampFreshFoodBackWallLightOnWhenRightSideFreshFoodDoorIsOpenedForDimmableLight0ConfigAndDoNothingWhenLeftSideFreezerDoorIsOpenedBasedOnDimmableLight0ConfigAndStatus)
{
   GivenDimmableLight0PercentLevelStatusIs(UserRequestedPercentage);
   GivenApplicationHasBeenInitialized();
   GivenFreshFoodBackWallLightRampingPwmDutyCycleSubscriptionIsInitialized();

   TheFreshFoodBackWallLightRampingPwmDutyCycleShouldChangeTo((uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.minPwmDutyCyclePercentage * UINT16_MAX) / 100));
   TheFreshFoodBackWallLightRampingPwmDutyCycleShouldChangeTo((uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000));
   WhenTheRightSideFreshFoodDoorIs(Open);

   TheFreshFoodBackWallLightRampingPwmShouldBe({ .pwmDutyCycle = (uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000),
      .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
      .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec });

   NothingShouldHappen();
   WhenTheLeftSideFreezerDoorIs(Open);
}

TEST(LightingIntegration, ShouldRampFreshFoodTopLightOnWhenRightSideFreshFoodDoorIsOpenedForDimmableLight0ConfigAndDoNothingWhenLeftSideFreezerDoorIsOpenedBasedOnDimmableLight0ConfigAndStatus)
{
   GivenDimmableLight0PercentLevelStatusIs(UserRequestedPercentage);
   GivenApplicationHasBeenInitialized();
   GivenFreshFoodTopLightRampingPwmDutyCycleSubscriptionIsInitialized();

   TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo((uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.minPwmDutyCyclePercentage * UINT16_MAX) / 100));
   TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo((uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000));
   WhenTheRightSideFreshFoodDoorIs(Open);

   TheFreshFoodTopLightRampingPwmShouldBe({ .pwmDutyCycle = (uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000),
      .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
      .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec });

   NothingShouldHappen();
   WhenTheLeftSideFreezerDoorIs(Open);
}

TEST(LightingIntegration, ShouldRampFreshFoodTopLightOffWhenRightSideFreshFoodDoorIsClosedBasedOnDimmableLight0ConfigAndStatus)
{
   GivenDimmableLight0PercentLevelStatusIs(UserRequestedPercentage);
   GivenApplicationHasBeenInitialized();
   GivenFreshFoodTopLightRampingPwmDutyCycleSubscriptionIsInitialized();

   TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo((uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.minPwmDutyCyclePercentage * UINT16_MAX) / 100));
   TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo((uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000));
   WhenTheRightSideFreshFoodDoorIs(Open);

   TheFreshFoodTopLightRampingPwmShouldBe({ .pwmDutyCycle = (uint16_t)((uint32_t)(lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage * UserRequestedPercentage * UINT16_MAX) / 10000),
      .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
      .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec });

   TheFreshFoodTopLightRampingPwmDutyCycleShouldChangeTo(PercentageDutyCycle_Min);
   WhenTheRightSideFreshFoodDoorIs(Closed);

   TheFreshFoodTopLightRampingPwmShouldBe({ .pwmDutyCycle = PercentageDutyCycle_Min,
      .rampingUpCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingUpCountInMsec,
      .rampingDownCountPerMsec = lightingData->doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmRampingDownCountInMsec });
}
