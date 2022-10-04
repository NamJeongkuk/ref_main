/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorSpeedDriver.h"
#include "PersonalityParametricData.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"

#define Given
#define When

static const CompressorSpeedDriverConfig_t config = {
   .compressorRelayErd = Erd_CompressorRelay,
   .compressorFrequencyErd = Erd_CompressorInverterDriver,
   .coolingModeErd = Erd_CoolingMode,
   .freezerSetpointZoneErd = Erd_FreezerSetpointZone,
   .compressorControllerSpeedErd = Erd_CompressorControllerSpeed
};

static CompressorSpeedDriver_t instance;

TEST_GROUP(CompressorSpeedDriver_SingleSpeed)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble, TddPersonality_DevelopmentSingleSpeedCompressor);
      dataModel = dataModelTestDouble.dataModel;
   }

   void TheModuleIsInitialized()
   {
      CompressorSpeedDriver_Init(&instance, dataModel, &config);
   }

   void CompressorRelayIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorRelay, &state);
   }

   void CompressorRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorRelay, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CompressorSpeedIs(CompressorSpeed_t speed)
   {
      DataModel_Write(dataModel, Erd_CompressorControllerSpeed, &speed);
   }
};

TEST(CompressorSpeedDriver_SingleSpeed, ShouldInitialize)
{
   TheModuleIsInitialized();
}

TEST(CompressorSpeedDriver_SingleSpeed, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsOff)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_SuperLow);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Off);
   CompressorRelayShouldBe(OFF);
}

TEST(CompressorSpeedDriver_SingleSpeed, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsSuperLow)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperLow);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_SingleSpeed, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsLow)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_SingleSpeed, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsMedium)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_SingleSpeed, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsHigh)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_High);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_SingleSpeed, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsSuperHigh)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperHigh);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_SingleSpeed, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsStartup)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Startup);
   CompressorRelayShouldBe(ON);
}

TEST_GROUP(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   const CompressorData_t *compressorData;
   const CompressorSpeeds_t *compressorSpeeds;

   void setup()
   {
      ReferDataModel_TestDouble_Init(
         &dataModelTestDouble,
         TddPersonality_DevelopmentVariableSpeedCoolingModeIndependentCompressor);
      dataModel = dataModelTestDouble.dataModel;

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
      compressorSpeeds = &compressorData->compressorSpeeds;
   }

   void TheModuleIsInitialized()
   {
      CompressorSpeedDriver_Init(&instance, dataModel, &config);
   }

   void CompressorSpeedIs(CompressorSpeed_t speed)
   {
      DataModel_Write(dataModel, Erd_CompressorControllerSpeed, &speed);
   }

   void CompressorFrequencyShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_CompressorInverterDriver, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CompressorRelayIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorRelay, &state);
   }

   void CompressorRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorRelay, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldUpdateCompressorFrequencyWithStartUpSpeedFrequencyWhenCompressorSpeedIsStartup)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Startup);
   CompressorFrequencyShouldBe(compressorSpeeds->startupSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldUpdateCompressorFrequencyWithSuperLowSpeedFrequencyWhenCompressorSpeedIsSuperLow)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperLow);
   CompressorFrequencyShouldBe(compressorSpeeds->superLowSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldUpdateCompressorFrequencyWithSuperHighSpeedFrequencyWhenCompressorSpeedIsSuperHigh)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperHigh);
   CompressorFrequencyShouldBe(compressorSpeeds->superHighSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldUpdateCompressorFrequencyWithOffValueWhenCompressorSpeedChangesToOff)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperHigh);
   CompressorFrequencyShouldBe(compressorSpeeds->superHighSpeedFrequencyInHz);

   When CompressorSpeedIs(CompressorSpeed_Off);
   CompressorFrequencyShouldBe(CompressorSpeedFrequency_Off);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldUpdateCompressorFrequencyWithLowSpeedFrequencyWhenCompressorSpeedIsLow)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeIndependentCompressorSpeeds.lowSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldUpdateCompressorFrequencyWithMediumSpeedFrequencyWhenCompressorSpeedIsMedium)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeIndependentCompressorSpeeds.mediumSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldUpdateCompressorFrequencyWithHighSpeedFrequencyWhenCompressorSpeedIsHigh)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_High);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeIndependentCompressorSpeeds.highSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsSuperLowAndRelayIsEnabled)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperLow);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsLowAndRelayIsEnabled)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsMediumAndRelayIsEnabled)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsHighAndRelayIsEnabled)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_High);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsSuperHighAndRelayIsEnabled)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperHigh);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsStartupAndRelayIsEnabled)
{
   Given CompressorRelayIs(OFF);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Startup);
   CompressorRelayShouldBe(ON);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeIndependentAndRelayEnabled, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsOffAndRelayIsEnabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Startup);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Off);
   CompressorRelayShouldBe(OFF);
}

