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
#include "DataModelErdPointerAccess.h"
}
#include "Eeprom_TestDouble.h"
#include "Action_Mock.h"
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
   Erd_EepromEraseRequestSignal,
   Erd_SystemResetAction
};

static const DataModel_TestDoubleConfigurationEntry_t dataModelConfig[] = {
   { Erd_EepromEraseRequestSignal, sizeof(Signal_t) },
   { Erd_SystemResetAction, sizeof(I_Action_t *) }
};

TEST_GROUP(EepromEraser)
{
   EepromEraser_t instance;

   DataModel_TestDouble_t dataModelTestDouble;
   Eeprom_TestDouble_t eepromTestDouble;
   Action_Mock_t resetActionMock;

   I_DataModel_t *dataModel;
   I_Eeprom_t *eeprom;
   I_Action_t *resetAction;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, dataModelConfig, NUM_ELEMENTS(dataModelConfig));
      dataModel = dataModelTestDouble.dataModel;

      Eeprom_TestDouble_Init(&eepromTestDouble);
      eeprom = &eepromTestDouble.interface;

      Action_Mock_Init(&resetActionMock);
      resetAction = &resetActionMock.interface;
   }

   void ModuleInitialized()
   {
      DataModelErdPointerAccess_Write(dataModel, Erd_SystemResetAction, resetAction);

      EepromEraser_Init(
         &instance,
         dataModel,
         eeprom,
         Erd_EepromEraseRequestSignal,
         Erd_SystemResetAction);
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

   void EepromErasedEventOccurs()
   {
      Eeprom_TestDouble_TriggerReceiveErasedEvent(&eepromTestDouble);
   }

   void ResetActionToBeInvoked()
   {
      mock().expectOneCall("Invoke").onObject(resetAction);
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

TEST(EepromEraser, ShouldInvokeResetActionWhenEepromErasedEventOccurs)
{
   Given ModuleInitialized();

   Expect ResetActionToBeInvoked();
   When EepromErasedEventOccurs();
}
