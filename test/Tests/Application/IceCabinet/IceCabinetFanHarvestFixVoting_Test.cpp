/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceCabinetFanHarvestFixVoting.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

const IceCabinetFanHarvestFixVotingConfig_t config = {
   .iceMakerStateErd = Erd_IceMaker0_StateMachineState,
   .iceCabinetFanHarvestFixVoteErd = Erd_IceCabinetFanSpeed_HarvestFixVote
};

TEST_GROUP(IceCabinetFanHarvestFixVoting)
{
   IceCabinetFanHarvestFixVoting_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenModuleIsInitialized()
   {
      IceCabinetFanHarvestFixVoting_Init(
         &instance,
         dataModel,
         &config);
   }

   void GivenIceMakerStateMachineStateIs(IceMakerStateMachineState_t iceMakerState)
   {
      DataModel_Write(dataModel, Erd_IceMaker0_StateMachineState, &iceMakerState);
   }

   void WhenIceMakerStateMachineStateIs(IceMakerStateMachineState_t iceMakerState)
   {
      GivenIceMakerStateMachineStateIs(iceMakerState);
   }

   void IceCabinetFanVoteShouldBe(FanVotedSpeed_t expected)
   {
      FanVotedSpeed_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetFanSpeed_HarvestFixVote, &actual);

      CHECK_EQUAL(expected.speed, actual.speed);
      CHECK_EQUAL(expected.care, actual.care);
   }
};

TEST(IceCabinetFanHarvestFixVoting, ShouldVoteOffForIceCabinetFanWhenIceMakerStateMachineStateIsHarvestFixOnInit)
{
   GivenIceMakerStateMachineStateIs(IceMakerStateMachineState_HarvestFix);
   GivenModuleIsInitialized();

   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_Care });
}

TEST(IceCabinetFanHarvestFixVoting, ShouldVoteDontCareForIceCabinetFanWhenIceMakerStateMachineStateIsNotHarvestFixOnInit)
{
   GivenIceMakerStateMachineStateIs(IceMakerStateMachineState_Harvest);
   GivenModuleIsInitialized();

   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(IceCabinetFanHarvestFixVoting, ShouldVoteDontCareForIceCabinetFanWhenIceMakerStateMachineStateChangesToNotHarvestFix)
{
   GivenIceMakerStateMachineStateIs(IceMakerStateMachineState_HarvestFix);
   GivenModuleIsInitialized();
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_Care });

   WhenIceMakerStateMachineStateIs(IceMakerStateMachineState_Harvest);
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });
}

TEST(IceCabinetFanHarvestFixVoting, ShouldVoteOffForIceCabinetFanWhenIceMakerStateMachineStateChangesToHarvestFix)
{
   GivenIceMakerStateMachineStateIs(IceMakerStateMachineState_Harvest);
   GivenModuleIsInitialized();
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_DontCare });

   WhenIceMakerStateMachineStateIs(IceMakerStateMachineState_HarvestFix);
   IceCabinetFanVoteShouldBe({ FanSpeed_Off, Vote_Care });
}
