/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Application.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "ParametricDataErds.h"
#include "SabbathPlugin.h"
#include "DoorPlugin.h"
#include "EepromAppPlugin.h"
#include "FaultWrapperPlugin.h"
#include "OverrideArbiterPlugin.h"
#include "SetpointResolverPlugin.h"

enum
{
   StackUsageUpdatePeriodInMsec = 1000
};

static const ApplianceApiResetDiagnosticsConfiguration_t applianceApiResetDiagnosticsConfiguration = {
   .resetReasonErd = Erd_ResetReason,
   .resetCountErd = Erd_ResetCount,
   .secondsSinceLastResetErd = Erd_SecondsSinceLastReset
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
   const StackConfiguration_t *stackConfiguration,
   ResetReason_t resetReason)
{
   instance->_private.dataModel = dataModel;

   ParametricDataErds_Init(
      dataModel,
      Erd_ParametricDataImageCrc,
      Erd_AppliancePersonality,
      Erd_PersonalityParametricData,
      Erd_PersonalityIdOutOfRangeFlag);

   AdjustedSetpointPlugin_Init(&instance->_private.adjustedSetpointPlugin, dataModel);
   ShiftOffsetCalculatorCommonPlugin_Init(&instance->_private.shiftOffsetCalculatorCommonPlugin, dataModel);
   OverrideArbiterPlugin_Init(dataModel);

   HeartbeatLedPlugin_Init(&instance->_private.heartbeatLedPlugin, dataModel);
   EepromAppPlugin_Init(dataModel);
   SabbathPlugin_Init(dataModel);
   SetpointResolverPlugin_Init(dataModel);
   SetpointZonePlugin_Init(&instance->_private.setpointZonePlugin, dataModel);
   ConvertibleCompartmentStatePlugin_Init(&instance->_private.convertibleCompartmentStatePlugin, dataModel);
   DoorPlugin_Init(dataModel);
   PeriodicNvUpdaterPlugin_Init(&instance->_private.periodicNvUpdaterPlugin, dataModel);
   FaultWrapperPlugin_Init(dataModel);

   SideBySideConfigurationPlugin_Init(&instance->_private.platformConfigurations.sideBySidePlugin, dataModel);
   UserSetpointPlugin_Init(&instance->_private.userSetpointPlugin, dataModel);

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

   ApplianceApiResetDiagnostics_Init(
      &instance->_private.applianceApiResetDiagnostics,
      dataModel,
      timerModule,
      resetReason,
      &applianceApiResetDiagnosticsConfiguration);
}
