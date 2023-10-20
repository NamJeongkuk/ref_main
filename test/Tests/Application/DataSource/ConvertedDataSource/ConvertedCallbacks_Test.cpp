/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ConvertedCallbacks.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"
#include "SystemErds.h"

TEST_GROUP(ConvertedCallbacks)
{
   I_DataSource_t *dataSource;
   ReferDataModel_TestDouble_t dataModelTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataSource = DataModel_AsDataSource(dataModelTestDouble.dataModel);
   }

   void GivenUnitSerialNumberIs(const uint8_t *unitSerialNumber)
   {
      DataSource_Write(dataSource, Erd_SerialNumber, unitSerialNumber);
   }

   void RfidFilterUnitSerialNumberMainboardShouldBe(const uint8_t *expected)
   {
      UnitSerialNumber_t actual;
      ErdConvertedCallBack_ReadRfidFilterUnitSerialNumber(dataSource, &actual);

      MEMCMP_EQUAL(expected, &actual, sizeof(UnitSerialNumber_t));
   }
};

TEST(ConvertedCallbacks, ShouldBeAbleToReadRfidFilterUnitSerialNumber)
{
   const uint8_t GivenUnitSerialNumber[32] = "123456789ABCDEFG";
   const uint8_t GivenUnitSerialNumberFirstEightBytes[UnitSerialNumberSizeInBytes] = { '1', '2', '3', '4', '5', '6', '7', '8' };

   GivenUnitSerialNumberIs(GivenUnitSerialNumber);
   RfidFilterUnitSerialNumberMainboardShouldBe(GivenUnitSerialNumberFirstEightBytes);
}
