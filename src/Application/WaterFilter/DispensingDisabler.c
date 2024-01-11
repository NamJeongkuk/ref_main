/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DispensingDisabler.h"
#include "DispensingInhibitedReasonBitmap.h"
#include "IceMakerFillInhibitReasonBitmap.h"
#include "utils.h"
#include "WaterFilterState.h"

static void UpdateDispensingInhibitedBitmap(DispensingDisabler_t *instance)
{
   WaterFilterState_t rfidFilterState;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidFilterStateErd,
      &rfidFilterState);

   DispensingInhibitedReasonBitmap_t dispensingInhibitedBitmap;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->dispensingInhibitedReasonBitmapErd,
      &dispensingInhibitedBitmap);

   IceMakerFillInhibitedReasonBitmap_t iceMakerFillInhibitedReasonBitmap;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->iceMakerFillInhibitedReasonBitmapErd,
      &iceMakerFillInhibitedReasonBitmap);

   if((rfidFilterState == WaterFilterState_Leak) ||
      (rfidFilterState == WaterFilterState_Error))
   {
      BITMAP_SET(&dispensingInhibitedBitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      BITMAP_SET(&iceMakerFillInhibitedReasonBitmap, IceMakerFillInhibitedReason_DueToRfidFilter);
   }
   else
   {
      BITMAP_CLEAR(&dispensingInhibitedBitmap, DispensingInhibitedReason_WaterDueToRfidFilter);
      BITMAP_CLEAR(&iceMakerFillInhibitedReasonBitmap, IceMakerFillInhibitedReason_DueToRfidFilter);
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->dispensingInhibitedReasonBitmapErd,
      &dispensingInhibitedBitmap);
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->iceMakerFillInhibitedReasonBitmapErd,
      &iceMakerFillInhibitedReasonBitmap);
}

static void RfidFilterStateChanged(void *context, const void *args)
{
   DispensingDisabler_t *instance = context;
   IGNORE(args);

   UpdateDispensingInhibitedBitmap(instance);
}

void DispensingDisabler_Init(
   DispensingDisabler_t *instance,
   I_DataModel_t *dataModel,
   const DispensingDisablerConfig_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   UpdateDispensingInhibitedBitmap(instance);

   EventSubscription_Init(
      &instance->_private.rfidFilterStateSubscription,
      instance,
      RfidFilterStateChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->rfidFilterStateErd,
      &instance->_private.rfidFilterStateSubscription);
}
