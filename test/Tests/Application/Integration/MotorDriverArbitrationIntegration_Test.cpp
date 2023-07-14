/*!
 * @file
 * @brief Integration test for the motor driver on the application level
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "Application.h"
#include "SystemErds.h"
#include "Rx2xxResetSource.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"
#include "DataModelErdPointerAccess.h"
}

#include "CppUTest/TestHarness.h"
#include "ReferDataModel_TestDouble.h"
#include "Interrupt_TestDouble.h"

enum
{
   ValidAdcCount = 30336,
   InvalidAdcCount = 0,
   Valid = true,
   Invalid = false,
   TwistTrayMotorControllerPollingTimeInMsec = 150,
   TwistTrayMotorBrakingDurationInMsec = 1 * MSEC_PER_SEC,
   CoastingTimeInMs = 1000
};

TEST_GROUP(MotorDriverArbitrationIntegration)
{
   Application_t instance;
   ReferDataModel_TestDouble_t dataModelDouble;
   I_DataModel_t *dataModel;
   ResetReason_t resetReason;
   TimerModule_TestDouble_t *timerModuleTestDouble;
   Interrupt_TestDouble_t *interruptTestDouble;
   const TwistTrayIceMakerData_t *twistTrayIceMakerData;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelDouble);
      dataModel = dataModelDouble.dataModel;
      timerModuleTestDouble = ReferDataModel_TestDouble_GetTimerModuleTestDouble(&dataModelDouble);
      interruptTestDouble = (Interrupt_TestDouble_t *)DataModelErdPointerAccess_GetInterrupt(dataModel, Erd_SystemTickInterrupt);

      twistTrayIceMakerData = PersonalityParametricData_Get(dataModel)->iceMakerData->twistTrayIceMakerData;
   }

   void GivenTheApplicationHasBeenInitialized()
   {
      Application_Init(
         &instance,
         dataModel,
         resetReason);
   }

   void TheDamperMotorDriveEnableShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheDamperMotorDriveControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_FreshFoodDamperStepperMotorControlRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheTwistTrayMotorControlRequestShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorControlRequest, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TwistTrayIceMakerThermistorShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_ThermistorIsValid, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheTwistTrayMotorEnableShouldBe(bool expected)
   {
      bool actual;
      DataModel_Read(dataModel, Erd_TwistIceMakerMotorDriveEnable, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void GivenTheIceMakerThermistorAdcCountIs(AdcCounts_t count)
   {
      DataModel_Write(dataModel, Erd_TwistTrayIceMakerThermistor_AdcCount, &count);
   }

   void GivenTheIceMakerIsEnabled()
   {
      DataModel_Write(dataModel, Erd_IceMaker0EnableStatus, set);
      DataModel_Write(dataModel, Erd_IceMakerEnabledByGrid, set);
   }

   void WhenTheIceMakerThermistorAdcCountIs(AdcCounts_t count)
   {
      GivenTheIceMakerThermistorAdcCountIs(count);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(timerModuleTestDouble, ticks);
   }

   static TimerTicks_t DamperHomingTime(I_DataModel_t * dataModel)
   {
      return (PersonalityParametricData_Get(dataModel)->freshFoodDamperData->stepsToHome * (PersonalityParametricData_Get(dataModel)->freshFoodDamperData->delayBetweenStepEventsInMs + 1)) + 1;
   }

   void AfterNInterrupts(int numberOfInterrupts)
   {
      for(int i = 0; i < numberOfInterrupts; i++)
      {
         Interrupt_TestDouble_TriggerInterrupt(interruptTestDouble);
      }
   }

   void WhenTheMotorSwitchIsDebouncedHigh(void)
   {
      DataModel_Write(dataModel, Erd_Gpio_GPIO_IN_02, clear);
   }

   void WhenTheMotorSwitchIsDebouncedLow(void)
   {
      DataModel_Write(dataModel, Erd_Gpio_GPIO_IN_02, set);
   }

   void WhenTheDamperIsRequestToPosition(DamperPosition_t position)
   {
      DamperVotedPosition_t votedPosition = {
         .position = position,
         .care = Vote_Care
      };

      DataModel_Write(dataModel, Erd_FreshFoodDamperPosition_FactoryVote, &votedPosition);
   }

   void TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_t expected)
   {
      TwistTrayIceMakerMotorActionResult_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_MotorActionResult, &actual);

      CHECK_EQUAL(expected, actual);
   }

   void TheTwistTrayOperationShouldBe(TwistTrayIceMakerOperationState_t expected)
   {
      TwistTrayIceMakerOperationState_t actual;
      DataModel_Read(dataModel, Erd_TwistTrayIceMaker_OperationState, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void WhenTwistTrayMotorIsRequestedToHarvest()
   {
      IceMakerTestRequest_t testRequestValue = IceMakerTestRequest_Harvest;
      DataModel_Write(dataModel, Erd_TwistTrayIceMakerTestRequest, &testRequestValue);
   }

   void GivenTheDamperHasHomed()
   {
      TheDamperMotorDriveEnableShouldBe(ENABLED);
      TheDamperMotorDriveControlRequestShouldBe(ENABLED);
      TheTwistTrayMotorEnableShouldBe(DISABLED);

      After(DamperHomingTime(dataModel));

      TheDamperMotorDriveEnableShouldBe(DISABLED);
      TheDamperMotorDriveControlRequestShouldBe(DISABLED);
   }

   void WhenTheTwistTrayMotorHomes()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.initialHomingTwistPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedLow();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);
      AfterNInterrupts(TwistTrayMotorBrakingDurationInMsec);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Homed);
   }

   void WhenTheTwistTrayMotorIsDoneHarvesting()
   {
      AfterNInterrupts(twistTrayIceMakerData->harvestData.fullBucketDetectionPeriodSecX10 * 100);
      WhenTheMotorSwitchIsDebouncedLow();
      WhenTheMotorSwitchIsDebouncedHigh();
      WhenTheMotorSwitchIsDebouncedLow();
      AfterNInterrupts(CoastingTimeInMs);
      WhenTheMotorSwitchIsDebouncedHigh();
      WhenTheMotorSwitchIsDebouncedLow();

      WhenTheMotorSwitchIsDebouncedHigh();
      AfterNInterrupts(twistTrayIceMakerData->harvestData.homeLandingDelayPeriodSecX10 * 100);

      AfterNInterrupts(CoastingTimeInMs);

      After(TwistTrayMotorControllerPollingTimeInMsec);
      TheMotorActionResultShouldBe(TwistTrayIceMakerMotorActionResult_Harvested);
   }
};

TEST(MotorDriverArbitrationIntegration, DamperShouldHaveTheMotorAndBeHomingOnInitialization)
{
   GivenTheApplicationHasBeenInitialized();

   TheDamperMotorDriveEnableShouldBe(ENABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   After(DamperHomingTime(dataModel));

   TheDamperMotorDriveEnableShouldBe(DISABLED);
   TheDamperMotorDriveControlRequestShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldBeEnabledAndHomeAfterTheDamperIsDoneHoming)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);

   WhenTheTwistTrayMotorHomes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldNotBeEnabledAfterTheDamperIsDoneHomingIfTheThermistorIsInvalid)
{
   GivenTheIceMakerThermistorAdcCountIs(InvalidAdcCount);
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(DISABLED);
   TwistTrayIceMakerThermistorShouldBe(Invalid);
}

TEST(MotorDriverArbitrationIntegration, DamperShouldHaveMotorAfterItHomesAndTheTwistTrayHomesIfItRequestsTheMotorFirst)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TwistTrayIceMakerThermistorShouldBe(Valid);

   WhenTheTwistTrayMotorHomes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   WhenTheDamperIsRequestToPosition(DamperPosition_Open);
   WhenTwistTrayMotorIsRequestedToHarvest();

   TheDamperMotorDriveEnableShouldBe(ENABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldHaveMotorAfterItHomesAndTheDamperHomesIfItRequestsTheMotorFirst)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TwistTrayIceMakerThermistorShouldBe(Valid);

   WhenTheTwistTrayMotorHomes();
   TheTwistTrayMotorEnableShouldBe(DISABLED);

   WhenTwistTrayMotorIsRequestedToHarvest();
   WhenTheDamperIsRequestToPosition(DamperPosition_Open);

   TheTwistTrayMotorControlRequestShouldBe(ENABLED);
   TheTwistTrayMotorEnableShouldBe(ENABLED);
   TheDamperMotorDriveEnableShouldBe(DISABLED);
   TheDamperMotorDriveControlRequestShouldBe(ENABLED);
}

TEST(MotorDriverArbitrationIntegration, TwistTrayMotorShouldGiveUpControlAfterAHarvestCompletes)
{
   GivenTheIceMakerThermistorAdcCountIs(ValidAdcCount);
   GivenTheIceMakerIsEnabled();
   GivenTheApplicationHasBeenInitialized();
   GivenTheDamperHasHomed();

   WhenTheTwistTrayMotorHomes();
   WhenTwistTrayMotorIsRequestedToHarvest();
   WhenTheTwistTrayMotorIsDoneHarvesting();

   // This ensures we've exited the Twist Tray Ice Maker Harvest State
   After(twistTrayIceMakerData->harvestData.fillTubeHeaterOnTimeInSeconds * MSEC_PER_SEC);
   TheTwistTrayOperationShouldBe(TwistTrayIceMakerOperationState_FillingTrayWithWater);

   TheTwistTrayMotorControlRequestShouldBe(DISABLED);
   TheTwistTrayMotorEnableShouldBe(DISABLED);
}
