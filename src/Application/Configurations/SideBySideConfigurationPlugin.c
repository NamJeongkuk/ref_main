/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SideBySideConfigurationPlugin.h"
#include "RampingPwmDutyCycle.h"

void SideBySideConfigurationPlugin_Init(SideBySideConfigurationPlugin_t *instance, I_DataModel_t *dataModel)
{
   SideBySideSingleEvaporatorCoolingSystemPlugin_Init(&instance->_private.coolingSystemPlugin, dataModel);
   IsolationWaterValvePlugin_Init(&instance->_private.isolationWaterValvePlugin, dataModel);
   IceMakerSlotsPlugin_Init(&instance->_private.iceMakerSlotsPlugin, dataModel);
   DispenserWaterValvePlugin_Init(&instance->_private.dispenserWaterValvePlugin, dataModel);
   AugerMotorPlugin_Init(&instance->_private.augerMotorPlugin, dataModel);
   SideBySideStaggeredHeater_Init(&instance->_private.staggeredHeaterPlugin, dataModel);
   RecessHeaterPlugin_Init(&instance->_private.recessHeaterPlugin, dataModel);
   SideBySideFactoryPlugin_Init(&instance->_private.factoryPlugin, dataModel);
   SideBySideDispensePlugin_Init(&instance->_private.dispensePlugin, dataModel);
   SideBySideLightingPlugin_Init(&instance->_private.lightingPlugin, dataModel);
   SideBySideBooleanRequestStatusUpdaterPlugin_Init(&instance->_private.requestStatusUpdaterPlugin, dataModel);
   SideBySideEnhancedSabbathPlugin_Init(&instance->_private.enhancedSabbathPlugin, dataModel);
   SideBySideDoorPlugin_Init(&instance->_private.doorPlugin, dataModel);
   SideBySideWaterFilterTrackingPlugin_Init(&instance->_private.waterFilterTrackingPlugin, dataModel);
}
