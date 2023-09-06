/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "UserAllowableInteriorLightingResolver.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

static UserAllowableLightConfig_t userAllowableLightConfigs[] = {
   {
      .statusErd = Erd_DimmableLight0PercentLevelStatus,
      .configErd = Erd_DimmableLight0Configuration,
   },
   {
      .statusErd = Erd_DimmableLight1PercentLevelStatus,
      .configErd = Erd_DimmableLight1Configuration,
   },
   {
      .statusErd = Erd_DimmableLight2PercentLevelStatus,
      .configErd = Erd_DimmableLight2Configuration,
   }
};

static UserAllowableInteriorLightingResolverConfiguration_t config = {
   .lights = {
      .freshFoodBackLightUserAllowableStateErd = Erd_FreshFoodBackLightUserAllowableState,
      .freshFoodTopLightUserAllowableStateErd = Erd_FreshFoodTopLightUserAllowableState,
      .freezerBackLightUserAllowableStateErd = Erd_FreezerBackLightUserAllowableState,
      .freezerTopLightUserAllowableStateErd = Erd_FreezerTopLightUserAllowableState,
   },
   .userAllowableLightConfigs = userAllowableLightConfigs,
   .numberOfUserAllowableLightConfigs = NUM_ELEMENTS(userAllowableLightConfigs),
};

static const DoorLightingData_t doorLightingData = {
   .normalOperationRampingPwmDutyCycle = {
      .maxPwmDutyCyclePercentage = 100,
      .minPwmDutyCyclePercentage = 20,
      .maxPwmRampingUpCountInMsec = 65,
      .maxPwmRampingDownCountInMsec = 65,
   }
};

static UserAllowableInteriorLightingData_t userAllowableInteriorLightingData = {
   .userAllowable0InteriorLightingBitmap = { 0 },
   .userAllowable1InteriorLightingBitmap = { 0 },
   .userAllowable2InteriorLightingBitmap = { 0 }
};

TEST_GROUP(UserAllowableInteriorLightingResolver)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   UserAllowableInteriorLightingResolver_t instance;
   LightingData_t lightingData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      memset(&lightingData, 0, sizeof(LightingData_t));
      lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData = &doorLightingData;
      lightingData.doorLightingDataSet.freshFoodTopAndSideDoorLightingData = &doorLightingData;
      lightingData.doorLightingDataSet.freezerBackWallDoorLightingData = &doorLightingData;
      lightingData.doorLightingDataSet.freezerTopAndSideDoorLightingData = &doorLightingData;

      BITMAP_SET(userAllowableInteriorLightingData.userAllowable0InteriorLightingBitmap.bitmap, UserAllowableInteriorLighting_FreshFoodBackLight);
      BITMAP_SET(userAllowableInteriorLightingData.userAllowable0InteriorLightingBitmap.bitmap, UserAllowableInteriorLighting_FreshFoodTopLight);
      BITMAP_SET(userAllowableInteriorLightingData.userAllowable1InteriorLightingBitmap.bitmap, UserAllowableInteriorLighting_FreezerBackLight);
      BITMAP_SET(userAllowableInteriorLightingData.userAllowable2InteriorLightingBitmap.bitmap, UserAllowableInteriorLighting_FreezerTopLight);
      lightingData.userAllowableInteriorLightingData = &userAllowableInteriorLightingData;
   }

   void GivenModuleIsInitialized()
   {
      UserAllowableInteriorLightingResolver_Init(
         &instance,
         dataModel,
         &lightingData,
         &config);
   }

   void GivenConfigurationIs(Erd_t configErd, UserAllowableInteriorLightingBitmap_t bitmap)
   {
      DataModel_Write(dataModel, configErd, &bitmap);
   }

   void GivenStatusIs(Erd_t statusErd, PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, statusErd, &percentageDutyCycle);
   }

   void WhenStatusIs(Erd_t statusErd, PercentageDutyCycle_t percentageDutyCycle)
   {
      GivenStatusIs(statusErd, percentageDutyCycle);
   }

   void LightUserAllowableStateShouldBe(Erd_t lightStateErd, bool expectedUserAllowable, PercentageDutyCycle_t expectedPercentageDutyCycle)
   {
      UserAllowableInteriorLightState_t actualLightState;
      DataModel_Read(dataModel, lightStateErd, &actualLightState);

      CHECK_EQUAL(expectedUserAllowable, actualLightState.userAllowable);
      CHECK_EQUAL(expectedPercentageDutyCycle, actualLightState.percentageDutyCycle);
   }
};

