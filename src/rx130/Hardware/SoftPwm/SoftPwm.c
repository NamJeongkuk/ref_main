/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SoftPwm.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "PwmDutyCycle.h"

static void DutyCycleChanged(void *context, const void *args);

static PwmDutyCycle_t PwmDutyCycle(SoftPwm_t *instance)
{
   PwmDutyCycle_t pwmDutyCycle;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->pwmDutyCycleErd,
      &pwmDutyCycle);

   return pwmDutyCycle;
}

static void SetGpioTo(SoftPwm_t *instance, bool state)
{
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->gpioErd,
      &state);
}

static void DutyCycleOnComplete(void *context)
{
   SoftPwm_t *instance = context;
   SetGpioTo(instance, OFF);
}

static void StartOnTimer(SoftPwm_t *instance, const PwmDutyCycle_t *pwmDutyCycle)
{
   TimerTicks_t onTicks = (((TimerTicks_t)*pwmDutyCycle * instance->_private.config->frequencyX100 * MSEC_PER_SEC) / PwmDutyCycle_Max);
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.dutyCycleOnTimer,
      onTicks,
      DutyCycleOnComplete,
      instance);
}

static void PeriodComplete(void *context)
{
   SoftPwm_t *instance = context;

   PwmDutyCycle_t pwmDutyCycle = PwmDutyCycle(instance);
   DutyCycleChanged(instance, &pwmDutyCycle);
}

static void StartPeriodTimer(SoftPwm_t *instance)
{
   TimerTicks_t periodTicks = instance->_private.config->frequencyX100 * MSEC_PER_SEC;
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.periodTimer,
      periodTicks,
      PeriodComplete,
      instance);
}

static void DutyCycleChanged(void *context, const void *args)
{
   SoftPwm_t *instance = context;
   const PwmDutyCycle_t *pwmDutyCycle = args;
   if(*pwmDutyCycle > 0)
   {
      SetGpioTo(instance, ON);
      StartOnTimer(instance, pwmDutyCycle);
      StartPeriodTimer(instance);
   }
   else
   {
      SetGpioTo(instance, OFF);
   }
};

void SoftPwm_Init(
   SoftPwm_t *instance,
   I_DataModel_t *dataModel,
   const SoftPwmConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.pwmDutyCycleSubscription,
      instance,
      DutyCycleChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->pwmDutyCycleErd,
      &instance->_private.pwmDutyCycleSubscription);
}