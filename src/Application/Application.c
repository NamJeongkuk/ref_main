/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "Application.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

enum
{
   StackUsageUpdatePeriodInMsec = 1000
};

static void UpdateStackUsage(void *context)
{
   REINTERPRET(instance, context, Application_t *);
   uint8_t stackUsage;

   Input_Read(&instance->_private.stackUsageCalculator.interface, &stackUsage);
   DataModel_Write(instance->_private.dataModel, Erd_StackPercentFree, &stackUsage);
}

void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel,
   const StackConfiguration_t *stackConfiguration)
{
   instance->_private.dataModel = dataModel;

   HeartbeatLedPlugin_Init(&instance->_private.heartbeatLedPlugin, dataModel);
   SwitchedLedPlugin_Init(&instance->_private.switchedLedPlugin, dataModel);
   KeyStreamPlugin_Init(&instance->_private.keyStreamPlugin, dataModel);

   TimerModule_t *timerModule = DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);
   TimerModuleDiagnostics_Init(
      &instance->_private.timerModuleDiagnostics,
      timerModule,
      DataModel_GetInput(dataModel, Erd_TimerModuleDiagnosticsEnable),
      DataModel_GetOutput(dataModel, Erd_TimerModuleDiagnosticsResult));

   Input_StackUsageCalculator_Init(&instance->_private.stackUsageCalculator, stackConfiguration);

   TimerModule_StartPeriodic(
      timerModule,
      &instance->_private.stackUsageUpdateTimer,
      StackUsageUpdatePeriodInMsec,
      UpdateStackUsage,
      instance);
}
