/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "AugerMotorController.h"
#include "AugerMotorVotedIceType.h"
#include "AugerMotorControllerFsmState.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   PressureReliefTimeInMsec = 10,
   DelayBetweenRelayOperationsInMsec = 20,
};

static const AugerMotorControllerConfig_t config = {
   .augerMotorControllerFsmStateErd = Erd_AugerMotorControllerFsmState,
   .augerMotorIceTypeVoteErd = Erd_AugerMotor_ResolvedVote,
   .augerMotorPowerErd = Erd_AugerMotorPower,
   .augerMotorDirectionErd = Erd_AugerMotorDirection,
   .timerModuleErd = Erd_TimerModule
};

static const AugerMotorData_t motorData = {
   .augerMotorReverseDirectionDelayInMsec = DelayBetweenRelayOperationsInMsec,
   .augerMotorDispenseCompleteInMsec = PressureReliefTimeInMsec,
   .augerMotorControlledByMainboard = true
};

TEST_GROUP(AugerMotorController)
{
   AugerMotorController_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      timerModuleDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenInitialization()
   {
      AugerMotorController_Init(
         &instance,
         dataModel,
         &config,
         &motorData);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleDouble, ticks, 1000);
   }

   void GivenTheAugerMotorIceTypeVoteIs(AugerMotorIceType_t iceType)
   {
      AugerMotorVotedIceType_t iceVote = {
         .iceType = iceType,
         .care = true
      };

      DataModel_Write(dataModel, Erd_AugerMotor_ResolvedVote, &iceVote);
   }

   void WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_t iceType)
   {
      GivenTheAugerMotorIceTypeVoteIs(iceType);
   }

   void TheAugerMotorPowerShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AugerMotorPower, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheAugerMotorPowerIs(bool augerMotorPower)
   {
      DataModel_Write(dataModel, Erd_AugerMotorPower, &augerMotorPower);
   }

   void GivenTheAugerMotorDirectionRelayIs(bool augerMotorPower)
   {
      DataModel_Write(dataModel, Erd_AugerMotorDirection, &augerMotorPower);
   }

   void TheAugerMotorDirectionRelayShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_AugerMotorDirection, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_t expected)
   {
      AugerMotorControllerFsmState_t actual;
      DataModel_Read(dataModel, Erd_AugerMotorControllerFsmState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheAugerMotorControllerIsInitalizedInTheCubedState()
   {
      GivenInitialization();
      GivenTheAugerMotorIceTypeVoteIs(AugerMotorIceType_Cubed);
      TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_Cubed);
   }

   void GivenTheAugerMotorControllerIsInitalizedInTheCrushedState()
   {
      GivenInitialization();
      GivenTheAugerMotorIceTypeVoteIs(AugerMotorIceType_Crushed);
      TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_Crushed);
   }
};

TEST(AugerMotorController, AugerMotorPowerShouldBeOnDirectionShouldBeOffWhenEnteringTheCubedState)
{
   GivenTheAugerMotorPowerIs(OFF);
   GivenTheAugerMotorDirectionRelayIs(OFF);
   GivenInitialization();

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Cubed);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_Cubed);
}

TEST(AugerMotorController, AugerMotorPowerShouldBeOffDirectionShouldBeOnWhenEnteringTheCrushedState)
{
   GivenTheAugerMotorPowerIs(OFF);
   GivenTheAugerMotorDirectionRelayIs(OFF);
   GivenInitialization();

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Crushed);
   TheAugerMotorDirectionRelayShouldBe(ON);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_Crushed);
}

TEST(AugerMotorController, AugerMotorPowerPowerShouldComeOnAfterDelayBetweenOperationsWhenInCrushed)
{
   GivenTheAugerMotorPowerIs(OFF);
   GivenTheAugerMotorDirectionRelayIs(OFF);
   GivenTheAugerMotorControllerIsInitalizedInTheCrushedState();

   After(DelayBetweenRelayOperationsInMsec - 1);
   TheAugerMotorPowerShouldBe(OFF);

   After(1);
   TheAugerMotorPowerShouldBe(ON);
}

TEST(AugerMotorController, AugerMotorPowerShouldBeOffThenReverseDirectionWhenTransitioningFromCubedToComplete)
{
   GivenTheAugerMotorControllerIsInitalizedInTheCubedState();
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Off);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(ON);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_MoveInReverseDirection);

   After(PressureReliefTimeInMsec - 1);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(ON);

   After(1);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_EnsureRelaysAreOff);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorDirectionRelayShouldBe(OFF);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_PreIdleDelay);
}

TEST(AugerMotorController, AugerMotorPowerShouldBeOffThenReverseDirectionWhenTransitioningFromCrushedToComplete)
{
   GivenTheAugerMotorControllerIsInitalizedInTheCrushedState();

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(ON);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Off);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_MoveInReverseDirection);

   After(PressureReliefTimeInMsec - 1);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(1);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_EnsureRelaysAreOff);
}

TEST(AugerMotorController, AugerMotorShouldGoBackToCubedStateIfCubedIceIsRequestedDuringDispenseCompleteAfterTheDirectionChangeDelay)
{
   GivenTheAugerMotorControllerIsInitalizedInTheCubedState();

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Off);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Cubed);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(ON);

   After(PressureReliefTimeInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_Cubed);
}

TEST(AugerMotorController, AugerMotorShouldGoToCrushedStateIfCrushedIceIsRequestedDuringDispenseCompleteAfterTheDirectionChangeDelay)
{
   GivenTheAugerMotorControllerIsInitalizedInTheCrushedState();

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(ON);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Off);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Crushed);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(PressureReliefTimeInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_Crushed);
}

TEST(AugerMotorController, ShouldSwitchFromCubedToCrushedStateWhenRequested)
{
   GivenTheAugerMotorControllerIsInitalizedInTheCrushedState();

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(ON);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Off);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(ON);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   WhenAugerMotorIceTypeVoteChangesTo(AugerMotorIceType_Cubed);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_DispensingComplete);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(PressureReliefTimeInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(OFF);
   TheAugerMotorDirectionRelayShouldBe(OFF);

   After(DelayBetweenRelayOperationsInMsec);
   TheAugerMotorPowerShouldBe(ON);
   TheAugerMotorDirectionRelayShouldBe(OFF);
   TheAugerMotorFsmStateShouldBe(AugerMotorControllerFsmState_Cubed);
}
