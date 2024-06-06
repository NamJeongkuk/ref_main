/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Fsm.h"
#include "FoxFsm.h"
#include "utils.h"
};

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "GpioGroup_TestDouble.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define After
#define Then

FoxFsmConfig_t foxtestconfigfortdd = {
   .clockwise = Erd_Clockwise,
   .cclockwise = Erd_CClockwise,
   .printstate = Erd_TestCurState
};

TEST_GROUP(TestingFsmGroup)
{
   FoxFsmPlugin_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);

      FoxFSM_Init(
         &instance->foxfsm,
         dataModelDouble.dataModel);
   }

   // void GivenInitialization()
   // {
   // }
};

TEST(TestingFsmGroup, Test1)
{ /*
 A to B using clockwise
 B to C using clockwise
 C to A using clockwise
 A to B using clockwise
 B to C using clockwise
 C to A using clockwise
 */
}

TEST(TestingFsmGroup, Test2)
{ /*
 A to C using clockwise
 C to B using clockwise
 B to A using clockwise
 A to C using clockwise
 C to B using clockwise
 B to A using clockwise
 */
}
