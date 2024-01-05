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
#include "ErdAccumulatorService.h"
#include "WaterFilterStateResolver.h"
#include "WaterFilterRemainingUsageUpdater.h"
#include "WaterFilterUsageSinceExpirationUpdater.h"
#include "IceMakerFillBlocker.h"
#include "LastTwelveMonthsOfWaterUsageUpdater.h"

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
      ErdAccumulatorService_t unitLifetimeWaterValveAccumulator;
      ErdAccumulatorService_t waterFilterValveAccumulator;
      ErdAccumulatorService_t unitLifetimeDispensedWaterValveAccumulator;
      WaterFilterStateResolver_t waterFilterStateResolver;
      WaterFilterRemainingUsageUpdater_t waterFilterRemainingUsageUpdater;
      WaterFilterUsageSinceExpirationUpdater_t waterFilterUsageSinceExpirationUpdater;
      IceMakerFillBlocker_t iceMakerFillBlocker;
      LastTwelveMonthsOfWaterUsageUpdater_t lastTwelveMonthsOfWaterUsageUpdater;
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
