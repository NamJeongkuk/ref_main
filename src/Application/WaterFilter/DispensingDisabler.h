/*!
 * @file
 * @brief The module disables dispensing and ice maker fill when RFID leak fault(805) is set
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGDISABLER_H
#define DISPENSINGDISABLER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t rfidFilterStateErd; // bool
   Erd_t dispensingInhibitedReasonBitmapErd; // DispensingInhibitedReasonBitmap_t
   Erd_t iceMakerFillInhibitedReasonBitmapErd; // IceMakerFillInhibitedReasonBitmap_t
} DispensingDisablerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t rfidFilterStateSubscription;
      const DispensingDisablerConfig_t *config;
   } _private;
} DispensingDisabler_t;

void DispensingDisabler_Init(
   DispensingDisabler_t *instance,
   I_DataModel_t *dataModel,
   const DispensingDisablerConfig_t *config);

#endif
