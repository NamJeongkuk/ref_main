/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERPLUGIN_H
#define WATERFILTERPLUGIN_H

#include "DispensingDisabler.h"
#include "RfidCommunicationPlugin.h"
#include "WaterFilterCalendarUsageUpdater.h"
#include "WaterVolumeUsageCalculator.h"
#include "NewFilterInstalledHandler.h"

typedef struct
{
   struct
   {
      WaterFilterCalendarUsageUpdater_t waterFilterCalendarUsageUpdater;
      RfidCommunicationPlugin_t rfidCommunicationPlugin;
      WaterVolumeUsageCalculator_t aluminumMoldIceMakerWaterVolumeUsageCalculator;
      WaterVolumeUsageCalculator_t twistTrayIceMakerWaterVolumeUsageCalculator;
      WaterVolumeUsageCalculator_t dispensedWaterVolumeUsageCalculator;
      DispensingDisabler_t dispensingDisabler;
      NewFilterInstalledHandler_t newFilterInstalledHandler;
   } _private;
} WaterFilterPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void WaterFilterPlugin_Init(
   WaterFilterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
