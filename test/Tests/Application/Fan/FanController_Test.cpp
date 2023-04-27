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
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DataModel_TestDouble.h"
#include "TimerModule_TestDouble.h"
#include "uassert_test.h"
#include "PidController_Mock.h"

enum
{
   Erd_CalculatedFanControl,
   Erd_InputCapture,
   Erd_FanPwm,
   Erd_ActualFanRpm,

   NumberOfPulsesPerRevolution = 4,
   PeriodicFanFrequencyInMsec = 200,

   SomeInputCaptureTimeInMicroSec = 8000,
   SomeOtherInputCaptureTimeInMicroSec = 4 * USEC_SEC,
   SomeSpeedInRpm = 2900,
   SomeOtherSpeedInRpm = 1000,
   SomeDutyCycle = 10,
   SomeOtherDutyCycle = 80,
   MaxDutyCycle = 100,
   SomeActualRpm = USEC_PER_MIN / (SomeInputCaptureTimeInMicroSec * NumberOfPulsesPerRevolution),
   SomeOtherActualRpm = USEC_PER_MIN / (SomeOtherInputCaptureTimeInMicroSec * NumberOfPulsesPerRevolution),

   Pid_UpperLimit = 5 * SomeSpeedInRpm,
   Pid_LowerLimit = 0,
   Pid_KpValue = 4,
   Pid_KpQBits = 0,
   Pid_KiValue = 0,
   Pid_KiQBits = 0,
   Pid_KdValue = 0,
   Pid_KdQBits = 0,

   UnusedButInclude = 0
};

const DataModel_TestDoubleConfigurationEntry_t erds[] = {
   { Erd_CalculatedFanControl, sizeof(FanControl_t) },
   { Erd_InputCapture, sizeof(InputCaptureMicroSeconds_t) },
   { Erd_FanPwm, sizeof(PwmDutyCycle_t) },
   { Erd_ActualFanRpm, sizeof(FanRpm_t) },
};

const PidControllerGains_t gains = {
   .kpValue = {
      .value = Pid_KpValue,
      .qBits = Pid_KpQBits,
   },
   .kiValue = {
      .value = Pid_KiValue,
      .qBits = Pid_KiQBits,
   },
   .kdValue = {
      .value = Pid_KdValue,
      .qBits = Pid_KdQBits,
   }
};

const FanSpeedData_t fanSpeedData = {
   .careAboutHighAmbientTemperature = false,
   .superLowSpeed = { UnusedButInclude, { UnusedButInclude } },
   .lowSpeed = { UnusedButInclude, { UnusedButInclude } },
   .mediumSpeed = { UnusedButInclude, { UnusedButInclude } },
   .highSpeed = { UnusedButInclude, { UnusedButInclude } },
   .superHighSpeed = { UnusedButInclude, { UnusedButInclude } },
   .superLowSpeedHighAmbientTemperature = { UnusedButInclude, { UnusedButInclude } },
   .highAmbientTriggerTemperatureInDegFx100 = UnusedButInclude,
   .highAmbientTriggerHumidityInPercentx100 = UnusedButInclude
};

const FanData_t fanData = {
   .fanId = 2,
   .pulsesPerRevolution = NumberOfPulsesPerRevolution,
   .careAboutCoolingMode = false,
   .powerUsageInWatts = 8,
   .gains = gains,
   .lowerLimit = Pid_LowerLimit,
   .upperLimit = Pid_UpperLimit,
   .fanMissedTargetFaultTimeoutInSeconds = 60,
   .missingFanFeedbackFaultTimeoutInSeconds = 60,
   .feedbackPresentWhenFanIsOffTimeoutInSeconds = 60,
   .speedData = &fanSpeedData
};

const FanControllerConfig_t fanConfig = {
   .calculatedFanControlErd = Erd_CalculatedFanControl,
   .inputCaptureErd = Erd_InputCapture,
   .fanPwmErd = Erd_FanPwm,
   .fanActualRpmErd = Erd_ActualFanRpm
};

