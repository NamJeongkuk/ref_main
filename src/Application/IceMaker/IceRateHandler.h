/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICERATEHANDLER_H
#define ICERATEHANDLER_H

#include "I_DataModel.h"
#include "SystemErds.h"

typedef struct
{
   Erd_t iceRateTriggerSignal; // Signal_t
   Erd_t freezerSetpointUserVote; // SetpointVotedTemperature_t
   Erd_t freezerSetpointIceRateVote; // SetpointVotedTemperature_t
   Erd_t freezerEvapFanSpeedIceRateVote; // FanVotedSpeed_t
   Erd_t freezerEvapFanSpeedResolvedVote; // FanVotedSpeed_t
} IceRateHandlerConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t iceRateTriggerSignalSubscription;
      EventSubscription_t dataModelOnChangeSubscription;
      Timer_t iceRateTimer;
      I_DataModel_t *dataModel;
      const IceRateHandlerConfig_t *config;
   } _private;
} IceRateHandler_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void IceRateHandler_Init(
   IceRateHandler_t *instance,
   I_DataModel_t *dataModel,
   const IceRateHandlerConfig_t *config);
#endif
