/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "I_DataModel.h"
#include "GridPlugin.h"
#include "ConstArrayMap_SideBySideSingleEvap.h"
#include "CompressorPlugin.h"
#include "DefrostPlugin.h"
#include "FreshFoodDamperPlugin.h"
#include "SideBySideSetpointPlugin.h"
#include "ConvertibleCompartmentStatePlugin.h"
#include "SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin.h"
#include "SideBySideFanPlugin.h"

typedef struct
{
   struct
   {
      SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_t sensorFilteringPlugin;
      GridPlugin_t gridPlugin;
      ConstArrayMap_SideBySideSingleEvap_t coolingStateBasedGridVotesTable;
      CompressorPlugin_t compressorPlugin;
      DefrostPlugin_t defrostPlugin;
      FreshFoodDamperPlugin_t freshFoodDamperPlugin;
      SideBySideSetpointPlugin_t sideBySideSetpointPlugin;
      ConvertibleCompartmentStatePlugin_t convertibleCompartmentStatePlugin; // remove me asap please
      SideBySideFanPlugin_t sideBySideFanPlugin;
   } _private;
} SideBySideSingleEvaporatorCoolingSystemPlugin_t;

void SideBySideSingleEvaporatorCoolingSystemPlugin_Init(SideBySideSingleEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel);

#endif
