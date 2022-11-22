/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VALVEPLUGIN_H
#define VALVEPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"

typedef struct
{
   struct
   {
      ErdResolver_t valvePositionErdResolver;
   } _private;
} ValvePlugin_t;

void ValvePlugin_Init(ValvePlugin_t *instance, I_DataModel_t *dataModel);

#endif
