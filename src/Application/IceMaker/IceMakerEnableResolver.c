/*!
 * @file
 * @brief Resolves the ice maker enable ERDs into 1 resolved ERD
 * (The ERDs are OR'd - if any ERD is enabled, then the ice maker is enabled.
 * All ERDs must be disabled for the ice maker to be disabled.)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceMakerEnableResolver.h"

void IceMakerEnableResolver_Init(
   IceMakerEnableResolver_t *instance,
   I_DataModel_t *dataModel,
   const ErdLogicServiceConfiguration_t *erdLogicServiceConfiguration)
{
   ErdLogicService_Init(
      &instance->logicService,
      erdLogicServiceConfiguration,
      DataModel_AsDataSource(dataModel));
}
