/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WaterFilterUsageSinceExpirationUpdater.h"
#include "WaterFilterUsageSinceExpiration.h"
#include "WaterFilterLifeStatus.h"
#include "CalendarUsageInSeconds.h"
#include "Constants_Time.h"
#include "utils.h"

static void CalendarUsageChanged(void *context, const void *args)
{
   WaterFilterUsageSinceExpirationUpdater_t *instance = context;
   const CalendarUsageInSeconds_t *calendarUsageInSeconds = args;
   WaterFilterUsageSinceExpiration_t waterFilterUsageSinceExpiration;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterUsageSinceExpirationErd,
      &waterFilterUsageSinceExpiration);

   waterFilterUsageSinceExpiration.daysSinceFilterExpired =
      (TRUNCATE_UNSIGNED_SUBTRACTION(
         *calendarUsageInSeconds,
         instance->_private.commonFilterData->filterRatedLifeInMinutes * SECONDS_PER_MINUTE)) /
      SECONDS_PER_DAY;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFilterUsageSinceExpirationErd,
      &waterFilterUsageSinceExpiration);
}

static void WaterFilterVolumeUsageChanged(void *context, const void *args)
{
   WaterFilterUsageSinceExpirationUpdater_t *instance = context;
   IGNORE(args);
   WaterFilterUsageSinceExpiration_t waterFilterUsageSinceExpiration;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterUsageSinceExpirationErd,
      &waterFilterUsageSinceExpiration);

   waterFilterUsageSinceExpiration.dispensesSinceFilterExpired++;

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->waterFilterUsageSinceExpirationErd,
      &waterFilterUsageSinceExpiration);
}

static void WaterFilterLifeStatusChanged(void *context, const void *args)
{
   WaterFilterUsageSinceExpirationUpdater_t *instance = context;
   const WaterFilterLifeStatus_t *waterFilterLifeStatus = args;
   WaterFilterUsageSinceExpiration_t waterFilterUsageSinceExpiration;
   CalendarUsageInSeconds_t calendarUsageInSeconds;

   if(WaterFilterLifeStatus_Expired == *waterFilterLifeStatus)
   {
      DataModel_Read(
         instance->_private.dataModel,
         instance->_private.config->calendarUsageInSecondsErd,
         &calendarUsageInSeconds);
      CalendarUsageChanged(instance, &calendarUsageInSeconds);

      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.config->calendarUsageInSecondsErd,
         &instance->_private.calendarUsageSubscription);

      DataModel_Subscribe(
         instance->_private.dataModel,
         instance->_private.config->waterFilterVolumeUsageInOuncesX100Erd,
         &instance->_private.waterFilterVolumeUsageSubscription);
   }
   else
   {
      DataModel_Unsubscribe(
         instance->_private.dataModel,
         instance->_private.config->calendarUsageInSecondsErd,
         &instance->_private.calendarUsageSubscription);

      DataModel_Unsubscribe(
         instance->_private.dataModel,
         instance->_private.config->waterFilterVolumeUsageInOuncesX100Erd,
         &instance->_private.waterFilterVolumeUsageSubscription);

      memset(&waterFilterUsageSinceExpiration, 0, sizeof(WaterFilterUsageSinceExpiration_t));

      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->waterFilterUsageSinceExpirationErd,
         &waterFilterUsageSinceExpiration);
   }
}

void WaterFilterUsageSinceExpirationUpdater_Init(
   WaterFilterUsageSinceExpirationUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterUsageSinceExpirationUpdaterConfig_t *config,
   const CommonFilterData_t *commonFilterData)
{
   instance->_private.config = config;
   instance->_private.dataModel = dataModel;
   instance->_private.commonFilterData = commonFilterData;

   EventSubscription_Init(
      &instance->_private.waterFilterLifeStatusSubscription,
      instance,
      WaterFilterLifeStatusChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->waterFilterLifeStatusErd,
      &instance->_private.waterFilterLifeStatusSubscription);

   EventSubscription_Init(
      &instance->_private.calendarUsageSubscription,
      instance,
      CalendarUsageChanged);

   EventSubscription_Init(
      &instance->_private.waterFilterVolumeUsageSubscription,
      instance,
      WaterFilterVolumeUsageChanged);

   WaterFilterLifeStatus_t waterFilterLifeStatus;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterLifeStatusErd,
      &waterFilterLifeStatus);

   WaterFilterLifeStatusChanged(instance, &waterFilterLifeStatus);
}
