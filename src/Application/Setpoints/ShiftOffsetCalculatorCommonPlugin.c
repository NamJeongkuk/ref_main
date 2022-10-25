/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ShiftOffsetCalculatorCommonPlugin.h"
#include "SystemErds.h"

static const PostDwellCompletionSignalRequesterConfig_t postDwellCompletionSignalRequesterConfig = {
   .postDwellCompletionSignalErd = Erd_PostDwellCompletionSignal,
   .defrostHsmStateErd = Erd_DefrostHsmState
};

void ShiftOffsetCalculatorCommonPlugin_Init(
   ShiftOffsetCalculatorCommonPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   PostDwellCompletionSignalRequester_Init(
      &instance->_private.postDwellCompletionSignalRequester,
      dataModel,
      &postDwellCompletionSignalRequesterConfig);
}
