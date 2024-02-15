/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERHEATERVOTINGFRAMEWORKPLUGIN_H
#define DAMPERHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "I_DataModel.h"
#include "PercentageDutyCycleVoteToPwmDutyCycleConverter.h"

typedef struct
{
   struct
   {
      ErdResolver_t damperHeaterErdResolver;
      PercentageDutyCycleVoteToPwmDutyCycleConverter_t dutyCycleToPercentageCalculator;
   } _private;
} DamperHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void DamperHeaterVotingFrameworkPlugin_Init(DamperHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel);

#endif
