/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostConfiguration.h"
#include "SystemErds.h"

const DefrostConfiguration_t defrostConfiguration = {
   .freshFoodEvaporator = {
      .heaterVoteErd = Erd_FreshFoodDefrostHeater_DefrostVote,
      .heaterRelayErd = Erd_FreshFoodDefrostHeaterRelay,
      .evaporatorFanVoteErd = Erd_FreshFoodEvapFanSpeed_DefrostVote,
      .defrostWasAbnormalErd = Erd_FreshFoodDefrostWasAbnormal,
      .numberOfAbnormalDefrostsErd = Erd_NumberOfFreshFoodAbnormalDefrosts,
      .numberOfDefrostsErd = Erd_NumberOfFreshFoodDefrosts,
      .defrostHeaterOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterOnTimeInMinutes,
      .defrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
      .defrostHeaterOnForMaxTimeFaultErd = Erd_FreshFoodDefrostHeaterOnForMaxTimeFault,
      .evaporatorFilteredTemperatureResolvedInDegFx100Erd = Erd_FreshFoodEvap_FilteredTemperatureResolvedInDegFx100,
      .evaporatorThermistorIsValidResolvedErd = Erd_FreshFoodEvapThermistor_IsValidResolved,
      .offsetInParametricForEvaporatorHeaterTerminationTemperature = OFFSET_OF(DefrostData_t, heaterOnData.freshFoodHeater.defrostTerminationTemperatureInDegFx100),
   },
   .freezerEvaporator = {
      .heaterVoteErd = Erd_FreezerDefrostHeater_DefrostVote,
      .heaterRelayErd = Erd_FreezerDefrostHeaterRelay,
      .evaporatorFanVoteErd = Erd_FreezerEvapFanSpeed_DefrostVote,
      .defrostWasAbnormalErd = Erd_FreezerDefrostWasAbnormal,
      .numberOfAbnormalDefrostsErd = Erd_NumberOfFreezerAbnormalDefrosts,
      .numberOfDefrostsErd = Erd_NumberOfFreezerDefrosts,
      .defrostHeaterOnTimeInMinutesErd = Erd_FreezerDefrostHeaterOnTimeInMinutes,
      .defrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
      .defrostHeaterOnForMaxTimeFaultErd = Erd_FreezerDefrostHeaterOnForMaxTimeFault,
      .evaporatorFilteredTemperatureResolvedInDegFx100Erd = Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,
      .evaporatorThermistorIsValidResolvedErd = Erd_FreezerEvapThermistor_IsValidResolved,
      .offsetInParametricForEvaporatorHeaterTerminationTemperature = OFFSET_OF(DefrostData_t, heaterOnData.freezerHeater.defrostTerminationTemperatureInDegFx100),
   },
   .convertibleCompartmentEvaporator = {
      .heaterVoteErd = Erd_ConvertibleCompartmentDefrostHeater_DefrostVote,
      .heaterRelayErd = Erd_ConvertibleCompartmentDefrostHeaterRelay,
      .evaporatorFanVoteErd = Erd_ConvertibleCompartmentEvapFanSpeed_DefrostVote,
      .defrostWasAbnormalErd = Erd_ConvertibleCompartmentDefrostWasAbnormal,
      .numberOfAbnormalDefrostsErd = Erd_NumberOfConvertibleCompartmentAbnormalDefrosts,
      .numberOfDefrostsErd = Erd_NumberOfConvertibleCompartmentDefrosts,
      .defrostHeaterOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterOnTimeInMinutes,
      .defrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes,
      .defrostHeaterOnForMaxTimeFaultErd = Erd_ConvertibleCompartmentDefrostHeaterOnForMaxTimeFault,
      .evaporatorFilteredTemperatureResolvedInDegFx100Erd = Erd_ConvertibleCompartmentEvap_FilteredTemperatureResolvedInDegFx100,
      .evaporatorThermistorIsValidResolvedErd = Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
      .offsetInParametricForEvaporatorHeaterTerminationTemperature = OFFSET_OF(DefrostData_t, heaterOnData.convertibleCompartmentHeater.defrostTerminationTemperatureInDegFx100),
   },

   .defrostHsmStateErd = Erd_DefrostHsmState,
   .defrostStateErd = Erd_DefrostState,
   .waitingForDefrostErd = Erd_WaitingToDefrost,
   .defrostingErd = Erd_Defrosting,
   .disableDefrostErd = Erd_DisableDefrost,
   .readyToDefrostErd = Erd_ReadyToDefrost,
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .cabinetFilteredTemperatureTooWarmOnPowerUpErd = Erd_CabinetFilteredTemperatureTooWarmOnPowerUp,
   .defrostPowerUpReadyErd = Erd_DefrostPowerUpReady,
   .disableCompressorMinimumTimesVoteErd = Erd_DisableMinimumCompressorTimes_DefrostVote,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .compressorSpeedVoteErd = Erd_CompressorSpeed_DefrostVote,
   .condenserFanSpeedVoteErd = Erd_CondenserFanSpeed_DefrostVote,
   .iceCabinetFanSpeedVoteErd = Erd_IceCabinetFanSpeed_DefrostVote,
   .deliFanSpeedVoteErd = Erd_DeliFanSpeed_DefrostVote,
   .freshFoodDamperPositionVoteErd = Erd_FreshFoodDamperPosition_DefrostVote,
   .nextDefrostTypeOverrideErd = Erd_NextDefrostTypeOverride,
   .numberOfSecondaryOnlyDefrostsErd = Erd_NumberOfSecondaryOnlyDefrosts,
   .numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd = Erd_NumberOfSecondaryOnlyDefrostsBeforeAFullDefrost,
   .currentDefrostTypeErd = Erd_CurrentDefrostType,
   .eepromClearedErd = Erd_Eeprom_ClearedDefrostEepromStartup,
   .sealedSystemValveHomingRequestErd = Erd_SealedSystemValveHomingRequest,
   .sealedSystemValvePositionVoteErd = Erd_SealedSystemValvePosition_DefrostVote,
   .sealedSystemValvePositionGridVoteErd = Erd_SealedSystemValvePosition_GridVote,
   .freshFoodDamperPositionGridVoteErd = Erd_FreshFoodDamperPosition_GridVote,
   .convertibleCompartmentDamperPositionVoteErd = Erd_ConvertibleCompartmentDamperPosition_DefrostVote,
   .deliDamperPositionVoteErd = Erd_DeliDamperPosition_DefrostVote,
   .calculatedGridLinesErd = Erd_FreshFoodAndFreezerGrid_CalculatedGridLines,

   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill,
   .invalidEvaporatorThermistorDuringDefrostErd = Erd_InvalidEvaporatorThermistorDuringDefrost,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,

   .sabbathModeErd = Erd_SabbathModeEnable,
   .enhancedSabbathModeErd = Erd_EnhancedSabbathModeEnable,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .enhancedSabbathIsRequestingDefrostErd = Erd_EnhancedSabbathIsRequestingDefrost,

   .convertibleCompartmentFilteredTemperatureResolvedInDegFx100Erd = Erd_FeaturePan_FilteredTemperatureResolvedInDegFx100,
   .convertibleCompartmentThermistorIsValidResolvedErd = Erd_FeaturePanCabinetThermistor_IsValidResolved,

   .freezerEvaporatorFilteredTemperatureResolvedInDegFx100Erd = Erd_FreezerEvap_FilteredTemperatureResolvedInDegFx100,
   .freezerEvaporatorThermistorIsValidResolvedErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freezerEvaporatorThermistorHasBeenDiscoveredErd = Erd_FreezerEvaporatorThermistorDiscovered,

   .freshFoodEvaporatorThermistorHasBeenDiscoveredErd = Erd_FreshFoodEvaporatorThermistorDiscovered,
   .convertibleCompartmentEvaporatorThermistorHasBeenDiscoveredErd = Erd_ConvertibleCompartmentEvapThermistorDiscovered,

   .freezerFilteredTemperatureResolvedInDegFx100Erd = Erd_Freezer_FilteredTemperatureResolvedInDegFx100,
   .freezerThermistorIsValidResolvedErd = Erd_FreezerThermistor_IsValidResolved,
   .freezerThermistorHasBeenDiscoveredErd = Erd_FreezerThermistorDiscovered,

   .freshFoodFilteredTemperatureResolvedInDegFx100Erd = Erd_FreshFood_FilteredTemperatureResolvedInDegFx100,
   .freshFoodThermistorIsValidResolvedErd = Erd_FreshFoodThermistor_IsValidResolved,
   .freshFoodThermistorHasBeenDiscoveredErd = Erd_FreshFoodThermistorDiscovered
};

