/*!
 * @file
 * @brief Determines and stores the following times into ERDs based on various conditions:
 * - max prechill time
 * - post dwell exit time
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTPARAMETERSELECTOR_H
#define DEFROSTPARAMETERSELECTOR_H

#include "I_DataModel.h"
#include "EventSubscription.h"

typedef struct
{
   struct
   {
      EventSubscription_t dataModelSubscription;
      I_DataModel_t *dataModel;
   } _private;
} DefrostParameterSelector_t;

void DefrostParameterSelector_Init(DefrostParameterSelector_t *instance, I_DataModel_t *dataModel);

#endif
