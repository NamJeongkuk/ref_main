/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentDamperPlugin.h"
#include "SystemErds.h"
#include "DataModelErdPointerAccess.h"

static const DamperVotedPosition_t defaultData = {
   .position = DamperPosition_Closed,
   .care = Vote_DontCare
};

static bool VotingErdCareDelegate(const void *votingErdData)
{
   const DamperVotedPosition_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t damperPositionErdResolverConfiguration = {
   .votingErdCare = VotingErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_ConvertibleCompartmentDamperPosition_WinningVoteErd,
   .resolvedStateErd = Erd_ConvertibleCompartmentDamperPosition_ResolvedVote,
   .numberOfVotingErds = (Erd_ConvertibleCompartmentDamperPosition_GridVote - Erd_ConvertibleCompartmentDamperPosition_WinningVoteErd)
};

static bool VotingDamperHeaterErdCareDelegate(const void *votingErdData)
{
   const HeaterVotedState_t *data = votingErdData;
   return (data->care);
}

static const ErdResolverConfiguration_t damperHeaterErdResolverConfiguration = {
   .votingErdCare = VotingDamperHeaterErdCareDelegate,
   .defaultData = &defaultData,
   .winningVoterErd = Erd_DamperHeater_WinningVoteErd,
   .resolvedStateErd = Erd_DamperHeater_ResolvedVote,
   .numberOfVotingErds = (Erd_DamperHeater_DefrostHeaterSyncVote - Erd_DamperHeater_WinningVoteErd)
};

static const DamperRequestManagerConfiguration_t requestManagerConfig = {
   .damperPositionRequestResolvedVoteErd = Erd_ConvertibleCompartmentDamperPosition_ResolvedVoteWithSabbathDelay,
   .damperStepperMotorPositionRequestErd = Erd_DamperStepperMotorPositionRequest,
   .damperHomingRequestErd = Erd_DamperHomingRequest,
   .damperCurrentPositionErd = Erd_DamperCurrentPosition,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState
};

static const DamperMaxOpenTimeConfiguration_t maxOpenDamperConfig = {
   .damperPositionMaxOpenTimeVoteErd = Erd_ConvertibleCompartmentDamperPosition_MaxOpenTimeVote,
   .damperCurrentPositionErd = Erd_DamperCurrentPosition
};

static const DamperFreezePreventionConfiguration_t damperFreezePreventionConfig = {
   .damperHeaterVoteErd = Erd_DamperHeater_DamperFreezePreventionVote,
   .damperPositionVoteErd = Erd_ConvertibleCompartmentDamperPosition_DamperFreezePreventionVote,
   .sourceThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .targetThermistorIsValidResolvedErd = Erd_FeaturePanCabinetThermistor_IsValidResolved,
   .sourceFilteredTemperatureErd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .targetFilteredTemperatureErd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
   .damperCurrentPositionErd = Erd_DamperCurrentPosition,
   .timerModuleErd = Erd_TimerModule,
   .damperFreezePreventionFsmStateErd = Erd_DamperFreezePreventionFsmState
};

static const DamperHeaterDefrostControlConfig_t damperHeaterDefrostControlConfig = {
   .defrostHeaterStateErd = Erd_FreezerDefrostHeaterRelay,
   .damperHeaterDefrostHeaterSyncVoteErd = Erd_DamperHeater_DefrostHeaterSyncVote,
   .defrostHeaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote
};

static const SabbathDelayHandlerConfiguration_t sabbathDelayConfig = {
   .compressorResolvedVote = Erd_CompressorSpeed_ResolvedVote,
   .loadResolvedVote = Erd_ConvertibleCompartmentDamperPosition_ResolvedVote,
   .loadResolvedVoteWithSabbathDelay = Erd_ConvertibleCompartmentDamperPosition_ResolvedVoteWithSabbathDelay
};

void ConvertibleCompartmentDamperPlugin_Init(
   ConvertibleCompartmentDamperPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   ErdResolver_Init(
      &instance->_private.damperPositionErdResolver,
      DataModel_AsDataSource(dataModel),
      &damperPositionErdResolverConfiguration);

   ErdResolver_Init(
      &instance->_private.damperHeaterErdResolver,
      DataModel_AsDataSource(dataModel),
      &damperHeaterErdResolverConfiguration);

   SabbathDelayHandler_Init(
      &instance->_private.sabbathDelayHandler,
      dataModel,
      &sabbathDelayConfig);

   DamperMotorPlugin_Init(
      &instance->_private.damperMotorPlugin,
      dataModel);

   DamperRequestManager_Init(
      &instance->_private.damperRequestManager,
      dataModel,
      &requestManagerConfig,
      PersonalityParametricData_Get(dataModel)->damperData);

   DamperHeaterVotingFrameworkPlugin_Init(
      &instance->_private.damperHeaterVotingFrameworkPlugin,
      dataModel);

   DamperFreezePrevention_Init(
      &instance->_private.damperFreezePrevention,
      dataModel,
      &damperFreezePreventionConfig,
      PersonalityParametricData_Get(dataModel)->damperData);

   DamperMaxOpenTimeMonitor_Init(
      &instance->_private.damperMaxOpenTime,
      dataModel,
      &maxOpenDamperConfig,
      PersonalityParametricData_Get(dataModel)->damperData);

   DamperHeaterDefrostControl_Init(
      &instance->_private.damperHeaterDefrostControl,
      dataModel,
      &damperHeaterDefrostControlConfig,
      DataModelErdPointerAccess_GetTimerModule(dataModel, Erd_TimerModule),
      PersonalityParametricData_Get(dataModel)->damperHeaterData);
}
