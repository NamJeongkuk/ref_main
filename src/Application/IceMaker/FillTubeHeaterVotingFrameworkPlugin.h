/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FILLTUBEHEATERVOTINGFRAMEWORKPLUGIN_H
#define FILLTUBEHEATERVOTINGFRAMEWORKPLUGIN_H

#include "I_DataModel.h"
#include "SoftPwm.h"
#include "PercentageDutyCycleVoteToPwmDutyCycleConverter.h"
#include "NonHarvestFillTubeHeaterControl.h"
#include "NonHarvestFillTubeHeaterData.h"

typedef struct
{
   struct
   {
      PercentageDutyCycleVoteToPwmDutyCycleConverter_t dutyCycleToPercentageCalculator;
      NonHarvestFillTubeHeaterControl_t nonHarvestFillTubeHeaterControl;
   } _private;
} FillTubeHeaterVotingFrameworkPlugin_t;

/*!
 * @param instance
 * @param dataModel
 * @param nonHarvestFillTubeHeaterData
 * @param dutyCycleToPercentageCalculatorConfig
 * @param nonHarvestFillTubeHeaterControlConfig
 */
void FillTubeHeaterVotingFrameworkPlugin_Init(
   FillTubeHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel,
   const NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData,
   const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t *dutyCycleToPercentageCalculatorConfig,
   const NonHarvestFillTubeHeaterControlConfig_t *nonHarvestFillTubeHeaterControlConfig);

#endif
