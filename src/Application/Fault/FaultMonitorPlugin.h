/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTMONITORPLUGIN_H
#define FAULTMONITORPLUGIN_H

#include "I_DataModel.h"
#include "FreshFoodNoFreezeLimitTrippedFaultDetector.h"

typedef struct
{
   FreshFoodNoFreezeLimitTrippedFaultDetector_t freshFoodNoFreezeLimitTrippedFaultDetector;
} FaultMonitorPlugin_t;

void FaultMonitorPlugin_Init(FaultMonitorPlugin_t *instance, I_DataModel_t *dataModel);

#endif
