/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERCONFIGURATIONPLUGIN_H
#define SINGLEDOORFREEZERCONFIGURATIONPLUGIN_H

#include "SingleDoorFreezerEvaporatorCoolingSystemPlugin.h"
#include "AugerMotorPlugin.h"
#include "DispenserWaterValvePlugin.h"
#include "IceMakerSlotsPlugin.h"
#include "I_DataModel.h"
#include "IsolationWaterValvePlugin.h"
#include "RecessHeaterPlugin.h"
#include "SingleDoorFreezerDoorPlugin.h"
#include "SingleDoorFreezerFactoryPlugin.h"

typedef struct
{
   struct
   {
      SingleDoorFreezerSingleEvaporatorCoolingSystemPlugin_t coolingSystemPlugin;
      IsolationWaterValvePlugin_t isolationWaterValvePlugin;
      IceMakerSlotsPlugin_t iceMakerSlotsPlugin;
      DispenserWaterValvePlugin_t dispenserWaterValvePlugin;
      AugerMotorPlugin_t augerMotorPlugin;
      RecessHeaterPlugin_t recessHeaterPlugin;
      SingleDoorFreezerDoorPlugin_t singleDoorFreezerDoorPlugin;
      SingleDoorFreezerFactoryPlugin_t factoryPlugin;
   } _private;
} SingleDoorFreezerConfigurationPlugin_t;

void SingleDoorFreezerConfigurationPlugin_Init(SingleDoorFreezerConfigurationPlugin_t *instance, I_DataModel_t *dataModel);

#endif
