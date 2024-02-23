/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORDUALEVAPORATORCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H
#define FOURDOORDUALEVAPORATORCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H

#include "I_DataModel.h"
#include "SensorFiltering.h"
#include "OverrideArbiter.h"

typedef struct
{
   OverrideArbiter_t freezerEvaporatorFilteredTemperatureArbiter;
   OverrideArbiter_t freezerCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodCabinetFilteredTemperatureArbiter;
   OverrideArbiter_t freshFoodEvaporatorFilteredTemperatureArbiter;
   OverrideArbiter_t convertibleCompartmentCabinetFilteredTemperatureArbiter;

   OverrideArbiter_t freshFoodThermistorValidArbiter;
   OverrideArbiter_t freezerThermistorValidArbiter;
   OverrideArbiter_t freshFoodEvapThermistorValidArbiter;
   OverrideArbiter_t freezerEvapThermistorValidArbiter;
   OverrideArbiter_t convertibleCompartmentCabinetThermistorValidArbiter;

   SensorFiltering_t freezerCabinetThermistor;
   SensorFiltering_t freshFoodCabinetThermistor;
   SensorFiltering_t freezerEvapThermistor;
   SensorFiltering_t freshFoodEvapThermistor;
} FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t;

void FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_Init(
   FourDoorDualEvaporatorCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
