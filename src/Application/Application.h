/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "ApplianceApiResetDiagnostics.h"
#include "ApplianceFeatureApiUpdaterPlugin.h"
#include "BoardFeaturesToApplianceApiFeaturesUpdaterPlugin.h"
#include "BroadcastResetPlugin.h"
#include "FaultMonitorPlugin.h"
#include "HeartbeatLedPlugin.h"
#include "I_DataModel.h"
#include "Input_StackUsageCalculator.h"
#include "NonVolatileUsageMonitorPlugin.h"
#include "ParallelMotorControlPlugin.h"
#include "PeriodicNvUpdaterPlugin.h"
#include "PlatformPlugin.h"
#include "ServiceModePlugin.h"
#include "ShiftOffsetCalculatorCommonPlugin.h"
#include "SoundLevelPlugin.h"
#include "TestingFsm.h"
#include "TimerModuleDiagnostics.h"
#include "WaterFilterPlugin.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      HeartbeatLedPlugin_t heartbeatLedPlugin;
      TimerModuleDiagnostics_t timerModuleDiagnostics;
      ApplianceApiResetDiagnostics_t applianceApiResetDiagnostics;
      ApplianceFeatureApiUpdaterPlugin_t applianceFeatureApiUpdaterPlugin;
      PeriodicNvUpdaterPlugin_t periodicNvUpdaterPlugin;
      ShiftOffsetCalculatorCommonPlugin_t shiftOffsetCalculatorCommonPlugin;
      WaterFilterPlugin_t waterFilterPlugin;
      BroadcastResetPlugin_t broadcastResetPlugin;
      SoundLevelPlugin_t soundLevelPlugin;
      ServiceModePlugin_t serviceModePlugin;
      FaultMonitorPlugin_t faultMonitorPlugin;
      BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_t boardFeaturesToApplianceFeaturesPlugin;
      NonVolatileUsageMonitorPlugin_t nvUsageMonitorPlugin;
      PlatformPlugin_t platformPlugin;
      ParallelMotorControlPlugin_t parallelMotorControlPlugin;
      TestingFsm_t testingfsm;
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
