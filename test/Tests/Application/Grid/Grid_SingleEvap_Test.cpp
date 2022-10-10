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

TEST_GROUP(Grid_SingleEvap_Test)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
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

   void GridAreaIs(GridArea_t gridArea)
   {
      DataModel_Write(dataModel, Erd_GridArea, &gridArea);
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
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      GridAreaShouldBe(GridArea_1);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks2And3)
{
   for(GridBlockNumber_t gridBlockNumber = 2; gridBlockNumber <= 3; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Unknown);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks4And5And6)
{
   for(GridBlockNumber_t gridBlockNumber = 4; gridBlockNumber <= 6; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      GridAreaShouldBe(GridArea_2);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks7And8And14)
{
   GridBlockNumber_t gridBlockNumbers[] = { 7, 8, 14 };
   for(uint8_t i = 0; i < 3; i++)
   {
      When GridBlockIs(gridBlockNumbers[i]);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      GridAreaShouldBe(GridArea_1);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks9And10)
{
   for(GridBlockNumber_t gridBlockNumber = 9; gridBlockNumber <= 10; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Unknown);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks11And12And13)
{
   for(GridBlockNumber_t gridBlockNumber = 11; gridBlockNumber <= 13; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      GridAreaShouldBe(GridArea_2);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock15)
{
   When GridBlockIs(15);
   And CoolingModeIs(CoolingMode_Unknown);
   And GridAreaIs(GridArea_Unknown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   GridAreaShouldBe(GridArea_1);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks16And17)
{
   for(GridBlockNumber_t gridBlockNumber = 16; gridBlockNumber <= 17; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Unknown);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock18)
{
   When GridBlockIs(18);
   And CoolingModeIs(CoolingMode_Unknown);
   And GridAreaIs(GridArea_Unknown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   GridAreaShouldBe(GridArea_2);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks19And20And27And34And41)
{
   GridBlockNumber_t gridBlockNumbers[] = { 19, 20, 27, 34, 41 };
   for(uint8_t i = 0; i < 5; i++)
   {
      When GridBlockIs(gridBlockNumbers[i]);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      GridAreaShouldBe(GridArea_2);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock21)
{
   When GridBlockIs(21);
   And CoolingModeIs(CoolingMode_Unknown);
   And GridAreaIs(GridArea_Unknown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   GridAreaShouldBe(GridArea_1);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock22)
{
   When GridBlockIs(22);
   And CoolingModeIs(CoolingMode_Unknown);
   And GridAreaIs(GridArea_Unknown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   GridAreaShouldBe(GridArea_1);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks23And24WhenCoolingSpeedIsOff)
{
   for(GridBlockNumber_t gridBlockNumber = 23; gridBlockNumber <= 24; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And CoolingSpeedIs(CoolingSpeed_Off);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks23And24WhenCoolingSpeedIsNotOff)
{
   for(GridBlockNumber_t gridBlockNumber = 23; gridBlockNumber <= 24; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And CoolingSpeedIs(CoolingSpeed_Low);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Unknown);

      CoolingModeIs(CoolingMode_Unknown);
      And CoolingSpeedIs(CoolingSpeed_Mid);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Unknown);

      CoolingModeIs(CoolingMode_Unknown);
      And CoolingSpeedIs(CoolingSpeed_High);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Unknown);

      CoolingModeIs(CoolingMode_Unknown);
      And CoolingSpeedIs(CoolingSpeed_PullDown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Unknown);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks25And32And39)
{
   for(GridBlockNumber_t gridBlockNumber = 25; gridBlockNumber <= 39; gridBlockNumber += 7)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      GridAreaShouldBe(GridArea_2);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks26And33And40)
{
   for(GridBlockNumber_t gridBlockNumber = 26; gridBlockNumber <= 40; gridBlockNumber += 7)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      GridAreaShouldBe(GridArea_2);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlocks28And29And35)
{
   GridBlockNumber_t gridBlockNumbers[] = { 28, 29, 35 };
   for(uint8_t i = 0; i < 3; i++)
   {
      When GridBlockIs(gridBlockNumbers[i]);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      GridAreaShouldBe(GridArea_1);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock30)
{
   When GridBlockIs(30);
   And CoolingModeIs(CoolingMode_Unknown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Unknown);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock31WhenCoolingModeIsOff)
{
   When GridBlockIs(31);
   And CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_Off);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock31WhenCoolingModeIsNotOff)
{
   When GridBlockIs(31);
   And CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_Low);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_Mid);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_High);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_PullDown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock36)
{
   When GridBlockIs(36);
   And CoolingModeIs(CoolingMode_Unknown);
   And GridAreaIs(GridArea_Unknown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   GridAreaShouldBe(GridArea_1);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock37)
{
   When GridBlockIs(37);
   And CoolingModeIs(CoolingMode_Unknown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Unknown);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock38WhenCoolingSpeedIsOff)
{
   When GridBlockIs(38);
   And CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_Off);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock38WhenCoolingSpeedIsNotOff)
{
   When GridBlockIs(38);
   And CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_Low);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   And CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_Mid);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   And CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_High);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);

   And CoolingModeIs(CoolingMode_Unknown);
   And CoolingSpeedIs(CoolingSpeed_PullDown);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock42And43)
{
   for(GridBlockNumber_t gridBlockNumber = 42; gridBlockNumber <= 43; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      GridAreaShouldBe(GridArea_1);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock44And45WhenAreaIsOne)
{
   for(GridBlockNumber_t gridBlockNumber = 44; gridBlockNumber <= 45; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_1);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock44And45WhenAreaIsNotOne)
{
   for(GridBlockNumber_t gridBlockNumber = 44; gridBlockNumber <= 45; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_2);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);

      CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock46And47)
{
   for(GridBlockNumber_t gridBlockNumber = 46; gridBlockNumber <= 47; gridBlockNumber++)
   {
      When GridBlockIs(gridBlockNumber);
      And CoolingModeIs(CoolingMode_Unknown);
      And GridAreaIs(GridArea_Unknown);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      GridAreaShouldBe(GridArea_2);
   }
}

TEST(Grid_SingleEvap_Test, ShouldOutputCorrectValuesForBlock48)
{
   When GridBlockIs(48);
   And CoolingModeIs(CoolingMode_Unknown);
   And GridAreaIs(GridArea_Unknown);

   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   GridAreaShouldBe(GridArea_2);
}
