/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DUTYCYCLEVOTEPERCENTAGETODUTYCYCLECONVERTER_H
#define DUTYCYCLEVOTEPERCENTAGETODUTYCYCLECONVERTER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t inputDutyCyclePercentageVoteErd; // DutyCyclePercentageVote_t
   Erd_t outputPwmDutyCycleErd; // PwmDutyCycle_t
} DutyCycleVotePercentageToPwmDutyCycleConverterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const DutyCycleVotePercentageToPwmDutyCycleConverterConfig_t *config;
      EventSubscription_t dutyCyclePercentageOnChangeSubscription;
   } _private;
} DutyCycleVotePercentageToPwmDutyCycleConverter_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void DutyCycleVotePercentageToPwmDutyCycleConverter_Init(
   DutyCycleVotePercentageToPwmDutyCycleConverter_t *instance,
   I_DataModel_t *dataModel,
   const DutyCycleVotePercentageToPwmDutyCycleConverterConfig_t *config);

#endif
