/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIGNOFLIFEPLUGIN_H
#define SIGNOFLIFEPLUGIN_H

#include "AndroidUiSignOfLifeMonitor.h"

typedef struct
{
   struct
   {
      AndroidUiSignOfLifeMonitor_t androidUiSignOfLifeMonitor;
   } _private;
} SignOfLifePlugin_t;

/*!
 * Plugin for Sign of Life
 * @param instance
 * @param dataModel
 * @return
 */
void SignOfLifePlugin_Init(
   SignOfLifePlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
