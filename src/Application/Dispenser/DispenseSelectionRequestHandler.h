/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSESELECTIONREQUESTHANDLER_H
#define DISPENSESELECTIONREQUESTHANDLER_H

#include "DispenseSelection.h"
#include "EventSubscription.h"
#include "SystemErds.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t dispenseSelectionRequestErd; // DispenseSelection_t
   Erd_t dispenseSelectionStatusErd; // DispenseSelection_t
} DispenseSelectionRequestHandlerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const DispenseSelectionRequestHandlerConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
   } _private;
} DispenseSelectionRequestHandler_t;

/*!
 * @param instance
 * @param config
 * @param dataModel
 */
void DispenseSelectionRequestHandler_Init(
   DispenseSelectionRequestHandler_t *instance,
   const DispenseSelectionRequestHandlerConfig_t *config,
   I_DataModel_t *dataModel);

#endif
