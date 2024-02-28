/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Grid_SingleDoorSingleEvap.h"
#include "ConstArrayMap_SingleDoorSingleEvap.h"
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

enum
{
   Valid = true,
   Invalid = false
};

TEST_GROUP(Grid_SingleDoorSingleEvap)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   ConstArrayMap_SingleDoorSingleEvap_t singleDoorSingleEvapConstArrayMap;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentSingleDoorSingleEvapAllFreezer);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      DataModelErdPointerAccess_Write(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface, ConstArrayMap_SingleDoorSingleEvap_Init(&singleDoorSingleEvapConstArrayMap));
   }

   void GivenGridBlockIs(GridBlockNumber_t actual)
   {
      DataModel_Write(dataModel, Erd_GridBlockNumberOverrideResolved, &actual);
   }

   void WhenTheGridIsRun()
   {
      Grid_SingleDoorSingleEvap(dataModel);
   }

   void GivenCoolingSpeedIs(CoolingSpeed_t coolingSpeed)
   {
      DataModel_Write(dataModel, Erd_CoolingSpeed, &coolingSpeed);
   }

   void CoolingSpeedShouldBe(CoolingSpeed_t expected)
   {
      CoolingSpeed_t actual;
      DataModel_Read(dataModel, Erd_CoolingSpeed, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenGridAreaIs(GridArea_t gridArea)
   {
      DataModel_Write(dataModel, Erd_GridArea, &gridArea);
   }

   void TheIceMakerEnableShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMakerEnabledByGrid, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheCondenserFanAntiSweatBehaviorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheCondenserFanAntiSweatBehaviorIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &state);
   }

   void GivenTheIceMakerEnableIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, &state);
   }

   void TheFreezerEvapFanAntiSweatBehaviorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanAntiSweatBehaviorEnabledByGrid, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GridVotesShouldBe(
      CompressorSpeed_t expectedCompressorSpeed,
      FanSpeed_t expectedCondenserFanSpeed,
      FanSpeed_t expectedPrimaryEvapFanSpeed)
   {
      CompressorVotedSpeed_t actualCompressorVotedSpeed;
      FanVotedSpeed_t actualCondenserFanVotedSpeed;
      FanVotedSpeed_t actualFreezerEvapFanVotedSpeed;
      FanVotedSpeed_t actualFreshFoodEvapFanVotedSpeed;

      DataModel_Read(dataModel, Erd_CompressorSpeed_GridVote, &actualCompressorVotedSpeed);
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_GridVote, &actualCondenserFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &actualFreezerEvapFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &actualFreshFoodEvapFanVotedSpeed);

      CHECK_EQUAL(expectedCompressorSpeed, actualCompressorVotedSpeed.speed);
      CHECK_EQUAL(expectedCondenserFanSpeed, actualCondenserFanVotedSpeed.speed);
      CHECK_EQUAL(expectedPrimaryEvapFanSpeed, actualFreezerEvapFanVotedSpeed.speed);
      CHECK_EQUAL(expectedPrimaryEvapFanSpeed, actualFreshFoodEvapFanVotedSpeed.speed);

      CHECK_TRUE(actualCompressorVotedSpeed.care);
      CHECK_TRUE(actualCondenserFanVotedSpeed.care);
      CHECK_TRUE(actualFreezerEvapFanVotedSpeed.care);
      CHECK_TRUE(actualFreshFoodEvapFanVotedSpeed.care);
   }

   void GridAreaShouldBe(GridArea_t expected)
   {
      GridArea_t actual;
      DataModel_Read(dataModel, Erd_GridArea, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenFreshFoodThermistorIsValidResolvedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void GivenFreezerThermistorIsValidResolvedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void WhenFreezerThermistorIsValidResolvedIs(bool state)
   {
      GivenFreezerThermistorIsValidResolvedIs(state);
   }

   void BothThermistorsAreValid(void)
   {
      GivenFreshFoodThermistorIsValidResolvedIs(Valid);
      GivenFreezerThermistorIsValidResolvedIs(Valid);
   }

   void GridCareVotesShouldBeDontCare(void)
   {
      CompressorVotedSpeed_t actualCompressorVotedSpeed;
      FanVotedSpeed_t actualCondenserFanVotedSpeed;
      FanVotedSpeed_t actualFreezerEvapFanVotedSpeed;
      FanVotedSpeed_t actualFreshFoodEvapFanVotedSpeed;

      DataModel_Read(dataModel, Erd_CompressorSpeed_GridVote, &actualCompressorVotedSpeed);
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_GridVote, &actualCondenserFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &actualFreezerEvapFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &actualFreshFoodEvapFanVotedSpeed);

      CHECK_FALSE(actualCompressorVotedSpeed.care);
      CHECK_FALSE(actualCondenserFanVotedSpeed.care);
      CHECK_FALSE(actualFreezerEvapFanVotedSpeed.care);
      CHECK_FALSE(actualFreshFoodEvapFanVotedSpeed.care);
   }
};

