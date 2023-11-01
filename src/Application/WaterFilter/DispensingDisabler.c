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

static void UpdateDispensingInhibitedBitmap(DispensingDisabler_t *instance)
{
   bool leakDetected;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->rfidBoardLeakDetectedFaultErd,
      &leakDetected);

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

   if(leakDetected)
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

static void LeakFaultChanged(void *context, const void *args)
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
      &instance->_private.leakFaultSubscription,
      instance,
      LeakFaultChanged);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->rfidBoardLeakDetectedFaultErd,
      &instance->_private.leakFaultSubscription);
}
