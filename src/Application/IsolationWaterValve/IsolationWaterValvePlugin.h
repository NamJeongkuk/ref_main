/*!
 * @file
 * @brief Initialize modules related to the isolation water valve
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ISOLATIONWATERVALVEPLUGIN_H
#define ISOLATIONWATERVALVEPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"

typedef struct
{
   struct
   {
      ErdResolver_t isolationWaterValveErdResolver;
   } _private;
} IsolationWaterValvePlugin_t;

void IsolationWaterValvePlugin_Init(
   IsolationWaterValvePlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
