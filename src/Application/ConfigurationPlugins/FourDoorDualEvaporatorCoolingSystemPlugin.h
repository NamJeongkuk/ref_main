/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORDUALEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define FOURDOORDUALEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "I_DataModel.h"
#include "FourDoorSetpointPlugin.h"
#include "FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin.h"
#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CompressorPlugin.h"
#include "FourDoorFanPlugin.h"
#include "CoolingSystemRequestHandler.h"
#include "ThreeWaySealedSystemValvePlugin.h"

typedef struct
{
   struct
   {
      FourDoorSetpointPlugin_t fourDoorSetpointPlugin;
      FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t fourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin;
      AmbientTemperatureAndHumidityPlugin_t ambientTemperatureAndHumidityPlugin;
      CompressorPlugin_t compressorPlugin;
      FourDoorFanPlugin_t fourDoorFanPlugin;
      CoolingSystemRequestHandler_t coolingSystemRequestHandler;
      ThreeWaySealedSystemValvePlugin_t threeWaySealedSystemValvePlugin;
   } _private;
} FourDoorDualEvaporatorCoolingSystemPlugin_t;

void FourDoorDualEvaporatorCoolingSystemPlugin_Init(FourDoorDualEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel);

#endif
