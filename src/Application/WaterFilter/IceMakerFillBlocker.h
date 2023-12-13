/*!
 * @file
 * @brief This module will inhibit the ice dispensing on a dispensing unit
 * (determined by unit lifetime water dispensed is != 0) if any of the following
 * conditions are met:
 * water filter total volume is less than the new filter purge volume
 * water filter calendar usage is less than the new filter fill state delay time
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERFILLBLOCKER_H
#define ICEMAKERFILLBLOCKER_H

#include "I_DataModel.h"
#include "IceMakerFillBlockerData.h"

typedef struct
{
   Erd_t unitLifetimeDispensedWaterInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t waterFilterVolumeUsageInOuncesX100Erd; // VolumeInOuncesX100_t
   Erd_t waterFilterCalendarUsageInSecondsErd; // CalendarUsageInSeconds_t
   Erd_t iceMakerFillInhibitedReasonErd; // IceMakerFillInhibitedReasonBitmap_t
} IceMakerFillBlockerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t onDataModelChangeSubscription;
      const IceMakerFillBlockerConfig_t *config;
      const IceMakerFillBlockerData_t *iceMakerFillBlockerData;
   } _private;
} IceMakerFillBlocker_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param iceMakerFillBlockerData
 */
void IceMakerFillBlocker_Init(
   IceMakerFillBlocker_t *instance,
   I_DataModel_t *dataModel,
   const IceMakerFillBlockerConfig_t *config,
   const IceMakerFillBlockerData_t *iceMakerFillBlockerData);

#endif
