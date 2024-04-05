/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERSINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define SINGLEDOORFREEZERSINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CompressorPlugin.h"
#include "CoolingSystemRequestHandler.h"
#include "ConstArrayMap_SingleDoorSingleEvap.h"
#include "I_DataModel.h"
#include "SingleDoorFreezerSetpointPlugin.h"
#include "SingleDoorFreezerCoolingSystemSensorFilteringPlugin.h"
#include "SingleDoorFreezerFanPlugin.h"
#include "SingleDoorFreezerDefrostPlugin.h"
#include "AllFreezerDamperPlugin.h"
#include "FreshFoodAndFreezerGridPlugin.h"

typedef struct
{
   struct
   {
      SingleDoorFreezerSetpointPlugin_t singleDoorFreezerSetpointPlugin;
      SingleDoorFreezerCoolingSystemSensorFilteringPlugin_t singleDoorFreezerCoolingSystemSensorFilteringPlugin;
      SingleDoorFreezerFanPlugin_t singleDoorFreezerFanPlugin;
      AmbientTemperatureAndHumidityPlugin_t ambientTemperatureAndHumidityPlugin;
      CompressorPlugin_t compressorPlugin;
      CoolingSystemRequestHandler_t coolingSystemRequestHandler;
      ConstArrayMap_SingleDoorSingleEvap_t coolingStateBasedGridVotesTable;
      SingleDoorFreezerDefrostPlugin_t singleDoorFreezerDefrostPlugin;
      AllFreezerDamperPlugin_t allFreezerDamperPlugin;
      FreshFoodAndFreezerGridPlugin_t freshFoodAndFreezerGridPlugin;
   } _private;
} SingleDoorFreezerSingleEvaporatorCoolingSystemPlugin_t;

void SingleDoorFreezerSingleEvaporatorCoolingSystemPlugin_Init(
   SingleDoorFreezerSingleEvaporatorCoolingSystemPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
