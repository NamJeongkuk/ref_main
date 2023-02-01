/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
#include "MotorDriverArbitrator.h"
#include "SystemErds.h"
#include "utils.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"

#define And
#define Given
#define The
#define Then
#define When

static const MotorDriverArbitratorConfiguration_t motorConfig = {
   .motor0ControlRequestErd = Erd_FreshFoodDamperStepperMotorControlRequest,
   .motor1ControlRequestErd = Erd_TwistIceMakerMotorControlRequest,
   .motor0DriveEnableErd = Erd_FreshFoodDamperStepperMotorDriveEnable,
   .motor1DriveEnableErd = Erd_TwistIceMakerMotorDriveEnable
};

TEST_GROUP(MotorDriverArbitrator)
{
   MotorDriverArbitrator_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
   }

   void Motor0ControlRequestErdIs(bool status)
   {
      DataModel_Write(dataModel, Erd_FreshFoodDamperStepperMotorControlRequest, &status);
   }

   void Motor1ControlRequestErdIs(bool status)
   {
      DataModel_Write(dataModel, Erd_TwistIceMakerMotorControlRequest, &status);
   }

   void Motor0DriveEnableErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void Motor1DriveEnableErdShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void ModuleIsInitialized()
   {
      MotorDriverArbitrator_Init(&instance, dataModel, &motorConfig);
   }

   void ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear()
   {
      MotorDriverArbitrator_Init(&instance, dataModel, &motorConfig);
      Motor0ControlRequestErdIs(CLEAR);
      Motor1ControlRequestErdIs(CLEAR);
   }

   void NothingShouldHappen()
   {
   }
};

TEST(MotorDriverArbitrator, ShouldInitializeWithMotor0ControlRequestAndMotor1ControlRequestBothBeingClear)
{
   ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();
}

TEST(MotorDriverArbitrator, ShouldOnlySetMotor0DriveEnableErdWhenOnlyMotor0ControlIsRequested)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor0ControlRequestErdIs(SET);
   Motor0DriveEnableErdShouldBe(SET);
   Motor1DriveEnableErdShouldBe(CLEAR);
}

TEST(MotorDriverArbitrator, ShouldOnlySetMotor1DriveEnableErdWhenOnlyMotor1ControlIsRequested)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor1ControlRequestErdIs(SET);
   Motor0DriveEnableErdShouldBe(CLEAR);
   Motor1DriveEnableErdShouldBe(SET);
}

TEST(MotorDriverArbitrator, ShouldNotSetMotor0DriveEnableErdWhenMotor0ControlIsNotRequested)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor0ControlRequestErdIs(SET);
   Then Motor0ControlRequestErdIs(CLEAR);
   Motor0DriveEnableErdShouldBe(CLEAR);
   Motor1DriveEnableErdShouldBe(CLEAR);
}

TEST(MotorDriverArbitrator, ShouldNotSetMotor1DriveEnableErdWhenMotor1ControlIsNotRequested)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor1ControlRequestErdIs(SET);
   Then Motor1ControlRequestErdIs(CLEAR);
   Motor0DriveEnableErdShouldBe(CLEAR);
   Motor1DriveEnableErdShouldBe(CLEAR);
}

TEST(MotorDriverArbitrator, ShouldOnlySetMotor0DriveEnableErdWhenMotor0ControlIsRequestedFirstThenMotor1ControlIsRequested)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor0ControlRequestErdIs(SET);
   And Motor1ControlRequestErdIs(SET);
   Motor0DriveEnableErdShouldBe(SET);
   Motor1DriveEnableErdShouldBe(CLEAR);
}

TEST(MotorDriverArbitrator, ShouldOnlySetMotor1DriveEnableErdWhenMotor1ControlIsRequestedFirstThenMotor0ControlIsRequested)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor1ControlRequestErdIs(SET);
   And Motor0ControlRequestErdIs(SET);
   Motor0DriveEnableErdShouldBe(CLEAR);
   Motor1DriveEnableErdShouldBe(SET);
}

TEST(MotorDriverArbitrator, ShouldSetMotor1DriveEnableErdWhenMotor0ControlRequestIsSetAndMotor1ControlRequestIsSetThenMotor0ControlRequestIsClear)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor0ControlRequestErdIs(SET);
   When Motor1ControlRequestErdIs(SET);
   Motor0DriveEnableErdShouldBe(SET);
   Motor1DriveEnableErdShouldBe(CLEAR);

   When Motor0ControlRequestErdIs(CLEAR);
   Motor0DriveEnableErdShouldBe(CLEAR);
   Motor1DriveEnableErdShouldBe(SET);
}

TEST(MotorDriverArbitrator, ShouldSetMotor0DriveEnableErdWhenMotor1ControlRequestIsSetAndMotor0ControlRequestIsSetThenMotor1ControlRequestIsClear)
{
   Given The ModuleIsInitializedAndMotor0ControlRequestAndMotor1ControlRequestAreBothClear();

   When Motor1ControlRequestErdIs(SET);
   When Motor0ControlRequestErdIs(SET);
   Motor0DriveEnableErdShouldBe(CLEAR);
   Motor1DriveEnableErdShouldBe(SET);

   When Motor1ControlRequestErdIs(CLEAR);
   Motor0DriveEnableErdShouldBe(SET);
   Motor1DriveEnableErdShouldBe(CLEAR);
}
