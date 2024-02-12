/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentDamperPlugin.h"

void ConvertibleCompartmentDamperPlugin_Init(
   ConvertibleCompartmentDamperPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ConvertibleCompartmentDamperHeaterVotingFrameworkPlugin_Init(
      &instance->_private.votingFramework,
      dataModel);
}
