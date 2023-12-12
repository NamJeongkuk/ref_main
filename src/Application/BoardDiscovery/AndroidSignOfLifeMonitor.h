/*!
 * @file
 * @brief Monitors the primary sign of life if the Android SBC exists in the system
 * Will set a fault if the sign of life does not change after some time
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ANDROIDSIGNOFLIFEMONITOR_H
#define ANDROIDSIGNOFLIFEMONITOR_H

#include "ErdWatchdog.h"
#include "I_DataModel.h"
#include "EventSubscription.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t androidInSystemChanged;
      ErdWatchdog_t androidWatchdog;
   } _private;
} AndroidSignOfLifeMonitor_t;

/*!
 * @param instance
 * @param dataModel
 */
void AndroidSignOfLifeMonitor_Init(
   AndroidSignOfLifeMonitor_t *instance,
   I_DataModel_t *dataModel);

#endif
