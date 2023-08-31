/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "UserAllowableInteriorLightingRequestHandler.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   SomePercentageDutyCycleOver100 = 233,
   SomePercentageDutyCycle = 35,
   AnotherPercentageDutyCycle = 52
};

static const UserAllowableLightingConfiguration_t config = {
   .userAllowableInteriorLightingRequestErd = Erd_DimmableLight0PercentLevelRequest,
   .userAllowableInteriorLightingStateErd = Erd_DimmableLight0PercentLevelStatus,
   .userAllowableInteriorLightingConfigurationErd = Erd_DimmableLight0Configuration,
};

TEST_GROUP(UserAllowableInteriorLightingRequestHandler)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   UserAllowableInteriorLightingRequestHandler_t instance;
   UserAllowableInteriorLightingBitmap_t lightingBitmap = { 0 };

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      BITMAP_SET(lightingBitmap.bitmap, UserAllowableInteriorLighting_FreshFoodBackLight);
      BITMAP_SET(lightingBitmap.bitmap, UserAllowableInteriorLighting_FreshFoodTopLight);
   }

   void GivenModuleIsInitialized()
   {
      UserAllowableInteriorLightingRequestHandler_Init(
         &instance,
         dataModel,
         lightingBitmap,
         &config);
   }

   void WhenRequestIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, config.userAllowableInteriorLightingRequestErd, &percentageDutyCycle);
   }

   void GivenStatusIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      DataModel_Write(dataModel, config.userAllowableInteriorLightingStateErd, &percentageDutyCycle);
   }

   void GivenRequestIs(PercentageDutyCycle_t percentageDutyCycle)
   {
      WhenRequestIs(percentageDutyCycle);
   }

   void StatusShouldBe(PercentageDutyCycle_t expected)
   {
      PercentageDutyCycle_t actual;
      DataModel_Read(dataModel, config.userAllowableInteriorLightingStateErd, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void RequestShouldBe(PercentageDutyCycle_t expected)
   {
      PercentageDutyCycle_t actual;
      DataModel_Read(dataModel, config.userAllowableInteriorLightingRequestErd, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ConfigurationShouldBe(const UserAllowableInteriorLightingBitmap_t expected)
   {
      UserAllowableInteriorLightingBitmap_t actual;
      DataModel_Read(dataModel, config.userAllowableInteriorLightingConfigurationErd, &actual);

      MEMCMP_EQUAL(&expected, &actual, sizeof(expected));
   }
};

TEST(UserAllowableInteriorLightingRequestHandler, ShouldInitialize)
{
   GivenModuleIsInitialized();
}

TEST(UserAllowableInteriorLightingRequestHandler, ShouldSetConfigurationToUserAllowableLightingBitmapOnInit)
{
   GivenModuleIsInitialized();
   ConfigurationShouldBe(lightingBitmap);
}

TEST(UserAllowableInteriorLightingRequestHandler, ShouldNotSetStatusToRequestOnInit)
{
   GivenStatusIs(SomePercentageDutyCycle);
   GivenRequestIs(AnotherPercentageDutyCycle);
   GivenModuleIsInitialized();

   StatusShouldBe(SomePercentageDutyCycle);
}

TEST(UserAllowableInteriorLightingRequestHandler, ShouldCopyStatusToRequestOnInit)
{
   GivenStatusIs(SomePercentageDutyCycle);
   GivenRequestIs(0);
   GivenModuleIsInitialized();

   RequestShouldBe(SomePercentageDutyCycle);
}

TEST(UserAllowableInteriorLightingRequestHandler, ShouldSetStatusToRequestWhenRequestChanges)
{
   GivenRequestIs(SomePercentageDutyCycle);
   GivenModuleIsInitialized();

   WhenRequestIs(AnotherPercentageDutyCycle);
   StatusShouldBe(AnotherPercentageDutyCycle);

   WhenRequestIs(SomePercentageDutyCycle);
   StatusShouldBe(SomePercentageDutyCycle);
}

TEST(UserAllowableInteriorLightingRequestHandler, ShouldLimitStatusTo100Max)
{
   GivenRequestIs(SomePercentageDutyCycleOver100);
   GivenModuleIsInitialized();

   StatusShouldBe(100);

   WhenRequestIs(SomePercentageDutyCycle);
   StatusShouldBe(SomePercentageDutyCycle);

   WhenRequestIs(SomePercentageDutyCycleOver100);
   StatusShouldBe(100);
}
