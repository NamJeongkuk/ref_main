/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ShiftOffsetCalculatorPlugin.h"
#include "PostDefrostShiftSeedingRequester.h"
#include "SystemErds.h"

static const PostDefrostShiftSeedingRequesterConfig_t postDefrostShiftSeedingRequesterConfig = {
   .RequestPostDefrostShiftSeedingErd = Erd_PostDefrostShiftSeedingRequest,
   .DefrostHsmStateErd = Erd_DefrostHsmState
};

void ShiftOffsetCalculatorPlugin_Init(
   ShiftOffsetCalculatorPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   PostDefrostShiftSeedingRequester_Init(
      &instance->_private.postDefrostShiftSeedingRequester,
      dataModel,
      &postDefrostShiftSeedingRequesterConfig);
}