// Realistically we don't have to split into two test groups, the grid itself should work the
// same no matter what parametric we have. Parametric just changes what gets loaded, ERD preconditions
TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock1)
{
   GivenGridBlockIs(1);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   GivenFreezerThermistorIsValidResolvedIs(Valid);
   GivenGridAreaIs(GridArea_Unknown);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_PullDown);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_1);
   GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock8IfGivenGridAreaIs1)
{
   GivenGridBlockIs(8);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   GivenFreezerThermistorIsValidResolvedIs(Valid);
   GivenGridAreaIs(GridArea_1);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheIceMakerEnableShouldBe(ENABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   GridAreaShouldBe(GridArea_1);
   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock8IfGivenGridAreaIsNot1)
{
   GivenGridBlockIs(8);
   GivenTheIceMakerEnableIs(DISABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
   GivenFreezerThermistorIsValidResolvedIs(Valid);
   GivenGridAreaIs(GridArea_2);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_High);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_2);
   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock15IfGivenGridAreaIs1)
{
   GivenGridBlockIs(15);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   GivenFreezerThermistorIsValidResolvedIs(Valid);
   GivenCoolingSpeedIs(CoolingSpeed_Off);
   GivenGridAreaIs(GridArea_1);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_High);
   TheIceMakerEnableShouldBe(ENABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   GridAreaShouldBe(GridArea_1);
   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock15IfGivenGridAreaIsNot1AndGivenCoolingSpeedIsHigh)
{
   GivenGridBlockIs(15);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   GivenFreezerThermistorIsValidResolvedIs(Valid);
   GivenCoolingSpeedIs(CoolingSpeed_High);
   GivenGridAreaIs(GridArea_2);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_High);
   TheIceMakerEnableShouldBe(ENABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   GridAreaShouldBe(GridArea_2);
   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock15IfGivenGridAreaIsNot1AndGivenCoolingSpeedIsNotHigh)
{
   GivenGridBlockIs(15);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   GivenFreezerThermistorIsValidResolvedIs(Valid);
   GivenCoolingSpeedIs(CoolingSpeed_Off);
   GivenGridAreaIs(GridArea_2);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheIceMakerEnableShouldBe(ENABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   GridAreaShouldBe(GridArea_2);
   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlocks22And46IfGivenCoolingSpeedIsOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 22, 46 };
   for(size_t i = 0; i < NUM_ELEMENTS(gridBlockNumbers); i++)
   {
      GivenGridBlockIs(gridBlockNumbers[i]);
      GivenTheIceMakerEnableIs(DISABLED);
      GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
      BothThermistorsAreValid();
      GivenCoolingSpeedIs(CoolingSpeed_Off);
      GivenGridAreaIs(GridArea_1);
      WhenTheGridIsRun();

      CoolingSpeedShouldBe(CoolingSpeed_Low);
      TheIceMakerEnableShouldBe(ENABLED);
      TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      GridAreaShouldBe(GridArea_2);
      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low);
   }
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlocks22And46IfGivenCoolingSpeedIsNotOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 22, 46 };
   for(size_t i = 0; i < NUM_ELEMENTS(gridBlockNumbers); i++)
   {
      GivenGridBlockIs(gridBlockNumbers[i]);
      GivenTheIceMakerEnableIs(DISABLED);
      GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
      BothThermistorsAreValid();
      GivenCoolingSpeedIs(CoolingSpeed_High);
      GivenGridAreaIs(GridArea_1);
      WhenTheGridIsRun();

      CoolingSpeedShouldBe(CoolingSpeed_High);
      TheIceMakerEnableShouldBe(ENABLED);
      TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      GridAreaShouldBe(GridArea_2);
      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
   }
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlocks29And45IfGivenCoolingSpeedIsHigh)
{
   GridBlockNumber_t gridBlockNumbers[] = { 29, 45 };
   for(size_t i = 0; i < NUM_ELEMENTS(gridBlockNumbers); i++)
   {
      GivenGridBlockIs(gridBlockNumbers[i]);
      GivenTheIceMakerEnableIs(DISABLED);
      GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      BothThermistorsAreValid();
      GivenCoolingSpeedIs(CoolingSpeed_High);
      GivenGridAreaIs(GridArea_1);
      WhenTheGridIsRun();

      CoolingSpeedShouldBe(CoolingSpeed_Mid);
      TheIceMakerEnableShouldBe(ENABLED);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      GridAreaShouldBe(GridArea_2);
      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium);
   }
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlocks29And45IfGivenCoolingSpeedIsNotHigh)
{
   GridBlockNumber_t gridBlockNumbers[] = { 29, 45 };
   for(size_t i = 0; i < NUM_ELEMENTS(gridBlockNumbers); i++)
   {
      GivenGridBlockIs(gridBlockNumbers[i]);
      GivenTheIceMakerEnableIs(DISABLED);
      GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      BothThermistorsAreValid();
      GivenCoolingSpeedIs(CoolingSpeed_Low);
      GivenGridAreaIs(GridArea_1);
      WhenTheGridIsRun();

      CoolingSpeedShouldBe(CoolingSpeed_Low);
      TheIceMakerEnableShouldBe(ENABLED);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      GridAreaShouldBe(GridArea_2);
      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low);
   }
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlocks36And44IfGivenCoolingSpeedIsOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 36, 44 };
   for(size_t i = 0; i < NUM_ELEMENTS(gridBlockNumbers); i++)
   {
      GivenGridBlockIs(gridBlockNumbers[i]);
      GivenTheIceMakerEnableIs(DISABLED);
      GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      BothThermistorsAreValid();
      GivenCoolingSpeedIs(CoolingSpeed_Off);
      GivenGridAreaIs(GridArea_1);
      WhenTheGridIsRun();

      CoolingSpeedShouldBe(CoolingSpeed_Off);
      TheIceMakerEnableShouldBe(ENABLED);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      GridAreaShouldBe(GridArea_2);
      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off);
   }
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlocks36And44IfGivenCoolingSpeedIsNotOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 36, 44 };
   for(size_t i = 0; i < NUM_ELEMENTS(gridBlockNumbers); i++)
   {
      GivenGridBlockIs(gridBlockNumbers[i]);
      GivenTheIceMakerEnableIs(DISABLED);
      GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      BothThermistorsAreValid();
      GivenCoolingSpeedIs(CoolingSpeed_High);
      GivenGridAreaIs(GridArea_1);
      WhenTheGridIsRun();

      CoolingSpeedShouldBe(CoolingSpeed_Low);
      TheIceMakerEnableShouldBe(ENABLED);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      GridAreaShouldBe(GridArea_2);
      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low);
   }
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlocks42And43)
{
   GridBlockNumber_t gridBlockNumbers[] = { 42, 43 };
   for(size_t i = 0; i < NUM_ELEMENTS(gridBlockNumbers); i++)
   {
      GivenGridBlockIs(gridBlockNumbers[i]);
      GivenTheIceMakerEnableIs(DISABLED);
      GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
      BothThermistorsAreValid();
      GivenCoolingSpeedIs(CoolingSpeed_Low);
      GivenGridAreaIs(GridArea_1);
      WhenTheGridIsRun();

      CoolingSpeedShouldBe(CoolingSpeed_Off);
      TheIceMakerEnableShouldBe(ENABLED);
      TheCondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      GridAreaShouldBe(GridArea_2);
      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off);
   }
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock47IfAreaIs1)
{
   GivenGridBlockIs(47);
   GivenTheIceMakerEnableIs(DISABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
   BothThermistorsAreValid();
   GivenCoolingSpeedIs(CoolingSpeed_Low);
   GivenGridAreaIs(GridArea_1);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_High);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_1);
   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock47IfAreaIsNot1AndSpeedIsHigh)
{
   GivenGridBlockIs(47);
   GivenTheIceMakerEnableIs(DISABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
   BothThermistorsAreValid();
   GivenCoolingSpeedIs(CoolingSpeed_High);
   GivenGridAreaIs(GridArea_2);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_High);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_2);
   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
}

TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock47IfAreaIsNot1AndSpeedIsNotHigh)
{
   GivenGridBlockIs(47);
   GivenTheIceMakerEnableIs(DISABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(DISABLED);
   BothThermistorsAreValid();
   GivenCoolingSpeedIs(CoolingSpeed_Low);
   GivenGridAreaIs(GridArea_2);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_2);
   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium);
}

IGNORE_TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock48IfAboveFFExtreme)
{
   // TODO: Enable me when single door fresh food pulldown is added
   GivenGridBlockIs(48);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   BothThermistorsAreValid();
   GivenCoolingSpeedIs(CoolingSpeed_Low);
   GivenGridAreaIs(GridArea_2);
   // FreshFoodFilteredTempGreaterThanExtreme(True);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_PullDown);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_1);
   GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low);
}

IGNORE_TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock48IfBelowFFExtremeAndAreaIsNot1)
{
   // TODO: Enable me when single door fresh food pulldown is added
   GivenGridBlockIs(48);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   BothThermistorsAreValid();
   GivenCoolingSpeedIs(CoolingSpeed_Low);
   GivenGridAreaIs(GridArea_2);
   // FreshFoodFilteredTempGreaterThanExtreme(False);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_High);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_2);
   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
}

IGNORE_TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock48IfBelowFFExtremeAndAreaIs1AndSpeedNotHigh)
{
   // TODO: Enable me when single door fresh food pulldown is added
   GivenGridBlockIs(48);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   BothThermistorsAreValid();
   GivenCoolingSpeedIs(CoolingSpeed_Low);
   GivenGridAreaIs(GridArea_1);
   // FreshFoodFilteredTempGreaterThanExtreme(False);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_1);
   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium);
}

