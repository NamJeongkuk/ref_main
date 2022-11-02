/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H
#define SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMSENSORFILTERINGPLUGIN_H

#include "I_DataModel.h"
#include "SensorFiltering.h"

typedef struct
{
   SensorFiltering_t freezerCabinetThermistor;
   SensorFiltering_t freshFoodCabinetThermistor;
   SensorFiltering_t freezerEvapThermistor;
} SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_t;

void SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_Init(
   SideBySideSingleEvaporatorCoolingSystemSensorFilteringPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
