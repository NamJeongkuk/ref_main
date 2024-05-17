/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "IceCabinetHeaterControl.h"
#include "Constants_Binary.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "uassert_test.h"

enum
{
   Inactive = false,
   Active = true
};

enum
{
   FreezerSetpoint1 = -100,
   FreezerSetpoint2 = -200,
   IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOffAndIceFormationActive = 95,
   IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOnAndIceFormationInactive = 100,
   IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOffAndIceFormationInactive = 95,
   IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOnAndIceFormationActive = 100,
   IceCabinetHeaterDutyCycleWithFreezerSetpoint2AndFanOffAndIceFormationInactive = 100
};

static const IceCabinetHeaterControlConfig_t config = {
   .iceFormationIsActiveErd = Erd_IceCabinet_IceFormationIsActive,
   .iceCabinetFanSpeedVoteErd = Erd_IceCabinetFanSpeed_ResolvedVote,
   .iceCabinetHeaterVoteErd = Erd_IceCabinetGasketHeater_IceCabinetHeaterControlVote,
   .freezerResolvedSetpointErd = Erd_Freezer_ResolvedSetpointInDegFx100
};

static const IceCabinetHeaterData_t iceCabinetHeaterData = {
   .iceCabinetHeaterMinDutyCyclePercentage = 0,
   .iceCabinetHeaterInterceptCoefficient = 0,
   .iceCabinetHeaterIceFormationModeCoefficient = 10000,
   .iceCabinetHeaterIceCabinetFanCoefficient = 1480,
   .iceCabinetHeaterFreezerSetpointSlopeCoefficient = -95
};

TEST_GROUP(IceCabinetHeaterControl)
{
   IceCabinetHeaterControl_t instance;
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;
   }

   void GivenModuleIsInitialized()
   {
      IceCabinetHeaterControl_Init(
         &instance,
         dataModel,
         &config,
         &iceCabinetHeaterData);
   }

   void GivenIceFormationIs(bool state)
   {
      DataModel_Write(dataModel, Erd_IceCabinet_IceFormationIsActive, &state);
   }

   void WhenIceFormationIs(bool state)
   {
      GivenIceFormationIs(state);
   }

   void GivenIceCabinetFanIs(FanSpeed_t fanSpeed)
   {
      FanVotedSpeed_t fanVote = {
         .speed = fanSpeed,
         .care = Vote_Care
      };
      DataModel_Write(dataModel, Erd_IceCabinetFanSpeed_ResolvedVote, &fanVote);
   }

   void WhenIceCabinetFanIs(FanSpeed_t fanSpeed)
   {
      GivenIceCabinetFanIs(fanSpeed);
   }

   void TheDutyCycleInPercentShouldBe(PercentageDutyCycle_t expectedPercentageDutyCycle)
   {
      PercentageDutyCycleVote_t actual;
      DataModel_Read(dataModel, Erd_IceCabinetGasketHeater_IceCabinetHeaterControlVote, &actual);
      CHECK_EQUAL(expectedPercentageDutyCycle, actual.percentageDutyCycle);
   }

   void WhenFreezerResolvedSetpointIs(TemperatureDegFx100_t setpoint)
   {
      DataModel_Write(dataModel, Erd_Freezer_ResolvedSetpointInDegFx100, &setpoint);
   }
};

TEST(IceCabinetHeaterControl, ShouldCalculateIceCabinetHeaterDutyCycleWhenIceFormationChangesToActive)
{
   GivenIceFormationIs(Inactive);
   GivenIceCabinetFanIs(FanSpeed_Off);
   GivenModuleIsInitialized();
   WhenFreezerResolvedSetpointIs(FreezerSetpoint1);

   WhenIceFormationIs(Active);
   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOffAndIceFormationActive);
}

TEST(IceCabinetHeaterControl, ShouldCalculateIceCabinetHeaterDutyCycleWhenIceCabinetFanChangesToOn)
{
   GivenIceFormationIs(Inactive);
   GivenIceCabinetFanIs(FanSpeed_Off);
   GivenModuleIsInitialized();
   WhenFreezerResolvedSetpointIs(FreezerSetpoint1);

   WhenIceCabinetFanIs(FanSpeed_On);
   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOnAndIceFormationInactive);
}

TEST(IceCabinetHeaterControl, ShouldCalculateIceCabinetHeaterDutyCycleWhenIceFormationChangesToInactive)
{
   GivenIceCabinetFanIs(FanSpeed_Off);
   GivenModuleIsInitialized();
   WhenIceFormationIs(Active);
   WhenFreezerResolvedSetpointIs(FreezerSetpoint1);

   WhenIceFormationIs(Inactive);
   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOffAndIceFormationInactive);
}

TEST(IceCabinetHeaterControl, ShouldCalculateIceCabinetHeaterDutyCycleWhenIceCabinetFanChangesToOff)
{
   GivenIceFormationIs(Inactive);
   GivenModuleIsInitialized();
   WhenIceCabinetFanIs(FanSpeed_On);
   WhenFreezerResolvedSetpointIs(FreezerSetpoint1);

   WhenIceCabinetFanIs(FanSpeed_Off);
   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOffAndIceFormationInactive);
}

TEST(IceCabinetHeaterControl, ShouldCalculateIceCabinetHeaterDutyCycleWhenIceFormationChangesToActiveAndIceCabinetFanChangesToOn)
{
   GivenModuleIsInitialized();
   WhenIceFormationIs(Active);
   WhenIceCabinetFanIs(FanSpeed_On);
   WhenFreezerResolvedSetpointIs(FreezerSetpoint1);

   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOnAndIceFormationActive);
}

TEST(IceCabinetHeaterControl, ShouldCalculateIceCabinetHeaterDutyCycleWhenIceFormationChangesToInactiveAndIceCabinetFanChangesToOff)
{
   GivenModuleIsInitialized();
   WhenIceFormationIs(Active);
   WhenIceCabinetFanIs(FanSpeed_On);
   WhenFreezerResolvedSetpointIs(FreezerSetpoint1);

   WhenIceFormationIs(Inactive);
   WhenIceCabinetFanIs(FanSpeed_Off);
   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOffAndIceFormationInactive);
}

TEST(IceCabinetHeaterControl, ShouldCalculateIceCabinetHeaterDutyCycleWhenFreezerResolvedSetpointChangesToAnotherSetpoint)
{
   GivenIceFormationIs(Inactive);
   GivenIceCabinetFanIs(FanSpeed_Off);
   GivenModuleIsInitialized();
   WhenFreezerResolvedSetpointIs(FreezerSetpoint1);
   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint1AndFanOffAndIceFormationInactive);

   WhenFreezerResolvedSetpointIs(FreezerSetpoint2);
   TheDutyCycleInPercentShouldBe(IceCabinetHeaterDutyCycleWithFreezerSetpoint2AndFanOffAndIceFormationInactive);
}
