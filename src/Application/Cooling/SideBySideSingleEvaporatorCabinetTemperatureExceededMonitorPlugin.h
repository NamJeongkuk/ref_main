/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESINGLEEVAPORATORCABINETTEMPERATUREEXCEEDEDMONITORPLUGIN_H
#define SIDEBYSIDESINGLEEVAPORATORCABINETTEMPERATUREEXCEEDEDMONITORPLUGIN_H

#include "CabinetTemperatureExceededMonitor.h"

typedef struct
{
   struct
   {
      CabinetTemperatureExceededMonitor_t freshFoodCabinetTemperatureExceededMonitor;
      CabinetTemperatureExceededMonitor_t freezerCabinetTemperatureExceededMonitor;
   } _private;
} SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin_t;

void SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin_Init(
   SideBySideSingleEvaporatorCabinetTemperatureExceededMonitorPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
