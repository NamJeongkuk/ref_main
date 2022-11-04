/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FREEZERICERATEHANDLER_H
#define FREEZERICERATEHANDLER_H

#include "I_DataModel.h"
#include "SystemErds.h"

typedef struct
{
   Erd_t freezerIceRateTriggerSignal; // Signal_t
   Erd_t freezerSetpointUserVote; // SetpointVotedTemperature_t
   Erd_t freezerSetpointFreezerIceRateVote; // SetpointVotedTemperature_t
   Erd_t freezerEvapFanSpeedFreezerIceRateVote; // FanVotedSpeed_t
   Erd_t freezerEvapFanSpeedResolvedVote; // FanVotedSpeed_t
} FreezerIceRateHandlerConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t freezerIceRateTriggerSignalSubscription;
      EventSubscription_t dataModelOnChangeSubscription;
      Timer_t iceRateTimer;
      I_DataModel_t *dataModel;
      const FreezerIceRateHandlerConfig_t *config;
      const FreezerIceRateData_t *freezerIceRateData;
   } _private;
} FreezerIceRateHandler_t;

void FreezerIceRateHandler_Init(
   FreezerIceRateHandler_t *instance,
   I_DataModel_t *dataModel,
   const FreezerIceRateHandlerConfig_t *config,
   const FreezerIceRateData_t *freezerIceRateData);
#endif
