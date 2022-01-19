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

   void WriteToEepromErd(uint32_t data)
   {
      DataModel_Write(dataModel, Erd_SomeData, &data);
   }

   void ReadEepromErd(uint32_t expectedData)
   {
      uint32_t actualData;
      DataModel_Read(dataModel, Erd_SomeData, &actualData);

      CHECK_EQUAL(expectedData, actualData);
   }
};

TEST(Test, ShouldDoTheNeedful)
{
   WriteToEepromErd(12345);
   ReadEepromErd(12345);

   WriteToEepromErd(26);
   ReadEepromErd(26);
}
