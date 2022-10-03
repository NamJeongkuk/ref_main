/*!
 * @file
 * @brief Matches Defrost HSM state to whether or not actively waiting for next defrost
 * Actively waiting for next defrost is true when Defrost HSM state is Idle or Post Dwell
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ACTIVELYWAITINGFORDEFROSTONCOMPAREMATCH_H
#define ACTIVELYWAITINGFORDEFROSTONCOMPAREMATCH_H

#include "I_DataModel.h"
#include "EventSubscription.h"

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
   } _private;
} ActivelyWaitingForDefrostOnCompareMatch_t;

void ActivelyWaitingForDefrostOnCompareMatch_Init(ActivelyWaitingForDefrostOnCompareMatch_t *instance, I_DataModel_t *dataModel);

#endif
