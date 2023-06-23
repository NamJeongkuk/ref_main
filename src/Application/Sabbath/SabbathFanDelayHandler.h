/*!
 * @file - handles turning off Condenser and Evaporator fans when compressor is in sabbath delay state (delays all of them at same time).
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SABBATHFANDELAYHANDLER_H
#define SABBATHFANDELAYHANDLER_H

#include "I_DataModel.h"
#include "ErdList.h"

typedef struct
{
   Erd_t compressorStateErd; // CompressorState_t
   ErdList_t sabbathFanVoteErdList; // FanVotedSpeed_t
} SabbathFanDelayHandlerConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const SabbathFanDelayHandlerConfiguration_t *config;
      EventSubscription_t compressorStateSubscription;
   } _private;
} SabbathFanDelayHandler_t;

/*!
 * Initialize Sabbath Fan Delay Handler
 * @param instance sabbath fan delay handler
 * @param dataModel
 * @param config sabbath fan delay handler config - Erd
 */
void SabbathFanDelayHandler_Init(
   SabbathFanDelayHandler_t *instance,
   I_DataModel_t *dataModel,
   const SabbathFanDelayHandlerConfiguration_t *config);

#endif
