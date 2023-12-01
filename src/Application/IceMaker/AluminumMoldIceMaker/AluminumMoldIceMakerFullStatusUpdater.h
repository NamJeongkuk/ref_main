/*!
 * @file
 * @brief Update Aluminum Mold Ice Maker full status based on feeler arm position and aluminum mold ice maker hsm state
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKERFULLSTATUSUPDATER_H
#define ALUMINUMMOLDICEMAKERFULLSTATUSUPDATER_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t feelerArmPositionErd; // FeelerArmPosition_t
   Erd_t aluminumMoldIceMakerHsmStateErd; // AluminumMoldIceMakerHsmState_t
   Erd_t iceMakerFullStatusErd; // bool
} AluminumMoldIceMakerFullStatusUpdaterConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t feelerArmPositionSubscription;
      EventSubscription_t aluminumMoldIceMakerHsmStateSubscription;
      I_DataModel_t *dataModel;
      const AluminumMoldIceMakerFullStatusUpdaterConfig_t *config;
   } _private;
} AluminumMoldIceMakerFullStatusUpdater_t;

void AluminumMoldIceMakerFullStatusUpdater_Init(
   AluminumMoldIceMakerFullStatusUpdater_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerFullStatusUpdaterConfig_t *config);

#endif
