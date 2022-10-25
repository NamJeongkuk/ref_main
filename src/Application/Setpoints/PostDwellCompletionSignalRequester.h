/*!
 * @file
 * @brief The module monitors defrost HSM state and signals post dwell completion
 *        when the defrost HSM state changes from post dwell to idle.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef POSTDWELLCOMPLETIONSIGNALREQUESTER_H
#define POSTDWELLCOMPLETIONSIGNALREQUESTER_H

#include "DefrostHsmState.h"
#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t postDwellCompletionSignalErd;
   Erd_t defrostHsmStateErd;
} PostDwellCompletionSignalRequesterConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t defrostHsmStateSubscription;
      I_DataModel_t *dataModel;
      DefrostHsmState_t oldDefrostHsmState;
      const PostDwellCompletionSignalRequesterConfig_t *config;
   } _private;
} PostDwellCompletionSignalRequester_t;

void PostDwellCompletionSignalRequester_Init(
   PostDwellCompletionSignalRequester_t *instance,
   I_DataModel_t *dataModel,
   const PostDwellCompletionSignalRequesterConfig_t *config);

#endif
