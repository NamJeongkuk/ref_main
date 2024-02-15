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
   SoftPwmPeriodInSeconds = 10, // Period must be the same for all heaters to ensure the ON scattering works.
   PeriodInMsec = 10 * MSEC_PER_SEC,
   NumberOfHeaters = 5,
   DelayBetweenHeaters = PeriodInMsec / NumberOfHeaters,
   FirstHeaterDelay = 0,
   SecondHeaterDelay = DelayBetweenHeaters,
   ThirdHeaterDelay = 2 * DelayBetweenHeaters,
   FourthHeaterDelay = 3 * DelayBetweenHeaters,
   FifthHeaterDelay = 4 * DelayBetweenHeaters
};

static void ConvertibleCompartmentDamperHeaterSoftPwm_Init(
   SoftPwm_t *softPwm,
   I_DataModel_t *dataModel,
   TimerTicks_t delay)
{
   static const SoftPwmConfiguration_t config = {
      .pwmDutyCycleErd = Erd_DamperHeaterPwmDutyCycle,
      .gpioErd = Erd_DamperHeater,
      .timerModuleErd = Erd_TimerModule,
      .periodInSeconds = SoftPwmPeriodInSeconds
   };

   SoftPwm_Init(softPwm, dataModel, delay, &config);
}

static void IceMaker0FillTubeSoftPwm_Init(
   SoftPwm_t *softPwm,
   I_DataModel_t *dataModel,
   TimerTicks_t delay)
{
   static const SoftPwmConfiguration_t config = {
      .pwmDutyCycleErd = Erd_IceMaker0_FillTubeHeater_Pwm,
      .gpioErd = Erd_IceMaker0_FillTubeHeater,
      .timerModuleErd = Erd_TimerModule,
      .periodInSeconds = SoftPwmPeriodInSeconds
   };

   SoftPwm_Init(softPwm, dataModel, delay, &config);
}

static void IceMaker1FillTubeSoftPwm_Init(
   SoftPwm_t *softPwm,
   I_DataModel_t *dataModel,
   TimerTicks_t delay)
{
   static const SoftPwmConfiguration_t config = {
      .pwmDutyCycleErd = Erd_IceMaker1_FillTubeHeater_Pwm,
      .gpioErd = Erd_IceMaker1_FillTubeHeater,
      .timerModuleErd = Erd_TimerModule,
      .periodInSeconds = SoftPwmPeriodInSeconds
   };

   SoftPwm_Init(softPwm, dataModel, delay, &config);
}

static void IceMaker2FillTubeSoftPwm_Init(
   SoftPwm_t *softPwm,
   I_DataModel_t *dataModel,
   TimerTicks_t delay)
{
   static const SoftPwmConfiguration_t config = {
      .pwmDutyCycleErd = Erd_IceMaker2_FillTubeHeater_Pwm,
      .gpioErd = Erd_IceMaker2_FillTubeHeater,
      .timerModuleErd = Erd_TimerModule,
      .periodInSeconds = SoftPwmPeriodInSeconds
   };

   SoftPwm_Init(softPwm, dataModel, delay, &config);
}

void FourDoorStaggeredHeater_Init(FourDoorStaggeredHeater_t *instance,
   I_DataModel_t *dataModel)
{
   ConvertibleCompartmentDamperHeaterSoftPwm_Init(
      &instance->_private.convertibleCompartmentDamperHeaterSoftPwm, dataModel, FirstHeaterDelay);
   IceMaker0FillTubeSoftPwm_Init(
      &instance->_private.iceMaker0FillTubeHeaterSoftPwm, dataModel, SecondHeaterDelay);
   IceMaker1FillTubeSoftPwm_Init(
      &instance->_private.iceMaker1FillTubeHeaterSoftPwm, dataModel, ThirdHeaterDelay);
   IceMaker2FillTubeSoftPwm_Init(
      &instance->_private.iceMaker2FillTubeHeaterSoftPwm, dataModel, FourthHeaterDelay);
}
