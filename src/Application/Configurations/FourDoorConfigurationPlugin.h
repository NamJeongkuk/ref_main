/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORCONFIGURATIONPLUGIN_H
#define FOURDOORCONFIGURATIONPLUGIN_H

#include "FourDoorDualEvaporatorCoolingSystemPlugin.h"
#include "I_DataModel.h"
#include "RecessHeaterPlugin.h"
#include "IsolationWaterValvePlugin.h"
#include "DispenserWaterValvePlugin.h"
#include "AugerMotorPlugin.h"
#include "IceMakerSlotsPlugin.h"
#include "FourDoorBooleanRequestStatusUpdaterPlugin.h"

typedef struct
{
   struct
   {
      FourDoorDualEvaporatorCoolingSystemPlugin_t coolingSystemPlugin;
      IsolationWaterValvePlugin_t isolationWaterValvePlugin;
      IceMakerSlotsPlugin_t iceMakerSlotsPlugin;
      DispenserWaterValvePlugin_t dispenserWaterValvePlugin;
      AugerMotorPlugin_t augerMotorPlugin;
      RecessHeaterPlugin_t recessHeaterPlugin;
      FourDoorBooleanRequestStatusUpdaterPlugin_t booleanRequestStatusUpdaterPlugin;
   } _private;
} FourDoorConfigurationPlugin_t;

void FourDoorConfigurationPlugin_Init(FourDoorConfigurationPlugin_t *instance, I_DataModel_t *dataModel);

#endif
