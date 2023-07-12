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
#include "RecessHeaterPlugin.h"
#include "SideBySideFactoryPlugin.h"
#include "IsolationWaterValvePlugin.h"
#include "SideBySideDispensePlugin.h"
#include "TwistTrayIceMakerPlugin.h"
#include "DispenserWaterValvePlugin.h"
#include "AugerMotorPlugin.h"
#include "SideBySideLightingPlugin.h"
#include "SignOfLifePlugin.h"
#include "SideBySideBooleanRequestStatusUpdaterPlugin.h"
#include "SideBySideDoorPlugin.h"

typedef struct
{
   struct
   {
      SideBySideSingleEvaporatorCoolingSystemPlugin_t coolingSystemPlugin;
      IsolationWaterValvePlugin_t isolationWaterValvePlugin;
      AluminumMoldIceMakerPlugin_t aluminumMoldIceMakerPlugin;
      TwistTrayIceMakerPlugin_t twistTrayIceMakerPlugin;
      DispenserWaterValvePlugin_t dispenserWaterValvePlugin;
      AugerMotorPlugin_t augerMotorPlugin;
      SideBySideStaggeredHeater_t staggeredHeaterPlugin;
      RecessHeaterPlugin_t recessHeaterPlugin;
      SideBySideFactoryPlugin_t factoryPlugin;
      SideBySideDispensePlugin_t dispensePlugin;
      SideBySideLightingPlugin_t lightingPlugin;
      SignOfLifePlugin_t signOfLifePlugin;
      SideBySideBooleanRequestStatusUpdaterPlugin_t requestStatusUpdaterPlugin;
      SideBySideDoorPlugin_t doorPlugin;
   } _private;
} SideBySideConfigurationPlugin_t;

void SideBySideConfigurationPlugin_Init(SideBySideConfigurationPlugin_t *instance, I_DataModel_t *dataModel);

#endif
