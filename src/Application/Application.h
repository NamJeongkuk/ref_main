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
#include "RfidCommunicationPlugin.h"
#include "BroadcastResetPlugin.h"
#include "SoundLevelPlugin.h"

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
      RfidCommunicationPlugin_t rfidCommunicationPlugin;
      BroadcastResetPlugin_t broadcastResetPlugin;
      SoundLevelPlugin_t soundLevelPlugin;
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
