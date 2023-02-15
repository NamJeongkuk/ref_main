/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FLOWMETERPLUGIN_H
#define FLOWMETERPLUGIN_H

#include "I_DataModel.h"
#include "FlowMeterMonitor.h"

typedef struct
{
   struct
   {
      FlowMeterMonitor_t flowMeterMonitor;
   } _private;
} FlowMeterPlugin_t;

void FlowMeterPlugin_Init(
   FlowMeterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
