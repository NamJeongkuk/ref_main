/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Grid_IceCabinet.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TddPersonality.h"
#include "uassert_test.h"

#define When
#define Given
#define And
#define The

enum
{
   Valid = true,
   Invalid = false
};

TEST_GROUP(Grid_IceCabinet)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   EventSubscription_t dataModelOnChangeSubscription;
   const GammaTable_t *gammaTable;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_SwqaMultidoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      gammaTable = PersonalityParametricData_Get(dataModel)->iceCabinetData->gammaTable;
   }

   void GridBlockBecomes(GridBlockNumber_t actual)
   {
      DataModel_Write(dataModel, Erd_IceCabinetGridBlockNumberOverrideResolved, &actual);
   }

   void GridIsRun()
   {
      Grid_IceCabinet(dataModel);
   }

   void IceCabinetThermistorIsValid()
   {
      DataModel_Write(dataModel, Erd_IceCabinetThermistor_IsValidResolved, set);
   }

   void HighSpeedOverrideIsEnabledByGrid()
   {
      DataModel_Write(dataModel, Erd_IceCabinetHighSpeedOverrideEnabledByGrid, set);
   }

   void IceCabinetFanIsDisabledByFreshFoodGrid()
   {
      DataModel_Write(dataModel, Erd_IceCabinetFanDisabledByGrid, set);
   }

   void IceCabinetThermistorIsNotValid()
   {
      DataModel_Write(dataModel, Erd_IceCabinetThermistor_IsValidResolved, clear);
   }

   void IceCabinetGammaIs(Gamma_t gamma)
   {
      DataModel_Write(dataModel, Erd_IceCabinetGamma, &gamma);
   }

   void IceCabinetCoolingModeIs(IceCabinetCoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_IceCabinetCoolingMode, &coolingMode);
   }

   void IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_t expected)
   {
      IceCabinetCoolingMode_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetCoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void IceCabinetGridBlockNumberIs(GridBlockNumber_t gridBlockNumber)
   {
      DataModel_Write(dataModel, Erd_IceCabinetGrid_BlockNumber, &gridBlockNumber);
   }

   void IceCabinetFanGridVoteShouldBe(FanSpeed_t expectedFanSpeed)
   {
      FanVotedSpeed_t actualIceCabinetFanSpeed;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_IceCabinetGridVote, &actualIceCabinetFanSpeed);

      CHECK_EQUAL(actualIceCabinetFanSpeed.speed, expectedFanSpeed);
      CHECK_TRUE(actualIceCabinetFanSpeed.care);
   }
};

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock0)
{
   Given IceCabinetThermistorIsValid();
   When GridBlockBecomes(0);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ForcedCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_SuperHigh);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock1)
{
   Given IceCabinetThermistorIsValid();
   When GridBlockBecomes(1);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ForcedCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_SuperHigh);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock2)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   And GridBlockBecomes(2);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ForcedCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_SuperHigh);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock2WhenNotInForcedCoolingAndWhenGammaChanges)
{
   Given IceCabinetCoolingModeIs(IceCabinetCoolingMode_Neutral);
   And IceCabinetGammaIs(gammaTable->iceCabinetMedSpeedLimit - 1);
   And IceCabinetThermistorIsValid();
   When GridBlockBecomes(2);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ActiveCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Low);

   Given IceCabinetGammaIs(gammaTable->iceCabinetMedSpeedLimit);
   When The GridIsRun();
   IceCabinetFanGridVoteShouldBe(FanSpeed_Medium);

   Given IceCabinetGammaIs(gammaTable->iceCabinetHighSpeedLimit - 1);
   When The GridIsRun();
   IceCabinetFanGridVoteShouldBe(FanSpeed_Medium);

   Given IceCabinetGammaIs(gammaTable->iceCabinetHighSpeedLimit);
   When The GridIsRun();
   IceCabinetFanGridVoteShouldBe(FanSpeed_High);

   Given IceCabinetGammaIs(gammaTable->iceCabinetSuperHighSpeedLimit - 1);
   When The GridIsRun();
   IceCabinetFanGridVoteShouldBe(FanSpeed_High);

   Given IceCabinetGammaIs(gammaTable->iceCabinetSuperHighSpeedLimit);
   When The GridIsRun();
   IceCabinetFanGridVoteShouldBe(FanSpeed_SuperHigh);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock3)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetGammaIs(gammaTable->iceCabinetMedSpeedLimit);
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   And GridBlockBecomes(3);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ActiveCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Medium);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock3WhenCoolingModeIsNeutral)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_Neutral);
   And GridBlockBecomes(3);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_Neutral);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock4)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetGammaIs(gammaTable->iceCabinetMedSpeedLimit);
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   And GridBlockBecomes(4);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ActiveCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Medium);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock4WhenCoolingModeIsNeutral)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_Neutral);
   And GridBlockBecomes(4);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_Neutral);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock5)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ActiveCooling);
   And GridBlockBecomes(5);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_Neutral);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock6)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ActiveCooling);
   And GridBlockBecomes(6);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_Neutral);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesForBlock7)
{
   Given IceCabinetThermistorIsValid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ActiveCooling);
   And GridBlockBecomes(7);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_Neutral);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}

