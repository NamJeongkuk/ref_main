/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FourDoorStaggeredHeater.h"
#include "SystemErds.h"
#include "Constants_Time.h"

enum
{
   NoDelay = 0,
   SoftPwmPeriodInSeconds = 10 // Period must be the same for all heaters to ensure the ON scattering works.
};

static void InitConvertibleCompartmentSoftPwm(SoftPwm_t *softPwm,
   I_DataModel_t *dataModel)
{
   static const SoftPwmConfiguration_t config = {
      .pwmDutyCycleErd = Erd_ConvertibleCompartmentDamperHeaterPwmDutyCycle,
      .gpioErd = Erd_ConvertibleCompartmentDamperHeater,
      .timerModuleErd = Erd_TimerModule,
      .periodInSeconds = SoftPwmPeriodInSeconds
   };

   SoftPwm_Init(softPwm, dataModel, NoDelay, &config);
}

static void InitFillTubeSoftPwm(SoftPwm_t *softPwm, I_DataModel_t *dataModel)
{
   enum
   {
      PeriodInMsec = 10 * MSEC_PER_SEC,
      NumberOfHeaters = 5,
      Delay = PeriodInMsec / NumberOfHeaters,
   };

   static const SoftPwmConfiguration_t config = {
      .pwmDutyCycleErd = Erd_FillTubeHeater_Pwm,
      .gpioErd = Erd_FillTubeHeater,
      .timerModuleErd = Erd_TimerModule,
      .periodInSeconds = SoftPwmPeriodInSeconds
   };

   SoftPwm_Init(softPwm, dataModel, Delay, &config);
}

void FourDoorStaggeredHeater_Init(FourDoorStaggeredHeater_t *instance,
   I_DataModel_t *dataModel)
{
   InitConvertibleCompartmentSoftPwm(
      &instance->_private.convertibleCompartmentSoftPwm, dataModel);
   InitFillTubeSoftPwm(
      &instance->_private.fillTubeSoftPwm, dataModel);
}
