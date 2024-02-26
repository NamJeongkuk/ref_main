/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALLFREEZERSINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define ALLFREEZERSINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CompressorPlugin.h"
#include "CoolingSystemRequestHandler.h"
#include "ConstArrayMap_SingleDoorSingleEvap.h"
#include "I_DataModel.h"
#include "SingleDoorFreezerSetpointPlugin.h"
#include "SingleDoorFreezerCoolingSystemSensorFilteringPlugin.h"
#include "SingleDoorFreezerFanPlugin.h"
#include "AllFreezerDefrostPlugin.h"

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
      AllFreezerDefrostPlugin_t allFreezerDefrostPlugin;
   } _private;
} AllFreezerSingleEvaporatorCoolingSystemPlugin_t;

void AllFreezerSingleEvaporatorCoolingSystemPlugin_Init(
   AllFreezerSingleEvaporatorCoolingSystemPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
