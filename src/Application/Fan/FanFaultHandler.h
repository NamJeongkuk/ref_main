/*!
 * @file
 * @brief Sets/clears fan related faults
 * - Missing feedback when fan is on
 * - Feedback present when fan is off
 * - Fan cannot reach target RPM
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANFAULTHANDLER_H
#define FANFAULTHANDLER_H

#include "I_DataModel.h"
#include "FanData.h"
#include "Timer.h"
#include "FanSpeed.h"

typedef struct
{
   Erd_t calculatedFanControlErd; // FanControl_t
   Erd_t fanActualRpmErd; // FanRpm_t
   Erd_t fanPwmErd; // PwmDutyCycle_t
   Erd_t fanMissingFeedbackWhenOnFaultErd; // bool
   Erd_t fanFeedbackPresentWhenFanOffFaultErd; // bool
   Erd_t fanCannotReachTargetRpmFaultErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} FanFaultHandlerConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t timer;
      const FanFaultHandlerConfiguration_t *config;
      const FanData_t *data;
      FanControl_t previousFanControl;
      uint8_t cannotReachTargetRpmTimeInMinutes;
      uint8_t missingFeedbackWhenOnTimeInMinutes;
      uint8_t feedbackPresentWhenOffTimeInMinutes;
   } _private;
} FanFaultHandler_t;

/*!
 * @param instance
 * @param dataModel
 * @param fanConfig
 * @param fanData
 */
void FanFaultHandler_Init(
   FanFaultHandler_t *instance,
   I_DataModel_t *dataModel,
   const FanFaultHandlerConfiguration_t *fanConfig,
   const FanData_t *fanData);

#endif
