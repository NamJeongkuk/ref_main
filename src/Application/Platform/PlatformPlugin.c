/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PlatformPlugin.h"
#include "PersonalityParametricData.h"
#include "uassert.h"

void PlatformPlugin_Init(PlatformPlugin_t *instance, I_DataModel_t *dataModel)
{
   const RefrigeratorModel_t *refrigeratorModel = PersonalityParametricData_Get(dataModel)->refrigeratorModelData;

   if(refrigeratorModel->refrigeratorModelType == RefrigeratorModelType_SideBySide)
   {
      SideBySideConfigurationPlugin_Init(&instance->_private.platformConfigurations.sideBySidePlugin, dataModel);
   }
   else
   {
      uassert(!"Platform not supported");
   }
}
