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
   Erd_t freezerIceRateTriggerSignalErd; // Signal_t
   Erd_t freezerSetpointFreezerIceRateVoteErd; // SetpointVotedTemperature_t
   Erd_t freezerEvapFanSpeedFreezerIceRateVoteErd; // FanVotedSpeed_t
   Erd_t freezerEvapFanSpeedResolvedVoteErd; // FanVotedSpeed_t
   Erd_t iceRateIsActiveErd; // bool
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