TEST(UserAllowableInteriorLightingResolver, ShouldHaveFreshFoodBackWallDoorLightingDataFirstAndFollowTheBelowOrder)
{
   CHECK_EQUAL(0, offsetof(DoorLightingDataSet_t, freshFoodBackWallDoorLightingData));
   CHECK_EQUAL(8, offsetof(DoorLightingDataSet_t, freshFoodTopAndSideDoorLightingData));
   CHECK_EQUAL(16, offsetof(DoorLightingDataSet_t, freezerBackWallDoorLightingData));
   CHECK_EQUAL(24, offsetof(DoorLightingDataSet_t, freezerTopAndSideDoorLightingData));
}

TEST(UserAllowableInteriorLightingResolver, ShouldBeAbleToIndexLightStateErdsWithBitmapIndexAndStructFieldName)
{
   CHECK_EQUAL(((const Erd_t *)&config.lights)[UserAllowableInteriorLighting_FreshFoodBackLight], config.lights.freshFoodBackLightUserAllowableStateErd);
   CHECK_EQUAL(((const Erd_t *)&config.lights)[UserAllowableInteriorLighting_FreshFoodTopLight], config.lights.freshFoodTopLightUserAllowableStateErd);
   CHECK_EQUAL(((const Erd_t *)&config.lights)[UserAllowableInteriorLighting_FreezerBackLight], config.lights.freezerBackLightUserAllowableStateErd);
   CHECK_EQUAL(((const Erd_t *)&config.lights)[UserAllowableInteriorLighting_FreezerTopLight], config.lights.freezerTopLightUserAllowableStateErd);
}

TEST(UserAllowableInteriorLightingResolver, ShouldBeAbleToAccessDoorLightDataViaOffsetOrViaName)
{
   POINTERS_EQUAL((&lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData + UserAllowableInteriorLighting_FreshFoodBackLight), &lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData);
   POINTERS_EQUAL((&lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData + UserAllowableInteriorLighting_FreshFoodTopLight), &lightingData.doorLightingDataSet.freshFoodTopAndSideDoorLightingData);
   POINTERS_EQUAL((&lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData + UserAllowableInteriorLighting_FreezerBackLight), &lightingData.doorLightingDataSet.freezerBackWallDoorLightingData);
   POINTERS_EQUAL((&lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData + UserAllowableInteriorLighting_FreezerTopLight), &lightingData.doorLightingDataSet.freezerTopAndSideDoorLightingData);
}

