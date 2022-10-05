/*!
 * @file
 * @brief Publishes setpoint votes from public setpoint changes
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef USERSETPOINTREQUESTHANDLER_H
#define USERSETPOINTREQUESTHANDLER_H

#include "I_DataModel.h"
#include "EventSubscription.h"

typedef struct
{
   Erd_t userSetpointRequestErd; // Setpoint_t
   Erd_t userSetpointVoteErd; // SetpointVotedTemperature_t
   Erd_t userSetpointRangeDataErd; // UserSetpointRangeData_t
   Erd_t userSetpointStatusErd; // Setpoint_t
} UserSetpointRequestHandlerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const UserSetpointRequestHandlerConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
   } _private;
} UserSetpointRequestHandler_t;

/*!
 * @param instance
 * @param config
 * @param dataModel
 */
void UserSetpointRequestHandler_Init(
   UserSetpointRequestHandler_t *instance,
   const UserSetpointRequestHandlerConfig_t *config,
   I_DataModel_t *dataModel);

#endif
