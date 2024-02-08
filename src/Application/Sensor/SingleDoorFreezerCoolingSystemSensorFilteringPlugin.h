/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SINGLEDOORFREEZERCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H
#define SINGLEDOORFREEZERCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H

#include "I_DataModel.h"
#include "SensorFiltering.h"
#include "OverrideArbiter.h"

typedef struct
{
   OverrideArbiter_t freezerEvaporatorFilteredTemperatureArbiter;
   OverrideArbiter_t freezerCabinetFilteredTemperatureArbiter;

   OverrideArbiter_t freezerThermistorValidArbiter;
   OverrideArbiter_t freezerEvapThermistorValidArbiter;

   SensorFiltering_t freezerCabinetThermistor;
   SensorFiltering_t freezerEvapThermistor;
} SingleDoorFreezerCoolingSystemSensorFilteringPlugin_t;

void SingleDoorFreezerCoolingSystemSensorFilteringPlugin_Init(
   SingleDoorFreezerCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
