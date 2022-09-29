/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "I_DataModel.h"
#include "AdjustedSetpointPlugin.h"
#include "HeartbeatLedPlugin.h"
#include "SwitchedLedPlugin.h"
#include "KeyStreamPlugin.h"
#include "TimerModuleDiagnostics.h"
#include "Input_StackUsageCalculator.h"
#include "ApplianceApiResetDiagnostics.h"
#include "GridPlugin.h"
#include "PeriodicNvUpdaterPlugin.h"
#include "ConvertibleCompartmentStatePlugin.h"
#include "FanPlugin.h"
#include "SetpointZonePlugin.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;

      AdjustedSetpointPlugin_t adjustedSetpointPlugin;
      HeartbeatLedPlugin_t heartbeatLedPlugin;
      SwitchedLedPlugin_t switchedLedPlugin;
      KeyStreamPlugin_t keyStreamPlugin;
      GridPlugin_t gridPlugin;
      TimerModuleDiagnostics_t timerModuleDiagnostics;
      Input_StackUsageCalculator_t stackUsageCalculator;
      ApplianceApiResetDiagnostics_t applianceApiResetDiagnostics;
      PeriodicNvUpdaterPlugin_t periodicNvUpdaterPlugin;
      ConvertibleCompartmentStatePlugin_t convertibleCompartmentStatePlugin;
      FanPlugin_t fanPlugin;
      SetpointZonePlugin_t setpointZonePlugin;
      Timer_t stackUsageUpdateTimer;
   } _private;
} Application_t;

/*!
 * Initialize Application
 * @param instance
 * @param dataModel
 * @param resetReason
 */
void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel,
   const StackConfiguration_t *stackConfiguration,
   ResetReason_t resetReason);

#endif
