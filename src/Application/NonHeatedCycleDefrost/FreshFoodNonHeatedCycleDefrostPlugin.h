/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NONHEATEDCYCLEDEFROSTPLUGIN_H
#define NONHEATEDCYCLEDEFROSTPLUGIN_H

#include "FreshFoodNonHeatedCycleDefrost.h"
#include "I_DataModel.h"

typedef struct
{
   struct
   {
      FreshFoodNonHeatedCycleDefrost_t freshFoodNonHeatedCycleDefrost;
   } _private;
} FreshFoodNonHeatedCycleDefrostPlugin_t;

void FreshFoodNonHeatedCycleDefrostPlugin_Init(
   FreshFoodNonHeatedCycleDefrostPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
