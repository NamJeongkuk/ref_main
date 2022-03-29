/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERIODICNVUPDATERPLUGIN_H
#define PERIODICNVUPDATERPLUGIN_H

#include "I_DataModel.h"
#include "PeriodicNvUpdateService.h"

typedef struct
{
   struct
   {
      PeriodicNvUpdateService_t periodicNvUpdate;
   } _private;
} PeriodicNvUpdaterPlugin_t;

void PeriodicNvUpdaterPlugin_Init(PeriodicNvUpdaterPlugin_t *instance, I_DataModel_t *dataModel);

#endif
