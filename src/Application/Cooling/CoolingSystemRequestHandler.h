/*!
 * @file
 * @brief cooling system request handler
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COOLINGSYSTEMREQUESTHANDLER_H
#define COOLINGSYSTEMREQUESTHANDLER_H

#include "I_DataModel.h"
#include "ErdList.h"
#include "Timer.h"

typedef struct
{
   Erd_t coolingSystemRequestVoteErd;
   uint32_t offValue;
} CoolingSystemRequestVotePair_t;

typedef struct
{
   const CoolingSystemRequestVotePair_t *pairs;
   uint16_t numberOfPairs;
} CoolingSystemRequestVoteList_t;

typedef struct
{
   Erd_t requestErd; // uint8_t
   Erd_t statusErd; // uint8_t
   Erd_t disableDefrostErd; // bool
   Erd_t turboCoolOnOffRequestErd; // bool
   Erd_t turboFreezeOnOffRequestErd; // bool
   const CoolingSystemRequestVoteList_t coolingSystemRequestVoteList;
} CoolingSystemRequestHandlerConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t coolingSystemRequestSubscription;
      const CoolingSystemRequestHandlerConfiguration_t *config;
   } _private;
} CoolingSystemRequestHandler_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 */
void CoolingSystemRequestHandler_Init(
   CoolingSystemRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const CoolingSystemRequestHandlerConfiguration_t *config);

#endif