TEST(Grid_IceCabinet, ShouldOutputCorrectValuesWhenInFallbackCooling)
{
   Given IceCabinetThermistorIsNotValid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ActiveCooling);
   And GridBlockBecomes(3);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_FallbackCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_High);
}

TEST(Grid_IceCabinet, ShouldUseHighSpeedOverrideFlagFromFreshFoodGrid)
{
   Given IceCabinetThermistorIsValid();
   And HighSpeedOverrideIsEnabledByGrid();
   And IceCabinetGammaIs(gammaTable->iceCabinetMedSpeedLimit);
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   And GridBlockBecomes(3);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ActiveCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_High);
}

TEST(Grid_IceCabinet, ShouldNotTurnFanOnInNeutralIfHighSpeedOverrideFlagFromFreshFoodGridIsSet)
{
   Given IceCabinetThermistorIsValid();
   And HighSpeedOverrideIsEnabledByGrid();
   And IceCabinetGammaIs(gammaTable->iceCabinetMedSpeedLimit);
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_Neutral);
   And GridBlockBecomes(3);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_Neutral);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}

TEST(Grid_IceCabinet, ShouldLowerSpeedInForcedCoolingModeIfHighSpeedOverrideFlagFromFreshFoodGridIsSet)
{
   Given IceCabinetThermistorIsValid();
   And HighSpeedOverrideIsEnabledByGrid();
   And IceCabinetGammaIs(gammaTable->iceCabinetMedSpeedLimit);
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_Neutral);
   And GridBlockBecomes(0);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ForcedCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_High);
}

TEST(Grid_IceCabinet, ShouldSupersedeHighSpeedOverrideIfIceCabinetIsDisabled)
{
   Given IceCabinetThermistorIsValid();
   And HighSpeedOverrideIsEnabledByGrid();
   And IceCabinetFanIsDisabledByFreshFoodGrid();
   And IceCabinetCoolingModeIs(IceCabinetCoolingMode_ActiveCooling);
   And GridBlockBecomes(3);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ActiveCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}

TEST(Grid_IceCabinet, ShouldRecoverIfInvalidIceCabinetCoolingModeIsSetExternally)
{
   Given IceCabinetThermistorIsValid();
   And HighSpeedOverrideIsEnabledByGrid();
   And IceCabinetFanIsDisabledByFreshFoodGrid();
   And IceCabinetCoolingModeIs(0xCA);
   And GridBlockBecomes(3);
   And The GridIsRun();

   IceCabinetCoolingModeShouldBe(IceCabinetCoolingMode_ActiveCooling);
   IceCabinetFanGridVoteShouldBe(FanSpeed_Off);
}
