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
#include "ConstArrayMap_FourDoorDualEvap.h"
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

TEST_GROUP(Grid_DualEvap_Test)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   ConstArrayMap_FourDoorDualEvap_t fourDoorDualEvapConstArrayMap;
   const FreshFoodAdjustedSetpointData_t *freshFoodAdjustedSetpointData;
   EventSubscription_t dataModelOnChangeSubscription;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvapFourDoor);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      freshFoodAdjustedSetpointData = PersonalityParametricData_Get(dataModel)->setpointData->adjustedSetpointData->freshFoodAdjustedSetpointData;

      DataModelErdPointerAccess_Write(dataModel, Erd_TimerModule, &timerModuleTestDouble->timerModule);
      DataModelErdPointerAccess_Write(dataModel, Erd_CoolingStatesGridVotesConstArrayMapInterface, ConstArrayMap_FourDoorDualEvap_Init(&fourDoorDualEvapConstArrayMap));
   }

   static void DataModelChanged(void *context, const void *_args)
   {
      IGNORE(context);
      const DataModelOnDataChangeArgs_t *args = (const DataModelOnDataChangeArgs_t *)_args;

      if(args->erd == Erd_SealedSystemValvePosition_GridVote)
      {
         const SealedSystemValveVotedPosition_t *vote = (const SealedSystemValveVotedPosition_t *)args->data;

         mock()
            .actualCall("SealedSystemValveVoteChanged")
            .withParameter("position", vote->position)
            .withParameter("care", vote->care);
      }
      else if(args->erd == Erd_CompressorSpeed_GridVote)
      {
         const CompressorVotedSpeed_t *vote = (const CompressorVotedSpeed_t *)args->data;

         mock()
            .actualCall("CompressorSpeedVoteChanged")
            .withParameter("speed", vote->speed)
            .withParameter("care", vote->care);
      }
   }

   void TheSealedSystemValveVoteShouldChangeTo(SealedSystemValveVotedPosition_t vote)
   {
      mock()
         .expectOneCall("SealedSystemValveVoteChanged")
         .withParameter("position", vote.position)
         .withParameter("care", vote.care);
   }

   void TheCompressorSpeedVoteShouldChangeTo(CompressorVotedSpeed_t vote)
   {
      mock()
         .expectOneCall("CompressorSpeedVoteChanged")
         .withParameter("speed", vote.speed)
         .withParameter("care", vote.care);
   }

   void GivenDataModelSubscriptionIsInitialized()
   {
      EventSubscription_Init(
         &dataModelOnChangeSubscription,
         NULL,
         DataModelChanged);

      DataModel_SubscribeAll(
         dataModel,
         &dataModelOnChangeSubscription);
   }

   void GridBlockBecomes(GridBlockNumber_t actual)
   {
      DataModel_Write(dataModel, Erd_GridBlockNumberOverrideResolved, &actual);
   }

   void GridIsRun()
   {
      Grid_DualEvap(dataModel);
   }

   void CoolingModeIs(CoolingMode_t coolingMode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &coolingMode);
   }

   void CoolingSpeedIs(CoolingSpeed_t coolingSpeed)
   {
      DataModel_Write(dataModel, Erd_CoolingSpeed, &coolingSpeed);
   }

   void CoolingModeShouldBe(CoolingMode_t expected)
   {
      CoolingMode_t actual;
      DataModel_Read(dataModel, Erd_CoolingMode, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void CoolingSpeedShouldBe(CoolingSpeed_t expected)
   {
      CoolingSpeed_t actual;
      DataModel_Read(dataModel, Erd_CoolingSpeed, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void LowAmbientValveBehaviorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_LowAmbientValveBehaviorEnabledByGrid, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodPulldownOffsetShouldBe(TemperatureDegFx100_t expected)
   {
      TemperatureDegFx100_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodPulldownOffsetFromGrid, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void PulldownFanBehaviorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_PulldownFanBehaviorEnabledByGrid, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void PulldownValveBehaviorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_PulldownValveBehaviorEnabledByGrid, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void CondenserFanAntiSweatBehaviorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_CondenserFanAntiSweatBehaviorEnabledByGrid, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodAndFreezerIceMakersShouldBe(bool expected)
   {
      bool actualIceMakerEnabled;
      DataModel_Read(dataModel, Erd_IceMakerEnabledByGrid, &actualIceMakerEnabled);

      CHECK_EQUAL(expected, actualIceMakerEnabled);
   }

   void IceCabinetHighSpeedOverrideShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_IceCabinetHighSpeedOverrideEnabledByGrid, &actual);

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

   void UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_UseDelayedConvertibleCompartmentCoolingSpeedEnabledByGrid, &actual);
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

   void MaxTimeInValvePositionAShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_MaxValveTimeInPosAEnabled, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void FreshFoodThermistorValidResolvedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreshFoodThermistor_IsValidResolved, &state);
   }

   void FreezerThermistorValidResolvedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_FreezerThermistor_IsValidResolved, &state);
   }

   void BothThermistorsAreValid(void)
   {
      FreshFoodThermistorValidResolvedIs(Valid);
      FreezerThermistorValidResolvedIs(Valid);
   }

   void BothThermistorsAreInvalid(void)
   {
      FreshFoodThermistorValidResolvedIs(Invalid);
      FreezerThermistorValidResolvedIs(Invalid);
   }

   void FreshFoodEvapFanSpeedIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t actual = {
         .speed = speed,
         .care = true
      };

      DataModel_Write(dataModel, Erd_FreshFoodEvapFanSpeed_ResolvedVote, &actual);
   }

   void FreezerEvapFanSpeedIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t actual = {
         .speed = speed,
         .care = true
      };

      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_ResolvedVote, &actual);
   }

   void CoolConvertibleCompartmentBeforeOffIs(bool state)
   {
      DataModel_Write(dataModel, Erd_CoolConvertibleCompartmentBeforeOff, &state);
   }

   void DelayConvertibleCompartmentCoolingIs(bool state)
   {
      DataModel_Write(dataModel, Erd_DelayConvertibleCompartmentCooling, &state);
   }

   void UseDelayedConvertibleCompartmentCoolingSpeedIs(bool state)
   {
      DataModel_Write(dataModel, Erd_UseDelayedConvertibleCompartmentCoolingSpeedEnabledByGrid, &state);
   }

   void FeaturePanGridBlockNumberIs(GridBlockNumber_t gridBlockNumber)
   {
      DataModel_Write(dataModel, Erd_FeaturePanGrid_BlockNumber, &gridBlockNumber);
   }

   void GridVotesShouldBe(
      CompressorSpeed_t expectedCompressorSpeed,
      FanSpeed_t expectedCondenserFanSpeed,
      FanSpeed_t expectedFreezerEvapFanSpeed,
      FanSpeed_t expectedFreshFoodEvapFanSpeed,
      SealedSystemValvePosition_t expectedSealedSystemValvePosition)
   {
      CompressorVotedSpeed_t actualCompressorVotedSpeed;
      FanVotedSpeed_t actualCondenserFanVotedSpeed;
      FanVotedSpeed_t actualFreezerEvapFanVotedSpeed;
      FanVotedSpeed_t actualFreshFoodEvapFanVotedSpeed;
      SealedSystemValveVotedPosition_t actualSealedSystemValveVotedPosition;

      DataModel_Read(dataModel, Erd_CompressorSpeed_GridVote, &actualCompressorVotedSpeed);
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_GridVote, &actualCondenserFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &actualFreezerEvapFanVotedSpeed);
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &actualFreshFoodEvapFanVotedSpeed);
      DataModel_Read(dataModel, Erd_SealedSystemValvePosition_GridVote, &actualSealedSystemValveVotedPosition);

      CHECK_EQUAL(expectedCompressorSpeed, actualCompressorVotedSpeed.speed);
      CHECK_EQUAL(expectedCondenserFanSpeed, actualCondenserFanVotedSpeed.speed);
      CHECK_EQUAL(expectedFreezerEvapFanSpeed, actualFreezerEvapFanVotedSpeed.speed);
      CHECK_EQUAL(expectedFreshFoodEvapFanSpeed, actualFreshFoodEvapFanVotedSpeed.speed);
      CHECK_EQUAL(expectedSealedSystemValvePosition, actualSealedSystemValveVotedPosition.position);

      CHECK_TRUE(actualCompressorVotedSpeed.care);
      CHECK_TRUE(actualCondenserFanVotedSpeed.care);
      CHECK_TRUE(actualFreezerEvapFanVotedSpeed.care);
      CHECK_TRUE(actualFreshFoodEvapFanVotedSpeed.care);
      CHECK_TRUE(actualSealedSystemValveVotedPosition.care);
   }

   void AllGridVotesShouldBeDontCare()
   {
      CompressorVotedSpeed_t compressorSpeedVote;
      FanVotedSpeed_t fanSpeedVote;
      SealedSystemValveVotedPosition_t sealedSystemValvePositionVote;

      DataModel_Read(dataModel, Erd_CompressorSpeed_GridVote, &compressorSpeedVote);
      CHECK_FALSE(compressorSpeedVote.care);
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_GridVote, &fanSpeedVote);
      CHECK_FALSE(fanSpeedVote.care);
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &fanSpeedVote);
      CHECK_FALSE(fanSpeedVote.care);
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_GridVote, &fanSpeedVote);
      CHECK_FALSE(fanSpeedVote.care);
      DataModel_Read(dataModel, Erd_SealedSystemValvePosition_GridVote, &sealedSystemValvePositionVote);
      CHECK_FALSE(sealedSystemValvePositionVote.care);
   }
};

