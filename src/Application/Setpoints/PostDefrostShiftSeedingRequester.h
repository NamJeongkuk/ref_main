/*!
 * @file
 * @brief The module monitors defrost HSM state and requests post defrost shift seeding
 *        when the defrost HSM state changes from post dwell to idle.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PostDefrostShiftSeedingRequester_H
#define PostDefrostShiftSeedingRequester_H

#include "DefrostHsmState.h"
#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t RequestPostDefrostShiftSeedingErd;
   Erd_t DefrostHsmStateErd;
} PostDefrostShiftSeedingRequesterConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t defrostHsmStateSubscription;
      I_DataModel_t *dataModel;
      DefrostHsmState_t oldDefrostHsmState;
      const PostDefrostShiftSeedingRequesterConfig_t *config;
   } _private;
} PostDefrostShiftSeedingRequester_t;

void PostDefrostShiftSeedingRequester_Init(
   PostDefrostShiftSeedingRequester_t *instance,
   I_DataModel_t *dataModel,
   const PostDefrostShiftSeedingRequesterConfig_t *config);

#endif
