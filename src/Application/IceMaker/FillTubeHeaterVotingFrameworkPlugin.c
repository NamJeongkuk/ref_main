/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FillTubeHeaterVotingFrameworkPlugin.h"

void FillTubeHeaterVotingFrameworkPlugin_Init(
   FillTubeHeaterVotingFrameworkPlugin_t *instance,
   I_DataModel_t *dataModel,
   const NonHarvestFillTubeHeaterData_t *nonHarvestFillTubeHeaterData,
   const PercentageDutyCycleVoteToPwmDutyCycleConverterConfig_t *dutyCycleToPercentageCalculatorConfig,
   const NonHarvestFillTubeHeaterControlConfig_t *nonHarvestFillTubeHeaterControlConfig)
{
   PercentageDutyCycleVoteToPwmDutyCycleConverter_Init(
      &instance->_private.dutyCycleToPercentageCalculator,
      dataModel,
      dutyCycleToPercentageCalculatorConfig);

   NonHarvestFillTubeHeaterControl_Init(
      &instance->_private.nonHarvestFillTubeHeaterControl,
      dataModel,
      nonHarvestFillTubeHeaterControlConfig,
      nonHarvestFillTubeHeaterData);
}
