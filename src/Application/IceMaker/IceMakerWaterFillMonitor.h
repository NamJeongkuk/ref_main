/*!
 * @file
 * @brief Water fill monitor in charge of requesting the flow meter monitor.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERWATERFILLMONITOR_H
#define ICEMAKERWATERFILLMONITOR_H

#include "I_DataModel.h"
#include "IceMakerFillMonitorData.h"
#include "Timer.h"

typedef struct
{
   Erd_t stopIceMakerFillSignalErd; // Signal_t
   Erd_t flowMeterWaterDispensedOzx100Erd; // uint32_t
   Erd_t iceMakerWaterFillMonitoringRequestErd; // bool
   Erd_t flowMeterMonitoringRequestErd; // bool
   Erd_t timerModuleErd; // TimerModule *
} IceMakerWaterFillMonitorConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t flowMeterWaterDispensedSubscription;
      EventSubscription_t fillMonitoringRequestSubscription;
      const IceMakerWaterFillMonitorConfig_t *config;
      const IceMakerFillMonitorData_t *iceMakerFillMonitorData;
      Timer_t maxFillTimer;
      Timer_t timedFillTimer;
   } _private;
} IceMakerWaterFillMonitor_t;

void IceMakerWaterFillMonitor_Init(
   IceMakerWaterFillMonitor_t *instance,
   I_DataModel_t *dataModel,
   const IceMakerWaterFillMonitorConfig_t *config,
   const IceMakerFillMonitorData_t *iceMakerFillMonitorData);

#endif
