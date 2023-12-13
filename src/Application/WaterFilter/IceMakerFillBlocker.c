/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceMakerFillBlocker.h"
#include "utils.h"
#include "VolumeInOuncesX100.h"
#include "CalendarUsageInSeconds.h"
#include "IceMakerFillInhibitReasonBitmap.h"
#include "Constants_Time.h"
#include "Constants_Binary.h"

static bool UnitIsADispensingModel(IceMakerFillBlocker_t *instance)
{
   VolumeInOuncesX100_t unitLifetimeDispensedWaterInOuncesX100Erd;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->unitLifetimeDispensedWaterInOuncesX100Erd,
      &unitLifetimeDispensedWaterInOuncesX100Erd);

   return (unitLifetimeDispensedWaterInOuncesX100Erd != 0);
}

static bool UnitIsANonDispensingModel(IceMakerFillBlocker_t *instance)
{
   return !UnitIsADispensingModel(instance);
}

static bool FilterHasBeenPurged(IceMakerFillBlocker_t *instance)
{
   VolumeInOuncesX100_t waterFilterTotalVolumeUsage;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterVolumeUsageInOuncesX100Erd,
      &waterFilterTotalVolumeUsage);

   return (waterFilterTotalVolumeUsage >= instance->_private.iceMakerFillBlockerData->newFilterDispenserPurgeVolumeInOuncesX100);
}

static bool NewFilterFillStateDelayHasExpired(IceMakerFillBlocker_t *instance)
{
   CalendarUsageInSeconds_t waterFilterCalendarUsageInSeconds;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->waterFilterCalendarUsageInSecondsErd,
      &waterFilterCalendarUsageInSeconds);

   return (waterFilterCalendarUsageInSeconds >= (instance->_private.iceMakerFillBlockerData->newFilterFillStateDelayInMinutes * SECONDS_PER_MINUTE));
}

static bool IceMakerInhibitClearConditionsHaveBeenMet(IceMakerFillBlocker_t *instance)
{
   return (UnitIsANonDispensingModel(instance) ||
            FilterHasBeenPurged(instance) ||
            NewFilterFillStateDelayHasExpired(instance));
}

static void SetIceMakerInhibitDueToNewFilterTo(IceMakerFillBlocker_t *instance, bool status)
{
   IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReasonBitmap;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceMakerFillInhibitedReasonErd,
      &iceMakerFillInhibitedReasonBitmap);

   if(status)
   {
      BITMAP_SET(&iceMakerFillInhibitedReasonBitmap, IceMakerFillInhibitedReason_DueToNewFilter);
   }
   else
   {
      BITMAP_CLEAR(&iceMakerFillInhibitedReasonBitmap, IceMakerFillInhibitedReason_DueToNewFilter);
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceMakerFillInhibitedReasonErd,
      &iceMakerFillInhibitedReasonBitmap);
}

static void OnDataModelChange(void *context, const void *_args)
{
   IceMakerFillBlocker_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->unitLifetimeDispensedWaterInOuncesX100Erd ||
      args->erd == instance->_private.config->waterFilterCalendarUsageInSecondsErd ||
      args->erd == instance->_private.config->waterFilterVolumeUsageInOuncesX100Erd)
   {
      IceMakerInhibitClearConditionsHaveBeenMet(instance) ? SetIceMakerInhibitDueToNewFilterTo(instance, CLEAR) : SetIceMakerInhibitDueToNewFilterTo(instance, SET);
   }
}

void IceMakerFillBlocker_Init(
   IceMakerFillBlocker_t *instance,
   I_DataModel_t *dataModel,
   const IceMakerFillBlockerConfig_t *config,
   const IceMakerFillBlockerData_t *iceMakerFillBlockerData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.iceMakerFillBlockerData = iceMakerFillBlockerData;

   IceMakerInhibitClearConditionsHaveBeenMet(instance) ? SetIceMakerInhibitDueToNewFilterTo(instance, CLEAR) : SetIceMakerInhibitDueToNewFilterTo(instance, SET);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
