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
#include "SideBySideStaggeredHeater.h"
#include "RecessHeaterPlugin.h"
#include "SideBySideFactoryPlugin.h"
#include "IsolationWaterValvePlugin.h"
#include "SideBySideDispensePlugin.h"
#include "DispenserWaterValvePlugin.h"
#include "AugerMotorPlugin.h"
#include "SideBySideLightingPlugin.h"
#include "SideBySideBooleanRequestStatusUpdaterPlugin.h"
#include "SideBySideDoorPlugin.h"
#include "SideBySideEnhancedSabbathPlugin.h"
#include "SideBySideWaterFilterTrackingPlugin.h"
#include "IceMakerSlotsPlugin.h"

typedef struct
{
   struct
   {
      SideBySideSingleEvaporatorCoolingSystemPlugin_t coolingSystemPlugin;
      IsolationWaterValvePlugin_t isolationWaterValvePlugin;
      IceMakerSlotsPlugin_t iceMakerSlotsPlugin;
      DispenserWaterValvePlugin_t dispenserWaterValvePlugin;
      AugerMotorPlugin_t augerMotorPlugin;
      SideBySideStaggeredHeater_t staggeredHeaterPlugin;
      RecessHeaterPlugin_t recessHeaterPlugin;
      SideBySideFactoryPlugin_t factoryPlugin;
      SideBySideDispensePlugin_t dispensePlugin;
      SideBySideLightingPlugin_t lightingPlugin;
      SideBySideBooleanRequestStatusUpdaterPlugin_t requestStatusUpdaterPlugin;
      SideBySideDoorPlugin_t doorPlugin;
      SideBySideEnhancedSabbathPlugin_t enhancedSabbathPlugin;
      SideBySideWaterFilterTrackingPlugin_t waterFilterTrackingPlugin;
   } _private;
} SideBySideConfigurationPlugin_t;

void SideBySideConfigurationPlugin_Init(SideBySideConfigurationPlugin_t *instance, I_DataModel_t *dataModel);

#endif
