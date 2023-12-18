/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "I_DataModel.h"
#include "HeartbeatLedPlugin.h"
#include "TimerModuleDiagnostics.h"
#include "Input_StackUsageCalculator.h"
#include "ApplianceApiResetDiagnostics.h"
#include "PeriodicNvUpdaterPlugin.h"
#include "SideBySideConfigurationPlugin.h"
#include "ShiftOffsetCalculatorCommonPlugin.h"
#include "WaterFilterPlugin.h"
#include "BroadcastResetPlugin.h"
#include "SoundLevelPlugin.h"
#include "ServiceModePlugin.h"
#include "FaultMonitorPlugin.h"
#include "BoardFeaturesToApplianceApiFeaturesUpdaterPlugin.h"
#include "NonVolatileUsageMonitorPlugin.h"

typedef struct
{
   struct
   {
      union
      {
         SideBySideConfigurationPlugin_t sideBySidePlugin;
      } platformConfigurations;

      I_DataModel_t *dataModel;
      HeartbeatLedPlugin_t heartbeatLedPlugin;
      TimerModuleDiagnostics_t timerModuleDiagnostics;
      ApplianceApiResetDiagnostics_t applianceApiResetDiagnostics;
      PeriodicNvUpdaterPlugin_t periodicNvUpdaterPlugin;
      ShiftOffsetCalculatorCommonPlugin_t shiftOffsetCalculatorCommonPlugin;
      WaterFilterPlugin_t waterFilterPlugin;
      BroadcastResetPlugin_t broadcastResetPlugin;
      SoundLevelPlugin_t soundLevelPlugin;
      ServiceModePlugin_t serviceModePlugin;
      FaultMonitorPlugin_t faultMonitorPlugin;
      BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_t boardFeaturesToApplianceFeaturesPlugin;
      NonVolatileUsageMonitorPlugin_t nvUsageMonitorPlugin;
   } _private;
} Application_t;

/*!
 * @brief
 * @param instance
 * @param dataModel
 * @param resetReason
 */
void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel,
   ResetReason_t resetReason);

#endif
