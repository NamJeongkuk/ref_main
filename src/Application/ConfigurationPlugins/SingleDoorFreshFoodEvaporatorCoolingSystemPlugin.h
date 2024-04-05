/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLDOORFRESHFOODEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define SINGLDOORFRESHFOODEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CompressorPlugin.h"
#include "CoolingSystemRequestHandler.h"
#include "ConstArrayMap_SingleDoorSingleEvap.h"
#include "SingleDoorFreshFoodSetpointPlugin.h"
#include "SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin.h"
#include "SingleDoorFreshFoodFanPlugin.h"
#include "AllFreshFoodDefrostPlugin.h"
#include "AllFreshFoodDamperPlugin.h"
#include "FreshFoodAndFreezerGridPlugin.h"
#include "FreshFoodNonHeatedCycleDefrostPlugin.h"

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
      AllFreshFoodDefrostPlugin_t allFreshFoodDefrostPlugin;
      AllFreshFoodDamperPlugin_t allFreshFoodDamperPlugin;
      FreshFoodAndFreezerGridPlugin_t freshFoodAndFreezerGridPlugin;
      FreshFoodNonHeatedCycleDefrostPlugin_t freshFoodNonHeatedCycleDefrostPlugin;
   } _private;
} SingleDoorFreshFoodEvaporatorCoolingSystemPlugin_t;

void SingleDoorFreshFoodEvaporatorCoolingSystemPlugin_Init(
   SingleDoorFreshFoodEvaporatorCoolingSystemPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