const DefrostHeaterMaxOnTimeConfiguration_t defrostHeaterMaxOnTimeConfig = {
   .freezerEvaporatorThermistorIsValidResolvedErd = Erd_FreezerEvapThermistor_IsValidResolved,
   .freshFoodEvaporatorThermistorIsValidResolvedErd = Erd_FreshFoodEvapThermistor_IsValidResolved,
   .hasConvertibleCompartmentErd = Erd_HasConvertibleCompartment,
   .convertibleCompartmentEvaporatorThermistorIsValidResolvedErd = Erd_ConvertibleCompartmentEvapThermistor_IsValidResolved,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .freshFoodDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreshFoodDefrostHeaterMaxOnTimeInMinutes,
   .freezerDefrostHeaterMaxOnTimeInMinutesErd = Erd_FreezerDefrostHeaterMaxOnTimeInMinutes,
   .convertibleCompartmentDefrostHeaterMaxOnTimeInMinutesErd = Erd_ConvertibleCompartmentDefrostHeaterMaxOnTimeInMinutes
};

const TimeThatPrechillConditionsAreMetConfiguration_t timeThatPrechillConditionsAreMetConfig = {
   .compressorIsOnErd = Erd_CompressorIsOn,
   .coolingModeErd = Erd_CoolingMode,
   .timeThatPrechillConditionsAreMetInMinutesErd = Erd_TimeThatPrechillConditionsAreMetInMinutes,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState,
   .timerModuleErd = Erd_TimerModule
};

const DefrostTestRequestHandlerConfiguration_t defrostTestRequestHandlerConfig = {
   .defrostStateErd = Erd_DefrostState,
   .defrostTestRequestErd = Erd_DefrostTestRequest,
   .disableDefrostErd = Erd_DisableDefrost,
   .defrostTestStateRequestErd = Erd_DefrostTestStateRequest,
   .nextDefrostTypeOverrideErd = Erd_NextDefrostTypeOverride,
   .useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd = Erd_UseAhamPrechillReadyToDefrostTimeAndResetDefrostCounts,
   .defrostTestRequestStatusErd = Erd_DefrostTestRequestStatus,
   .dontSkipDefrostPrechillErd = Erd_DontSkipDefrostPrechill
};

const SabbathReadyToDefrostConfig_t sabbathReadyToDefrostConfig = {
   .timerModuleErd = Erd_TimerModule,
   .waitingToDefrostErd = Erd_WaitingToDefrost,
   .sabbathIsReadyToDefrostErd = Erd_SabbathIsReadyToDefrost,
   .sabbathTimeBetweenDefrostsInMinutesErd = Erd_SabbathTimeBetweenDefrostsInMinutes,
   .sabbathWaitingForDefrostTimeInMinutesErd = Erd_SabbathWaitingForDefrostTimeInMinutes
};
