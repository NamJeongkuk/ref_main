/*!
 * @file
 * @brief Updates newMonthWaterUsageInGallon erd when waterFilterUsage erd
 * is updated. The most recent newMonth will be stored in index 0 of
 * lastTwelveMonthsOfWaterUsageInGallonsEeprom erd.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LASTTWELVEMONTHSOFWATERUSAGEUPDATER_H
#define LASTTWELVEMONTHSOFWATERUSAGEUPDATER_H

#include "I_DataModel.h"
#include "Timer.h"
#include "RfidFilterData.h"

typedef struct
{
   Erd_t currentWaterFilterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t previousWaterFilterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t currentWaterFilterMonthTimeInMinutesErd; // uint16_t
   Erd_t lastTwelveMonthsOfWaterUsageInGallonsEepromErd; // RfidTwelveMonthsGallonUsage_t
} LastTwelveMonthsOfWaterUsageUpdaterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const LastTwelveMonthsOfWaterUsageUpdaterConfig_t *config;
      const RfidFilterData_t *rfidFilterData;
      Timer_t monthlyWaterUsageTimer;
   } _private;
} LastTwelveMonthsOfWaterUsageUpdater_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param rfidFilterData
 */
void LastTwelveMonthsOfWaterUsageUpdater_Init(
   LastTwelveMonthsOfWaterUsageUpdater_t *instance,
   I_DataModel_t *dataModel,
   const LastTwelveMonthsOfWaterUsageUpdaterConfig_t *config,
   const RfidFilterData_t *rfidFilterData);

#endif
