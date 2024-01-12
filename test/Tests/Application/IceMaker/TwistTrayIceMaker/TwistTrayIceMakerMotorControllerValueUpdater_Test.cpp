/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "TwistTrayIceMakerMotorControllerValueUpdater.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "TwistTrayIceMakerMotorController_TestDouble.h"

enum
{
   PollingPeriodInMs = 150
};

static const TwistTrayIceMakerMotorControllerValueUpdaterConfig_t config = {
   .motorActionResultErd = Erd_IceMaker0_MotorActionResult,
   .motorOperationStateErd = Erd_IceMaker0_MotorOperationState,
   .motorErrorReasonErd = Erd_IceMaker0_MotorErrorReason
};

TEST_GROUP(TwistTrayIceMakerMotorControllerValueUpdater)
{
   TwistTrayIceMakerMotorControllerValueUpdater_t instance;
   TwistTrayIceMakerMotorController_TestDouble_t motorControllerTestDouble;

   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t *timerModuleTestDouble;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;

      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);

      TwistTrayIceMakerMotorController_TestDouble_Init(&motorControllerTestDouble);
   }

   void GivenTheModuleIsInitialized()
   {
      TwistTrayIceMakerMotorControllerValueUpdater_Init(&instance, dataModel, (TwistTrayIceMakerMotorController_t *)&motorControllerTestDouble, &config);
   }

   void WhenTheMotorActionResultIs(IceMakerMotorActionResult_t actionResult)
   {
      TwistTrayIceMakerMotorController_TestDouble_UpdateMotorActionResult(&motorControllerTestDouble, actionResult);
   }

   void TheMotorActionResultErdShouldBe(IceMakerMotorActionResult_t expected)
   {
      IceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorActionResult, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheMotorOperationStateIs(IceMakerMotorOperationState_t motorOperationState)
   {
      TwistTrayIceMakerMotorController_TestDouble_UpdateMotorOperationState(&motorControllerTestDouble, motorOperationState);
   }

   void TheMotorOperationStateErdShouldBe(IceMakerMotorActionResult_t expected)
   {
      IceMakerMotorOperationState_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorOperationState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheMotorErrorIs(IceMakerMotorErrorReason_t motorErrorReason)
   {
      TwistTrayIceMakerMotorController_TestDouble_UpdateMotorErrorReason(&motorControllerTestDouble, motorErrorReason);
   }

   void TheMotorErrorErdShouldBe(IceMakerMotorErrorReason_t expected)
   {
      IceMakerMotorErrorReason_t actual;
      DataModel_Read(dataModel, Erd_IceMaker0_MotorErrorReason, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }
};

TEST(TwistTrayIceMakerMotorControllerValueUpdater, ShouldInitialize)
{
   GivenTheModuleIsInitialized();
}

TEST(TwistTrayIceMakerMotorControllerValueUpdater, ShouldUpdateMotorActionResultEveryPollingTime)
{
   GivenTheModuleIsInitialized();
   TheMotorActionResultErdShouldBe(IceMakerMotorActionResult_NoAction);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_Harvesting);

   After(PollingPeriodInMs - 1);
   TheMotorActionResultErdShouldBe(IceMakerMotorActionResult_NoAction);

   After(1);
   TheMotorActionResultErdShouldBe(IceMakerMotorActionResult_Harvesting);

   WhenTheMotorActionResultIs(IceMakerMotorActionResult_Harvested);

   After(PollingPeriodInMs - 1);
   TheMotorActionResultErdShouldBe(IceMakerMotorActionResult_Harvesting);

   After(1);
   TheMotorActionResultErdShouldBe(IceMakerMotorActionResult_Harvested);
}

TEST(TwistTrayIceMakerMotorControllerValueUpdater, ShouldUpdateMotorOperationStateEveryPollingTime)
{
   GivenTheModuleIsInitialized();
   TheMotorOperationStateErdShouldBe(IceMakerMotorOperationState_Idle);

   WhenTheMotorOperationStateIs(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   After(PollingPeriodInMs - 1);
   TheMotorOperationStateErdShouldBe(IceMakerMotorOperationState_Idle);

   After(1);
   TheMotorOperationStateErdShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   WhenTheMotorOperationStateIs(IceMakerMotorOperationState_HomingReadyToLandInHomePosition);

   After(PollingPeriodInMs - 1);
   TheMotorOperationStateErdShouldBe(IceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   After(1);
   TheMotorOperationStateErdShouldBe(IceMakerMotorOperationState_HomingReadyToLandInHomePosition);
}

TEST(TwistTrayIceMakerMotorControllerValueUpdater, ShouldUpdateMotorErrorEveryPollingTime)
{
   GivenTheModuleIsInitialized();
   TheMotorErrorErdShouldBe(IceMakerMotorErrorReason_None);

   WhenTheMotorErrorIs(IceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);

   After(PollingPeriodInMs - 1);
   TheMotorErrorErdShouldBe(IceMakerMotorErrorReason_None);

   After(1);
   TheMotorErrorErdShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);

   WhenTheMotorErrorIs(IceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome);

   After(PollingPeriodInMs - 1);
   TheMotorErrorErdShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);

   After(1);
   TheMotorErrorErdShouldBe(IceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome);
}
