/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"
#include "ReferDataModel_TestDouble.h"

TEST_GROUP(Test)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   template <typename T>
   void ErdIs(Erd_t erd, T value)
   {
      DataModel_Write(dataModel, erd, &value);
   }

   template <typename T>
   void ErdShouldBe(Erd_t erd, T expected)
   {
      T actual;
      DataModel_Read(dataModel, erd, &actual);
      CHECK_EQUAL(expected, actual);
   }
};

TEST(Test, ShouldWriteToErd)
{
   ErdIs(Erd_OtherLed, true);
   ErdShouldBe(Erd_OtherLed, true);

   ErdIs(Erd_OtherLed, false);
   ErdShouldBe(Erd_OtherLed, false);
}

TEST(Test, ShouldWriteToAnotherErd)
{
   ErdIs(Erd_SomeData, 1234);
   ErdShouldBe(Erd_SomeData, 1234);

   ErdIs(Erd_SomeData, 26);
   ErdShouldBe(Erd_SomeData, 26);
}
