/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Application.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "SnackModePlugin.h"
#include "NotificationsPlugin.h"

static const ApplianceApiResetDiagnosticsConfiguration_t applianceApiResetDiagnosticsConfiguration = {
   .resetReasonErd = Erd_ResetReason,
   .resetCountErd = Erd_ResetCount,
   .secondsSinceLastResetErd = Erd_SecondsSinceLastReset
};

static void InitializePlatformParametricErds(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *parametricData = PersonalityParametricData_Get(dataModel);
   bool hasConvertibleCompartment = BITMAP_STATE(&parametricData->platformData->compartmentBitmap, Compartment_Convertible);

   DataModel_Write(dataModel, Erd_HasConvertibleCompartment, &hasConvertibleCompartment);
   DataModel_Write(dataModel, Erd_RefrigeratorModel, parametricData->refrigeratorModelData);
}

void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel,
   ResetReason_t resetReason)
{
   instance->_private.dataModel = dataModel;

   InitializePlatformParametricErds(dataModel);

   ApplianceFeatureApiUpdaterPlugin_Init(&instance->_private.applianceFeatureApiUpdaterPlugin, dataModel);
   BoardFeaturesToApplianceApiFeaturesUpdaterPlugin_Init(&instance->_private.boardFeaturesToApplianceFeaturesPlugin, dataModel);

   NotificationsPlugin_Init(dataModel);

   ShiftOffsetCalculatorCommonPlugin_Init(&instance->_private.shiftOffsetCalculatorCommonPlugin, dataModel);

   HeartbeatLedPlugin_Init(&instance->_private.heartbeatLedPlugin, dataModel);
   PeriodicNvUpdaterPlugin_Init(&instance->_private.periodicNvUpdaterPlugin, dataModel);

   ParallelMotorControlPlugin_Init(&instance->_private.parallelMotorControlPlugin, dataModel);

   PlatformPlugin_Init(&instance->_private.platformPlugin, dataModel);

   SnackModePlugin_Init(dataModel);
   SoundLevelPlugin_Init(&instance->_private.soundLevelPlugin, dataModel);

   WaterFilterPlugin_Init(&instance->_private.waterFilterPlugin, dataModel);

   TimerModule_t *timerModule = DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule);
   TimerModuleDiagnostics_Init(
      &instance->_private.timerModuleDiagnostics,
      timerModule,
      DataModel_GetInput(dataModel, Erd_TimerModuleDiagnosticsEnable),
      DataModel_GetOutput(dataModel, Erd_TimerModuleDiagnosticsResult));

   ApplianceApiResetDiagnostics_Init(
      &instance->_private.applianceApiResetDiagnostics,
      dataModel,
      timerModule,
      resetReason,
      &applianceApiResetDiagnosticsConfiguration);

   BroadcastResetPlugin_Init(
      &instance->_private.broadcastResetPlugin,
      dataModel);

   ServiceModePlugin_Init(&instance->_private.serviceModePlugin, dataModel);

   FaultMonitorPlugin_Init(&instance->_private.faultMonitorPlugin, dataModel);

   NonVolatileUsageMonitorPlugin_Init(&instance->_private.nvUsageMonitorPlugin, dataModel);
}
