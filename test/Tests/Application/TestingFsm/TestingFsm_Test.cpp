/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Fsm.h"
#include "TestingFsm.h"
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

TestingFsmConfig_t namtestconfigfortdd = {
   .clockwise = Erd_Clockwise,
   .cclockwise = Erd_CClockwise,
   .printstate = Erd_TestCurState
};

// TEST_GROUP(TestingFsmGroup)
// {
//    void setup()
//    {
//    }

//    void GivenInitialization()
//    {
//    }
// }

// TEST(TestingFsmGroup, Test1)
// {
// }
