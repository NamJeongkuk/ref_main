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
#include "PersonalityParametricData_TestDouble.h"
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
   .periodicNvUpdateInMinutes = 60
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroEntry_t ResetNvErdImmediatelyWhenRamErdChangesToZeroEntries[] = {
   { Erd_DefrostCompressorOnTimeInSeconds, Erd_Eeprom_DefrostCompressorOnTimeInSeconds },
   { Erd_DefrostFreezerDoorAccelerationCount, Erd_Eeprom_DefrostFreezerDoorAccelerationCount },
   { Erd_DefrostFreshFoodDoorAccelerationCount, Erd_Eeprom_DefrostFreshFoodDoorAccelerationCount },
   { Erd_DefrostConvertibleCompartmentDoorAccelerationCount, Erd_Eeprom_DefrostConvertibleCompartmentDoorAccelerationCount }
};

static const ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration_t ResetNvErdImmediatelyWhenRamErdChangesToZeroConfiguration = {
   ResetNvErdImmediatelyWhenRamErdChangesToZeroEntries,
   NUM_ELEMENTS(ResetNvErdImmediatelyWhenRamErdChangesToZeroEntries)
};

TEST_GROUP(ResetNvErdImmediatelyWhenRamErdChangesToZero)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   PersonalityParametricData_t personalityParametricData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      PersonalityParametricData_TestDouble_Init(&personalityParametricData);
      PersonalityParametricData_TestDouble_SetSystemMonitor(&personalityParametricData, &systemMonitorData);
      DataModelErdPointerAccess_Write(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
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

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForFreezerDoorAccelerationCounts)
{
   Given RamErd(Erd_DefrostFreezerDoorAccelerationCount Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostFreezerDoorAccelerationCount Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostFreezerDoorAccelerationCount Is 0);
   EepromErdShouldBe(Erd_Eeprom_DefrostFreezerDoorAccelerationCount ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForFreshFoodDoorAccelerationCounts)
{
   Given RamErd(Erd_DefrostFreshFoodDoorAccelerationCount Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostFreshFoodDoorAccelerationCount Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostFreshFoodDoorAccelerationCount Is 0);
   EepromErdShouldBe(Erd_Eeprom_DefrostFreshFoodDoorAccelerationCount ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldResetEepromErdToZeroImmediatelyWhenRamErdIsSetToZeroForConvertibleCompartmentDoorAccelerationCounts)
{
   Given RamErd(Erd_DefrostConvertibleCompartmentDoorAccelerationCount Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostConvertibleCompartmentDoorAccelerationCount Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostConvertibleCompartmentDoorAccelerationCount Is 0);
   EepromErdShouldBe(Erd_Eeprom_DefrostConvertibleCompartmentDoorAccelerationCount ShouldBe 0);
}

TEST(ResetNvErdImmediatelyWhenRamErdChangesToZero, ShouldNotResetEepromErdToZeroImmediatelyWhenRamErdIsSetToANonZeroValueForCompressorOnTimeInSeconds)
{
   Given RamErd(Erd_DefrostCompressorOnTimeInSeconds Is SomeValue);
   And EepromErd(Erd_Eeprom_DefrostCompressorOnTimeInSeconds Is SomeOtherValue);
   And Initialize();

   When RamErd(Erd_DefrostCompressorOnTimeInSeconds Is AnotherValue);
   EepromErdShouldBe(Erd_Eeprom_DefrostCompressorOnTimeInSeconds ShouldBe SomeOtherValue);
}
