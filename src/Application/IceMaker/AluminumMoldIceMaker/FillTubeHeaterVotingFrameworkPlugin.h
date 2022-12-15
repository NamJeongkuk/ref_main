/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILLTUBEHEATERVOTINGFRAMEWORKPLUGIN_H
#define FILLTUBEHEATERVOTINGFRAMEWORKPLUGIN_H

#include "ErdResolver.h"
#include "I_DataModel.h"
#include "SoftPwm.h"
#include "PercentageDutyCycleVoteToPwmDutyCycleConverter.h"

typedef struct
{
   struct
   {
      ErdResolver_t fillTubeHeaterErdResolver;
      PercentageDutyCycleVoteToPwmDutyCycleConverter_t dutyCycleToPercentageCalculator;
      SoftPwm_t fillTubeHeaterSoftPwm;
   } _private;
} FillTubeHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FillTubeHeaterVotingFrameworkPlugin_Init(FillTubeHeaterVotingFrameworkPlugin_t *instance, I_DataModel_t *dataModel);

#endif
