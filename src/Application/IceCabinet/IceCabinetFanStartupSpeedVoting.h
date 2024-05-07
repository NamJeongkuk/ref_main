/*!
 * @file
 * @brief Votes for a startup speed for a startup time when the
 * ice cabinet's resolved vote changes from off to any non-zero value
 * and continues any running startup time when it changes from
 * any non-zero value to a different non-zero value
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETFANSTARTUPSPEEDVOTING_H
#define ICECABINETFANSTARTUPSPEEDVOTING_H

#include "I_DataModel.h"
#include "Timer.h"
#include "IceCabinetFanStartupData.h"

typedef struct
{
   Erd_t iceCabinetFanResolvedVoteErd; // FanVotedSpeed_t
   Erd_t iceCabinetFanStartupSpeedVoteErd; // FanVotedSpeed_t
} IceCabinetFanStartupSpeedVotingConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t iceCabinetFanResolvedVoteSubscription;
      Timer_t timer;
      I_DataModel_t *dataModel;
      const IceCabinetFanStartupSpeedVotingConfig_t *config;
      const IceCabinetFanStartupData_t *iceCabinetFanStartupData;
      FanSpeed_t previousResolvedFanSpeed;
   } _private;
} IceCabinetFanStartupSpeedVoting_t;

/*!
 * @param instance
 * @param dataModel
 * @param iceCabinetFanStartupData
 * @param config
 */
void IceCabinetFanStartupSpeedVoting_Init(
   IceCabinetFanStartupSpeedVoting_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetFanStartupData_t *iceCabinetFanStartupData,
   const IceCabinetFanStartupSpeedVotingConfig_t *config);

#endif
