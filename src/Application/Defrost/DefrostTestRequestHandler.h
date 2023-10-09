/*!
 * @file
 * @brief Module to handle Defrost Test Request
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTESTREQUESTHANDLER_H
#define DEFROSTTESTREQUESTHANDLER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t defrostStateErd; // DefrostState_t
   Erd_t defrostTestRequestErd; // DefrostTestRequest_t
   Erd_t disableDefrostErd; // bool
   Erd_t defrostTestStateRequestErd; // DefrostTestStateRequestMessage_t
   Erd_t nextDefrostTypeErd; // DefrostType_t
   Erd_t useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd; // bool
   Erd_t defrostTestRequestStatusErd; // DefrostTestStateRequestMessage_t
   Erd_t dontSkipDefrostPrechillErd; // bool
} DefrostTestRequestHandlerConfiguration_t;

typedef struct
{
   struct
   {
      EventSubscription_t defrostStateSubscription;
      EventSubscription_t defrostTestRequestSubscription;
      I_DataModel_t *dataModel;
      const DefrostTestRequestHandlerConfiguration_t *config;
   } _private;
} DefrostTestRequestHandler_t;

/*!
 *
 * @param instance
 * @param dataModel
 * @param config
 */
void DefrostTestRequestHandler_Init(
   DefrostTestRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTestRequestHandlerConfiguration_t *config);

#endif
