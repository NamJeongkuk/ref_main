/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CompressorSpeedController.h"
#include "PersonalityParametricData.h"
void CompressorSpeedController_Init(
   CompressorSpeedController_t *instance,
   I_DataModel_t *dataModel,
   TimerModule_t *timerModule,
   const CompressorSpeedControllerConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.timerModule = timerModule;
   instance->_private.compressorData = PersonalityParametricData_Get(dataModel)->compressorData;
   instance->_private.config = config;
}
