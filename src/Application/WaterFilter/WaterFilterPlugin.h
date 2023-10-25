/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERPLUGIN_H
#define WATERFILTERPLUGIN_H

#include "WaterFilterCalendarUsageUpdater.h"
#include "RfidCommunicationPlugin.h"

typedef struct
{
   struct
   {
      WaterFilterCalendarUsageUpdater_t waterFilterCalendarUsageUpdater;
      RfidCommunicationPlugin_t rfidCommunicationPlugin;
   } _private;
} WaterFilterPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void WaterFilterPlugin_Init(
   WaterFilterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
