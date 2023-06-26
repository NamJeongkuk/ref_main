/*!
 * @file
 * @brief Sets setpoint votes from turbo mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SETPOINTTURBOMODE_H
#define SETPOINTTURBOMODE_H

#include "EventSubscription.h"
#include "I_DataModel.h"
#include "Timer.h"
#include "TurboModeSetpointData.h"

typedef struct
{
   Erd_t currentSetpointResolvedVoteErd; // SetpointVotedTemperature_t
   Erd_t turboModeSetpointVoteErd; // SetpointVotedTemperature_t
   Erd_t turboModeOnOffStatusErd; // bool
   Erd_t turboModeOnTimeInMinutesErd; // uint16_t
   Erd_t timerModuleErd; // TimerModule_t *
} SetpointTurboModeConfig_t;

typedef struct
{
   struct
   {
      Timer_t turboModeTimer;
      EventSubscription_t onDataModelChangeSubscription;
      I_DataModel_t *dataModel;
      const SetpointTurboModeConfig_t *config;
      const TurboModeData_t *turboModeData;
   } _private;
} SetpointTurboMode_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param turboModeData
 */
void SetpointTurboMode_Init(
   SetpointTurboMode_t *instance,
   I_DataModel_t *dataModel,
   const SetpointTurboModeConfig_t *config,
   const TurboModeData_t *data);

#endif
