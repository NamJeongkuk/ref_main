/*!
 * @file
 * @brief Update water filter usage since expiration data
 *        based on calendar usage and total water volume usage while water filter life status is expired
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERUSAGESINCEEXPIRATIONUPDATER_H
#define WATERFILTERUSAGESINCEEXPIRATIONUPDATER_H

#include "I_DataModel.h"
#include "CommonFilterData.h"

typedef struct
{
   Erd_t waterFilterLifeStatusErd; // WaterFilterLifeStatus_t
   Erd_t waterFilterUsageSinceExpirationErd; // WaterFilterUsageSinceExpiration_t
   Erd_t calendarUsageInSecondsErd; // CalendarUsageInSeconds_t
   Erd_t totalWaterVolumeUsageInOuncesx100Erd; // VolumeInOuncesX100_t
} WaterFilterUsageSinceExpirationUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t waterFilterLifeStatusSubscription;
      EventSubscription_t totalWaterVolumeUsageSubscription;
      EventSubscription_t calendarUsageSubscription;
      const WaterFilterUsageSinceExpirationUpdaterConfig_t *config;
      const CommonFilterData_t *commonFilterData;
   } _private;
} WaterFilterUsageSinceExpirationUpdater_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param commonFilterData
 */
void WaterFilterUsageSinceExpirationUpdater_Init(
   WaterFilterUsageSinceExpirationUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterUsageSinceExpirationUpdaterConfig_t *config,
   const CommonFilterData_t *commonFilterData);

#endif