TEST_GROUP(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   const CompressorData_t *compressorData;
   const CompressorSpeeds_t *compressorSpeeds;

   void setup()
   {
      ReferDataModel_TestDouble_Init(
         &dataModelTestDouble,
         TddPersonality_DevelopmentVariableSpeedCoolingModeDependentCompressor);
      dataModel = dataModelTestDouble.dataModel;

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
      compressorSpeeds = &compressorData->compressorSpeeds;
   }

   void TheModuleIsInitialized()
   {
      CompressorSpeedDriver_Init(&instance, dataModel, &config);
   }

   void CompressorSpeedIs(CompressorSpeed_t speed)
   {
      DataModel_Write(dataModel, Erd_CompressorControllerSpeed, &speed);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void FreezerSetpointZoneIs(SetpointZone_t setpointZone)
   {
      DataModel_Write(dataModel, Erd_FreezerSetpointZone, &setpointZone);
   }

   void CompressorFrequencyShouldBe(uint8_t expected)
   {
      uint8_t actual;
      DataModel_Read(dataModel, Erd_CompressorInverterDriver, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CompressorRelayIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorRelay, &state);
   }

   void CompressorRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CompressorRelay, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithStartUpSpeedFrequencyWhenCompressorSpeedIsStartup)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Startup);
   CompressorFrequencyShouldBe(compressorSpeeds->startupSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithSuperLowSpeedFrequencyWhenCompressorSpeedIsSuperLow)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperLow);
   CompressorFrequencyShouldBe(compressorSpeeds->superLowSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithSuperHighSpeedFrequencyWhenCompressorSpeedIsSuperHigh)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperHigh);
   CompressorFrequencyShouldBe(compressorSpeeds->superHighSpeedFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithOffValueWhenCompressorSpeedIsOff)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperHigh);
   CompressorFrequencyShouldBe(compressorSpeeds->superHighSpeedFrequencyInHz);

   When CompressorSpeedIs(CompressorSpeed_Off);
   CompressorFrequencyShouldBe(CompressorSpeedFrequency_Off);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedFreshFoodWarmFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsFreshFoodAndFreezerSetpointZoneIsWarm)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given FreezerSetpointZoneIs(SetpointZone_Warm);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodWarmFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedFreshFoodMidFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsFreshFoodAndFreezerSetpointZoneIsMiddle)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given FreezerSetpointZoneIs(SetpointZone_Middle);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodMidFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedFreshFoodColdFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsFreshFoodAndFreezerSetpointZoneIsCold)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given FreezerSetpointZoneIs(SetpointZone_Cold);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreshFoodColdFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedFreezerWarmFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsFreezerAndFreezerSetpointZoneIsWarm)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_Freezer);
   Given FreezerSetpointZoneIs(SetpointZone_Warm);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerWarmFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedFreezerMidFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsFreezerAndFreezerSetpointZoneIsMiddle)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_Freezer);
   Given FreezerSetpointZoneIs(SetpointZone_Middle);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerMidFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedFreezerColdFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsFreezerAndFreezerSetpointZoneIsCold)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_Freezer);
   Given FreezerSetpointZoneIs(SetpointZone_Cold);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedFreezerColdFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedConvertibleCompartmentWarmFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsConvertibleCompartmentAndFreezerSetpointZoneIsWarm)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
   Given FreezerSetpointZoneIs(SetpointZone_Warm);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentWarmFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedConvertibleCompartmentMidFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsConvertibleCompartmentAndFreezerSetpointZoneIsMiddle)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
   Given FreezerSetpointZoneIs(SetpointZone_Middle);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentMidFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithLowSpeedConvertibleCompartmentColdFrequencyWhenCompressorSpeedIsLowAndCoolingModeIsConvertibleCompartmentAndFreezerSetpointZoneIsCold)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
   Given FreezerSetpointZoneIs(SetpointZone_Cold);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.lowSpeedConvertibleCompartmentColdFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedFreshFoodWarmFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsFreshFoodAndFreezerSetpointZoneIsWarm)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given FreezerSetpointZoneIs(SetpointZone_Warm);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodWarmFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedFreshFoodMidFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsFreshFoodAndFreezerSetpointZoneIsMiddle)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given FreezerSetpointZoneIs(SetpointZone_Middle);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodMidFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedFreshFoodColdFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsFreshFoodAndFreezerSetpointZoneIsCold)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given FreezerSetpointZoneIs(SetpointZone_Cold);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreshFoodColdFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedFreezerWarmFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsFreezerAndFreezerSetpointZoneIsWarm)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_Freezer);
   Given FreezerSetpointZoneIs(SetpointZone_Warm);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerWarmFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedFreezerMidFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsFreezerAndFreezerSetpointZoneIsMiddle)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_Freezer);
   Given FreezerSetpointZoneIs(SetpointZone_Middle);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerMidFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedFreezerColdFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsFreezerAndFreezerSetpointZoneIsCold)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_Freezer);
   Given FreezerSetpointZoneIs(SetpointZone_Cold);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedFreezerColdFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedConvertibleCompartmentWarmFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsConvertibleCompartmentAndFreezerSetpointZoneIsWarm)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
   Given FreezerSetpointZoneIs(SetpointZone_Warm);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentWarmFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedConvertibleCompartmentMidFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsConvertibleCompartmentAndFreezerSetpointZoneIsMiddle)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
   Given FreezerSetpointZoneIs(SetpointZone_Middle);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentMidFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithMediumSpeedConvertibleCompartmentColdFrequencyWhenCompressorSpeedIsMediumAndCoolingModeIsConvertibleCompartmentAndFreezerSetpointZoneIsCold)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
   Given FreezerSetpointZoneIs(SetpointZone_Cold);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.mediumSpeedConvertibleCompartmentColdFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithHighSpeedFreshFoodFrequencyWhenCompressorSpeedIsHighAndCoolingModeIsFreshFood)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_High);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedFreshFoodFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithHighSpeedFreezerFrequencyWhenCompressorSpeedIsHighAndCoolingModeIsFreezer)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_Freezer);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_High);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedFreezerFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldUpdateCompressorFrequencyWithHighSpeedConvertibleCompartmentFrequencyWhenCompressorSpeedIsHighAndCoolingModeIsConvertibleCompartment)
{
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_High);
   CompressorFrequencyShouldBe(compressorSpeeds->coolingModeDependentCompressorSpeeds.highSpeedConvertibleCompartmentFrequencyInHz);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsSuperLowAndRelayIsDisabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperLow);
   CompressorRelayShouldBe(OFF);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsLowAndRelayIsDisabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Low);
   CompressorRelayShouldBe(OFF);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsMediumAndRelayIsDisabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Medium);
   CompressorRelayShouldBe(OFF);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldTurnOnCompressorRelayWhenCompressorSpeedIsHighAndRelayIsDisabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_High);
   CompressorRelayShouldBe(OFF);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsSuperHighAndRelayIsDisabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_SuperHigh);
   CompressorRelayShouldBe(OFF);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsStartupAndRelayIsDisabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Off);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Startup);
   CompressorRelayShouldBe(OFF);
}

TEST(CompressorSpeedDriver_VariableSpeedCoolingModeDependentAndRelayDisabled, ShouldTurnOffCompressorRelayWhenCompressorSpeedIsOffAndRelayIsDisabled)
{
   Given CompressorRelayIs(ON);
   Given CompressorSpeedIs(CompressorSpeed_Startup);
   Given TheModuleIsInitialized();

   When CompressorSpeedIs(CompressorSpeed_Off);
   CompressorRelayShouldBe(OFF);
}
