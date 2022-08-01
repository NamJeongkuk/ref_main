/*!
 * @file
 * @brief calculates the requested fan speed.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANSPEEDRESOLVER_H
#define FANSPEEDRESOLVER_H

#include "I_DataModel.h"
#include "FanData.h"
#include "FanControl.h"

typedef struct
{
   Erd_t resolvedFanSpeedVoteErd; // FanVotedSpeed_t
   Erd_t resolvedValvePositionVoteErd; // ValveVotedPosition_t
   Erd_t freezerSetpointErd; // FreezerSetpoint_t
   Erd_t calculatedRequestFanControlErd; // FanControl_t
   Erd_t ambientTempErd; // TemperatureDegFx100_t
} FanSpeedResolverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const FanSpeedResolverConfig_t *config;
      const ValveFanData_t *fanData;
      EventSubscription_t resolvedVoteOnChangeSubscription;
   } _private;
} FanSpeedResolver_t;

/*!
 * @param instance
 * @param dataModel
 * @param fanData
 * @param config
 */
void FanSpeedResolver_Init(
   FanSpeedResolver_t *instance,
   I_DataModel_t *dataModel,
   const ValveFanData_t *fanData,
   const FanSpeedResolverConfig_t *config);

#endif