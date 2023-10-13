/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERLIFESTATUSUPDATER_H
#define WATERFILTERLIFESTATUSUPDATER_H

#include "I_DataModel.h"
#include "FilterData.h"

typedef struct
{
   Erd_t totalWaterVolumeUsageInOuncesX100Erd; // uint32_t
   Erd_t filterCalendarUsageInSecondsErd; // uint32_t
   Erd_t waterFilterLifeStatusErd; // WaterFilterLifeStatus_t
} WaterFilterLifeStatusUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const WaterFilterLifeStatusUpdaterConfig_t *config;
      const CommonFilterData_t *data;
   } _private;
} WaterFilterLifeStatusUpdater_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param filterData
 */
void WaterFilterLifeStatusUpdater_Init(
   WaterFilterLifeStatusUpdater_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterLifeStatusUpdaterConfig_t *config,
   const CommonFilterData_t *filterData);

#endif