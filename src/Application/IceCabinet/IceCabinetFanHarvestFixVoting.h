/*!
 * @file
 * @brief Votes for the ice cabinet fan to be off when the ice maker is in the harvest fix state
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETFANHARVESTFIXVOTING_H
#define ICECABINETFANHARVESTFIXVOTING_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t iceMakerStateErd; // IceMakerStateMachineState_t
   Erd_t iceCabinetFanHarvestFixVoteErd; // FanVotedSpeed_t
} IceCabinetFanHarvestFixVotingConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t iceMakerStateSubscription;
      I_DataModel_t *dataModel;
      const IceCabinetFanHarvestFixVotingConfig_t *config;
   } _private;
} IceCabinetFanHarvestFixVoting_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void IceCabinetFanHarvestFixVoting_Init(
   IceCabinetFanHarvestFixVoting_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetFanHarvestFixVotingConfig_t *config);

#endif
