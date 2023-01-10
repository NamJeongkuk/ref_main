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

typedef struct
{
   struct
   {
      SideBySideSingleEvaporatorCoolingSystemPlugin_t coolingSystemPlugin;
      AluminumMoldIceMakerPlugin_t aluminumMoldIceMakerPlugin;
      SideBySideStaggeredHeater_t staggeredHeaterPlugin;
      SideBySideFactoryPlugin_t factoryPlugin;
   } _private;
} SideBySideConfigurationPlugin_t;

void SideBySideConfigurationPlugin_Init(SideBySideConfigurationPlugin_t *instance, I_DataModel_t *dataModel);

#endif
