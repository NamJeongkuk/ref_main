/*!
 * @file
 * @brief Connect any resolved vote ERD that has a percentage duty cycle to its output
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RESOLVEDVOTEPERCENTAGEDUTYCYCLECONNECTOR_H
#define RESOLVEDVOTEPERCENTAGEDUTYCYCLECONNECTOR_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t resolvedPercentageDutyCycleVoteErd; // PercentageDutyCycleVote_t
   Erd_t percentageDutyCycleOutputErd; // PercentageDutyCycle_t
} ResolvedVotePercentageDutyCycleConnectorConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const ResolvedVotePercentageDutyCycleConnectorConfiguration_t *config;
   } _private;
} ResolvedVotePercentageDutyCycleConnector_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void ResolvedVotePercentageDutyCycleConnector_Init(
   ResolvedVotePercentageDutyCycleConnector_t *instance,
   I_DataModel_t *dataModel,
   const ResolvedVotePercentageDutyCycleConnectorConfiguration_t *config);

#endif
