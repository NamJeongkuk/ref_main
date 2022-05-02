/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "EepromEraser.h"
#include "Signal.h"
}
#include "Eeprom_Mock.h"
#include "DataModel_TestDouble.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "uassert_test.h"

#define Given
#define And
#define When
#define Expect

enum
{
   Erd_EepromEraseRequestSignal
};

static const DataModel_TestDoubleConfigurationEntry_t dataModelConfig[] = {
   { Erd_EepromEraseRequestSignal, sizeof(Signal_t) }
};

TEST_GROUP(EepromEraser)
{
   EepromEraser_t instance;

   DataModel_TestDouble_t dataModelTestDouble;
   Eeprom_Mock_t eepromMock;

   I_DataModel_t *dataModel;
   I_Eeprom_t *eeprom;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, dataModelConfig, NUM_ELEMENTS(dataModelConfig));
      dataModel = dataModelTestDouble.dataModel;

      Eeprom_Mock_Init(&eepromMock);
      eeprom = &eepromMock.interface;
   }

   void ModuleInitialized()
   {
      EepromEraser_Init(
         &instance,
         dataModel,
         eeprom,
         Erd_EepromEraseRequestSignal);
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

   void EepromEraseRequstSignalIs(Signal_t value)
   {
      ErdIs(Erd_EepromEraseRequestSignal, (Signal_t)value);
   }

   void EepromEraseCalled()
   {
      mock().expectOneCall("Erase").onObject(eeprom);
   }
};

TEST(EepromEraser, ShouldInitialize)
{
   Given ModuleInitialized();
}

TEST(EepromEraser, ShouldEraseEepromWhenRequestedErase)
{
   Given EepromEraseRequstSignalIs(0);
   And ModuleInitialized();

   Expect EepromEraseCalled();
   When EepromEraseRequstSignalIs(1);
}
