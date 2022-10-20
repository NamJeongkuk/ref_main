/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Grid_SingleEvap.h"
#include "CoolingSpeed.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
#include "utils.h"
#include "Constants_Binary.h"
#include "ConstArrayMap_SideBySideSingleEvap.h"
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
   Active = true,
   InActive = false,
};

TEST_GROUP(Grid_SingleEvap_Test)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   ConstArrayMap_SideBySideSingleEvap_t sideBySideConstArrayMap;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      DataModelErdPointerAccess_Write(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface, ConstArrayMap_SideBySideSingleEvap_Init(&sideBySideConstArrayMap));
   }

   void GridBlockIs(GridBlockNumber_t actual)
   {
      DataModel_Write(dataModel, Erd_Grid_BlockNumber, &actual);
   }

   void GridIsRun()
   {
      Grid_SingleEvap(dataModel);
   }

   void CoolingModeShouldBe(CoolingMode_t expected)
   {
      CoolingMode_t actual;
      DataModel_Read(dataModel, Erd_CoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void CoolingSpeedIs(CoolingSpeed_t coolingSpeed)
   {
      DataModel_Write(dataModel, Erd_CoolingSpeed, &coolingSpeed);
   }

   void CoolingSpeedShouldBe(CoolingSpeed_t expected)
   {
      CoolingSpeed_t actual;
      DataModel_Read(dataModel, Erd_CoolingSpeed, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void PullDownIs(bool state)
   {
      DataModel_Write(dataModel, Erd_SingleEvaporatorPulldownActive, &state);
   }

   void PullDownInMediumCompressorSpeedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_PulldownInMediumCompressorSpeedEnabled, &state);
   }

   void CompressorTripMitigationIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CompressorTripMitigationActive, &state);
   }

   void GridAreaIs(GridArea_t gridArea)
   {
      DataModel_Write(dataModel, Erd_GridArea, &gridArea);
   }

   void GridVotesShouldBe(
      CompressorSpeed_t expectedCompressorSpeed,
      FanSpeed_t expectedCondenserFanSpeed,
      FanSpeed_t expectedFreezerEvapFanSpeed,
      DamperPosition_t expectedFreshFoodDamperPosition)
   {
      CompressorVotedSpeed_t actualCompressorVotedSpeed;
      FanVotedSpeed_t actualCondenserFanVotedSpeed;
      FanVotedSpeed_t actualFreezerEvapFanVotedSpeed;
      DamperVotedPosition_t actualFreshFoodDamperPositionVotedSpeed;

      DataModel_Read(dataModel, Erd_CompressorSpeed_GridVote, &actualCompressorVotedSpeed);
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_GridVote, &actualCondenserFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &actualFreezerEvapFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreshFoodDamperPosition_GridVote, &actualFreshFoodDamperPositionVotedSpeed);

      CHECK_EQUAL(expectedCompressorSpeed, actualCompressorVotedSpeed.speed);
      CHECK_EQUAL(expectedCondenserFanSpeed, actualCondenserFanVotedSpeed.speed);
      CHECK_EQUAL(expectedFreezerEvapFanSpeed, actualFreezerEvapFanVotedSpeed.speed);
      CHECK_EQUAL(expectedFreshFoodDamperPosition, actualFreshFoodDamperPositionVotedSpeed.position);

      CHECK_TRUE(actualCompressorVotedSpeed.care);
      CHECK_TRUE(actualCondenserFanVotedSpeed.care);
      CHECK_TRUE(actualFreezerEvapFanVotedSpeed.care);
      CHECK_TRUE(actualFreshFoodDamperPositionVotedSpeed.care);
   }

   void GridAreaShouldBe(GridArea_t expected)
   {
      GridArea_t actual;
      DataModel_Read(dataModel, Erd_GridArea, &actual);

      CHECK_EQUAL(expected, actual);
   }
};

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks0And1)
{
   for(GridBlockNumber_t gridBlockNumber = 0; gridBlockNumber <= 1; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And PullDownInMediumCompressorSpeedIs(DISABLED);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      GridAreaShouldBe(GridArea_1);
      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Closed);

      When PullDownInMediumCompressorSpeedIs(ENABLED);
      When The GridIsRun();
      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks2And3)
{
   for(GridBlockNumber_t gridBlockNumber = 2; gridBlockNumber <= 3; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And PullDownInMediumCompressorSpeedIs(DISABLED);
      And CoolingModeIs(CoolingMode_Freezer);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Closed);

      When PullDownInMediumCompressorSpeedIs(ENABLED);
      When The GridIsRun();
      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks4And5And6)
{
   for(GridBlockNumber_t gridBlockNumber = 4; gridBlockNumber <= 6; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And PullDownInMediumCompressorSpeedIs(DISABLED);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Open);

      When PullDownInMediumCompressorSpeedIs(ENABLED);
      When The GridIsRun();
      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks7And8And14)
{
   GridBlockNumber_t gridBlockNumbers[] = { 7, 8, 14 };
   for(uint8_t i = 0; i < 3; i++)
   {
      Given GridBlockIs(gridBlockNumbers[i]);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      GridAreaShouldBe(GridArea_1);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks9And10IfPulldownAndTripMitigationIsActiveAndCoolingSpeedIsNotHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 9; gridBlockNumber <= 10; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_Off);
      And PullDownIs(Active);
      And CompressorTripMitigationIs(Active);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks9And10IfPulldownAndTripMitigationIsActiveAndCoolingSpeedIsHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 9; gridBlockNumber <= 10; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_High);
      And PullDownIs(Active);
      And CompressorTripMitigationIs(Active);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks9And10IfPulldownAndTripMitigationIsNotActiveAndCoolingSpeedIsNotHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 9; gridBlockNumber <= 10; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_Off);
      And PullDownIs(InActive);
      And CompressorTripMitigationIs(InActive);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks9And10IfPulldownAndTripMitigationIsNotActiveAndCoolingSpeedIsHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 9; gridBlockNumber <= 10; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_High);
      And PullDownIs(InActive);
      And CompressorTripMitigationIs(InActive);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks11And12And13IfPullDownAndCompressorTripMitigationIsNotActive)
{
   for(GridBlockNumber_t gridBlockNumber = 11; gridBlockNumber <= 13; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      And PullDownIs(InActive);
      And CompressorTripMitigationIs(InActive);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks11And12And13IfPullDownAndCompressorTripMitigationIsActive)
{
   for(GridBlockNumber_t gridBlockNumber = 11; gridBlockNumber <= 13; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      And PullDownIs(Active);
      And CompressorTripMitigationIs(Active);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_High, FanSpeed_Medium, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock15IfCoolingSpeedIsHigh)
{
   Given GridBlockIs(15);
   And CoolingModeIs(CoolingMode_Freezer);
   And GridAreaIs(GridArea_Unknown);
   And CoolingSpeedIs(CoolingSpeed_High);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   GridAreaShouldBe(GridArea_1);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock15IfCoolingSpeedIsNotHigh)
{
   Given GridBlockIs(15);
   And CoolingModeIs(CoolingMode_Freezer);
   And GridAreaIs(GridArea_Unknown);
   And CoolingSpeedIs(CoolingSpeed_Low);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   GridAreaShouldBe(GridArea_1);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks16And17IfCoolingSpeedIsHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 16; gridBlockNumber <= 17; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_High);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks16And17IfCoolingSpeedIsNotHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 16; gridBlockNumber <= 17; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_Low);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock18)
{
   Given GridBlockIs(18);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_High);
   And GridAreaIs(GridArea_Unknown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   GridAreaShouldBe(GridArea_2);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Open);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks19And20And27And34And41)
{
   GridBlockNumber_t gridBlockNumbers[] = { 19, 20, 27, 34, 41 };
   for(uint8_t i = 0; i < 5; i++)
   {
      Given GridBlockIs(gridBlockNumbers[i]);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock21)
{
   Given GridBlockIs(21);
   And CoolingModeIs(CoolingMode_Freezer);
   And GridAreaIs(GridArea_Unknown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   GridAreaShouldBe(GridArea_1);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock22IfCoolingSpeedIsNotLow)
{
   Given GridBlockIs(22);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_High);
   And GridAreaIs(GridArea_Unknown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   GridAreaShouldBe(GridArea_1);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock22IfCoolingSpeedIsLow)
{
   Given GridBlockIs(22);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Low);
   And GridAreaIs(GridArea_Unknown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   GridAreaShouldBe(GridArea_1);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks23And24WhenCoolingSpeedIsOff)
{
   for(GridBlockNumber_t gridBlockNumber = 23; gridBlockNumber <= 24; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_Off);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Low);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks23And24WhenCoolingSpeedIsNotOff)
{
   for(GridBlockNumber_t gridBlockNumber = 23; gridBlockNumber <= 24; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_Low);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Low);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Closed);

      CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_Mid);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, DamperPosition_Closed);

      CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_High);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);

      CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_PullDown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks25And32And39WhenCoolingSpeedIsOff)
{
   for(GridBlockNumber_t gridBlockNumber = 25; gridBlockNumber <= 39; gridBlockNumber += 7)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_Off);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks25And32And39WhenCoolingSpeedIsNotOff)
{
   for(GridBlockNumber_t gridBlockNumber = 25; gridBlockNumber <= 39; gridBlockNumber += 7)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_High);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks26And33And40WhenCoolingSpeedIsHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 26; gridBlockNumber <= 40; gridBlockNumber += 7)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And CoolingSpeedIs(CoolingSpeed_High);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks28And29And35WhenCoolingSpeedIsNotHigh)
{
   GridBlockNumber_t gridBlockNumbers[] = { 28, 29, 35 };
   for(uint8_t i = 0; i < 3; i++)
   {
      Given GridBlockIs(gridBlockNumbers[i]);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      GridAreaShouldBe(GridArea_1);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock30)
{
   Given GridBlockIs(30);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Off);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock31WhenCoolingModeIsOff)
{
   Given GridBlockIs(31);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Off);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock31WhenCoolingModeIsNotOff)
{
   Given GridBlockIs(31);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Low);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);

   CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Mid);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, DamperPosition_Open);

   CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_High);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, DamperPosition_Open);

   CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_PullDown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_SuperHigh, FanSpeed_Low, DamperPosition_Open);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock36WhenCoolingSpeedIsOff)
{
   Given GridBlockIs(36);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Off);
   And GridAreaIs(GridArea_Unknown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Off);
   GridAreaShouldBe(GridArea_1);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock36WhenCoolingSpeedIsNotOff)
{
   Given GridBlockIs(36);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_High);
   And GridAreaIs(GridArea_Unknown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   GridAreaShouldBe(GridArea_1);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock37WhenCoolingSpeedIsNotOff)
{
   Given GridBlockIs(37);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_High);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock37WhenCoolingSpeedIsOff)
{
   Given GridBlockIs(37);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Off);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Off);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock38WhenCoolingSpeedIsOff)
{
   Given GridBlockIs(38);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Off);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Off);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock38WhenCoolingSpeedIsNotOff)
{
   Given GridBlockIs(38);
   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Low);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);

   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_Mid);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);

   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_High);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);

   And CoolingModeIs(CoolingMode_Freezer);
   And CoolingSpeedIs(CoolingSpeed_PullDown);
   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock42And43)
{
   for(GridBlockNumber_t gridBlockNumber = 42; gridBlockNumber <= 43; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Off);
      GridAreaShouldBe(GridArea_1);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock44And45WhenAreaIsOne)
{
   for(GridBlockNumber_t gridBlockNumber = 44; gridBlockNumber <= 45; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_1);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Off);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock44And45WhenAreaIsNotOne)
{
   for(GridBlockNumber_t gridBlockNumber = 44; gridBlockNumber <= 45; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_2);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Off);

      CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Off);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Low, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock46And47)
{
   for(GridBlockNumber_t gridBlockNumber = 46; gridBlockNumber <= 47; gridBlockNumber++)
   {
      Given GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Freezer);
      And GridAreaIs(GridArea_Unknown);
      When The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      GridAreaShouldBe(GridArea_2);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, DamperPosition_Open);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock48)
{
   Given GridBlockIs(48);
   And CoolingModeIs(CoolingMode_Freezer);
   And GridAreaIs(GridArea_Unknown);

   When The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   GridAreaShouldBe(GridArea_2);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, DamperPosition_Open);
}

TEST(Grid_SingleEvap_Test, ShouldVoteAllOffIfInvalidSearchWasPerformed)
{
   Given GridBlockIs(0xFF);
   And CoolingModeIs(0xFF);
   And CoolingSpeedIs(0xFF);
   And GridAreaIs(GridArea_Unknown);

   When The GridIsRun();

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, DamperPosition_Closed);
}