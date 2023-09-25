/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BROADCASTRESETPLUGIN_H
#define BROADCASTRESETPLUGIN_H

#include "BroadcastReset.h"

typedef struct
{
   struct
   {
      BroadcastReset_t broadcastReset;
   } _private;
} BroadcastResetPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void BroadcastResetPlugin_Init(
   BroadcastResetPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
