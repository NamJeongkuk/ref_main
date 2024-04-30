/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceCabinetForcedCoolingFreezerEvaporatorFanVoter.h"
#include "IceCabinetCoolingMode.h"
#include "SystemErds.h"
};

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

static const IceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig_t config = {
   .freezerEvapFanSpeedIceCabinetVoteErd = Erd_FreezerEvapFanSpeed_IceCabinetVote,
   .freezerEvapFanSpeedGridVoteErd = Erd_FreezerEvapFanSpeed_GridVote,
   .iceCabinetCoolingModeErd = Erd_IceCabinetCoolingMode
};

TEST_GROUP(IceCabinetForcedCoolingFreezerEvaporatorFanVoter)
{
   IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t instance;

   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenTheModuleIsInitialized()
   {
      IceCabinetForcedCoolingFreezerEvaporatorFanVoter_Init(&instance, dataModel, &config);
   }

   void GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_t mode)
   {
      DataModel_Write(dataModel, Erd_IceCabinetCoolingMode, &mode);
   }

   void WhenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_t mode)
   {
      GivenTheIceCabinetCoolingModeIs(mode);
   }

   void FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_IceCabinetVote, &actual);
      CHECK_EQUAL(expected, actual.speed);
      CHECK_TRUE(actual.care);
   }

   void GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_t speed)
   {
      FanVotedSpeed_t votedSpeed = { .speed = speed, .care = Vote_Care };
      DataModel_Write(dataModel, Erd_FreezerEvapFanSpeed_GridVote, &votedSpeed);
   }

   void WhenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_t speed)
   {
      GivenTheFreezerEvapFanGridVoteSpeedIs(speed);
   }

   void FreezerEvapFanSpeedIceCabinetVoteShouldBeDontCare()
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_FreezerEvapFanSpeed_IceCabinetVote, &actual);
      CHECK_FALSE(actual.care);
   }
};

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteLowForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsForcedCoolingAndFreezerEvapFanGridVoteSpeedIsOffOnInit)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_Off);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_Low);
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteLowForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsForcedCoolingAndFreezerEvapFanGridVoteSpeedIsSuperLowOnInit)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_SuperLow);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_Low);
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteLowForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsForcedCoolingAndFreezerEvapFanGridVoteSpeedIsLowOnInit)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_Low);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_Low);
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteMediumForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsForcedCoolingAndFreezerEvapFanGridVoteSpeedIsMediumOnInit)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_Medium);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_Medium);
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteHighForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsForcedCoolingAndFreezerEvapFanGridVoteSpeedIsHighOnInit)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_High);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_High);
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteSuperHighForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsForcedCoolingAndFreezerEvapFanGridVoteSpeedIsSuperHighOnInit)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_SuperHigh);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_SuperHigh);
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteDontCareForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsNotForcedCoolingOnInit)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_SuperHigh);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ActiveCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBeDontCare();
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteDontCareForFreezerEvapFanSpeedIceCabinetVoteWhenIceCabinetCoolingModeIsNotForcedCooling)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_SuperHigh);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_SuperHigh);

   WhenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_Neutral);
   FreezerEvapFanSpeedIceCabinetVoteShouldBeDontCare();
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteSuperHighForFreezerEvapFanSpeedIceCabinetVoteWhenGridVoteSpeedChangesToSuperHigh)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_Medium);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_Medium);

   WhenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_SuperHigh);
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_SuperHigh);
}

TEST(IceCabinetForcedCoolingFreezerEvaporatorFanVoter, ShouldVoteCustomSpeedForFreezerEvapFanSpeedIceCabinetVoteWhenGridVoteSpeedIsCustomSpeed)
{
   GivenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_Medium);
   GivenTheIceCabinetCoolingModeIs(IceCabinetCoolingMode_ForcedCooling);
   GivenTheModuleIsInitialized();
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_Medium);

   WhenTheFreezerEvapFanGridVoteSpeedIs(FanSpeed_NumberOfSpeeds + 1);
   FreezerEvapFanSpeedIceCabinetVoteShouldBe(FanSpeed_NumberOfSpeeds + 1);
}
