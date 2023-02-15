/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "CompressorStartupFanVotes.h"
#include "Constants_Time.h"
#include "SystemErds.h"
#include "TddPersonality.h"
#include "Vote.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

#define Given
#define When
#define And
#define Then

static CompressorStartupFanVotesConfiguration_t configuration = {
   .compressorStateErd = Erd_CompressorState,
   .condenserFanSpeedVoteErd = Erd_CondenserFanSpeed_CompressorStartUpVote,
   .freezerEvaporatorFanSpeedVoteErd = Erd_FreezerEvapFanSpeed_CompressorStartUpVote,
   .freshFoodEvaporatorFanSpeedVoteErd = Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote,
   .coolingModeErd = Erd_CoolingMode,
};

TEST_GROUP(CompressorStartupFanVotes_SingleEvaporator)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   CompressorStartupFanVotes_t instance;
   const CompressorData_t *compressorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   }

   void CompressorStartupFanVotesIsInitialized()
   {
      CompressorStartupFanVotes_Init(&instance, dataModel, &configuration);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void CondenserFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }
};

TEST(CompressorStartupFanVotes_SingleEvaporator, ShouldBeDontCareForCondenserFanSpeedVoteWhenTheModuleIsInitialized)
{
   Given CompressorStartupFanVotesIsInitialized();
   CondenserFanSpeedVoteShouldBeDontCare();
}

TEST(CompressorStartupFanVotes_SingleEvaporator, ShouldVoteForCondenserFanLowWhenCompressorStateChangesToStartup)
{
   Given CompressorStateIs(CompressorState_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_SingleEvaporator, ShouldVoteForFreezerEvapFanLowWhenCompressorStateChangesToStartup)
{
   Given CompressorStateIs(CompressorState_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_SingleEvaporator, ShouldVoteDontCareForCondenserFanWhenStartupTimeExpires)
{
   Given CompressorStateIs(CompressorState_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);

   After(1);
   CondenserFanSpeedVoteShouldBeDontCare();
}

TEST(CompressorStartupFanVotes_SingleEvaporator, ShouldVoteDontCareForFreezerEvapFanLowWhenStartupTimeExpires)
{
   Given CompressorStateIs(CompressorState_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);

   After(1);
   FreezerEvaporatorFanSpeedVoteShouldBeDontCare();
}

TEST_GROUP(CompressorStartupFanVotes_DualEvaporator)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   CompressorStartupFanVotes_t instance;
   const CompressorData_t *compressorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentDualEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   }

   void CompressorStartupFanVotesIsInitialized()
   {
      CompressorStartupFanVotes_Init(&instance, dataModel, &configuration);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void CondenserFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }
};

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForCondenserFanLowWhenCompressorStateChangesToStartup)
{
   Given CompressorStateIs(CompressorState_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreshFoodEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsFreshFood)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_FreshFood);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreezerEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsFreshFood)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_FreshFood);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreshFoodEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsFreezer)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Freezer);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreezerEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsFreezer)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Freezer);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreshFoodEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsConvertibleCompartment)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreezerEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsConvertibleCompartment)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreshFoodEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsUnknown)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteForFreezerEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsUnknown)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_DualEvaporator, ShouldVoteDontCareForFreshFoodEvapFanWhenStartupTimeExpires)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_FreshFood);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);

   After(compressorData->compressorTimes.startupOnTimeInSeconds * MSEC_PER_SEC - 1);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);

   After(1);
   FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare();
}

TEST_GROUP(CompressorStartupFanVotes_TripleEvaporator)
{
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   CompressorStartupFanVotes_t instance;
   const CompressorData_t *compressorData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble, TddPersonality_DevelopmentTripleEvaporator);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   }

   void CompressorStartupFanVotesIsInitialized()
   {
      CompressorStartupFanVotes_Init(&instance, dataModel, &configuration);
   }

   void After(TimerTicks_t ticks, TimeSourceTickCount_t ticksToElapseAtATime = 1000)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks, ticksToElapseAtATime);
   }

   void CompressorStateIs(CompressorState_t state)
   {
      DataModel_Write(dataModel, Erd_CompressorState, &state);
   }

   void CoolingModeIs(CoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_CoolingMode, &mode);
   }

   void CondenserFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void CondenserFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_CondenserFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreshFoodEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreshFoodEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void FreezerEvaporatorFanSpeedVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_CompressorStartUpVote, &actual);

      CHECK_FALSE(actual.care);
   }
};

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForCondenserFanLowWhenCompressorStateChangesToStartup)
{
   Given CompressorStateIs(CompressorState_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   CondenserFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreshFoodEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsFreshFood)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_FreshFood);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreezerEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsFreshFood)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_FreshFood);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreshFoodEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsFreezer)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Freezer);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreezerEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsFreezer)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Freezer);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreshFoodEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsConvertibleCompartment)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreezerEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsConvertibleCompartment)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_ConvertibleCompartment);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreshFoodEvapFanOffWhenCompressorStateChangesToStartupAndCoolingModeIsUnknown)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreshFoodEvaporatorFanSpeedVoteShouldBe(FanSpeed_Off);
}

TEST(CompressorStartupFanVotes_TripleEvaporator, ShouldVoteForFreezerEvapFanLowWhenCompressorStateChangesToStartupAndCoolingModeIsUnknown)
{
   Given CompressorStateIs(CompressorState_Off);
   And CoolingModeIs(CoolingMode_Off);
   And CompressorStartupFanVotesIsInitialized();

   When CompressorStateIs(CompressorState_Startup);
   FreezerEvaporatorFanSpeedVoteShouldBe(FanSpeed_Low);
}
