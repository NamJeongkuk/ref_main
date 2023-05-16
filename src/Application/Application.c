/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Application.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"
#include "SabbathPlugin.h"
#include "DoorPlugin.h"
#include "FaultWrapperPlugin.h"
#include "ApplianceFeatureApiUpdater.h"

static const ApplianceApiResetDiagnosticsConfiguration_t applianceApiResetDiagnosticsConfiguration = {
   .resetReasonErd = Erd_ResetReason,
   .resetCountErd = Erd_ResetCount,
   .secondsSinceLastResetErd = Erd_SecondsSinceLastReset
};

static void InitializePlatformParametricErds(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *parametricData = PersonalityParametricData_Get(dataModel);

   DataModel_Write(dataModel, Erd_HasConvertibleCompartment, &parametricData->convertibleCompartmentData->hasConvertibleCompartment);
}

void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel,
   ResetReason_t resetReason)
{
   instance->_private.dataModel = dataModel;

   InitializePlatformParametricErds(dataModel);

   // TODO Remove after updating icemaker
   {
      IceMakerTypeInformation_t iceMakerInfo;
      iceMakerInfo.type = IceMakerType_Twist;
      iceMakerInfo.location = IceMakerLocation_Fridge;
      DataModel_Write(dataModel, WinningVoteErd_Erd_IceMaker1TypeInformation, &iceMakerInfo);
   }
   ApplianceFeatureApiUpdater_Init(dataModel);

   ShiftOffsetCalculatorCommonPlugin_Init(&instance->_private.shiftOffsetCalculatorCommonPlugin, dataModel);

   HeartbeatLedPlugin_Init(&instance->_private.heartbeatLedPlugin, dataModel);
   SabbathPlugin_Init(dataModel);
   PeriodicNvUpdaterPlugin_Init(&instance->_private.periodicNvUpdaterPlugin, dataModel);

   SideBySideConfigurationPlugin_Init(&instance->_private.platformConfigurations.sideBySidePlugin, dataModel);
   DoorPlugin_Init(dataModel);
   FaultWrapperPlugin_Init(dataModel);

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
}
