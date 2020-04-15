/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLIANCEAPIPLUGIN_H
#define APPLIANCEAPIPLUGIN_H

#include "I_TinyDataSource.h"
#include "TinyTimer.h"
#include "TinyEventSubscription.h"

typedef struct
{
   struct
   {
      I_TinyDataSource_t *dataSource;
      TinyEventSubscription_t dataChangedSubscription;
      TinyTimer_t resetTimer;
   } _private;
} ApplianceApiPlugin_t;

/*!
 * @param instance
 * @param dataSource
 */
void ApplianceApiPlugin_Init(
   ApplianceApiPlugin_t *instance,
   I_TinyDataSource_t *dataSource);

#endif
