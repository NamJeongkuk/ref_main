/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERCENTAGEDUTYCYCLEVOTETODUTYCYCLECONVERTER_H
#define PERCENTAGEDUTYCYCLEVOTETODUTYCYCLECONVERTER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t inputPercentageDutyCycleVoteErd; // PercentageDutyCycleVote_t
   Erd_t outputPwmDutyCycleErd; // PwmDutyCycle_t
} PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t *config;
      EventSubscription_t percentageDutyCycleOnChangeSubscription;
   } _private;
} PercentageDutyCycleVoteToPwmDutyCycleConverter_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
   PercentageDutyCycleVoteToPwmDutyCycleConverter_t *instance,
   I_DataModel_t *dataModel,
   const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t *config);

#endif
