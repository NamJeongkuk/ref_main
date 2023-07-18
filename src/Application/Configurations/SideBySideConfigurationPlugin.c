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
   AluminumMoldIceMakerPlugin_Init(&instance->_private.aluminumMoldIceMakerPlugin, dataModel);
   TwistTrayIceMakerPlugin_Init(&instance->_private.twistTrayIceMakerPlugin, dataModel);
   DispenserWaterValvePlugin_Init(&instance->_private.dispenserWaterValvePlugin, dataModel);
   AugerMotorPlugin_Init(&instance->_private.augerMotorPlugin, dataModel);
   SideBySideStaggeredHeater_Init(&instance->_private.staggeredHeaterPlugin, dataModel);
   RecessHeaterPlugin_Init(&instance->_private.recessHeaterPlugin, dataModel);
   SideBySideFactoryPlugin_Init(&instance->_private.factoryPlugin, dataModel);
   SideBySideDispensePlugin_Init(&instance->_private.dispensePlugin, dataModel);

   // SideBySideLightingPlugin_Init(&instance->_private.lightingPlugin, dataModel);

   SignOfLifePlugin_Init(&instance->_private.signOfLifePlugin, dataModel);
   SideBySideBooleanRequestStatusUpdaterPlugin_Init(&instance->_private.requestStatusUpdaterPlugin, dataModel);
   SideBySideDoorPlugin_Init(&instance->_private.doorPlugin, dataModel);
}
