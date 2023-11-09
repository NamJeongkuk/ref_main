/*!
 * @file
 * @brief This generic module sums together the valves' water usage.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TOTALWATERVALVEUSAGEUPDATER_H
#define TOTALWATERVALVEUSAGEUPDATER_H

#include "I_DataModel.h"
#include "SystemErds.h"

typedef struct
{
   ErdList_t waterVolumeUsageInOuncesX100ErdList;
   Erd_t cumulativeWaterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
} TotalWaterValveUsageUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const TotalWaterValveUsageUpdaterConfig_t *config;
      EventSubscription_t dataModelChangedSubscription;
   } _private;
} TotalWaterValveUsageUpdater_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void TotalWaterValveUsageUpdater_Init(
   TotalWaterValveUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const TotalWaterValveUsageUpdaterConfig_t *config);

#endif
