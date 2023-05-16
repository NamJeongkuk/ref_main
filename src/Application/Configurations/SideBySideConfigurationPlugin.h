/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDECONFIGURATIONPLUGIN_H
#define SIDEBYSIDECONFIGURATIONPLUGIN_H

#include "SideBySideSingleEvaporatorCoolingSystemPlugin.h"
#include "I_DataModel.h"
#include "AluminumMoldIceMakerPlugin.h"
#include "SideBySideStaggeredHeater.h"
#include "SideBySideFactoryPlugin.h"
#include "IsolationWaterValvePlugin.h"
#include "FlowMeterPlugin.h"
#include "SideBySideDispensePlugin.h"
#include "TwistTrayIceMakerPlugin.h"
#include "DispenserWaterValvePlugin.h"
#include "AugerMotorPlugin.h"
#include "SideBySideLightingPlugin.h"
#include "SignOfLifePlugin.h"

typedef struct
{
   struct
   {
      SideBySideSingleEvaporatorCoolingSystemPlugin_t coolingSystemPlugin;
      IsolationWaterValvePlugin_t isolationWaterValvePlugin;
      AluminumMoldIceMakerPlugin_t aluminumMoldIceMakerPlugin;
      TwistTrayIceMakerPlugin_t twistTrayIceMakerPlugin;
      FlowMeterPlugin_t flowMeterPlugin;
      DispenserWaterValvePlugin_t dispenserWaterValvePlugin;
      AugerMotorPlugin_t augerMotorPlugin;
      SideBySideStaggeredHeater_t staggeredHeaterPlugin;
      SideBySideFactoryPlugin_t factoryPlugin;
      SideBySideDispensePlugin_t dispensePlugin;
      SideBySideLightingPlugin_t lightingPlugin;
      SignOfLifePlugin_t signOfLifePlugin;
   } _private;
} SideBySideConfigurationPlugin_t;

void SideBySideConfigurationPlugin_Init(SideBySideConfigurationPlugin_t *instance, I_DataModel_t *dataModel);

#endif
