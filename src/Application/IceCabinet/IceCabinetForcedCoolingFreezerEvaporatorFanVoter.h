/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETFORCEDCOOLINGFREEZEREVAPORATORFANVOTER_H
#define ICECABINETFORCEDCOOLINGFREEZEREVAPORATORFANVOTER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t freezerEvapFanSpeedIceCabinetVoteErd; // FanVotedSpeed_t
   Erd_t freezerEvapFanSpeedGridVoteErd; // FanVotedSpeed_t
   Erd_t iceCabinetCoolingModeErd; // IceCabinetCoolingMode_t
} IceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;
      const IceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig_t *config;
   } _private;
} IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t;

void IceCabinetForcedCoolingFreezerEvaporatorFanVoter_Init(
   IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t *instance,
   I_DataModel_t *dataModel,
   const IceCabinetForcedCoolingFreezerEvaporatorFanVoterConfig_t *config);

#endif
