/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideStaggeredHeater.h"
#include "SystemErds.h"
#include "Constants_Time.h"

enum
{
   NoDelay = 0,
   PeriodInSec = 10,
   NumberOfHeaters = 5,
   SoftPwmPeriodInSeconds = 10 // Period must be the same for all heaters to ensure the ON scattering works.
};

static const SoftPwmConfiguration_t damperHeaterSoftPwmConfig = {
   .pwmDutyCycleErd = Erd_FreshFoodDamperHeaterPwmDutyCycle,
   .gpioErd = Erd_FreshFoodDamperHeater,
   .timerModuleErd = Erd_TimerModule,
   .periodInSeconds = SoftPwmPeriodInSeconds
};

static const SoftPwmConfiguration_t fillTubeHeaterSoftPwmConfig = {
   .pwmDutyCycleErd = Erd_FillTubeHeater_Pwm,
   .gpioErd = Erd_FillTubeHeater,
   .timerModuleErd = Erd_TimerModule,
   .periodInSeconds = SoftPwmPeriodInSeconds
};

void SideBySideStaggeredHeater_Init(SideBySideStaggeredHeater_t *instance, I_DataModel_t *dataModel)
{
   SoftPwm_Init(
      &instance->_private.freshFoodDamperHeaterSoftPwm,
      dataModel,
      NoDelay,
      &damperHeaterSoftPwmConfig);

   SoftPwm_Init(
      &instance->_private.fillTubeHeaterSoftPwm,
      dataModel,
      PeriodInSec / NumberOfHeaters * MSEC_PER_SEC,
      &fillTubeHeaterSoftPwmConfig);
}
