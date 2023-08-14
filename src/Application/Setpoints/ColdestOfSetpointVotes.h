/*!
 * @file
 * @brief Selects the coldest setpoint from a list of setpoint votes and
 * writes it to a coldest setpoint vote ERD that can be used in an ERD priority
 * resolver.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COLDESTOFSETPOINTVOTES_H
#define COLDESTOFSETPOINTVOTES_H

#include "ErdList.h"
#include "I_DataModel.h"
#include "EventSubscription.h"

typedef struct
{
   ErdList_t listOfSetpointVoteErds;
   Erd_t coldestSetpointVoteErd;
   Erd_t coldestSetpointWinningVoteErd;
} ColdestOfSetpointVotesConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const ColdestOfSetpointVotesConfiguration_t *config;
   } _private;
} ColdestOfSetpointVotes_t;

void ColdestOfSetpointVotes_Init(
   ColdestOfSetpointVotes_t *instance,
   I_DataModel_t *dataModel,
   const ColdestOfSetpointVotesConfiguration_t *config);

#endif
