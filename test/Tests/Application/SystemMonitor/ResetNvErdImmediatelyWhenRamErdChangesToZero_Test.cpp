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

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroEntry_t ResetNvErdImmediatelyWhenRamErdChangesToZeroEntries[] = {
   { Erd_DefrostCompressorOnTimeInSeconds, Erd_Eeprom_DefrostCompressorOnTimeInSeconds },
   { Erd_FreezerScaledDoorAccelerationInSeconds, Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds },
   { Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_LeftSideFreshFoodScaledDoorAccelerationInSeconds },
   { Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds, Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds }
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
   Given RamErd(Erd_FreezerScaledDoorAccelerationInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_FreezerScaledDoorAccelerationInSeconds Is 0);
   EepromErdShouldBe(Erd_Eeprom_FreezerScaledDoorAccelerationInSeconds ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForLeftSideFreshFoodScaledDoorAccelerationInSeconds)
{
   Given RamErd(Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_LeftSideFreshFoodScaledDoorAccelerationInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_LeftSideFreshFoodScaledDoorAccelerationInSeconds Is 0);
   EepromErdShouldBe(Erd_Eeprom_LeftSideFreshFoodScaledDoorAccelerationInSeconds ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds)
{
   Given RamErd(Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds Is 0);
   EepromErdShouldBe(Erd_Eeprom_ConvertibleCompartmentAsFreshFoodScaledDoorAccelerationInSeconds ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldNotResetEepromErdToZeroImmediatelyWhenRamErdIsSetToANonZeroValueForCompressorOnTimeInSeconds)
{
   Given RamErd(Erd_DefrostCompressorOnTimeInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostCompressorOnTimeInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostCompressorOnTimeInSeconds Is AnotherValue);
   EepromErdShouldBe(Erd_Eeprom_DefrostCompressorOnTimeInSeconds ShouldBe SomeOtherValue);
}
