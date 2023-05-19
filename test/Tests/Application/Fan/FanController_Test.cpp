/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "FanController.h"
#include "InputCaptureType.h"
#include "PwmDutyCycle.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "SystemErds.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "ReferDataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "uassert_test.h"
#include "PidController_Mock.h"

enum
{
   NumberOfPulsesPerRevolution = 4,
   PeriodicFanFrequencyInMsec = 200,

   SomeInputCaptureTimeInMicroSec = 8000,
   SomeOtherInputCaptureTimeInMicroSec = 4 * USEC_SEC,
   SomeSpeedInRpm = 2900,
   SomeOtherSpeedInRpm = 1000,
   SomeDutyCycle = 10,
   SomeOtherDutyCycle = 80,
   MaxDutyCycle = 100,
};

const FanControllerConfig_t fanConfig = {
   .calculatedFanControlErd = Erd_CalculatedCondenserFanControl,
   .inputCaptureErd = Erd_CondenserFan_InputCaptureTime,
   .fanPwmErd = Erd_CondenserFan_Pwm,
   .fanActualRpmErd = Erd_CondenserFan_ActualRpm
};

TEST_GROUP(FanController)
{
   ReferDataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleDouble;
   PidController_Mock_t pidMock;
   const CombinedFanData_t *combinedFanData;
   const FanData_t *condenserFanData;

   FanController_t instance;

   void setup()
   {
      ReferDataModel_TestDouble_Init(&dataModelTestDouble);
      dataModel = dataModelTestDouble.dataModel;

      combinedFanData = PersonalityParametricData_Get(dataModel)->fanData;
      condenserFanData = &combinedFanData->condenserFan;

      PidController_Mock_Init(&pidMock);
      instance._private.pidController.interface = pidMock.interface;

      TimerModule_TestDouble_Init(&timerModuleDouble);
   }

   void PidShouldReset(void)
   {
      mock().expectOneCall("ResetPid").onObject(&instance._private.pidController.interface);
   }

   void PidShouldExecute(void)
   {
      mock().expectOneCall("ExecutePid").onObject(&instance._private.pidController.interface).ignoreOtherParameters();
   }

   void GivenInitialization()
   {
      FanController_Init(
         &instance,
         dataModel,
         &timerModuleDouble.timerModule,
         condenserFanData,
         &fanConfig);

      instance._private.pidController.interface = pidMock.interface;
   }

   void GivenTheCalculatedClosedLoopFanSpeedIs(FanRpm_t rpm)
   {
      FanControl_t closedLoopControl;
      closedLoopControl.type = FanControlType_Rpm;
      closedLoopControl.rpm = rpm;
      DataModel_Write(dataModel, Erd_CalculatedCondenserFanControl, &closedLoopControl);
   }

   void GivenTheCalculatedOpenLoopFanSpeedIs(FanDutyCycle_t dutyCycle)
   {
      FanControl_t openLoopControl;
      openLoopControl.type = FanControlType_DutyCycle;
      openLoopControl.dutyCycle = dutyCycle;
      DataModel_Write(dataModel, Erd_CalculatedCondenserFanControl, &openLoopControl);
   }

   void WhenTheCalculatedOpenLoopFanSpeedChangesTo(FanDutyCycle_t dutyCycle)
   {
      GivenTheCalculatedOpenLoopFanSpeedIs(dutyCycle);
   }

   void WhenTheCalculatedClosedLoopFanSpeedChangesTo(FanRpm_t rpm)
   {
      GivenTheCalculatedClosedLoopFanSpeedIs(rpm);
   }

   void GivenTheFanInputCaptureIs(InputCaptureMicroSeconds_t inputCapture)
   {
      DataModel_Write(dataModel, Erd_CondenserFan_InputCaptureTime, &inputCapture);
   }

   void WhenTheFanInputCaptureChangesTo(InputCaptureMicroSeconds_t inputCapture)
   {
      GivenTheFanInputCaptureIs(inputCapture);
   }

   void FanPwmShouldBe(PwmDutyCycle_t expected)
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_CondenserFan_Pwm, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void ActualFanRpmShouldBe(FanRpm_t expected)
   {
      FanRpm_t actual;
      DataModel_Read(dataModel, Erd_CondenserFan_ActualRpm, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void After(TimerTicks_t ticks)
   {
      TimerModule_TestDouble_ElapseTime(&timerModuleDouble, ticks, 1);
   }

   PwmDutyCycle_t DutyCyclePercentageAsPwm(FanDutyCycle_t dutyCycle)
   {
      return (PwmDutyCycle_Max * dutyCycle) / 100;
   }
};

TEST(FanController, ShouldUpdateBothPwmAndActualRpmIfCalculatedFanControlIsClosedLoop)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(5125);
}

TEST(FanController, ShouldUpdateActualRpmPeriodicallyIfCalculatedFanControlIsClosedLoop)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   WhenTheFanInputCaptureChangesTo(SomeOtherInputCaptureTimeInMicroSec);

   After(PeriodicFanFrequencyInMsec - 1);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldExecute();
   After(1);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeOtherInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
}

