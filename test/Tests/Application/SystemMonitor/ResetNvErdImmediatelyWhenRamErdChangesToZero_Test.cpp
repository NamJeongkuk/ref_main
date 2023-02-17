/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ResetNvErdImmediatelyWhenRamErdChangesToZero.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define Is ,
#define ShouldBe ,

enum
{
   SomeValue = 450,
   SomeOtherValue = 2189,
   AnotherValue = 860
};

static const SystemMonitorData_t systemMonitorData = {
   .periodicNvUpdateInMinutes = 60,
   .periodicNvUpdateDefrostHeaterOnTimeInMinutes = 0,
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroEntry_t ResetNvErdImmediatelyWhenRamErdChangesToZeroEntries[] = {
   { Erd_DefrostCompressorOnTimeInSeconds, Erd_Eeprom_DefrostCompressorOnTimeInSeconds },
   { Erd_DefrostFreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_DefrostFreezerScaledDoorAccelerationInSeconds },
   { Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_DefrostFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds, Erd_Eeprom_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds }
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration = {
   ResetNvErdImmediatelyWhenRamErdChangesToZeroEntries,
   NUM_ELEMENTS(ResetNvErdImmediatelyWhenRamErdChangesToZeroEntries)
};

TEST_GROUP(ResetNvErdImmediatelyWhenRamErdChangesToZero)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void Initialize()
   {
      ResetNvErdImmediatelyWhenRamErdChangesToZero_Init(dataModel, &ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration);
   }

   void RamErd(Erd_t erd, uint32_t value)
   {
      DataModel_Write(dataModel, erd, &value);
   }

   void EepromErd(Erd_t erd, uint32_t value)
   {
      DataModel_Write(dataModel, erd, &value);
   }

   void EepromErdShouldBe(Erd_t erd, uint32_t expectedValue)
   {
      uint32_t actualValue;
      DataModel_Read(dataModel, erd, &actualValue);

      CHECK_EQUAL(expectedValue, actualValue);
   }
};

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldInitialize)
{
   Initialize();
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForCompressorOnTimeInSeconds)
{
   Given RamErd(Erd_DefrostCompressorOnTimeInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostCompressorOnTimeInSeconds Is SomeOtherValue);
   And Initialize();

   EepromErdShouldBe(Erd_Eeprom_DefrostCompressorOnTimeInSeconds ShouldBe SomeOtherValue);

   When RamErd(Erd_DefrostCompressorOnTimeInSeconds Is 0);
   EepromErdShouldBe(Erd_Eeprom_DefrostCompressorOnTimeInSeconds ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForFreezerScaledDoorAccelerationInSeconds)
{
   Given RamErd(Erd_DefrostFreezerScaledDoorAccelerationInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostFreezerScaledDoorAccelerationInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostFreezerScaledDoorAccelerationInSeconds Is 0);
   EepromErdShouldBe(Erd_Eeprom_DefrostFreezerScaledDoorAccelerationInSeconds ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForFreshFoodScaledDoorAccelerationInSeconds)
{
   Given RamErd(Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostFreshFoodScaledDoorAccelerationInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds Is 0);
   EepromErdShouldBe(Erd_Eeprom_DefrostFreshFoodScaledDoorAccelerationInSeconds ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForConvertibleCompartmentScaledDoorAccelerationInSeconds)
{
   Given RamErd(Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds Is 0);
   EepromErdShouldBe(Erd_Eeprom_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldNotResetEepromErdToZeroImmediatelyWhenRamErdIsSetToANonZeroValueForCompressorOnTimeInSeconds)
{
   Given RamErd(Erd_DefrostCompressorOnTimeInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostCompressorOnTimeInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostCompressorOnTimeInSeconds Is AnotherValue);
   EepromErdShouldBe(Erd_Eeprom_DefrostCompressorOnTimeInSeconds ShouldBe SomeOtherValue);
}
