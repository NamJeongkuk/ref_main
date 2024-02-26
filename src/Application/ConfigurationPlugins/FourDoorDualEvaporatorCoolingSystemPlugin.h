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
#include "ConstArrayMap_FourDoorDualEvap.h"
#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CompressorPlugin.h"
#include "FourDoorFanPlugin.h"
#include "CoolingSystemRequestHandler.h"
#include "FourDoorDualEvapDefrostPlugin.h"
#include "ThreeWaySealedSystemValvePlugin.h"
#include "ConvertibleCompartmentDamperPlugin.h"
#include "ConvertibleCompartmentStatePlugin.h"

typedef struct
{
   struct
   {
      FourDoorSetpointPlugin_t fourDoorSetpointPlugin;
      FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t fourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin;
      ConstArrayMap_FourDoorDualEvap_t coolingStateBasedGridVotesTable;
      AmbientTemperatureAndHumidityPlugin_t ambientTemperatureAndHumidityPlugin;
      CompressorPlugin_t compressorPlugin;
      FourDoorFanPlugin_t fourDoorFanPlugin;
      CoolingSystemRequestHandler_t coolingSystemRequestHandler;
      FourDoorDualEvapDefrostPlugin_t fourDoorDualEvapDefrostPlugin;
      ThreeWaySealedSystemValvePlugin_t threeWaySealedSystemValvePlugin;
      ConvertibleCompartmentDamperPlugin_t convertibleCompartmentDamperPlugin;
      ConvertibleCompartmentStatePlugin_t convertibleCompartmentStatePlugin;
   } _private;
} FourDoorDualEvaporatorCoolingSystemPlugin_t;

void FourDoorDualEvaporatorCoolingSystemPlugin_Init(FourDoorDualEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel);

#endif