TEST(FanController, ShouldUpdatePwmIfCalculatedFanControlIsClosedLoopAndInputCaptureChanges)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   WhenTheFanInputCaptureChangesTo(SomeOtherInputCaptureTimeInMicroSec);

   After(PeriodicFanFrequencyInMsec - 1);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldExecute();
   After(1);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeOtherInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
}

TEST(FanController, ShouldUpdateBothPwmAndActualRpmPeriodicallyWhenCalculatedFanControlChangesAndIsClosedLoop)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldExecute();
   WhenTheCalculatedClosedLoopFanSpeedChangesTo(SomeOtherSpeedInRpm);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
}

TEST(FanController, ShouldSwitchFromClosedToOpenLoopWhenCalculatedFanControlChanges)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldReset();
   WhenTheCalculatedOpenLoopFanSpeedChangesTo(SomeDutyCycle);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));

   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));
}

TEST(FanController, ShouldSwitchFromOpenToClosedLoopWhenCalculatedFanControlChanges)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));

   PidShouldExecute();
   WhenTheCalculatedClosedLoopFanSpeedChangesTo(SomeSpeedInRpm);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
}

TEST(FanController, ShouldUpdateBothPwmAndActualRpmIfTheCalculatedFanControlIsOpenLoop)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));
}

TEST(FanController, ShouldUpdateBothPwmAndActualRpmIfCalculatedFanControlChangesToOpenLoopMultipleTimes)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));

   WhenTheCalculatedOpenLoopFanSpeedChangesTo(SomeOtherDutyCycle);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeOtherDutyCycle));
}

TEST(FanController, PwmDutyCycleCantBeRequestedOver100PercentFromCalculatedFanControl)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(MaxDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(MaxDutyCycle));

   WhenTheCalculatedOpenLoopFanSpeedChangesTo(MaxDutyCycle + 1);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(MaxDutyCycle));

   WhenTheCalculatedOpenLoopFanSpeedChangesTo(MaxDutyCycle + 100);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
   FanPwmShouldBe(DutyCyclePercentageAsPwm(MaxDutyCycle));
}

TEST(FanController, ShouldUpdatePwmWhenInputCaptureIsZeroOnAnClosedLoopFanControl)
{
   GivenTheFanInputCaptureIs(0);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   FanPwmShouldBe(SomeSpeedInRpm * condenserFanData->gains.kpValue.value + SomeSpeedInRpm * condenserFanData->gains.kiValue.value);
   ActualFanRpmShouldBe(0);
}

TEST(FanController, ShouldUpdatePwmWhenInputCaptureIsZeroOnAnOpenLoopFanControl)
{
   GivenTheFanInputCaptureIs(0);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));
   ActualFanRpmShouldBe(0);
}

TEST(FanController, ShouldResetPidWhenCalculatedFanControlIsZero)
{
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   PidShouldReset();

   WhenTheCalculatedClosedLoopFanSpeedChangesTo(0);
}

TEST(FanController, ShouldResetPidWhenSwitchingFromClosedLoopToOpenLoopFanControl)
{
   GivenTheFanInputCaptureIs(0);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   PidShouldReset();
   WhenTheCalculatedOpenLoopFanSpeedChangesTo(SomeDutyCycle);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));
}

TEST(FanController, ActualRpmShouldStillUpdatePeriodicallyDuringAnOpenLoopFanControl)
{
   GivenTheFanInputCaptureIs(0);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));
   ActualFanRpmShouldBe(0);

   WhenTheFanInputCaptureChangesTo(SomeInputCaptureTimeInMicroSec);
   After(PeriodicFanFrequencyInMsec - 1);
   ActualFanRpmShouldBe(0);

   After(1);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));

   WhenTheFanInputCaptureChangesTo(SomeOtherInputCaptureTimeInMicroSec);
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(USEC_PER_MIN / (SomeOtherInputCaptureTimeInMicroSec * condenserFanData->pulsesPerRevolution));
}