TEST(Grid_DualEvap_Test, ShouldVoteForSealedSystemValveBeforeCompressorSpeedVote)
{
   Given BothThermistorsAreValid();
   When GridBlockBecomes(0);

   GivenDataModelSubscriptionIsInitialized();

   TheSealedSystemValveVoteShouldChangeTo({ SealedSystemValvePosition_B, Vote_Care });
   TheCompressorSpeedVoteShouldChangeTo({ CompressorSpeed_SuperLow, Vote_Care });
   When The GridIsRun();
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks0And1And2)
{
   for(GridBlockNumber_t gridBlockNumber = 0; gridBlockNumber <= 2; gridBlockNumber++)
   {
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks0And1And2WhileFreshFoodThermistorIsInvalid)
{
   for(GridBlockNumber_t gridBlockNumber = 0; gridBlockNumber <= 2; gridBlockNumber++)
   {
      Given FreshFoodThermistorValidResolvedIs(Invalid);
      Given FreezerThermistorValidResolvedIs(Valid);
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks0And1And2WhileFreezerThermistorIsInvalid)
{
   for(GridBlockNumber_t gridBlockNumber = 0; gridBlockNumber <= 2; gridBlockNumber++)
   {
      Given FreshFoodThermistorValidResolvedIs(Valid);
      Given FreezerThermistorValidResolvedIs(Invalid);
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks0And1And2WhileBothThermistorsAreInvalid)
{
   for(GridBlockNumber_t gridBlockNumber = 0; gridBlockNumber <= 2; gridBlockNumber++)
   {
      Given BothThermistorsAreInvalid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      AllGridVotesShouldBeDontCare();
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks3And4)
{
   for(GridBlockNumber_t gridBlockNumber = 3; gridBlockNumber <= 4; gridBlockNumber++)
   {
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(ENABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low, FanSpeed_Low, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks5And6)
{
   for(GridBlockNumber_t gridBlockNumber = 5; gridBlockNumber <= 6; gridBlockNumber++)
   {
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_PullDown);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(freshFoodAdjustedSetpointData->pulldownOffsetInDegFx100);
      PulldownFanBehaviorShouldBe(ENABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_SuperLow, FanSpeed_SuperHigh, FanSpeed_Low, FanSpeed_Low, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks7And8And9)
{
   for(GridBlockNumber_t gridBlockNumber = 7; gridBlockNumber <= 9; gridBlockNumber++)
   {
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(ENABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks10And11WhileCoolingModeIsFreezer)
{
   for(GridBlockNumber_t gridBlockNumber = 10; gridBlockNumber <= 11; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_Freezer);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(ENABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks10And11WhileCoolingModeIsNotFreezer)
{
   for(GridBlockNumber_t gridBlockNumber = 10; gridBlockNumber <= 11; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(ENABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks12And19)
{
   GridBlockNumber_t gridBlockNumbers[] = { 12, 19 };
   for(uint8_t i = 0; i < 2; i++)
   {
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumbers[i]);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(ENABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock13)
{
   Given BothThermistorsAreValid();
   When GridBlockBecomes(13);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(ENABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_SuperHigh, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks14And15And16WhileCoolingSpeedIsHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 14; gridBlockNumber <= 16; gridBlockNumber++)
   {
      Given CoolingSpeedIs(CoolingSpeed_High);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks14And15And16WhileCoolingSpeedIsNotHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 14; gridBlockNumber <= 16; gridBlockNumber++)
   {
      Given CoolingSpeedIs(CoolingSpeed_Low);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks17And18WhileCoolingModeIsFreezerAndCoolingSpeedIsHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 17; gridBlockNumber <= 18; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_Freezer);
      Given CoolingSpeedIs(CoolingSpeed_High);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks17And18WhileCoolingModeIsNotFreezerAndCoolingSpeedIsHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 17; gridBlockNumber <= 18; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
      Given CoolingSpeedIs(CoolingSpeed_High);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks17And18WhileCoolingModeIsFreezerAndCoolingSpeedIsNotHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 17; gridBlockNumber <= 18; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_Freezer);
      Given CoolingSpeedIs(CoolingSpeed_Low);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks17And18WhileCoolingModeIsNotFreezerAndCoolingSpeedIsNotHigh)
{
   for(GridBlockNumber_t gridBlockNumber = 17; gridBlockNumber <= 18; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
      Given CoolingSpeedIs(CoolingSpeed_Low);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock20WhileFreshFoodEvapFanSpeedIsOff)
{
   Given FreshFoodEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(20);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock20WhileFreshFoodEvapFanSpeedIsLow)
{
   Given FreshFoodEvapFanSpeedIs(FanSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(20);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock20WhileFreshFoodEvapFanSpeedIsNotOffOrLow)
{
   Given FreshFoodEvapFanSpeedIs(FanSpeed_Medium);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(20);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks21And22And23WithLowCoolingSpeedWhileCoolingSpeedIsOff)
{
   for(GridBlockNumber_t gridBlockNumber = 21; gridBlockNumber <= 23; gridBlockNumber++)
   {
      Given CoolingSpeedIs(CoolingSpeed_Off);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks21And22And23WithCurrentCoolingSpeedWhileCoolingSpeedIsNotOff)
{
   for(GridBlockNumber_t gridBlockNumber = 21; gridBlockNumber <= 23; gridBlockNumber++)
   {
      Given CoolingSpeedIs(CoolingSpeed_Mid);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Mid);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks24And25And32WhileCoolingModeIsFreezerAndCoolingSpeedIsOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 24, 25, 32 };
   for(uint8_t i = 0; i < 3; i++)
   {
      Given CoolingModeIs(CoolingMode_Freezer);
      Given CoolingSpeedIs(CoolingSpeed_Off);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumbers[i]);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks24And25And32WhileCoolingModeIsNotFreezerAndCoolingSpeedIsOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 24, 25, 32 };
   for(uint8_t i = 0; i < 3; i++)
   {
      Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
      Given CoolingSpeedIs(CoolingSpeed_Off);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumbers[i]);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks24And25And32WhileCoolingModeIsFreezerAndCoolingSpeedIsNotOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 24, 25, 32 };
   for(uint8_t i = 0; i < 3; i++)
   {
      Given CoolingModeIs(CoolingMode_Freezer);
      Given CoolingSpeedIs(CoolingSpeed_High);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumbers[i]);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks24And25And32WhileCoolingModeIsNotFreezerAndCoolingSpeedIsNotOff)
{
   GridBlockNumber_t gridBlockNumbers[] = { 24, 25, 32 };
   for(uint8_t i = 0; i < 3; i++)
   {
      Given CoolingModeIs(CoolingMode_ConvertibleCompartment);
      Given CoolingSpeedIs(CoolingSpeed_High);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumbers[i]);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_FreshFood);
      CoolingSpeedShouldBe(CoolingSpeed_High);
      LowAmbientValveBehaviorShouldBe(DISABLED);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(ENABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock26WhileFreezerEvapFanSpeedIsHighAndCoolingSpeedIsHigh)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(26);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock26WhileFreezerEvapFanSpeedIsNotHighAndCoolingSpeedIsHigh)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_Low);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(26);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Medium, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock26WhileFreezerEvapFanSpeedIsHighAndCoolingSpeedIsNotHigh)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(26);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_High, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock26WhileFreezerEvapFanSpeedIsNotHighAndCoolingSpeedIsNotHigh)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_Low);
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(26);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock27WhileFreezerEvapFanSpeedIsHigh)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(27);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_High, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock27WhileFreezerEvapFanSpeedIsNotHigh)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(27);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Medium, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks28And29And30WithCurrentCoolingModeWhileCoolingModeIsOff)
{
   for(GridBlockNumber_t gridBlockNumber = 28; gridBlockNumber <= 30; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_Off);
      Given CoolingSpeedIs(CoolingSpeed_Off);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Off);
      CoolingSpeedShouldBe(CoolingSpeed_Off);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_D);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks28And29And30WithFreezerCoolingModeWhileCoolingModeIsNotOff)
{
   for(GridBlockNumber_t gridBlockNumber = 28; gridBlockNumber <= 30; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_FreshFood);
      Given CoolingSpeedIs(CoolingSpeed_Low);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock31WithTheMaintenanceOfCoolingModeAndCoolingSpeedAndFreezerEvapFanSpeed)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(31);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock33WhileCoolingSpeedIsHighAndFreezerEvapFanSpeedIsOff)
{
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(33);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Low, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock33WhileCoolingSpeedIsHighAndFreezerEvapFanSpeedIsLow)
{
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given FreezerEvapFanSpeedIs(FanSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(33);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Low, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock33WhileCoolingSpeedIsHighAndFreezerEvapFanSpeedIsNotOffOrLow)
{
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(33);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Medium, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock33WhileCoolingSpeedIsNotHighAndFreezerEvapFanSpeedIsOff)
{
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(33);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Low, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock33WhileCoolingSpeedIsNotHighAndFreezerEvapFanSpeedIsLow)
{
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given FreezerEvapFanSpeedIs(FanSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(33);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Low, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock33WhileCoolingSpeedIsNotHighAndFreezerEvapFanSpeedIsNotOffOrLow)
{
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(33);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock34WhileFreezerEvapFanSpeedIsOff)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(34);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Low, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock34WhileFreezerEvapFanSpeedIsLow)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(34);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Low, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock34WhileFreezerEvapFanSpeedIsNotOffOrLow)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(34);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Medium, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks35And36And37WithCoolingModeOffAndCoolingSpeedOffWhileCoolingModeIsOff)
{
   for(GridBlockNumber_t gridBlockNumber = 35; gridBlockNumber <= 37; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_Off);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Off);
      CoolingSpeedShouldBe(CoolingSpeed_Off);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_D);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks35And36And37WithCoolingModeFreezerAndCoolingSpeedLowWhileCoolingModeIsNotOff)
{
   for(GridBlockNumber_t gridBlockNumber = 35; gridBlockNumber <= 37; gridBlockNumber++)
   {
      Given CoolingModeIs(CoolingMode_FreshFood);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(SET);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4AndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedOffWhileCoolingModeIsFreezerAndCoolingSpeedIsOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4AndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolingSpeedIs(CoolingSpeed_Off);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Off);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedOffWhileCoolingModeIsFreshFoodAndCoolingSpeedIsOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4AndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_Off);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Off);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsNotFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4AndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4AndFreezerEvapFanSpeedIsOff)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsNotFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4AndFreezerEvapFanSpeedIsOff)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsSmallerThan4AndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(SET);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsNotFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsSmallerThan4AndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(SET);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsSmallerThan4AndFreezerEvapFanSpeedIsOff)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(SET);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsNotFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsSmallerThan4AndFreezerEvapFanSpeedIsOff)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(SET);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsClearAndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsNotFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsClearAndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsClearAndFreezerEvapFanSpeedIsOff)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock38WithCoolingSpeedLowWhileCoolingModeIsNotFreshFoodAndCoolingSpeedIsNotOffAndCoolConvertibleCompartmentBeforeOffIsClearAndFreezerEvapFanSpeedIsOff)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(38);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(SET);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Low, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock39WhileCoolingSpeedIsLow)
{
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given FreezerEvapFanSpeedIs(FanSpeed_Medium);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(39);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock39WhileCoolingSpeedIsOff)
{
   Given CoolingSpeedIs(CoolingSpeed_Off);
   Given FreezerEvapFanSpeedIs(FanSpeed_Medium);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(39);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock39WhileCoolingSpeedIsNotLowOrOff)
{
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given FreezerEvapFanSpeedIs(FanSpeed_Medium);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(39);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Medium, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock40WhileCoolingSpeedIsHighAndFreezerEvapFanSpeedIsOff)
{
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(40);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Off, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock40WhileCoolingSpeedIsHighAndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(40);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Low, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock40WhileCoolingSpeedIsNotHighAndFreezerEvapFanSpeedIsOff)
{
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(40);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Off, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock40WhileCoolingSpeedIsNotHighAndFreezerEvapFanSpeedIsNotOff)
{
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(40);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Low, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock41WhileFreezerEvapFanSpeedIsOff)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(41);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Off, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock41WhileFreezerEvapFanSpeedIsNotOff)
{
   Given FreezerEvapFanSpeedIs(FanSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(41);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(ENABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Low, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks42And43And44WhileDelayConvertibleCompartmentCoolingIsSetAndFeaturePanGridBlockNumberIsSmallerThan2)
{
   for(GridBlockNumber_t gridBlockNumber = 42; gridBlockNumber <= 44; gridBlockNumber++)
   {
      Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
      Given DelayConvertibleCompartmentCoolingIs(SET);
      Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
      Given FeaturePanGridBlockNumberIs(1);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(SET);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks42And43And44WhileDelayConvertibleCompartmentCoolingIsSetAndFeaturePanGridBlockNumberIsSmallerThan4AndGreaterThanOrEqualTo2)
{
   for(GridBlockNumber_t gridBlockNumber = 42; gridBlockNumber <= 44; gridBlockNumber++)
   {
      Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
      Given DelayConvertibleCompartmentCoolingIs(SET);
      Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
      Given FeaturePanGridBlockNumberIs(2);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Off);
      CoolingSpeedShouldBe(CoolingSpeed_Off);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_D);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks42And43And44WhileCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsSmallerThan4)
{
   for(GridBlockNumber_t gridBlockNumber = 42; gridBlockNumber <= 44; gridBlockNumber++)
   {
      Given CoolConvertibleCompartmentBeforeOffIs(SET);
      Given DelayConvertibleCompartmentCoolingIs(CLEAR);
      Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
      Given FeaturePanGridBlockNumberIs(3);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Freezer);
      CoolingSpeedShouldBe(CoolingSpeed_Low);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(SET);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

      GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks42And43And44WhileCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterOrEqualThan4)
{
   for(GridBlockNumber_t gridBlockNumber = 42; gridBlockNumber <= 44; gridBlockNumber++)
   {
      Given CoolConvertibleCompartmentBeforeOffIs(SET);
      Given DelayConvertibleCompartmentCoolingIs(CLEAR);
      Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
      Given FeaturePanGridBlockNumberIs(4);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Off);
      CoolingSpeedShouldBe(CoolingSpeed_Off);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_D);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlocks42And43And44WhileCoolConvertibleCompartmentBeforeOffIsClearAndFeaturePanGridBlockNumberIsSmallerThan4)
{
   for(GridBlockNumber_t gridBlockNumber = 42; gridBlockNumber <= 44; gridBlockNumber++)
   {
      Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
      Given DelayConvertibleCompartmentCoolingIs(CLEAR);
      Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
      Given FeaturePanGridBlockNumberIs(3);
      Given BothThermistorsAreValid();
      When GridBlockBecomes(gridBlockNumber);
      And The GridIsRun();

      CoolingModeShouldBe(CoolingMode_Off);
      CoolingSpeedShouldBe(CoolingSpeed_Off);
      LowAmbientValveBehaviorShouldBe(ENABLED);
      FreshFoodPulldownOffsetShouldBe(0);
      PulldownFanBehaviorShouldBe(DISABLED);
      PulldownValveBehaviorShouldBe(DISABLED);
      CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
      FreshFoodAndFreezerIceMakersShouldBe(ENABLED);
      IceCabinetHighSpeedOverrideShouldBe(DISABLED);
      MaxTimeInValvePositionAShouldBe(DISABLED);
      DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
      CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
      UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

      GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_D);
   }
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock45WhileCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsSmallerThan4)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(3);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(45);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(SET);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock45WhileCoolingModeIsFreshFoodAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(45);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock45WhileCoolingModeIsNotFreshFoodAndCoolConvertibleCompartmentBeforeOffIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo4)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolConvertibleCompartmentBeforeOffIs(SET);
   Given DelayConvertibleCompartmentCoolingIs(CLEAR);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(4);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(45);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Off);
   CoolingSpeedShouldBe(CoolingMode_Off);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_D);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock45WhileDelayConvertibleCompartmentCoolingIsSetAndFeaturePanGridBlockNumberIsSmallerThan2)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
   Given DelayConvertibleCompartmentCoolingIs(SET);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(1);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(45);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Freezer);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(SET);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(SET);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_High, FanSpeed_Off, SealedSystemValvePosition_B);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock45WhileCoolingModeIsFreshFoodAndDelayConvertibleCompartmentCoolingIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo2)
{
   Given CoolingModeIs(CoolingMode_FreshFood);
   Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
   Given DelayConvertibleCompartmentCoolingIs(SET);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(2);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(45);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock45WhileCoolingModeIsNotFreshFoodAndDelayConvertibleCompartmentCoolingIsSetAndFeaturePanGridBlockNumberIsGreaterThanOrEqualTo2)
{
   Given CoolingModeIs(CoolingMode_Freezer);
   Given CoolConvertibleCompartmentBeforeOffIs(CLEAR);
   Given DelayConvertibleCompartmentCoolingIs(SET);
   Given UseDelayedConvertibleCompartmentCoolingSpeedIs(CLEAR);
   Given FeaturePanGridBlockNumberIs(2);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(45);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_Off);
   CoolingSpeedShouldBe(CoolingSpeed_Off);
   LowAmbientValveBehaviorShouldBe(ENABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   CondenserFanAntiSweatBehaviorShouldBe(ENABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Off, FanSpeed_Off, FanSpeed_Off, FanSpeed_Off, SealedSystemValvePosition_D);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock46WhileCoolingSpeedIsOff)
{
   Given CoolingSpeedIs(CoolingSpeed_Off);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(46);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Low);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Low, FanSpeed_Low, FanSpeed_Off, FanSpeed_Low, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock46WhileCoolingSpeedIsNotOff)
{
   Given CoolingSpeedIs(CoolingSpeed_Mid);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(46);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Off, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock47WhileCoolingSpeedIsHigh)
{
   Given CoolingSpeedIs(CoolingSpeed_High);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(47);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Off, FanSpeed_High, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock47WhileCoolingSpeedIsNotHigh)
{
   Given CoolingSpeedIs(CoolingSpeed_Low);
   Given BothThermistorsAreValid();
   When GridBlockBecomes(47);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_Mid);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(ENABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_Medium, FanSpeed_Medium, FanSpeed_Off, FanSpeed_Medium, SealedSystemValvePosition_A);
}

TEST(Grid_DualEvap_Test, ShouldOutputCorrectValuesForBlock48)
{
   Given BothThermistorsAreValid();
   When GridBlockBecomes(48);
   And The GridIsRun();

   CoolingModeShouldBe(CoolingMode_FreshFood);
   CoolingSpeedShouldBe(CoolingSpeed_High);
   LowAmbientValveBehaviorShouldBe(DISABLED);
   PulldownFanBehaviorShouldBe(DISABLED);
   PulldownValveBehaviorShouldBe(DISABLED);
   IceCabinetHighSpeedOverrideShouldBe(DISABLED);
   MaxTimeInValvePositionAShouldBe(DISABLED);
   DelayConvertibleCompartmentCoolingShouldBe(CLEAR);
   CoolConvertibleCompartmentBeforeOffShouldBe(CLEAR);
   UseDelayedConvertibleCompartmentCoolingSpeedShouldBe(CLEAR);

   GridVotesShouldBe(CompressorSpeed_High, FanSpeed_High, FanSpeed_Off, FanSpeed_High, SealedSystemValvePosition_A);
}
