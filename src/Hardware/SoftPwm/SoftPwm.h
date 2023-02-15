/*!
 * @file
 * @brief Uses a software timer to toggle the GPIO ERD
 * to create a PWM signal at a specific frequency
 * and varies the duty cycle based on the PWM duty cycle ERD.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SOFTPWM_H
#define SOFTPWM_H

#include "I_DataModel.h"
#include "Timer.h"

typedef struct
{
   Erd_t pwmDutyCycleErd; // PwmDutyCycle_t
   Erd_t gpioErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
   uint16_t frequencyX100;
} SoftPwmConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t pwmDutyCycleSubscription;
      Timer_t dutyCycleOnTimer;
      Timer_t periodTimer;
      I_DataModel_t *dataModel;
      const SoftPwmConfiguration_t *config;
   } _private;
} SoftPwm_t;

void SoftPwm_Init(
   SoftPwm_t *instance,
   I_DataModel_t *dataModel,
   TimerTicks_t initialDelay,
   const SoftPwmConfiguration_t *config);

#endif
