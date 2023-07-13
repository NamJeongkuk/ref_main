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
#include "SideBySideDefrostPlugin.h"
#include "FreshFoodDamperPlugin.h"
#include "SideBySideSetpointPlugin.h"
#include "ValvePlugin.h"
#include "SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin.h"
#include "SideBySideParallelMotorControlPlugin.h"
#include "SideBySideFanPlugin.h"
#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CoolingSystemRequestHandler.h"

typedef struct
{
   struct
   {
      SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_t sensorFilteringPlugin;
      GridPlugin_t gridPlugin;
      ConstArrayMap_SideBySideSingleEvap_t coolingStateBasedGridVotesTable;
      CompressorPlugin_t compressorPlugin;
      SideBySideDefrostPlugin_t defrostPlugin;
      FreshFoodDamperPlugin_t freshFoodDamperPlugin;
      SideBySideSetpointPlugin_t sideBySideSetpointPlugin;
      ValvePlugin_t valvePlugin;
      SideBySideFanPlugin_t sideBySideFanPlugin;
      AmbientTemperaturePlugin_t ambientTemperaturePlugin;
      SideBySideParallelMotorControlPlugin_t sideBySideParallelMotorControlPlugin;
      CoolingSystemRequestHandler_t coolingSystemRequestHandler;
   } _private;
} SideBySideSingleEvaporatorCoolingSystemPlugin_t;

void SideBySideSingleEvaporatorCoolingSystemPlugin_Init(SideBySideSingleEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel);

#endif