TEST_GROUP(FanController)
{
   DataModel_TestDouble_t dataModelTestDouble;
   I_DataModel_t *dataModel;
   TimerModule_TestDouble_t timerModuleDouble;
   PidController_Mock_t pidMock;

   FanController_t instance;

   void setup()
   {
      DataModel_TestDouble_Init(&dataModelTestDouble, erds, NUM_ELEMENTS(erds));
      dataModel = dataModelTestDouble.dataModel;

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
         &fanData,
         &fanConfig);

      instance._private.pidController.interface = pidMock.interface;
   }

   void GivenTheCalculatedClosedLoopFanSpeedIs(FanRpm_t rpm)
   {
      FanControl_t closedLoopControl;
      closedLoopControl.type = FanControlType_Rpm;
      closedLoopControl.rpm = rpm;
      DataModel_Write(dataModel, Erd_CalculatedFanControl, &closedLoopControl);
   }

   void GivenTheCalculatedOpenLoopFanSpeedIs(FanDutyCycle_t dutyCycle)
   {
      FanControl_t openLoopControl;
      openLoopControl.type = FanControlType_DutyCycle;
      openLoopControl.dutyCycle = dutyCycle;
      DataModel_Write(dataModel, Erd_CalculatedFanControl, &openLoopControl);
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
      DataModel_Write(dataModel, Erd_InputCapture, &inputCapture);
   }

   void WhenTheFanInputCaptureChangesTo(InputCaptureMicroSeconds_t inputCapture)
   {
      GivenTheFanInputCaptureIs(inputCapture);
   }

   void FanPwmShouldBe(PwmDutyCycle_t expected)
   {
      PwmDutyCycle_t actual;
      DataModel_Read(dataModel, Erd_FanPwm, &actual);
      CHECK_EQUAL(expected, actual);
   }

   void ActualFanRpmShouldBe(FanRpm_t expected)
   {
      FanRpm_t actual;
      DataModel_Read(dataModel, Erd_ActualFanRpm, &actual);
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

   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(4100);
}

TEST(FanController, ShouldUpdateActualRpmPeriodicallyIfCalculatedFanControlIsClosedLoop)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   ActualFanRpmShouldBe(SomeActualRpm);

   WhenTheFanInputCaptureChangesTo(SomeOtherInputCaptureTimeInMicroSec);

   After(PeriodicFanFrequencyInMsec - 1);
   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldExecute();
   After(1);
   ActualFanRpmShouldBe(SomeOtherActualRpm);
}

TEST(FanController, ShouldUpdatePwmIfCalculatedFanControlIsClosedLoopAndInputCaptureChanges)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   WhenTheFanInputCaptureChangesTo(SomeOtherInputCaptureTimeInMicroSec);

   After(PeriodicFanFrequencyInMsec - 1);
   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldExecute();
   After(1);
   ActualFanRpmShouldBe(SomeOtherActualRpm);
}

TEST(FanController, ShouldUpdateBothPwmAndActualRpmPeriodicallyWhenCalculatedFanControlChangesAndIsClosedLoop)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldExecute();
   WhenTheCalculatedClosedLoopFanSpeedChangesTo(SomeOtherSpeedInRpm);
   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(SomeActualRpm);
}

TEST(FanController, ShouldSwitchFromClosedToOpenLoopWhenCalculatedFanControlChanges)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldReset();
   WhenTheCalculatedOpenLoopFanSpeedChangesTo(SomeDutyCycle);
   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));

   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));
}

TEST(FanController, ShouldSwitchFromOpenToClosedLoopWhenCalculatedFanControlChanges)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));

   PidShouldExecute();
   WhenTheCalculatedClosedLoopFanSpeedChangesTo(SomeSpeedInRpm);
   ActualFanRpmShouldBe(SomeActualRpm);

   PidShouldExecute();
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(SomeActualRpm);
}

TEST(FanController, ShouldUpdateBothPwmAndActualRpmIfTheCalculatedFanControlIsOpenLoop)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));
}

TEST(FanController, ShouldUpdateBothPwmAndActualRpmIfCalculatedFanControlChangesToOpenLoopMultipleTimes)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(SomeDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeDutyCycle));

   WhenTheCalculatedOpenLoopFanSpeedChangesTo(SomeOtherDutyCycle);
   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(SomeOtherDutyCycle));
}

TEST(FanController, PwmDutyCycleCantBeRequestedOver100PercentFromCalculatedFanControl)
{
   GivenTheFanInputCaptureIs(SomeInputCaptureTimeInMicroSec);
   GivenTheCalculatedOpenLoopFanSpeedIs(MaxDutyCycle);
   GivenInitialization();

   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(MaxDutyCycle));

   WhenTheCalculatedOpenLoopFanSpeedChangesTo(MaxDutyCycle + 1);
   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(MaxDutyCycle));

   WhenTheCalculatedOpenLoopFanSpeedChangesTo(MaxDutyCycle + 100);
   ActualFanRpmShouldBe(SomeActualRpm);
   FanPwmShouldBe(DutyCyclePercentageAsPwm(MaxDutyCycle));
}

TEST(FanController, ShouldUpdatePwmWhenInputCaptureIsZeroOnAnClosedLoopFanControl)
{
   GivenTheFanInputCaptureIs(0);
   GivenTheCalculatedClosedLoopFanSpeedIs(SomeSpeedInRpm);
   GivenInitialization();

   FanPwmShouldBe(SomeSpeedInRpm * Pid_KpValue);
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
   ActualFanRpmShouldBe(SomeActualRpm);

   WhenTheFanInputCaptureChangesTo(SomeOtherInputCaptureTimeInMicroSec);
   After(PeriodicFanFrequencyInMsec);
   ActualFanRpmShouldBe(SomeOtherActualRpm);
}
