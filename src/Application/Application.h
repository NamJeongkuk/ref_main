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
#include "TimerModuleDiagnostics.h"
#include "Input_StackUsageCalculator.h"
#include "ApplianceApiResetDiagnostics.h"
#include "GridPlugin.h"
#include "PeriodicNvUpdaterPlugin.h"
#include "ConvertibleCompartmentStatePlugin.h"
#include "SetpointZonePlugin.h"
#include "UserSetpointPlugin.h"
#include "SideBySideConfigurationPlugin.h"
#include "ShiftOffsetCalculatorCommonPlugin.h"

typedef struct
{
   struct
   {
      union
      {
         SideBySideConfigurationPlugin_t sideBySidePlugin;
      } platformConfigurations;

      I_DataModel_t *dataModel;
      AdjustedSetpointPlugin_t adjustedSetpointPlugin;
      HeartbeatLedPlugin_t heartbeatLedPlugin;
      TimerModuleDiagnostics_t timerModuleDiagnostics;
      Input_StackUsageCalculator_t stackUsageCalculator;
      ApplianceApiResetDiagnostics_t applianceApiResetDiagnostics;
      PeriodicNvUpdaterPlugin_t periodicNvUpdaterPlugin;
      ConvertibleCompartmentStatePlugin_t convertibleCompartmentStatePlugin;
      SetpointZonePlugin_t setpointZonePlugin;
      UserSetpointPlugin_t userSetpointPlugin;
      ShiftOffsetCalculatorCommonPlugin_t shiftOffsetCalculatorCommonPlugin;
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
