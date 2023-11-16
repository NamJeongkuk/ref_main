/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ParametricData.h"
#include "ParametricDataVersionCheck.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"
#include "ParametricData_TestDouble.h"
#include "Action_Mock.h"

enum
{
   Critical_ValidMajorVersion = 0,
   Critical_ValidMinorVersion = 18,
   Critical_InvalidMajorVersion = Critical_ValidMajorVersion + 1,
   Critical_InvalidMinorVersion = Critical_ValidMinorVersion + 1,
};

TEST_GROUP(ParametricDataVersionCheck)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   Action_Mock_t actionMock;
   I_Action_t *action;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      Action_Mock_Init(&actionMock);
      action = &actionMock.interface;

      PersonalityParametricData_TestDouble_Init();
   }

   void Init(void)
   {
      ParametricDataVersionCheck_Init(action);
   }

   void ActionShouldBeInvoked()
   {
      mock().expectOneCall("Invoke").onObject(action);
   }

   void ActionShouldNotBeInvoked()
   {
      mock().expectNCalls(0, "Invoke").onObject(action);
   }

   void WhenInitializedWithCriticalMajorVersion(uint8_t version)
   {
      SetImageHeaderMajorVersion(version);
      SetImageHeaderMinorVersion(Critical_ValidMinorVersion);
      Init();
   }

   void WhenInitializedWithCriticalMinorVersion(uint8_t version)
   {
      SetImageHeaderMajorVersion(Critical_ValidMajorVersion);
      SetImageHeaderMinorVersion(version);
      Init();
   }

   void WhenInitializedWithValidCriticalVersions()
   {
      SetImageHeaderMajorVersion(Critical_ValidMajorVersion);
      SetImageHeaderMinorVersion(Critical_ValidMinorVersion);
      Init();
   }
};

TEST(ParametricDataVersionCheck, ShouldInvokeActionWhenCriticalMajorVersionIsInvalid)
{
   ActionShouldBeInvoked();

   WhenInitializedWithCriticalMajorVersion(Critical_InvalidMajorVersion);
}

TEST(ParametricDataVersionCheck, ShouldInvokeActionWhenCriticalMinorVersionIsInvalid)
{
   ActionShouldBeInvoked();

   WhenInitializedWithCriticalMinorVersion(Critical_InvalidMinorVersion);
}

TEST(ParametricDataVersionCheck, ShouldNotInvokeActionWhenCriticalMajorAndMinorVersionsAreValid)
{
   ActionShouldNotBeInvoked();

   WhenInitializedWithValidCriticalVersions();
}
