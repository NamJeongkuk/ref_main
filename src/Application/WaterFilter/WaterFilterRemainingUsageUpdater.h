/*!
 * @file
 * @brief Recalculate water filter remaining usage when total water volume usage
 *        or water filter calendar usage is changed
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERREMAININGUSAGEUPDATER_H
#define WATERFILTERREMAININGUSAGEUPDATER_H

#include "I_DataModel.h"
#include "CommonFilterData.h"

typedef struct
{
   Erd_t waterFilterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t waterFilterCalendarUsageInSecondsErd; // CalendarUsageInSeconds_t
   Erd_t waterFilterRemainingUsageErd; // WaterFilterRemainingUsage_t
} WaterFilterRemainingUsageUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelChangedSubscription;
      const WaterFilterRemainingUsageUpdaterConfig_t *config;
      const CommonFilterData_t *commonFilterData;
   } _private;
} WaterFilterRemainingUsageUpdater_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param commonFilterData
 */
void WaterFilterRemainingUsageUpdater_Init(
   WaterFilterRemainingUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterRemainingUsageUpdaterConfig_t *config,
   const CommonFilterData_t *commonFilterData);

#endif
