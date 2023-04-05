/*!
 * @file
 * @brief Initialize modules related to the dispenser water valve
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSERWATERVALVEPLUGIN_H
#define DISPENSERWATERVALVEPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"
#include "ResolvedVoteRelayConnector.h"

typedef struct
{
   struct
   {
      ErdResolver_t dispenserWaterValveErdResolver;
      ResolvedVoteRelayConnector_t dispenserWaterValveRelayConnector;
   } _private;
} DispenserWaterValvePlugin_t;

/*!
 *
 * @param instance instance of DispenserWaterValvePlugin_t
 * @param dataModel dataModel
 */
void DispenserWaterValvePlugin_Init(
   DispenserWaterValvePlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
