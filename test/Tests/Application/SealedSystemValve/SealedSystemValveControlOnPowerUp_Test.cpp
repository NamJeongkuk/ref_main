/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SealedSystemValveControlOnPowerUp.h"
#include "Constants_Time.h"
#include "SystemErds.h"
#include "SealedSystemValveData.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

static SealedSystemValveControlOnPowerUpConfig_t config = {
   .sealedSystemValvePositionPowerUpVoteErd = Erd_SealedSystemValvePosition_PowerUpVote
};

TEST_GROUP(SealedSystemValveControlOnPowerUp)
{
   SealedSystemValveControlOnPowerUp_t instance;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   const SealedSystemValveData_t *sealedSystemValveData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      sealedSystemValveData = PersonalityParametricData_Get(dataModel)->sealedSystemValveData;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   void GivenTheModuleIsInitialized()
   {
      SealedSystemValveControlOnPowerUp_Init(
         &instance,
         dataModel,
         &config,
         sealedSystemValveData);
   }

   void TheSealedSystemValvePositionPowerUpVoteShouldBe(SealedSystemValvePosition_t expected)
   {
      SealedSystemValveVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_SealedSystemValvePosition_PowerUpVote, &actual);
      CHECK_EQUAL(expected, actual.position);
      CHECK_TRUE(actual.care);
   }

   void TheSealedSystemValvePositionPowerUpVoteShouldBeDonCare()
   {
      SealedSystemValveVotedPosition_t actual;
      DataModel_Read(dataModel, Erd_SealedSystemValvePosition_PowerUpVote, &actual);
      CHECK_FALSE(actual.care);
   }
};

TEST(SealedSystemValveControlOnPowerUp, ShouldVoteRefrigerantValvePowerUpServicePositionToSealedSystemValvePositionPowerUpVoteOnInit)
{
   GivenTheModuleIsInitialized();
   TheSealedSystemValvePositionPowerUpVoteShouldBe(sealedSystemValveData->refrigerantValvePowerUpServicePosition);
}

TEST(SealedSystemValveControlOnPowerUp, ShouldVoteDontCareToSealedSystemValvePositionPowerUpVoteAfterTimeout)
{
   GivenTheModuleIsInitialized();
   TheSealedSystemValvePositionPowerUpVoteShouldBe(sealedSystemValveData->refrigerantValvePowerUpServicePosition);

   After(sealedSystemValveData->refrigerantValvePowerUpTimeInSeconds * MSEC_PER_SEC - 1);
   TheSealedSystemValvePositionPowerUpVoteShouldBe(sealedSystemValveData->refrigerantValvePowerUpServicePosition);

   After(1);
   TheSealedSystemValvePositionPowerUpVoteShouldBeDonCare();
}
