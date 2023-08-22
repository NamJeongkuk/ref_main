/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Grid_DualEvap.h"
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

TEST_GROUP(Grid_DualEvap_Test)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
   }

   void GridBlockBecomes(GridBlockNumber_t actual)
   {
      DataModel_Write(dataModel, Erd_GridBlockNumberOverrideResolved, &actual);
   }

   void GridIsRun()
   {
      Grid_DualEvap(dataModel);
   }

   void FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerEvapFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_GridVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void ValvePositionVoteShouldBe(ValvePosition_t expected)
   {
      ValveVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_ValvePosition_GridVote, &actual);

      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void IceMakerEnabledShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceMakerEnabledByGrid, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void DelayConvertibleCompartmentCoolingShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DelayConvertibleCompartmentCooling, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CoolConvertibleCompartmentBeforeOffShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &actual);
      CHECK_EQUAL(expected, actual);
   }
   void ActualCoolConvertibleCompartmentBeforeOffIs(bool actual)
   {
      DataModel_Write(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &actual);
   }

   void UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_DelayedConvertibleCompartmentCoolingLowSpeed, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodPulldownOffsetENABLEDShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodPulldownOffsetEnabled, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CompressorSpeedVoteShouldBe(CompressorSpeed_t expected)
   {
      CompressorVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CompressorSpeed_GridVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void ActualConvertibleCompartmentGridBlockIs(GridBlockNumber_t blockNumber)
   {
      DataModel_Write(dataModel, Erd_ConvertibleCompartmentGridBlockNumber, &blockNumber);
   }

   void MaxTimeInValvePositionAShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_MaxValveTimeInPosAEnabled, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ActualValvePositionIs(ValvePosition_t position)
   {
      ValveVotedPosition_t actual = {
         .position = position,
         .care = true,
      };

      DataModel_Write(dataModel, Erd_ValvePosition_ResolvedVote, &actual);
   }

   void ActualCompressorSpeedIs(CompressorSpeed_t speed)
   {
      CompressorVotedSpeed_t actual = {
         .speed = speed,
         .care = true
      };

      DataModel_Write(dataModel, Erd_CompressorSpeed_ResolvedVote, &actual);
   }

   void ActualFreshFoodEvapSpeedIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t actual = {
         .speed = speed,
         .care = true
      };

      DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote, &actual);
   }

   void ActualFreezerEvapSpeedIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t actual = {
         .speed = speed,
         .care = true
      };

      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote, &actual);
   }

   void ActualDelayConvertibleCompartmentCoolingIs(bool actual)
   {
      DataModel_Write(dataModel, Erd_DelayConvertibleCompartmentCooling, &actual);
   }

   void AllGridVotesShouldBeDontCare()
   {
      FanVotedSpeed_t fanSpeedVote;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_GridVote, &fanSpeedVote);
      CHECK_FALSE(fanSpeedVote.care);
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &fanSpeedVote);
      CHECK_FALSE(fanSpeedVote.care);
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &fanSpeedVote);
      CHECK_FALSE(fanSpeedVote.care);
   }
};

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks0And1And2)
{
   for(GridBlockNumber_t gridBlockNumber = 0; gridBlockNumber <= 2; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);
      And CondenserFanSpeedVoteShouldBe(FanSpeed_SuperHigh);
      And ValvePositionVoteShouldBe(ValvePosition_B);
      And IceMakerEnabledShouldBe(DISABLED);
      And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
      And CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
      And FreshFoodPulldownOffsetENABLEDShouldBe(DISABLED);
      And MaxTimeInValvePositionAShouldBe(DISABLED);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks3And4And5And6)
{
   for(GridBlockNumber_t gridBlockNumber = 3; gridBlockNumber <= 6; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      The CondenserFanSpeedVoteShouldBe(FanSpeed_SuperHigh);
      And ValvePositionVoteShouldBe(ValvePosition_A);
      And IceMakerEnabledShouldBe(DISABLED);
      And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
      And CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
      And FreshFoodPulldownOffsetENABLEDShouldBe(ENABLED);
      And MaxTimeInValvePositionAShouldBe(DISABLED);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks7And8And9)
{
   for(GridBlockNumber_t gridBlockNumber = 7; gridBlockNumber <= 9; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And CondenserFanSpeedVoteShouldBe(FanSpeed_High);
      And ValvePositionVoteShouldBe(ValvePosition_B);
      And IceMakerEnabledShouldBe(ENABLED);
      And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
      And CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      And FreshFoodPulldownOffsetENABLEDShouldBe(DISABLED);
      And MaxTimeInValvePositionAShouldBe(DISABLED);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks10And11)
{
   for(GridBlockNumber_t gridBlockNumber = 10; gridBlockNumber <= 11; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);

      // Need to test when valve is in non B or B position
      And The ActualValvePositionIs(ValvePosition_C);
      And The GridIsRun();

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And CondenserFanSpeedVoteShouldBe(FanSpeed_High);
      And ValvePositionVoteShouldBe(ValvePosition_A);
      And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
      And CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      And MaxTimeInValvePositionAShouldBe(DISABLED);

      // Test Valve B position
      When The ActualValvePositionIs(ValvePosition_B);
      And The GridIsRun();

      And ValvePositionVoteShouldBe(ValvePosition_B);
      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks12And19)
{
   for(GridBlockNumber_t gridBlockNumber = 12; gridBlockNumber <= 19; gridBlockNumber += 7)
   {
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And CondenserFanSpeedVoteShouldBe(FanSpeed_High);

      And ValvePositionVoteShouldBe(ValvePosition_A);
      And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
      And CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      And MaxTimeInValvePositionAShouldBe(DISABLED);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock13)
{
   When GridBlockBecomes(13);
   And The GridIsRun();

   The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
   And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
   And CondenserFanSpeedVoteShouldBe(FanSpeed_SuperHigh);
   And CompressorSpeedVoteShouldBe(CompressorSpeed_High);
   And MaxTimeInValvePositionAShouldBe(DISABLED);
   And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks14And15And16)
{
   for(GridBlockNumber_t gridBlockNumber = 14; gridBlockNumber <= 16; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And ActualCompressorSpeedIs(CompressorSpeed_High);
      And GridIsRun();

      The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And The CondenserFanSpeedVoteShouldBe(FanSpeed_High);
      And The CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      The ValvePositionVoteShouldBe(ValvePosition_B);
      And FreshFoodPulldownOffsetENABLEDShouldBe(DISABLED);
      The IceMakerEnabledShouldBe(ENABLED);
      And The DelayConvertibleCompartmentCoolingShouldBe(ENABLED);
      And The CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
      And MaxTimeInValvePositionAShouldBe(DISABLED);

      When ActualCompressorSpeedIs(CompressorSpeed_Low);
      And GridIsRun();

      The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
      And The CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
      And The CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks17And18)
{
   for(GridBlockNumber_t gridBlockNumber = 17; gridBlockNumber <= 18; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And ActualCompressorSpeedIs(CompressorSpeed_High);
      And ActualValvePositionIs(ValvePosition_B);
      And GridIsRun();

      MaxTimeInValvePositionAShouldBe(DISABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

      The ValvePositionVoteShouldBe(ValvePosition_B);
      The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And The CondenserFanSpeedVoteShouldBe(FanSpeed_High);

      The CompressorSpeedVoteShouldBe(CompressorSpeed_High);

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);

      When The ActualValvePositionIs(ValvePosition_C);
      And GridIsRun();

      The ValvePositionVoteShouldBe(ValvePosition_A);

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);

      When ActualCompressorSpeedIs(CompressorSpeed_Medium);
      And GridIsRun();

      The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
      And The CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);

      The CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock20)
{
   When GridBlockBecomes(20);
   And ActualFreshFoodEvapSpeedIs(FanSpeed_Low);
   And GridIsRun();

   The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
   And The CompressorSpeedVoteShouldBe(CompressorSpeed_High);
   And The CondenserFanSpeedVoteShouldBe(FanSpeed_High);

   And The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Low);

   And The ValvePositionVoteShouldBe(ValvePosition_A);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);

   And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

   When ActualFreshFoodEvapSpeedIs(FanSpeed_Medium);
   And GridIsRun();
   The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks21And22And23)
{
   for(GridBlockNumber_t gridBlockNumber = 21; gridBlockNumber <= 23; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And ActualCompressorSpeedIs(CompressorSpeed_High);
      And GridIsRun();

      FreshFoodPulldownOffsetENABLEDShouldBe(DISABLED);
      And IceMakerEnabledShouldBe(ENABLED);
      And MaxTimeInValvePositionAShouldBe(DISABLED);
      And DelayConvertibleCompartmentCoolingShouldBe(ENABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

      And CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      And The CondenserFanSpeedVoteShouldBe(FanSpeed_High);
      And The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);

      When The ActualCompressorSpeedIs(CompressorSpeed_Medium);
      And GridIsRun();

      The CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);
      The CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
      And The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Medium);

      When The ActualCompressorSpeedIs(CompressorSpeed_Low);
      And GridIsRun();

      The CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
      The CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
      And The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks24And25And32)
{
   GridBlockNumber_t gridblockNumbers[] = { 24, 25, 32 };
   for(uint8_t i = 0; i < 3; i++)
   {
      When GridBlockBecomes(gridblockNumbers[i]);
      And ActualCompressorSpeedIs(CompressorSpeed_High);
      And ActualValvePositionIs(ValvePosition_B);
      And GridIsRun();

      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(ENABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

      The ValvePositionVoteShouldBe(ValvePosition_B);
      The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      And The CondenserFanSpeedVoteShouldBe(FanSpeed_High);
      The CompressorSpeedVoteShouldBe(CompressorSpeed_High);

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);

      When The ActualValvePositionIs(ValvePosition_C);
      And GridIsRun();

      The ValvePositionVoteShouldBe(ValvePosition_A);

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);

      When ActualCompressorSpeedIs(CompressorSpeed_Medium);
      And GridIsRun();

      The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
      And The CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
      And The CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Medium);

      When The ActualValvePositionIs(ValvePosition_B);
      And GridIsRun();

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);

      When The ActualCompressorSpeedIs(CompressorSpeed_Low);
      And GridIsRun();

      The CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
      And The CompressorSpeedVoteShouldBe(CompressorSpeed_Low);

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      And The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);

      When The ActualValvePositionIs(ValvePosition_A);
      And GridIsRun();

      The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Low);
      And The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock26)
{
   When GridBlockBecomes(26);
   And ActualFreshFoodEvapSpeedIs(FanSpeed_Low);
   And The ActualCompressorSpeedIs(CompressorSpeed_High);
   And GridIsRun();

   The ValvePositionVoteShouldBe(ValvePosition_A);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

   The FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Medium);

   And The CompressorSpeedVoteShouldBe(CompressorSpeed_High);
   And The CondenserFanSpeedVoteShouldBe(FanSpeed_High);
   And The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);

   When The ActualCompressorSpeedIs(CompressorSpeed_Low);
   And GridIsRun();

   The CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);
   The CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
   The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock27)
{
   When GridBlockBecomes(27);
   And The ActualFreezerEvapSpeedIs(FanSpeed_Low);
   And GridIsRun();

   The ActualFreezerEvapSpeedIs(FanSpeed_Medium);

   And CompressorSpeedVoteShouldBe(CompressorSpeed_High);
   And CondenserFanSpeedVoteShouldBe(FanSpeed_High);
   And ValvePositionVoteShouldBe(ValvePosition_A);
   And MaxTimeInValvePositionAShouldBe(DISABLED);
   And DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks28And29And30)
{
   for(GridBlockNumber_t gridBlockNumber = 28; gridBlockNumber <= 30; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And ActualCompressorSpeedIs(CompressorSpeed_Off);
      And GridIsRun();

      FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);

      FreshFoodPulldownOffsetENABLEDShouldBe(DISABLED);
      And IceMakerEnabledShouldBe(ENABLED);
      And MaxTimeInValvePositionAShouldBe(DISABLED);
      And DelayConvertibleCompartmentCoolingShouldBe(ENABLED);
      And CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

      The ValvePositionVoteShouldBe(ValvePosition_D);

      And CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
      And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      And CondenserFanSpeedVoteShouldBe(FanSpeed_Off);

      When ActualCompressorSpeedIs(CompressorSpeed_Low);
      And GridIsRun();

      CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);
      CondenserFanSpeedVoteShouldBe(FanSpeed_Low);

      When ActualCompressorSpeedIs(CompressorSpeed_Medium);
      And GridIsRun();

      CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);
      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
      CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);

      When ActualCompressorSpeedIs(CompressorSpeed_High);
      And GridIsRun();

      CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      CondenserFanSpeedVoteShouldBe(FanSpeed_High);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock31)
{
   When GridBlockBecomes(31);
   And ActualCompressorSpeedIs(CompressorSpeed_Off);
   And ActualValvePositionIs(ValvePosition_A);
   And ActualCompressorSpeedIs(FanSpeed_Off);
   And ActualFreezerEvapSpeedIs(FanSpeed_Off);
   And ActualFreshFoodEvapSpeedIs(FanSpeed_Off);
   And GridIsRun();

   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(ENABLED);
   CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   ValvePositionVoteShouldBe(ValvePosition_D);

   When ActualCompressorSpeedIs(CompressorSpeed_Low);
   And GridIsRun();

   ValvePositionVoteShouldBe(ValvePosition_A);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Low);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);

   When ActualValvePositionIs(ValvePosition_B);
   And GridIsRun();

   ValvePositionVoteShouldBe(ValvePosition_B);
   And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks33And40)
{
   GridBlockNumber_t gridblockNumbers[] = { 33, 40 };
   for(uint8_t i = 0; i < 2; i++)
   {
      When GridBlockBecomes(gridblockNumbers[i]);
      And ActualCompressorSpeedIs(CompressorSpeed_High);
      And GridIsRun();

      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

      CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
      CondenserFanSpeedVoteShouldBe(FanSpeed_High);

      When ActualCompressorSpeedIs(CompressorSpeed_Medium);
      And GridIsRun();

      CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);
      FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
      CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks34And41)
{
   GridBlockNumber_t gridblockNumbers[] = { 34, 41 };
   for(uint8_t i = 0; i < 2; i++)
   {
      When GridBlockBecomes(gridblockNumbers[i]);
      And GridIsRun();

      FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);
      CondenserFanSpeedVoteShouldBe(FanSpeed_High);
      CompressorSpeedVoteShouldBe(CompressorSpeed_High);
      ValvePositionVoteShouldBe(ValvePosition_A);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesFor35And36And37)
{
   for(GridBlockNumber_t gridBlockNumber = 35; gridBlockNumber <= 37; gridBlockNumber++)
   {
      When GridBlockBecomes(gridBlockNumber);
      And ActualCompressorSpeedIs(CompressorSpeed_Off);
      And GridIsRun();

      FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);

      FreshFoodPulldownOffsetENABLEDShouldBe(DISABLED);
      IceMakerEnabledShouldBe(ENABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
      CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
      CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
      ValvePositionVoteShouldBe(ValvePosition_D);

      When ActualCompressorSpeedIs(CompressorSpeed_Low);
      And GridIsRun();

      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);
      CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
      CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38)
{
   When GridBlockBecomes(38);
   And ActualCompressorSpeedIs(CompressorSpeed_Off);
   And GridIsRun();

   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(ENABLED);

   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
   CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);

   When ActualCompressorSpeedIs(CompressorSpeed_High);
   And ActualConvertibleCompartmentGridBlockIs(0);
   And ActualValvePositionIs(ValvePosition_A);
   And ActualFreezerEvapSpeedIs(FanSpeed_High);
   And ActualFreshFoodEvapSpeedIs(FanSpeed_High);
   And ActualCoolConvertibleCompartmentBeforeOffIs(ENABLED);
   And GridIsRun();

   The FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Low);
   And FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Low);

   When ActualConvertibleCompartmentGridBlockIs(5);
   And ActualCoolConvertibleCompartmentBeforeOffIs(ON);
   And GridIsRun();

   CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
}
TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock39)
{
   When GridBlockBecomes(39);
   And ActualCompressorSpeedIs(CompressorSpeed_High);
   And GridIsRun();

   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

   CondenserFanSpeedVoteShouldBe(FanSpeed_High);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
   CompressorSpeedVoteShouldBe(CompressorSpeed_High);

   When ActualCompressorSpeedIs(CompressorSpeed_Medium);
   And GridIsRun();

   CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);

   When ActualCompressorSpeedIs(CompressorSpeed_Low);
   And GridIsRun();

   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Low);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
}
TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks42And43And44)
{
   for(GridBlockNumber_t gridBlockNumber = 35; gridBlockNumber <= 37; gridBlockNumber++)
   {
      When GridBlockBecomes(43);
      And ActualConvertibleCompartmentGridBlockIs(3);
      And ActualCoolConvertibleCompartmentBeforeOffIs(ENABLED);
      And GridIsRun();

      FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      IceMakerEnabledShouldBe(ENABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(DISABLED);

      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
      CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
      CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
      ValvePositionVoteShouldBe(ValvePosition_B);
      UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(ON);

      And ActualCoolConvertibleCompartmentBeforeOffIs(DISABLED);
      And GridIsRun();

      FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
      And CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
      And CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
      And ValvePositionVoteShouldBe(ValvePosition_D);
      And UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

      When ActualConvertibleCompartmentGridBlockIs(4);
      And GridIsRun();

      CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
      UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks45)
{
   When GridBlockBecomes(45);
   And ActualDelayConvertibleCompartmentCoolingIs(ENABLED);
   And ActualCoolConvertibleCompartmentBeforeOffIs(ENABLED);
   ActualValvePositionIs(ValvePosition_A);
   And ActualConvertibleCompartmentGridBlockIs(3);
   And ActualCompressorSpeedIs(CompressorSpeed_High);
   And GridIsRun();

   MaxTimeInValvePositionAShouldBe(DISABLED);

   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
   ValvePositionVoteShouldBe(ValvePosition_B);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(ENABLED);

   When ActualCoolConvertibleCompartmentBeforeOffIs(DISABLED);
   And GridIsRun();

   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Low);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
   ValvePositionVoteShouldBe(ValvePosition_A);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

   When ActualValvePositionIs(ValvePosition_B);
   And GridIsRun();

   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   ValvePositionVoteShouldBe(ValvePosition_D);

   When ActualCompressorSpeedIs(CompressorSpeed_Off);
   And GridIsRun();

   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Off);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Off);
   ValvePositionVoteShouldBe(ValvePosition_D);

   When ActualDelayConvertibleCompartmentCoolingIs(ENABLED);
   And ActualConvertibleCompartmentGridBlockIs(0);
   And ActualCoolConvertibleCompartmentBeforeOffIs(DISABLED);
   And GridIsRun();

   CoolConvertibleCompartmentBeforeOffShouldBe(ENABLED);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_High);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
   ValvePositionVoteShouldBe(ValvePosition_B);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(ENABLED);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks46)
{
   When GridBlockBecomes(46);
   ActualCompressorSpeedIs(CompressorSpeed_High);
   And GridIsRun();

   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);

   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
   CondenserFanSpeedVoteShouldBe(FanSpeed_High);
   CompressorSpeedVoteShouldBe(CompressorSpeed_High);

   When ActualCompressorSpeedIs(CompressorSpeed_Medium);
   And GridIsRun();

   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);

   When ActualCompressorSpeedIs(CompressorSpeed_Low);
   And GridIsRun();

   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Low);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Low);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks47)
{
   When GridBlockBecomes(47);
   ActualCompressorSpeedIs(CompressorSpeed_High);
   And GridIsRun();

   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ValvePositionVoteShouldBe(ValvePosition_A);

   CondenserFanSpeedVoteShouldBe(FanSpeed_High);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
   CompressorSpeedVoteShouldBe(CompressorSpeed_High);

   When ActualCompressorSpeedIs(CompressorSpeed_Low);
   And GridIsRun();

   CondenserFanSpeedVoteShouldBe(FanSpeed_Medium);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_Medium);
   CompressorSpeedVoteShouldBe(CompressorSpeed_Medium);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks48)
{
   When GridBlockBecomes(48);
   And GridIsRun();

   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(DISABLED);
   CoolConvertibleCompartmentBeforeOffShouldBe(DISABLED);
   UseDelayedConvertibleCompartmentCoolingLowSpeedShouldBe(DISABLED);
   FreezerEvapFanSpeedVoteShouldBe(FanSpeed_Off);
   ValvePositionVoteShouldBe(ValvePosition_A);
   CondenserFanSpeedVoteShouldBe(FanSpeed_High);
   FreshFoodEvapFanSpeedVoteShouldBe(FanSpeed_High);
   CompressorSpeedVoteShouldBe(CompressorSpeed_High);
}

TEST(Grid_DualEvap_Test, ShouldVoteDontCareForAllInitialVotes)
{
   When GridBlockBecomes(55); // Out of bounds means we are doing nothing (default switch)

   AllGridVotesShouldBeDontCare();
}
