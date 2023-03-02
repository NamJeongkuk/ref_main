/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideDispensePlugin.h"
#include "Constants_Binary.h"
#include "SystemErds.h"

static const DispenseSelectionRequestHandlerConfig_t dispenseRequest = {
   .dispenseSelectionRequestErd = Erd_DispenseSelectionRequest,
   .dispenseSelectionStatusErd = Erd_DispenseSelectionStatus
};

void SideBySideDispensePlugin_Init(SideBySideDispensePlugin_t *instance, I_DataModel_t *dataModel)
{
   DispenseSelectionRequestHandler_Init(
      &instance->_private.dispenseSelection,
      &dispenseRequest,
      dataModel);
}
