/*!
 * @file
 * @brief Writes to a Feature Pan user setpoint vote ERD based on the current Feature Pan mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANUSERSETPOINTVOTER_H
#define FEATUREPANUSERSETPOINTVOTER_H

#include "EventSubscription.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t featurePanCurrentModeErd; // FeaturePanCurrentMode_t
   Erd_t userSetpointVoteErd; // SetpointVotedTemperature_t
   const Erd_t *resolvedModeSetpointErds; // TemperatureDegFx100_t
   const uint8_t numberOfErds;
} FeaturePanUserSetpointVoterConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const FeaturePanUserSetpointVoterConfiguration_t *config;
      I_DataModel_t *dataModel;
   } _private;
} FeaturePanUserSetpointVoter_t;

void FeaturePanUserSetpointVoter_Init(
   FeaturePanUserSetpointVoter_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanUserSetpointVoterConfiguration_t *config);

#endif
