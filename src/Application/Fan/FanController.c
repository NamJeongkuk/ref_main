/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FanController.h"
#include "FanControl.h"
#include "InputCaptureType.h"
#include "PwmDutyCycle.h"
#include "Constants_Time.h"
#include "utils.h"

enum
{
   PeriodicFanFrequencyInMsec = 200,
   MaxDutyCyclePercentage = 100,
};

static void UpdateActualRpm(void *context)
{
   FanController_t *instance = context;
   uint32_t rpm;

   InputCaptureMicroSeconds_t inputCaptureTimeInUsec;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->inputCaptureErd,
      &inputCaptureTimeInUsec);

   if(inputCaptureTimeInUsec > 0)
   {
      rpm = USEC_PER_MIN / (inputCaptureTimeInUsec * instance->_private.fanData->pulsesPerRevolution);
   }
   else
   {
      rpm = 0;
   }

   instance->_private.actualRpm = (FanRpm_t)CLAMP(rpm, 0, UINT16_MAX);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fanActualRpmErd,
      &instance->_private.actualRpm);
}

static void UpdatePwm(void *context)
{
   FanController_t *instance = context;
   PwmDutyCycle_t pwm;

   FanControl_t requestedFanControl;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedFanControlErd,
      &requestedFanControl);

   if(requestedFanControl.type == FanControlType_Rpm)
   {
      if(requestedFanControl.rpm == 0)
      {
         PidController_Reset(&instance->_private.pidController.interface);
         pwm = 0;
      }
      else
      {
         pwm = PidController_Execute(
            &instance->_private.pidController.interface,
            instance->_private.actualRpm,
            requestedFanControl.rpm);
      }
   }
   else
   {
      if(requestedFanControl.dutyCycle > MaxDutyCyclePercentage)
      {
         requestedFanControl.dutyCycle = MaxDutyCyclePercentage;
      }

      if(instance->_private.previousFanControlType == FanControlType_Rpm)
      {
         PidController_Reset(&instance->_private.pidController.interface);
      }

      pwm = (PwmDutyCycle_Max * requestedFanControl.dutyCycle) / 100;
   }

   instance->_private.previousFanControlType = requestedFanControl.type;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fanPwmErd,
      &pwm);
}

static void StartPeriodicPwmTimer(FanController_t *instance)
{
   TimerModule_StartPeriodic(
      instance->_private.timerModule,
      &instance->_private.pwmTimer,
      PeriodicFanFrequencyInMsec,
      UpdatePwm,
      instance);
}

static void StopPeriodicPwmTimer(FanController_t *instance)
{
   TimerModule_Stop(
      instance->_private.timerModule,
      &instance->_private.pwmTimer);
}

static void StartPeriodicActualRpmCalculationTimer(FanController_t *instance)
{
   TimerModule_StartPeriodic(
      instance->_private.timerModule,
      &instance->_private.actualRpmTimer,
      PeriodicFanFrequencyInMsec,
      UpdateActualRpm,
      instance);
}

static void RunFanBasedOnFanControlType(FanController_t *instance)
{
   FanControl_t requestedFanControl;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedFanControlErd,
      &requestedFanControl);

   if(requestedFanControl.type == FanControlType_DutyCycle)
   {
      StopPeriodicPwmTimer(instance);
   }
   else
   {
      StartPeriodicPwmTimer(instance);
   }

   UpdatePwm(instance);
}

static void CalculatedFanSpeedChanged(void *context, const void *args)
{
   FanController_t *instance = context;
   IGNORE(args);

   RunFanBasedOnFanControlType(instance);
}

void FanController_Init(
   FanController_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const FanData_t *fanData,
   const FanControllerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.fanData = fanData;
   instance->_private.config = config;
   instance->_private.timerModule = timerModule;
   instance->_private.actualRpm = 0;
   instance->_private.previousFanControlType = 0;

   PidController_Concrete_Init(
      &instance->_private.pidController,
      &instance->_private.fanData->gains,
      instance->_private.fanData->upperLimit,
      instance->_private.fanData->lowerLimit);

   UpdateActualRpm(instance);
   StartPeriodicActualRpmCalculationTimer(instance);

   RunFanBasedOnFanControlType(instance);

   EventSubscription_Init(
      &instance->_private.calculatedSpeedChanged,
      instance,
      CalculatedFanSpeedChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->calculatedFanControlErd,
      &instance->_private.calculatedSpeedChanged);
}
