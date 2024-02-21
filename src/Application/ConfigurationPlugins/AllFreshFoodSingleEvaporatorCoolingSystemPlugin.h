/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALLFRESHFOODSINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define ALLFRESHFOODSINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CompressorPlugin.h"
#include "CoolingSystemRequestHandler.h"
#include "ConstArrayMap_SingleDoorSingleEvap.h"
#include "SingleDoorFreshFoodSetpointPlugin.h"
#include "SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin.h"
#include "SingleDoorFreshFoodFanPlugin.h"

typedef struct
{
   struct
   {
      SingleDoorFreshFoodSetpointPlugin_t singleDoorFreshFoodSetpointPlugin;
      SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_t singleDoorFreshFoodCoolingSystemSensorFilteringPlugin;
      SingleDoorFreshFoodFanPlugin_t singleDoorFreshFoodFanPlugin;
      AmbientTemperatureAndHumidityPlugin_t ambientTemperatureAndHumidityPlugin;
      CompressorPlugin_t compressorPlugin;
      CoolingSystemRequestHandler_t coolingSystemRequestHandler;
      ConstArrayMap_SingleDoorSingleEvap_t coolingStateBasedGridVotesTable;
   } _private;
} AllFreshFoodSingleEvaporatorCoolingSystemPlugin_t;

void AllFreshFoodSingleEvaporatorCoolingSystemPlugin_Init(
   AllFreshFoodSingleEvaporatorCoolingSystemPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
