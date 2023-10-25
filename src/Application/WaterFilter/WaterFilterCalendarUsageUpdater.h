/*!
 * @file
 * @brief Increments  filter calendar usage time every thirty minutes after passing minimum lifetime
 *        water volume usage or service diagnostics runtime thresholds.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERCALENDARUSAGEUPDATER_H
#define WATERFILTERCALENDARUSAGEUPDATER_H

#include "DataModel.h"
#include "EventSubscription.h"
#include "Timer.h"
#include "CommonFilterData.h"

typedef struct
{
   Erd_t waterFilterCalendarUsageInSecondsErd; // CalendarUsageInSeconds_t
   Erd_t lifetimeTotalWaterUsageInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t serviceDiagnosticsRunTimeInMinutesErd; // ApplianceRunTimeMinutes_t
} WaterFilterCalendarUsageUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelChangedSubscription;
      Timer_t waterFilterCalendarUsageTimer;
      const WaterFilterCalendarUsageUpdaterConfig_t *config;
      const CommonFilterData_t *commonFilterData;
   } _private;
} WaterFilterCalendarUsageUpdater_t;

/*!
 * @brief
 * @param instance
 * @param dataModel
 * @param config
 * @param commonFilterData
 */
void WaterFilterCalendarUsageUpdater_Init(
   WaterFilterCalendarUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterCalendarUsageUpdaterConfig_t *config,
   const CommonFilterData_t *commonFilterData);

#endif
