/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "FanFaultHandler.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "PwmDutyCycle.h"

static FanControl_t CalculatedFanControl(FanFaultHandler_t *instance)
{
   FanControl_t calculatedFanControl;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->calculatedFanControlErd,
      &calculatedFanControl);

   return calculatedFanControl;
}

static FanRpm_t FanRpm(FanFaultHandler_t *instance)
{
   FanRpm_t actualFanRpm;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fanActualRpmErd,
      &actualFanRpm);

   return actualFanRpm;
}

static PwmDutyCycle_t FanPwm(FanFaultHandler_t *instance)
{
   PwmDutyCycle_t fanPwm;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->fanPwmErd,
      &fanPwm);

   return fanPwm;
}

static void SetFanCannotReachTargetRpmFaultTo(FanFaultHandler_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fanCannotReachTargetRpmFaultErd,
      &state);
}

static void SetMissingFanFeedbackWhenFanIsRunningFaultTo(FanFaultHandler_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fanMissingFeedbackWhenOnFaultErd,
      &state);
}

static void SetFeedbackPresentWhenFanIsOffFaultTo(FanFaultHandler_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->fanFeedbackPresentWhenFanOffFaultErd,
      &state);
}

static void CopyCalculatedFanControlToPreviousFanControl(FanFaultHandler_t *instance, FanControl_t calculatedFanControl)
{
   memcpy(&instance->_private.previousFanControl, &calculatedFanControl, sizeof(FanControl_t));
}

static bool FanControlChanged(FanFaultHandler_t *instance, FanControl_t calculatedFanControl)
{
   return ((calculatedFanControl.type != instance->_private.previousFanControl.type) ||
      (calculatedFanControl.rpm != instance->_private.previousFanControl.rpm));
}

static void CheckMissingFeedbackWhenOn(FanFaultHandler_t *instance, FanControl_t calculatedFanControl, FanRpm_t actualFanRpm)
{
   if((calculatedFanControl.type == FanControlType_Rpm) && (calculatedFanControl.rpm > 0))
   {
      if(actualFanRpm == 0)
      {
         instance->_private.missingFeedbackWhenOnTimeInMinutes = TRUNCATE_UNSIGNED_ADDITION(instance->_private.missingFeedbackWhenOnTimeInMinutes, 1, UINT8_MAX);

         if(instance->_private.missingFeedbackWhenOnTimeInMinutes >=
            instance->_private.data->missingFeedbackWhenOnFaultTimeoutInMinutes)
         {
            SetMissingFanFeedbackWhenFanIsRunningFaultTo(instance, SET);
         }
      }
      else
      {
         instance->_private.missingFeedbackWhenOnTimeInMinutes = 0;
         SetMissingFanFeedbackWhenFanIsRunningFaultTo(instance, CLEAR);
      }
   }
   else if(calculatedFanControl.type == FanControlType_DutyCycle)
   {
      instance->_private.missingFeedbackWhenOnTimeInMinutes = 0;
   }
}

static void CheckFeedbackPresentWhenOff(FanFaultHandler_t *instance, FanControl_t calculatedFanControl, FanRpm_t actualFanRpm)
{
   if((calculatedFanControl.type == FanControlType_Rpm) && (calculatedFanControl.rpm > 0))
   {
      instance->_private.feedbackPresentWhenOffTimeInMinutes = 0;
   }
   else if((calculatedFanControl.type == FanControlType_DutyCycle) && (calculatedFanControl.dutyCycle == 0))
   {
      if(actualFanRpm > 0)
      {
         instance->_private.feedbackPresentWhenOffTimeInMinutes = TRUNCATE_UNSIGNED_ADDITION(instance->_private.feedbackPresentWhenOffTimeInMinutes, 1, UINT8_MAX);

         if(instance->_private.feedbackPresentWhenOffTimeInMinutes >=
            instance->_private.data->feedbackPresentWhenFanOffTimeoutInMinutes)
         {
            SetFeedbackPresentWhenFanIsOffFaultTo(instance, SET);
         }
      }
      else
      {
         instance->_private.feedbackPresentWhenOffTimeInMinutes = 0;
         SetFeedbackPresentWhenFanIsOffFaultTo(instance, CLEAR);
      }
   }
}

static void CheckCannotReachTargetRpm(FanFaultHandler_t *instance, FanControl_t calculatedFanControl, PwmDutyCycle_t fanPwm)
{
   if((calculatedFanControl.type == FanControlType_Rpm) && (calculatedFanControl.rpm > 0))
   {
      if(fanPwm == PwmDutyCycle_Max)
      {
         instance->_private.cannotReachTargetRpmTimeInMinutes = TRUNCATE_UNSIGNED_ADDITION(instance->_private.cannotReachTargetRpmTimeInMinutes, 1, UINT8_MAX);

         if(instance->_private.cannotReachTargetRpmTimeInMinutes >=
            instance->_private.data->cannotReachTargetFaultTimeoutInMinutes)
         {
            SetFanCannotReachTargetRpmFaultTo(instance, SET);
         }
      }
      else if(fanPwm < (PwmDutyCycle_Max * 95) / 100)
      {
         instance->_private.cannotReachTargetRpmTimeInMinutes = 0;
         SetFanCannotReachTargetRpmFaultTo(instance, CLEAR);
      }
   }

   if(FanControlChanged(instance, calculatedFanControl))
   {
      instance->_private.cannotReachTargetRpmTimeInMinutes = 0;
      SetFanCannotReachTargetRpmFaultTo(instance, CLEAR);
      CopyCalculatedFanControlToPreviousFanControl(instance, calculatedFanControl);
   }
}

static void OneMinutePeriodicCheck(void *context)
{
   FanFaultHandler_t *instance = context;

   FanControl_t calculatedFanControl = CalculatedFanControl(instance);
   FanRpm_t actualFanRpm = FanRpm(instance);
   PwmDutyCycle_t fanPwm = FanPwm(instance);

   CheckMissingFeedbackWhenOn(instance, calculatedFanControl, actualFanRpm);
   CheckFeedbackPresentWhenOff(instance, calculatedFanControl, actualFanRpm);
   CheckCannotReachTargetRpm(instance, calculatedFanControl, fanPwm);
}

void FanFaultHandler_Init(
   FanFaultHandler_t *instance,
   I_DataModel_t *dataModel,
   const FanFaultHandlerConfiguration_t *fanConfig,
   const FanData_t *fanData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = fanConfig;
   instance->_private.data = fanData;

   instance->_private.previousFanControl = CalculatedFanControl(instance);

   TimerModule_StartPeriodic(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timer,
      MSEC_PER_MIN,
      OneMinutePeriodicCheck,
      instance);
}