IGNORE_TEST(Grid_SingleDoorSingleEvap, ShouldOutputCorrectValuesForBlock48IfBelowFFExtremeAndAreaIs1AndSpeedIsHigh)
{
   // TODO: Enable me when single door fresh food pulldown is added
   GivenGridBlockIs(48);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   BothThermistorsAreValid();
   GivenCoolingSpeedIs(CoolingSpeed_High);
   GivenGridAreaIs(GridArea_1);
   // FreshFoodFilteredTempGreaterThanExtreme(False);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_High);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_1);
   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High);
}

TEST(Grid_SingleDoorSingleEvap, ShouldVoteAllOffIfInvalidSearchWasPerformed)
{
   GivenGridBlockIs(0xFF);
   GivenCoolingSpeedIs(0xFF);
   GivenGridAreaIs(GridArea_Unknown);
   BothThermistorsAreValid();
   WhenTheGridIsRun();

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off);
}

TEST(Grid_SingleDoorSingleEvap, ShouldSetCareVoteToDontCareWhenBothThermistorsAreInvalid)
{
   GivenGridBlockIs(1);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   GivenFreezerThermistorIsValidResolvedIs(Invalid);
   GivenFreshFoodThermistorIsValidResolvedIs(Invalid);
   GivenCoolingSpeedIs(CoolingSpeed_PullDown);
   GivenGridAreaIs(GridArea_2);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_PullDown);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_1);
   GridCareVotesShouldBeDontCare();
}

TEST(Grid_SingleDoorSingleEvap, ShouldSetCareVoteToCareWhenOneThermistorReturnsToValid)
{
   GivenGridBlockIs(1);
   GivenTheIceMakerEnableIs(ENABLED);
   GivenTheCondenserFanAntiSweatBehaviorIs(ENABLED);
   GivenFreezerThermistorIsValidResolvedIs(Invalid);
   GivenFreshFoodThermistorIsValidResolvedIs(Invalid);
   GivenCoolingSpeedIs(CoolingSpeed_PullDown);
   GivenGridAreaIs(GridArea_2);
   WhenTheGridIsRun();

   CoolingSpeedShouldBe(CoolingSpeed_PullDown);
   TheIceMakerEnableShouldBe(DISABLED);
   TheCondenserFanAntiSweatBehaviorShouldBe(DISABLED);
   GridAreaShouldBe(GridArea_1);
   GridCareVotesShouldBeDontCare();

   WhenFreezerThermistorIsValidResolvedIs(Valid);
   WhenTheGridIsRun();
   GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low);
}