TEST(UserAllowableInteriorLightingResolver, ShouldSetUserAllowableStateForEachLightOnInitBasedOnStatusAndConfig)
{
   GivenConfigurationIs(Erd_DimmableLight0Configuration, lightingData.userAllowableInteriorLightingData->userAllowable0InteriorLightingBitmap);
   GivenConfigurationIs(Erd_DimmableLight1Configuration, lightingData.userAllowableInteriorLightingData->userAllowable1InteriorLightingBitmap);
   GivenConfigurationIs(Erd_DimmableLight2Configuration, lightingData.userAllowableInteriorLightingData->userAllowable2InteriorLightingBitmap);

   GivenStatusIs(Erd_DimmableLight0PercentLevelStatus, 50);
   GivenStatusIs(Erd_DimmableLight1PercentLevelStatus, 30);
   GivenStatusIs(Erd_DimmableLight2PercentLevelStatus, 20);

   GivenModuleIsInitialized();

   LightUserAllowableStateShouldBe(
      Erd_FreshFoodBackLightUserAllowableState,
      true,
      (50 * lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreshFoodTopLightUserAllowableState,
      true,
      (50 * lightingData.doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerBackLightUserAllowableState,
      true,
      (30 * lightingData.doorLightingDataSet.freezerBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerTopLightUserAllowableState,
      true,
      (20 * lightingData.doorLightingDataSet.freezerTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
}

TEST(UserAllowableInteriorLightingResolver, ShouldUpdateUserAllowableStateForEachLightWhenItsConfigsStatusChanges)
{
   GivenConfigurationIs(Erd_DimmableLight0Configuration, lightingData.userAllowableInteriorLightingData->userAllowable0InteriorLightingBitmap);
   GivenConfigurationIs(Erd_DimmableLight1Configuration, lightingData.userAllowableInteriorLightingData->userAllowable1InteriorLightingBitmap);
   GivenConfigurationIs(Erd_DimmableLight2Configuration, lightingData.userAllowableInteriorLightingData->userAllowable2InteriorLightingBitmap);

   GivenStatusIs(Erd_DimmableLight0PercentLevelStatus, 50);
   GivenStatusIs(Erd_DimmableLight1PercentLevelStatus, 30);
   GivenStatusIs(Erd_DimmableLight2PercentLevelStatus, 20);

   GivenModuleIsInitialized();

   LightUserAllowableStateShouldBe(
      Erd_FreshFoodBackLightUserAllowableState,
      true,
      (50 * lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreshFoodTopLightUserAllowableState,
      true,
      (50 * lightingData.doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerBackLightUserAllowableState,
      true,
      (30 * lightingData.doorLightingDataSet.freezerBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerTopLightUserAllowableState,
      true,
      (20 * lightingData.doorLightingDataSet.freezerTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);

   WhenStatusIs(Erd_DimmableLight0PercentLevelStatus, 24);

   LightUserAllowableStateShouldBe(
      Erd_FreshFoodBackLightUserAllowableState,
      true,
      (24 * lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreshFoodTopLightUserAllowableState,
      true,
      (24 * lightingData.doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerBackLightUserAllowableState,
      true,
      (30 * lightingData.doorLightingDataSet.freezerBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerTopLightUserAllowableState,
      true,
      (20 * lightingData.doorLightingDataSet.freezerTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);

   WhenStatusIs(Erd_DimmableLight1PercentLevelStatus, 36);

   LightUserAllowableStateShouldBe(
      Erd_FreshFoodBackLightUserAllowableState,
      true,
      (24 * lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreshFoodTopLightUserAllowableState,
      true,
      (24 * lightingData.doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerBackLightUserAllowableState,
      true,
      (36 * lightingData.doorLightingDataSet.freezerBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerTopLightUserAllowableState,
      true,
      (20 * lightingData.doorLightingDataSet.freezerTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);

   WhenStatusIs(Erd_DimmableLight2PercentLevelStatus, 54);

   LightUserAllowableStateShouldBe(
      Erd_FreshFoodBackLightUserAllowableState,
      true,
      (24 * lightingData.doorLightingDataSet.freshFoodBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreshFoodTopLightUserAllowableState,
      true,
      (24 * lightingData.doorLightingDataSet.freshFoodTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerBackLightUserAllowableState,
      true,
      (36 * lightingData.doorLightingDataSet.freezerBackWallDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
   LightUserAllowableStateShouldBe(
      Erd_FreezerTopLightUserAllowableState,
      true,
      (54 * lightingData.doorLightingDataSet.freezerTopAndSideDoorLightingData->normalOperationRampingPwmDutyCycle.maxPwmDutyCyclePercentage) / 100);
}
