/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFRESHFOODCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H
#define SINGLEDOORFRESHFOODCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H

#include "I_DataModel.h"
#include "SensorFiltering.h"
#include "OverrideArbiter.h"

typedef struct
{
   OverrideArbiter_t freshFoodCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodThermistorValidArbiter;
   OverrideArbiter_t freshFoodEvapThermistorValidArbiter;

   SensorFiltering_t freshFoodCabinetThermistor;
} SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_t;

void SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_Init(
   SingleDoorFreshFoodCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
