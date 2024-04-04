/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "I_DataModel.h"
#include "FreshFoodAndFreezerGridPlugin.h"
#include "ConstArrayMap_SideBySideSingleEvap.h"
#include "CompressorPlugin.h"
#include "SideBySideDefrostPlugin.h"
#include "FreshFoodDamperPlugin.h"
#include "SideBySideSetpointPlugin.h"
#include "SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin.h"
#include "SideBySideFanPlugin.h"
#include "AmbientTemperatureAndHumidityPlugin.h"
#include "CoolingSystemRequestHandler.h"
#include "SideBySideSingleEvaporatorLoadOffDoorOpenPlugin.h"
#include "SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin.h"

typedef struct
{
   struct
   {
      SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_t sensorFilteringPlugin;
      FreshFoodAndFreezerGridPlugin_t freshFoodAndFreezerGridPlugin;
      ConstArrayMap_SideBySideSingleEvap_t coolingStateBasedGridVotesTable;
      CompressorPlugin_t compressorPlugin;
      SideBySideDefrostPlugin_t sideBySideDefrostPlugin;
      FreshFoodDamperPlugin_t freshFoodDamperPlugin;
      SideBySideSetpointPlugin_t sideBySideSetpointPlugin;
      SideBySideFanPlugin_t sideBySideFanPlugin;
      AmbientTemperatureAndHumidityPlugin_t ambientTemperatureAndHumidityPlugin;
      CoolingSystemRequestHandler_t coolingSystemRequestHandler;
      SideBySideSingleEvaporatorLoadOffDoorOpenPlugin_t sideBySideSingleEvaporatorLoadOffDoorOpenPlugin;
      SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin_t sideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin;
   } _private;
} SideBySideSingleEvaporatorCoolingSystemPlugin_t;

void SideBySideSingleEvaporatorCoolingSystemPlugin_Init(
   SideBySideSingleEvaporatorCoolingSystemPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
