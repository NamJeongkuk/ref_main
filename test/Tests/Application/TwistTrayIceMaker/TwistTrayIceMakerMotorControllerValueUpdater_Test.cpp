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
   .motorActionResultErd = Erd_TwistTrayIceMaker_MotorActionResult,
   .motorOperationStateErd = Erd_TwistTrayIceMaker_MotorOperationState,
   .motorErrorReasonErd = Erd_TwistTrayIceMaker_MotorErrorReason
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

   void WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_t actionResult)
   {
      TwistTrayIceMakerMotorController_TestDouble_UpdateMotorActionResult(&motorControllerTestDouble, actionResult);
   }

   void TheMotorActionResultErdShouldBe(TwistTrayIceMakerMotorActionResult_t expected)
   {
      TwistTrayIceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_MotorActionResult, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheMotorOperationStateIs(TwistTrayIceMakerMotorOperationState_t motorOperationState)
   {
      TwistTrayIceMakerMotorController_TestDouble_UpdateMotorOperationState(&motorControllerTestDouble, motorOperationState);
   }

   void TheMotorOperationStateErdShouldBe(TwistTrayIceMakerMotorActionResult_t expected)
   {
      TwistTrayIceMakerMotorOperationState_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_MotorOperationState, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void WhenTheMotorErrorIs(TwistTrayIceMakerMotorErrorReason_t motorErrorReason)
   {
      TwistTrayIceMakerMotorController_TestDouble_UpdateMotorErrorReason(&motorControllerTestDouble, motorErrorReason);
   }

   void TheMotorErrorErdShouldBe(TwistTrayIceMakerMotorErrorReason_t expected)
   {
      TwistTrayIceMakerMotorErrorReason_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_MotorErrorReason, &actual);

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
   TheMotorActionResultErdShouldBe(TwistTrayIceMakerMotorActionResult_NoAction);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_Harvesting);

   After(PollingPeriodInMs - 1);
   TheMotorActionResultErdShouldBe(TwistTrayIceMakerMotorActionResult_NoAction);

   After(1);
   TheMotorActionResultErdShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);

   WhenTheMotorActionResultIs(TwistTrayIceMakerMotorActionResult_Harvested);

   After(PollingPeriodInMs - 1);
   TheMotorActionResultErdShouldBe(TwistTrayIceMakerMotorActionResult_Harvesting);

   After(1);
   TheMotorActionResultErdShouldBe(TwistTrayIceMakerMotorActionResult_Harvested);
}

TEST(TwistTrayIceMakerMotorControllerValueUpdater, ShouldUpdateMotorOperationStateEveryPollingTime)
{
   GivenTheModuleIsInitialized();
   TheMotorOperationStateErdShouldBe(TwistTrayIceMakerMotorOperationState_Idle);

   WhenTheMotorOperationStateIs(TwistTrayIceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   After(PollingPeriodInMs - 1);
   TheMotorOperationStateErdShouldBe(TwistTrayIceMakerMotorOperationState_Idle);

   After(1);
   TheMotorOperationStateErdShouldBe(TwistTrayIceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   WhenTheMotorOperationStateIs(TwistTrayIceMakerMotorOperationState_HomingReadyToLandInHomePosition);

   After(PollingPeriodInMs - 1);
   TheMotorOperationStateErdShouldBe(TwistTrayIceMakerMotorOperationState_HomingMakingSureTheTrayIsHome);

   After(1);
   TheMotorOperationStateErdShouldBe(TwistTrayIceMakerMotorOperationState_HomingReadyToLandInHomePosition);
}

TEST(TwistTrayIceMakerMotorControllerValueUpdater, ShouldUpdateMotorErrorEveryPollingTime)
{
   GivenTheModuleIsInitialized();
   TheMotorErrorErdShouldBe(TwistTrayIceMakerMotorErrorReason_None);

   WhenTheMotorErrorIs(TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);

   After(PollingPeriodInMs - 1);
   TheMotorErrorErdShouldBe(TwistTrayIceMakerMotorErrorReason_None);

   After(1);
   TheMotorErrorErdShouldBe(TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);

   WhenTheMotorErrorIs(TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome);

   After(PollingPeriodInMs - 1);
   TheMotorErrorErdShouldBe(TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingCheckingTrayIsHome);

   After(1);
   TheMotorErrorErdShouldBe(TwistTrayIceMakerMotorErrorReason_MotorStallsInStateHomingJumpingOutOfHome);
}
