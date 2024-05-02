/*!
 * @file
 * @brief Handles the request and resolves the mode for the Feature Pan mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANMODEREQUESTHANDLER_H
#define FEATUREPANMODEREQUESTHANDLER_H

#include "EventSubscription.h"
#include "FeaturePanData.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
      const FeaturePanData_t *featurePanData;
      I_DataModel_t *dataModel;
   } _private;
} FeaturePanModeRequestHandler_t;

void FeaturePanModeRequestHandler_Init(
   FeaturePanModeRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const FeaturePanData_t *featurePanData);

#endif
