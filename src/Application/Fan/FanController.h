/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include "I_DataModel.h"
#include "PidController_Concrete.h"
#include "FanControl.h"
#include "FanData.h"

typedef struct
{
   Erd_t calculatedFanControlErd; // FanControl_t
   Erd_t inputCaptureErd; // InputCaptureMicroSeconds_t
   Erd_t fanPwmErd; // PwmDutyCycle_t
   Erd_t fanActualRpmErd; // FanRpm_t
} FanControllerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FanControllerConfig_t *config;
      const FanData_t *fanData;
      EventSubscription_t calculatedSpeedChanged;
      PidController_Concrete_t pidController;
      Timer_t actualRpmTimer;
      Timer_t pwmTimer;
      TimerModule_t *timerModule;
      FanRpm_t actualRpm;
      FanControlType_t previousFanControlType;
   } _private;
} FanController_t;

/*!
 * @param instance
 * @param dataModel
 * @param timerModule
 * @param fanData
 * @param config
 */
void FanController_Init(
   FanController_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const FanData_t *fanData,
   const FanControllerConfig_t *config);

#endif
